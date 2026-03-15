#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mbsupport.h"
#include "task_master.h"

#include "panelConfig.h"
#include "masterTransport.h"

#include "timers.h"

#include "meter.h"
#include "clock.h"



TypeDef_Master master;

extern Typedef_PanelConfig panelConfig;
extern Typedef_Meter meter;
extern Typedef_Clock clock;
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
        TypedefEnum_MasterTransportSates res = (xstate) ; \
       master.master_wdg[slaveAdr] = (res == MASTER_TRANSPORT_TIMEOUT); \
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

    // for (size_t i = 1; i < 5; i++) {
    //     uint16_t slaveAdr = i;
    //     if (xTimer == xTimers_StartDelay[i]) {
    //         TypeDef_MB_Holding* holding;
    //         TypeDef_MB_Table* table = &holdings_table[slaveAdr];
    //         master.start_req_hw[slaveAdr] = false;
    //         //send start;
    //         holding = GetHoldingByAdrFromTable(104, table);
    //         uint16_t valStart = *holding->pntr | 0x2;
    //         master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valStart);
    //     }
    // }
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

        // master_LEDonWaitForAcOkState(1);

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
            vTaskDelay(10); // dalay between slaves
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

            // Reset start req if TO
            if( master.slaveStates[slaveAdr] == MASTER_STATE_onTIMEOUT){
                master.start_req_hw[slaveAdr]  = false;
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
                                master.slaveStates[2] = MASTER_STATE_onREADY;

                                master.start_req_rdo[slaveAdr] = false; // always reset rdo start req in ready state
                                if(master.start_req_hw[slaveAdr]){
                                    // send start
                                    holding = GetHoldingByAdrFromTable(103, table);
                                    uint16_t valACStart = *holding->pntr | 8U; // bit #3
                                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                                        master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, valACStart)
                                    );
                                    master.start_req_hw[slaveAdr] = false;

                                    /* Use this to control two channel by dc1 buttons */
                                    // master.start_req_hw[2] = false;
                                    // master.start_req_rdo[2] = false;
                                    // master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_AC_OK;
                                    // master.slaveStates[2]        = MASTER_STATE_onWAIT_FOR_AC_OK;

                                    master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_AC_OK;
                                }
                            }else{
                                 // Here we check if output voltage reach reference

                                if(master.start_req_rdo[slaveAdr]){
                                        if( master.slaveStates[slaveAdr] == MASTER_STATE_onWAIT_FOR_AC_OK){

                                             uint16_t Uacr[3]; uint16_t ref_ra;

                                            master_readHoldingOs(slaveAdr, 240, &Uacr[0]);
                                            master_readHoldingOs(slaveAdr, 241, &Uacr[1]);
                                            master_readHoldingOs(slaveAdr, 242, &Uacr[2]);
                                            master_readHoldingOs(slaveAdr, 102, &ref_ra);

                                            bool reached = (Uacr[0] >= ref_ra) && (Uacr[1] >= ref_ra) &&  (Uacr[2] >= ref_ra);

                                            // if output voltage is good
                                            if(reached){
                                                // Enable contactor RDO
                                                 master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_MAIN_RELAY;
                                            }
                                        }

                                        master.start_req_rdo[slaveAdr] = false;
                                }

                                // check main contactor feedback
                                if(master.slaveStates[slaveAdr] == MASTER_STATE_onWAIT_FOR_MAIN_RELAY){
                                    //read HR 220
                                    uint16_t acreg220 = 0;
                                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                                        master_readHoldingOs(slaveAdr, 220, &acreg220)
                                    );

                                    if((acreg220 & 0x2) != 0){
                                         master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                                    }
                                }

                                if( // Timeout state
                                        (master.slaveStates[slaveAdr]  == MASTER_STATE_onTIMEOUT)
                                ){
                                     uint16_t acreg220_2 = 0;
                                    // Return after timeout
                                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                                        master_readHoldingOs(slaveAdr, 220, &acreg220_2)
                                    );

                                    if((acreg220_2 & 0x2) != 0){
                                        master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                                    }else{
                                        master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_AC_OK;
                                    }

                                }
                                

                                // Сделать обратный порядок выключения  краткое нажатие - контактор, длинное инвертор

                                /* Use this to control two channel by dc1 buttons */
                                // if(master.start_req_rdo[slaveAdr]){
                                //         if( master.slaveStates[slaveAdr] == MASTER_STATE_onWAIT_FOR_STOP){
                                //             if(reached) {
                                //                 master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                                //             };

                                //         }else{
                                //              master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_AC_OK;
                                //         }
                                //         master.start_req_rdo[2] = false;
                                // }

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
                        master.start_req_hw[slaveAdr]  = false;
                        master.start_req_rdo[slaveAdr] = false;
                        
                        /* Use this to control two channel by dc1 buttons */
                        // master.start_req_rdo[2] = false;
                        // master.start_req_hw[2] = false;
                        // master.slaveStates[2]          = MASTER_STATE_onFAULT;
                }

            }
            /*  Read status and control DC slaves */
            if (slaveAdr == CONFIG_SLAVE_DC1 || slaveAdr == CONFIG_SLAVE_DC2)
            {
                // Read status register
                holding = GetHoldingByAdrFromTable(220, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHoldingOs(slaveAdr,  220, holding->pntr)
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
                    else 
                    if ((*holding->pntr & 0x1) != 0) { //DC RUN STATE (in RUN state bow bit RUN and RDY are 1)

                        if(master.slaveStates[slaveAdr] == MASTER_STATE_onWAIT_FOR_AC_OK){
                            // wait for output good

                            uint16_t Uo = 0;
                            uint16_t Ur = 0;

                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_readHoldingOs(slaveAdr, 211, &Uo)
                            );


                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_readHoldingOs(slaveAdr, 102, &Ur)
                            );

                            if(Uo>Ur*8/10) {
                                master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                            }
                            
                        }
                        
                        // Timeout state
                        if (master.slaveStates[slaveAdr]  == MASTER_STATE_onTIMEOUT){
                            master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_AC_OK;
                        }


                      //  master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                        // DC onRun state
                       
                        // SLAVE START STOP
                        if (master.start_req_hw[slaveAdr] == true) {
                            //send stop;
                            uint16_t valStop = 0;

                            master_readHoldingOs(slaveAdr, 104, &valStop);

                            valStop = valStop & ~(1U << 1); 
                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_writeHoldingOs(slaveAdr, 104, valStop)
                            );
                            master.start_req_hw[slaveAdr] = false;

                            vTaskDelay(10);
                        }
                    } else

                    /* Handle DC start|stop req */
                    if ((*holding->pntr & 0x2) != 0) { //DC READY STATE

                        master.fault_source[slaveAdr] = false;
                        // DC onReady state
                        master.slaveStates[slaveAdr] = MASTER_STATE_onREADY;
                        // SLAVE START REQUEST
                        if (master.start_req_hw[slaveAdr] == true) {
                               /**
                                Нажимаем в течение 3-4 секунд кнопку DC, 
                                светодиод DC начинает моргать и продолжает так делать, 
                                пока выходное напряжение не будет равно заданному. 
                                После этого постоянно светит и также в этот момент загорается светодиод ВКЛ.
                            */

                            uint16_t valStart = 0;     
                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_readHoldingOs(slaveAdr, 104, &valStart)
                            );

                            valStart =  valStart | 0x2;  
                            master_writeHoldingOs(slaveAdr, 104, valStart);
                            master.slaveStates[slaveAdr] = MASTER_STATE_onWAIT_FOR_AC_OK;
                            master.start_req_hw[slaveAdr] = false;
                            vTaskDelay(100);
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

                // Write HR to slave
                if (holding->lock && holding->change_req)
                {

                    /* Send holding value to slave */
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_writeHoldingOs(slaveAdr, holding->reg_adr & 0x0FFF, *holding->pntr)
                    );
                    holding->lock = false;
                    holding->change_req = false;
                    vTaskDelay(10); // Delay after write
                    
                    // save to AC slave memory
                    if(slaveAdr == CONFIG_SLAVE_AC1 || slaveAdr == CONFIG_SLAVE_AC2){
                        uint16_t reg900CurVal = 0;
                        // read save control HR adr=900
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_readHoldingOs(slaveAdr, 900, &reg900CurVal)
                        );
                        // modify bit 1
                        reg900CurVal |= (1<<0);
                        // send back
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_writeHoldingOs(slaveAdr, 900, reg900CurVal)
                        );
                    }

                    // save to DC slave memory
                    if(slaveAdr == CONFIG_SLAVE_DC1 || slaveAdr == CONFIG_SLAVE_DC2){
                        uint16_t reg170CurVal = 0;
                        // read save control HR adr=900
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_readHoldingOs(slaveAdr, 170, &reg170CurVal)
                        );
                        // modify bit 1
                        reg170CurVal |= (1<<0);
                        // send back
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                            master_writeHoldingOs(slaveAdr, 170, reg170CurVal)
                        );
                    }
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


        // Read Meter
        if(meter.enable){
            meter.read(&meter); 
        }

         // Read Clock
        if(clock.enable){
             clock.read(&clock);  
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

bool blinker = false;
static uint16_t blinker_cntr = 0;
void vTask_MasterHWstates(void* argument) {

    bool onGlobalFault = false;
    bool onGlobalTimeout = false;
    bool onGlobalRUN = false;
    while (1) {

        if (blinker_cntr == 0) {
        blinker_cntr = 3;
        blinker = !blinker;
    } else {
        blinker_cntr--;
    //return;
    }

        vTaskDelay(100);

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
            case MASTER_STATE_onWAIT_FOR_AC_OK:
                master_LEDonWaitForAcOkState(i);
                break;
            case MASTER_STATE_onWAIT_FOR_MAIN_RELAY:
                master_LEDonWaitForMainRelayState(i);
                break;

            default:
                break;
            }


        }




    }

}
