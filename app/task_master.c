#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mbsupport.h"
#include "task_master.h"

#include "panelConfig.h"
#include "masterTransport.h"

#include "timers.h"


extern Typedef_PanelConfig panelConfig;
TypeDef_Master master;

extern void master_LEDonFaultReset();
extern void master_LEDonFaultState();
extern void master_LEDonRUNstate(int dev);
extern void master_LEDonReadyState(int dev);
extern void master_LEDonTimeoutState();
extern void master_LEDonGlobRunState(bool state);

extern void master_RDIOonRUNstate(int dev);

//Timers used for 10 sec delay before start
TimerHandle_t xTimers_StartDelay[5];

// We are using semaphore to protect transport from multiple tasks from delay timers
xSemaphoreHandle xDisplayMasterR485Semaphore;

// Used for encode slave fault bits
static inline int checkCode(TypeDef_MB_Holding* holding, uint16_t slaveAdr, int mask, int16_t offset);

#define MASTER_TRANSPORT_CHECK_TIMEOUT( xstate ) {\
        master.master_wdg[slaveAdr] = (xstate) != MASTER_TRANSPORT_NOERROR; \
        if( master.master_wdg[slaveAdr] ){ \
            master.slaveStates[slaveAdr] = MASTER_STATE_onTIMEOUT; \
        } \
};

void vTask_MasterHWstates(void* argument);


// Delay timers callbacks
void vTimerAC_Callback(TimerHandle_t xTimer) {

    xSemaphoreTake(xDisplayMasterR485Semaphore, portMAX_DELAY);

    // for (size_t i = 1; i < 5; i++) {
    //     uint16_t slaveAdr = i;
    //     if (xTimer == xTimers_StartDelay[i]) {
    //         TypeDef_MB_Holding* holding;
    //         TypeDef_MB_Table* table = &holdings_table[slaveAdr];
    //         if(master.slaveStates[slaveAdr] == MASTER_STATE_onRUN) {
    //             master_RDIOonRUNstate(slaveAdr);  // Enable output RDIO after start delay
    //         }
    //         break;
    //     }
    // }

    xSemaphoreGive(xDisplayMasterR485Semaphore);
}

void vTimerDC_Callback(TimerHandle_t xTimer) {

    xSemaphoreTake(xDisplayMasterR485Semaphore, portMAX_DELAY);

    for (size_t i = 1; i < 5; i++) {
        uint16_t slaveAdr = i;
        if (xTimer == xTimers_StartDelay[i]) {
            TypeDef_MB_Holding* holding;
            TypeDef_MB_Table* table = &holdings_table[slaveAdr];
            master.start_req_hw[slaveAdr] = false;
            //send start;
            holding = GetHoldingByAdrFromTable(104, table);
            uint16_t valStart = *holding->pntr | 0x2;
            master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valStart);
        }
    }
    xSemaphoreGive(xDisplayMasterR485Semaphore);
}

// Master main task
/*
    1. Get semaphore
    2. Check slave enabled
    3. Check slave states and handle start|stop req
    4. Check slave displayed
    5. Read displayed slave holding registers

*/

