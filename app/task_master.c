#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mbsupport.h"
#include "task_master.h"
#include "mb.h"
#include "mbcrc.h"
#include "panelConfig.h"



extern Typedef_PanelConfig panelConfig;
TypeDef_Master master;

extern void master_hwInit(uint16_t speed);
extern void master_hwRead(uint8_t* buf, size_t len);
extern void master_hwWrite(uint8_t* buf, size_t len);
extern void master_hwFault(int dev);
extern void master_hwStart(int dev);
extern void master_hwStop(int dev);
extern void master_hwTimeOut(int dev);
void master_hwFaultReset();
extern void master_hwBlink(int dev);
extern void master_hwClearRxTxBuf();
extern int  master_hwBytesToRead();
extern void master_expectedByteCnt(int size);

TypedefEnum_MasterTransportSates master_writeHolding(uint8_t slave, uint16_t adr, uint16_t val);
TypedefEnum_MasterTransportSates master_readHolding(uint8_t slave, uint16_t adr, uint16_t* out);
TypedefEnum_MasterTransportSates master_readHoldings(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff);

static uint8_t masterRxTxBuf[256] = { 0 };
static uint8_t masterRxBuf[256] = { 0 };

//Timers used for 10 sec delay before start
TimerHandle_t xTimers_StartDelay[5];

// We are using semaphore to protect transport from multiple tasks from delay timers
xSemaphoreHandle xDisplayMasterR485Semaphore; 

// Used for encode slave fault bits
static inline int checkCode(TypeDef_MB_Holding* holding, uint16_t slaveAdr, int mask);

#define MASTER_TRANSPORT_CHECK_TIMEOUT( xstate ) {\
        master.master_wdg[slaveAdr] = (xstate) != MASTER_TRANSPORT_NOERROR; \
};

// Delay timers callbacks
void vTimerAC_Callback(TimerHandle_t xTimer) {

    xSemaphoreTake(xDisplayMasterR485Semaphore, portMAX_DELAY);

    for (size_t i = 1; i < 5; i++) {
        uint16_t slaveAdr = i;
        if (xTimer == xTimers_StartDelay[i]) {
            TypeDef_MB_Holding* holding;
            TypeDef_MB_Table* table = &holdings_table[slaveAdr];
            // get control register pointer
            holding = GetHoldingByAdrFromTable(103, table);
            // Setup 
            uint16_t valACStart = 0xF9; // bit #3
            master_writeHolding(slaveAdr, holding->reg_adr & 0x0FFF, valACStart);
            master_hwStart(slaveAdr);
            // reset start request
            master.start_req[slaveAdr] = false;
            break;
        }
    }

    xSemaphoreGive(xDisplayMasterR485Semaphore);
}

