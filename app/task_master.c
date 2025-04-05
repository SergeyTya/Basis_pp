#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mbsupport.h"
#include "task_master.h"
#include "mb.h"
#include "mbcrc.h"
#include "panelConfig.h"

extern Typedef_PanelConfig panelConfig;

extern void master_hwInit(uint16_t speed);
extern void master_hwRead(uint8_t *buf, size_t len);
extern void master_hwWrite(uint8_t *buf, size_t len);
extern void master_hwFault(int dev);
extern void master_hwStart(int dev);
extern void master_hwStop(int dev);
extern void master_hwTimeOut(int dev);
extern void master_hwClearRxTxBuf();
extern int  master_hwBytesToRead();

TypedefEnum_MasterSates master_writeHolding(uint8_t slave, uint16_t adr, uint16_t val);
TypedefEnum_MasterSates master_readHolding(uint8_t slave, uint16_t adr, uint16_t *out);
TypedefEnum_MasterSates master_readHoldings(uint8_t slave, uint16_t adr, uint16_t len, uint16_t *buff);

static uint8_t masterRxTxBuf[256] = {0};
static uint8_t masterRxBuf[256] = {0};

static inline int checkCode(TypeDef_MB_Holding *holding, uint16_t slaveAdr, int mask);

void vTask_Master(__attribute__((unused)) void *argument)
{
    master_hwInit(panelConfig.modbus_master.speed);

    vTaskDelay(3000);
    TypeDef_MB_Holding *holding;

    uint16_t tmpFaultReg = 0;

    while (1)
    {
        for (size_t j = 1; j < 5; j++)
        {
            uint16_t slaveAdr = j;
            TypeDef_MB_Table *table = &holdings_table[j];

            bool slave_enable[5] = {false,
                                    panelConfig.enableAC1,
                                    panelConfig.enableAC2,
                                    panelConfig.enableDC1,
                                    panelConfig.enableDC2};

            if (slave_enable[j] != true)
            { // skip not enabled slave
                vTaskDelay(1);
                continue;
            }

            // read slaves status
            if (slaveAdr == CONFIG_SLAVE_AC1 || slaveAdr == CONFIG_SLAVE_AC2)
            {
                holding = GetHoldingByAdrFromTable(140, table);
                master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr);
                checkCode(holding, slaveAdr, 12);
                holding = GetHoldingByAdrFromTable(141, table);
                master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr);
                checkCode(holding, slaveAdr, 11);
            }

            if (slaveAdr == CONFIG_SLAVE_DC1 || slaveAdr == CONFIG_SLAVE_DC2)
            {
                holding = GetHoldingByAdrFromTable(220, table);
                master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, holding->pntr); // holding->pntr);
                if( (*holding->pntr & 0x4)!= 0){ // error flag
                    holding = GetHoldingByAdrFromTable(240, table);
                    master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, &tmpFaultReg); // holding->pntr);
                    checkCode(holding, slaveAdr, 8);
                    master_hwFault(slaveAdr);
                } else{
                    panelConfig.fault_source[slaveAdr] = false;
                }

                if( (*holding->pntr & 0x2)!= 0){ // ready flag
                    if(panelConfig.start_req[slaveAdr] == true){
                        panelConfig.start_req[slaveAdr]=false;
                        //send start;
                        holding = GetHoldingByAdrFromTable(104, table);
                        uint16_t valStart = *holding->pntr | 0x1;
                        master_writeHolding( slaveAdr ,holding->reg_adr&0x0FFF, valStart);
                        master_hwStart(slaveAdr);
                    }
                } 

                if( (*holding->pntr & 0x1)!= 0){ // run flag
                    if(panelConfig.start_req[slaveAdr] == true){
                        panelConfig.start_req[slaveAdr]=false;
                        //send stop;
                        holding = GetHoldingByAdrFromTable(104, table);
                        uint16_t valStart = *holding->pntr & (~0x1U);
                        master_writeHolding( slaveAdr ,holding->reg_adr&0x0FFF, valStart);
                        master_hwStop(slaveAdr);
                    }
                } 

            }

            if (slaveAdr != panelConfig.active_slave)
            { // skip not active slave
                vTaskDelay(1);
                continue;
            }

            for (size_t i = 0; i < table->len; i++)
            {
                holding = &table->holdings[i];

                if (holding->lock && holding->change_req)
                {

                    // send holding value to slave
                    master_writeHolding( slaveAdr ,holding->reg_adr&0x0FFF, *holding->pntr);
                    holding->lock = false;
                    holding->change_req = false;
                    vTaskDelay(300);
                }

                // read holding value to slave
                uint16_t out = 0;
                TypedefEnum_MasterSates res = master_readHolding(slaveAdr, holding->reg_adr & 0x0FFF, &out);
                if (res == MASTERSTATE_NOERROR)
                {
                    if (holding->lock == false)
                    {
                        *holding->pntr = out;
                    }

                    //feed master wdg
                    panelConfig.master_wdg[slaveAdr] = false;
                }else if(res == MASTERSTATE_TIMEOUT){
                    master_hwTimeOut(slaveAdr);
                }
            }

            // TODO MASTER ERROR HANDLER
        }
    }
}