volatile uint16_t reg_val_tmp = 0;
volatile uint16_t reg_adr_tmp = 220;
void vTask_Master(__attribute__((unused)) void* argument)
{
    // Init master hardware for modbus RTU
    master_hwInit(panelConfig.modbus_master.speed);

    vTaskDelay(300);
    /* 1. Get semaphore */
    vSemaphoreCreateBinary(xDisplayMasterR485Semaphore);

    xTimers_StartDelay[1] = xTimerCreate("", 10000, pdFALSE, NULL, vTimerAC_Callback);
    xTimers_StartDelay[2] = xTimerCreate("", 10000, pdFALSE, NULL, vTimerAC_Callback);
    xTimers_StartDelay[3] = xTimerCreate("", 10000, pdFALSE, NULL, vTimerDC_Callback);
    xTimers_StartDelay[4] = xTimerCreate("", 10000, pdFALSE, NULL, vTimerDC_Callback);

    xTaskCreate(vTask_MasterHWstates, "MasterStates", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

    while (1) {

        TypeDef_MB_Holding* holding;

        // master_LEDonWaitState(1);

         // /// test
         // xSemaphoreTake(xDisplayMasterR485Semaphore, portMAX_DELAY);
         // // Reset fault LED
         // master_LEDonFaultReset();

         //  vTaskDelay(50); 

         //   TypeDef_MB_Table* table1 = &holdings_table[3];
         //   holding = GetHoldingByAdrFromTable(reg_adr_tmp, table1);

         //   reg_val_tmp = master_readHoldingOs(3, holding->reg_adr & 0x0FFF, holding->pntr);

         //   reg_val_tmp = *holding->pntr;
         // xSemaphoreGive(xDisplayMasterR485Semaphore);
         // continue;

        for (size_t j = 1; j < 5; j++) // Read all slaves from j (slave addr) = 1 to 4
        {
            vTaskDelay(50); // dalay between slaves
            // Current slave address
            uint16_t slaveAdr = j;

            // Current slave holding register table pointer
            TypeDef_MB_Table* table = &holdings_table[slaveAdr];

            /* 2. Check if slave is enabled */
            bool slave_enable[5] = { false,
                                    panelConfig.enableAC1,
                                    panelConfig.enableAC2,
                                    panelConfig.enableDC1,
                                    panelConfig.enableDC2 };

            if (slave_enable[j] != true)
            { // skip if slave not enabled
                continue;
            }

            /* 3. Check slave states and handle start|stop req */
            if (slaveAdr == CONFIG_SLAVE_AC1 || slaveAdr == CONFIG_SLAVE_AC2)
            {

                // read AC slaves status
                holding = GetHoldingByAdrFromTable(270, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                );
                // check faults
                int fltcode = 0;
                fltcode = checkCode(holding, slaveAdr, 12, 0);
                holding = GetHoldingByAdrFromTable(271, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                );
                // check faults
                fltcode += checkCode(holding, slaveAdr, 11, 16);

                // Fault Reset
                if (fltcode == 0) {
                    if (master.slaveStates[slaveAdr] == MASTER_STATE_onFAULT) {
                        master.slaveStates[slaveAdr] = MASTER_STATE_onREADY;
                        master.fault_source[slaveAdr] = false;
                        master.fault_code[slaveAdr] = 0;
                    }
                }

                if (master.slaveStates[slaveAdr] != MASTER_STATE_onFAULT) {
                    // Read control register
                    holding = GetHoldingByAdrFromTable(103, table);
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                    )

                        uint16_t ac_slave_CR = *holding->pntr;

                    if (!master.master_wdg[slaveAdr]) { //Skip if onTimeout state
                        /* 
                                        READY - RDO off, inv - off --> start_hw
                           stop hw <--- WAIT  - RDO off, inv - on  --> start_rdo
                          stop rdo <--- RUN   - RDO on , inv - on
                        */

                            if((ac_slave_CR & 0x8) == 0){ // Ready state
                                master.slaveStates[slaveAdr] = MASTER_STATE_onREADY;

                                master.start_req_rdo[slaveAdr] = false; // always reset rdo start req in ready state
                                if(master.start_req_hw[slaveAdr]){
                                    // send start
                                    holding = GetHoldingByAdrFromTable(103, table);
                                    uint16_t valACStart = *holding->pntr | 8U; // bit #3
                                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                                        master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valACStart)
                                    );
                                    master.start_req_hw[slaveAdr] = false;
                                    master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT;
                                }
                            }else{
                                if(master.start_req_rdo[slaveAdr]){
                                        //enable RDO
                                        if( master.slaveStates[slaveAdr] == MASTER_STATE_onWAIT){
                                            master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;

                                        }else{
                                             master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT;
                                        }
                                        master.start_req_rdo[slaveAdr] = false;
                                }

                                if(master.start_req_hw[slaveAdr]){
                                        //stop inverter (long press)
                                    master.start_req_hw[slaveAdr] = false;
                                    master.start_req_rdo[slaveAdr] = false;
                                    uint16_t valStopACx = *holding->pntr ^ 8;
                                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                                        master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valStopACx)
                                    );
                                }
                            }
                    } //AC slave onTimeout state
                    
                }else{
                        // AC slave onFAULT                        
                        master.start_req_hw[slaveAdr] = false;
                        master.start_req_rdo[slaveAdr] = false;
                }

            }
            /*  Read status and control DC slaves */
            if (slaveAdr == CONFIG_SLAVE_DC1 || slaveAdr == CONFIG_SLAVE_DC2)
            {
                // Read status register
                holding = GetHoldingByAdrFromTable(220, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                );

                if (!master.master_wdg[slaveAdr]) {//Skip if onTimeout state 

                    if ((*holding->pntr & 0x4) != 0) {
                        // DC onFault state
                        master.slaveStates[slaveAdr] = MASTER_STATE_onFAULT;
                        // Read Fault code register 
                        holding = GetHoldingByAdrFromTable(240, table);
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                        );
                        checkCode(holding, slaveAdr, 8, -1); // Set slave onFAULT state here!
                        // stops active start requests
                        master.start_req_hw[slaveAdr] = false;
                    }
                    else {
                        master.fault_source[slaveAdr] = false;
                    }
                    /* Handle DC start|stop req */
                    if ((*holding->pntr & 0x2) != 0) {
                        // DC onReady state
                        master.slaveStates[slaveAdr] = MASTER_STATE_onREADY;
                        // SLAVE START REQUEST
                        if (master.start_req_hw[slaveAdr] == true) {
                            if (xTimerIsTimerActive(xTimers_StartDelay[slaveAdr]) == pdFALSE)
                            { // Timer not started
                                xTimerStart(xTimers_StartDelay[slaveAdr], 0);
                            }
                            else {
                                master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT;
                            }
                        }
                    }

                    if ((*holding->pntr & 0x1) != 0) {
                        // DC onRun state
                        master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                        // SLAVE START STOP
                        if (master.start_req_hw[slaveAdr] == true) {
                            //send stop;
                            holding = GetHoldingByAdrFromTable(104, table);
                            uint16_t valStart = *holding->pntr ^ 0x2U;
                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valStart)
                            )
                                master.start_req_hw[slaveAdr] = false;
                            vTaskDelay(300);
                        }
                    }


                }

            }

            /*  4. Check slave displayed */
            if (slaveAdr != panelConfig.active_slave)
            { // skip not active slave
                continue;
            }
            /*  5. Read displayed slave holding registers */
            int tocntr = 0;
            for (size_t i = 0; i < table->len; i++)
            {
                // vTaskDelay(10);
                holding = &table->holdings[i];

                if (holding->lock && holding->change_req)
                {

                    /* Send holding value to slave */
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, *holding->pntr)
                    );
                    holding->lock = false;
                    holding->change_req = false;
                    vTaskDelay(10); // Delay after write
                }

                // read holding value to slave
                uint16_t out = 0;
                TypedefEnum_MasterTransportSates res = master_readHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, &out);
                if (res == MASTER_TRANSPORT_NOERROR)
                {
                    if (holding->lock == false)
                    {
                        *holding->pntr = out;
                    }
                    //feed master wdg
                    master.master_wdg[slaveAdr] = false;
                }
                else {
                    master.master_wdg[slaveAdr] = true;
                    tocntr++;
                    master.slaveStates[slaveAdr] = MASTER_STATE_onTIMEOUT;
                    if (tocntr > 3) break;
                }
            }
            // TODO MASTER ERROR HANDLER
        }
        xSemaphoreGive(xDisplayMasterR485Semaphore);
    }
}