void vTimerDC_Callback(TimerHandle_t xTimer) {
    
    xSemaphoreTake(xDisplayMasterR485Semaphore, portMAX_DELAY);

    for (size_t i = 1; i < 5; i++) {
        uint16_t slaveAdr = i;
        if (xTimer == xTimers_StartDelay[i]) {
            TypeDef_MB_Holding* holding;
            TypeDef_MB_Table* table = &holdings_table[slaveAdr];
            master.start_req[slaveAdr] = false;
                        //send start;
            holding = GetHoldingByAdrFromTable(104, table);
            uint16_t valStart = *holding->pntr | 0x1;
            master_writeHolding(slaveAdr, holding->reg_adr & 0x0FFF, valStart);
            master_hwStart(slaveAdr);
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
    xTimers_StartDelay[2] =  xTimerCreate("", 10000, pdFALSE, NULL, vTimerAC_Callback);
    xTimers_StartDelay[3] =   xTimerCreate("", 10000, pdFALSE, NULL, vTimerDC_Callback);
    xTimers_StartDelay[4] =    xTimerCreate("", 10000, pdFALSE, NULL, vTimerDC_Callback);

    while (1){
         
        TypeDef_MB_Holding* holding;  
        
        
        // /// test
        // xSemaphoreTake(xDisplayMasterR485Semaphore, portMAX_DELAY);
        // // Reset fault LED
        // master_hwFaultReset();

        //  vTaskDelay(50); 

        //   TypeDef_MB_Table* table1 = &holdings_table[3];
        //   holding = GetHoldingByAdrFromTable(reg_adr_tmp, table1);
                
        //   reg_val_tmp = master_readHolding(3, holding->reg_adr & 0x0FFF, holding->pntr);

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
                holding = GetHoldingByAdrFromTable(140, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                );
                // check faults
                checkCode(holding, slaveAdr, 12);
                holding = GetHoldingByAdrFromTable(141, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                );
                // check faults
                checkCode(holding, slaveAdr, 11);

                if (master.slaveStates[slaveAdr] != MASTER_STATE_onFAULT) {
                    // Read control register
                    holding = GetHoldingByAdrFromTable(103, table);
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                    )
                    uint16_t ac_slave_CR = *holding->pntr;

                    //Ready state
                    if (ac_slave_CR & 0x8) {
                        master.slaveStates[slaveAdr] = MASTER_STATE_onREADY;
                        if (master.start_req[slaveAdr] == true) { // Start request
                            if (xTimerIsTimerActive(xTimers_StartDelay[slaveAdr]) == pdFALSE)
                            { // Timer not started
                                xTimerStart(xTimers_StartDelay[slaveAdr], 0);
                            }else{
                                master_hwBlink(slaveAdr);
                            }
                        }
                    }
                    else {
                        //Run state
                        master.slaveStates[slaveAdr] = MASTER_STATE_onRUN;
                        if (master.start_req[slaveAdr] == true) { // Stop request
                            master.start_req[slaveAdr] = false;
                            master_hwStop(slaveAdr);
                            uint16_t valACStop = 0xE9; // bit #3
                            MASTER_TRANSPORT_CHECK_TIMEOUT(
                                master_writeHolding(slaveAdr, holding->reg_adr & 0x0FFF, valACStop)
                            ); 
                            master_hwStart(slaveAdr);
                        }
                    }
                }
            }
            /*  Read status and control DC slaves */
            if (slaveAdr == CONFIG_SLAVE_DC1 || slaveAdr == CONFIG_SLAVE_DC2)
            {
                // Read status register
                holding = GetHoldingByAdrFromTable(220, table);
                MASTER_TRANSPORT_CHECK_TIMEOUT(
                    master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                );
                
                if ((*holding->pntr & 0x4) != 0) { // error flag
                    // Read Fault code register 
                    holding = GetHoldingByAdrFromTable(240, table);
                    MASTER_TRANSPORT_CHECK_TIMEOUT(
                        master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr)
                    );
                    checkCode(holding, slaveAdr, 8); // Set slave onFAULT state here!
                    master_hwFault(slaveAdr);
                    // stops active start requests
                    master.start_req[slaveAdr] = false;
                }
                else {
                    master.fault_source[slaveAdr] = false;

                }
                /* Handle DC start|stop req */
                if ((*holding->pntr & 0x2) != 0) { // ready flag
                    master.slaveStates[slaveAdr] = MASTER_STATE_onREADY;
                    // SLAVE START REQUEST
                    if (master.start_req[slaveAdr] == true) {
                        if (xTimerIsTimerActive(xTimers_StartDelay[slaveAdr]) == pdFALSE)
                        { // Timer not started
                            xTimerStart(xTimers_StartDelay[slaveAdr], 0);
                        }else{
                             master_hwBlink(slaveAdr);
                        }
                    }
                }

                if ((*holding->pntr & 0x1) != 0) { // run flag
                    master.slaveStates[slaveAdr] = MASTER_STATE_onFAULT;
                    // SLAVE START STOP
                    if (master.start_req[slaveAdr] == true) {
                        master.start_req[slaveAdr] = false;
                        //send stop;
                        holding = GetHoldingByAdrFromTable(104, table);
                        uint16_t valStart = *holding->pntr & (~0x1U);
                        MASTER_TRANSPORT_CHECK_TIMEOUT(
                             master_writeHolding(slaveAdr, holding->reg_adr & 0x0FFF, valStart)
                        )
                        master_hwStop(slaveAdr);
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
                        master_writeHolding(slaveAdr, holding->reg_adr & 0x0FFF, *holding->pntr)
                    );
                    holding->lock = false;
                    holding->change_req = false;
                    vTaskDelay(10); // Delay after write
                }

                // read holding value to slave
                uint16_t out = 0;
                TypedefEnum_MasterTransportSates res = master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, &out);
                if (res == MASTER_TRANSPORT_NOERROR)
                {
                    if (holding->lock == false)
                    {
                        *holding->pntr = out;
                    }
                    //feed master wdg
                    master.master_wdg[slaveAdr] = false;
                }else{
                    master.master_wdg[slaveAdr] = true;
                    tocntr++;
                    master_hwTimeOut(slaveAdr);
                    if(tocntr>3) break;
                }
            }
            // TODO MASTER ERROR HANDLER
        }
        xSemaphoreGive(xDisplayMasterR485Semaphore);
    }
}