TypedefEnum_MasterSates master_writeHolding(uint8_t slave, uint16_t adr, uint16_t val)
{
    masterRxTxBuf[0] = slave;
    masterRxTxBuf[1] = 0x10;
    masterRxTxBuf[2] = ((uint8_t *)&adr)[1];
    masterRxTxBuf[3] = ((uint8_t *)&adr)[0];
    masterRxTxBuf[4] = 0;
    masterRxTxBuf[5] = 1;
    masterRxTxBuf[6] = 2;
    masterRxTxBuf[7] = ((uint8_t *)&val)[1];
    masterRxTxBuf[8] = ((uint8_t *)&val)[0];
    uint16_t crc = usMBCRC16(masterRxTxBuf, 9);
    masterRxTxBuf[9] = ((uint8_t *)&crc)[0];
    masterRxTxBuf[10] = ((uint8_t *)&crc)[1];

    master_hwWrite(masterRxTxBuf, 8);

    uint16_t expectedSize = 8;

    for (size_t i = 0; i < 300; i++)
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if(bytesToRead == expectedSize){
            master_hwBytesToRead(masterRxTxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxBuf, expectedSize - 2);
            uint16_t crcReturned = (uint16_t)(masterRxBuf[expectedSize - 1] << 8 | masterRxBuf[expectedSize - 2]);
            if(crcExpected == crcReturned){
                return MASTERSTATE_NOERROR;
            }else{
                return MASTERSTATE_CRCERROR;
            }
        }else{
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
                vTaskDelay(10);
                return MASTERSTATE_CRCERROR;
            }
            vTaskDelay(1);
        }
        vTaskDelay(1);
    }

    return MASTERSTATE_TIMEOUT;
}

TypedefEnum_MasterSates master_readHolding(uint8_t slave, uint16_t adr, uint16_t *out)
{
    TypedefEnum_MasterSates retVal = master_readHoldings(slave, adr, 1, out);
    return retVal;
}

TypedefEnum_MasterSates master_readHoldings(uint8_t slave, uint16_t adr, uint16_t len, uint16_t *buff)
{
    masterRxTxBuf[0] = slave;
    masterRxTxBuf[1] = 0x03;
    masterRxTxBuf[2] = ((uint8_t *)&adr)[1];
    masterRxTxBuf[3] = ((uint8_t *)&adr)[0];
    masterRxTxBuf[4] = ((uint8_t *)&len)[1];
    masterRxTxBuf[5] = ((uint8_t *)&len)[0];
    uint16_t crc = usMBCRC16(masterRxTxBuf, 6);
    masterRxTxBuf[6] = ((uint8_t *)&crc)[0];
    masterRxTxBuf[7] = ((uint8_t *)&crc)[1];

    // master_hwClearRxTxBuf();

    master_hwWrite(masterRxTxBuf, 8);
    vTaskDelay(1);

    uint16_t expectedSize = 5 + len * 2;

    for (size_t i = 0; i < 100; i++)
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
                    buff[i] = ((uint16_t *)&masterRxBuf[3])[i];
                }
                return MASTERSTATE_NOERROR;
            }
        }
        else
        {
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
                vTaskDelay(10);
                return MASTERSTATE_CRCERROR;
            }
            vTaskDelay(1);
        }
    }

    return MASTERSTATE_TIMEOUT;
}

static inline int checkCode(TypeDef_MB_Holding *holding, uint16_t slaveAdr, int mask)
{
    uint8_t faultCode = (*holding->pntr) & 0x00FF;
    if (faultCode > 0)
    {
        for (int i = 0; i < mask; i++)
        { // fault code is bit number
            uint8_t bit = (faultCode & (1 << i)) >> i;
            if (bit == 1)
            {
                panelConfig.fault_source[slaveAdr] = true;
                panelConfig.fault_code[slaveAdr] = i + 1;
                return panelConfig.fault_code[slaveAdr];
            }
        }
    }
}