static inline int checkCode(TypeDef_MB_Holding* holding, uint16_t slaveAdr, int mask, int16_t offset)
{
    uint16_t faultCode = (*holding->pntr) & 0x0FFF;
    if (faultCode > 0)
    {
        for (int i = 0; i < mask; i++)
        { // fault code is bit number
            uint8_t bit = (faultCode & (1 << i)) >> i;
            if (bit == 1)
            {
                master.slaveStates[slaveAdr] = MASTER_STATE_onFAULT;
                master.fault_source[slaveAdr] = true;
                master.fault_code[slaveAdr] = i + 1 + offset;
                return  master.fault_code[slaveAdr];
            }
        }
    }
    return 0;
}


void vTask_MasterHWstates(void* argument) {

    bool onGlobalFault = false;
    bool onGlobalTimeout = false;
    bool onGlobalRUN = false;
    while (1) {

        vTaskDelay(300);

        if (!onGlobalFault && !onGlobalTimeout) {
            master_LEDonFaultReset();
        }
        else {
            if (onGlobalTimeout) {
                master_LEDonTimeoutState();
            }
            else {
                master_LEDonFaultState();
            }
        }

        master_LEDonGlobRunState(onGlobalRUN);

        onGlobalFault = false;
        onGlobalTimeout = false;
        onGlobalRUN = false;

        for (size_t i = 1; i < 5; i++)
        {
            onGlobalFault |= master.slaveStates[i] == MASTER_STATE_onFAULT;
            onGlobalTimeout |= master.slaveStates[i] == MASTER_STATE_onTIMEOUT;
            onGlobalRUN |= master.slaveStates[i] == MASTER_STATE_onRUN;

            switch (master.slaveStates[i])
            {
            case MASTER_STATE_onREADY: case MASTER_STATE_onFAULT:
                master_LEDonReadyState(i);
                break;

                break;
            case MASTER_STATE_onRUN:
                master_LEDonRUNstate(i);

                break;
            case MASTER_STATE_onWAIT:
                master_LEDonWaitState(i);

            default:
                break;
            }


        }




    }

}