/**
* @brief Function for writing a value to a slave holding register
*
* @param slave Node address
* @param adr Register address
* @param val Value to write
* @return Transport state
*/
TypedefEnum_MasterTransportSates master_writeHolding(uint8_t slave, uint16_t adr, uint16_t val)
{
    masterRxTxBuf[0] = slave;
    masterRxTxBuf[1] = 0x06;
    masterRxTxBuf[2] = ((uint8_t*)&adr)[1];
    masterRxTxBuf[3] = ((uint8_t*)&adr)[0];
    masterRxTxBuf[4] = ((uint8_t*)&val)[1];
    masterRxTxBuf[5] = ((uint8_t*)&val)[0];
    uint16_t crc = usMBCRC16(masterRxTxBuf, 6);
    masterRxTxBuf[6] = ((uint8_t*)&crc)[0];
    masterRxTxBuf[7] = ((uint8_t*)&crc)[1];

    master_hwWrite(masterRxTxBuf, 8);

    uint16_t expectedSize = 8;

    master_expectedByteCnt(expectedSize);

    for (size_t i = 0; i < 10; i++)
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if (bytesToRead == expectedSize) {
            master_hwBytesToRead(masterRxTxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxBuf, expectedSize - 2);
            uint16_t crcReturned = (uint16_t)(masterRxBuf[expectedSize - 1] << 8 | masterRxBuf[expectedSize - 2]);
            if (crcExpected == crcReturned) {
                return MASTER_TRANSPORT_NOERROR;
            }
            else {
                return MASTERS_TRANSPORT_CRCERROR;
            }
        }
        else {
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
                vTaskDelay(10);
                return MASTERS_TRANSPORT_CRCERROR;
            }
            vTaskDelay(1);
        }
        vTaskDelay(10);
    }

    return MASTER_TRANSPORT_TIMEOUT;
}

/** TODO!!!
* @brief Function for writing a value to a slave holding register
*
* @param slave Node address
* @param adr Register address
* @param val Value to write
* @return Transport state
*/
TypedefEnum_MasterTransportSates master_writeHoldings(uint8_t slave, uint16_t adr, uint16_t val)
{
    masterRxTxBuf[0] = slave;
    masterRxTxBuf[1] = 0x10;
    masterRxTxBuf[2] = ((uint8_t*)&adr)[1];
    masterRxTxBuf[3] = ((uint8_t*)&adr)[0];
    masterRxTxBuf[4] = 0;
    masterRxTxBuf[5] = 1;
    masterRxTxBuf[6] = 2;
    masterRxTxBuf[7] = ((uint8_t*)&val)[1];
    masterRxTxBuf[8] = ((uint8_t*)&val)[0];
    uint16_t crc = usMBCRC16(masterRxTxBuf, 9);
    masterRxTxBuf[9] = ((uint8_t*)&crc)[0];
    masterRxTxBuf[10] = ((uint8_t*)&crc)[1];

    master_hwWrite(masterRxTxBuf, 8);

    uint16_t expectedSize = 8;

    master_expectedByteCnt(expectedSize);

    for (size_t i = 0; i < 10; i++)
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if (bytesToRead == expectedSize) {
            master_hwBytesToRead(masterRxTxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxBuf, expectedSize - 2);
            uint16_t crcReturned = (uint16_t)(masterRxBuf[expectedSize - 1] << 8 | masterRxBuf[expectedSize - 2]);
            if (crcExpected == crcReturned) {
                return MASTER_TRANSPORT_NOERROR;
            }
            else {
                return MASTERS_TRANSPORT_CRCERROR;
            }
        }
        else {
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
                vTaskDelay(10);
                return MASTERS_TRANSPORT_CRCERROR;
            }
            vTaskDelay(1);
        }
        vTaskDelay(10);
    }

    return MASTER_TRANSPORT_TIMEOUT;
}

TypedefEnum_MasterTransportSates master_readHolding(uint8_t slave, uint16_t adr, uint16_t* out)
{
    TypedefEnum_MasterTransportSates retVal = master_readHoldings(slave, adr, 1, out);
    return retVal;
}

int timeout_err[5] = {0};
int crc_err[5] = {0};
int slave_rx_cnt[5] = {0};
int to_adr = 0;

TypedefEnum_MasterTransportSates master_readHoldings(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff)
{
    masterRxTxBuf[0] = slave;
    masterRxTxBuf[1] = 0x03;
    masterRxTxBuf[2] = ((uint8_t*)&adr)[1];
    masterRxTxBuf[3] = ((uint8_t*)&adr)[0];
    masterRxTxBuf[4] = ((uint8_t*)&len)[1];
    masterRxTxBuf[5] = ((uint8_t*)&len)[0];
    uint16_t crc = usMBCRC16(masterRxTxBuf, 6);
    masterRxTxBuf[6] = ((uint8_t*)&crc)[0];
    masterRxTxBuf[7] = ((uint8_t*)&crc)[1];

    // master_hwClearRxTxBuf();

    master_hwWrite(masterRxTxBuf, 8);
    uint16_t expectedSize = 5 + len * 2;
    master_expectedByteCnt(expectedSize);
    // vTaskDelay(1);

    for (size_t i = 0; i < 30; i++) // Timeout
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if (bytesToRead == expectedSize)
        {
            master_hwRead(masterRxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxBuf, expectedSize - 2);
            uint16_t crcReturned = (uint16_t)(masterRxBuf[expectedSize - 1] << 8 | masterRxBuf[expectedSize - 2]);
            if (crcExpected == crcReturned)
            {
                for (uint16_t i = 0; i < len; i++)
                {
                   // get value from buffer
                   uint16_t val = ((uint16_t*)&masterRxBuf[3])[i];
                   // swap bytes
                   buff[i] = ((uint8_t *) &val)[1] + (((uint8_t *) &val)[0] << 8);
                }
                slave_rx_cnt[slave]++;
                return MASTER_TRANSPORT_NOERROR;
            }
        }
        else
        {
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
                vTaskDelay(10);
                crc_err[slave]++;
                return MASTERS_TRANSPORT_CRCERROR;
            }
        }
        vTaskDelay(10);
    }

    timeout_err[slave]++;
    to_adr = adr;
    return MASTER_TRANSPORT_TIMEOUT;
}

static inline int checkCode(TypeDef_MB_Holding* holding, uint16_t slaveAdr, int mask)
{
    uint8_t faultCode = (*holding->pntr) & 0x00FF;
    if (faultCode > 0)
    {
        for (int i = 0; i < mask; i++)
        { // fault code is bit number
            uint8_t bit = (faultCode & (1 << i)) >> i;
            if (bit == 1)
            {
                master.slaveStates[slaveAdr] = MASTER_STATE_onFAULT;
                master.fault_source[slaveAdr] = true;
                master.fault_code[slaveAdr] = i + 1;
                return master.fault_code[slaveAdr];
            }
        }
    }
}
