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
extern void master_hwRead(uint8_t * buf, size_t len);
extern void master_hwWrite(uint8_t * buf, size_t len);
extern void master_hwClearRxTxBuf();
extern int  master_hwBytesToRead();


TypedefEnum_MasterSates master_writeHolding(uint8_t slave, uint16_t adr, uint16_t val);
TypedefEnum_MasterSates master_readHolding(uint8_t slave,uint16_t adr, uint16_t * out);
TypedefEnum_MasterSates master_readHoldings(uint8_t slave,uint16_t adr, uint16_t len, uint16_t * buff);

static uint8_t masterRxTxBuf[256] = {0};

void vTask_Master(__attribute__((unused)) void *argument){

    master_hwInit(panelConfig.modbus_master.speed);

    vTaskDelay(3000);
    
    while(1){

        for (size_t j = 1; j < 5; j++)
        {
            TypeDef_MB_Table * table = &holdings_table[j];

            bool slave_states[5] = {false, panelConfig.enableAC1, panelConfig.enableAC2, panelConfig.enableDC1, panelConfig.enableDC2};

            if(slave_states[j] != true) continue;

            uint16_t slaveAdr = j;

            for (size_t i = 0; i < table->len; i++) 
            { 
                TypeDef_MB_Holding * holding = &table->holdings[i];
                
                if(holding->lock && holding->change_req){
    
                    //send holding value to slave 
                    TypedefEnum_MasterSates res = master_writeHolding( slaveAdr ,holding->reg_adr&0x0FFF, *holding->pntr);
                    
                    holding->lock = false;
                    holding->change_req = false;
                    vTaskDelay(10);
                }

                //read holding value to slave 
                uint16_t out = 0;
                TypedefEnum_MasterSates res = master_readHolding( slaveAdr ,holding->reg_adr&0x0FFF, &out);
                if(res==MASTERSTATE_NOERROR){
                   *holding->pntr = out;
                }
            }

            // TODO MASTER ERROR HANDLER
        }

        vTaskDelay(1000);
    }

}

TypedefEnum_MasterSates     master_writeHolding(uint8_t slave,uint16_t adr, uint16_t val){
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
            uint16_t crcExpected = usMBCRC16(masterRxTxBuf, expectedSize - 2);
            uint16_t crcReturned = ( uint16_t )( masterRxTxBuf[expectedSize-2] << 8 | masterRxTxBuf[expectedSize-1] );
            if(crcExpected == crcReturned){
                return MASTERSTATE_NOERROR;
            }else{
                return MASTERSTATE_CRCERROR;
            }
        }
        vTaskDelay(1);
    }
    
    return MASTERSTATE_TIMEOUT;
}
TypedefEnum_MasterSates master_readHolding(uint8_t slave,uint16_t adr, uint16_t * out){
    TypedefEnum_MasterSates retVal = master_readHoldings(slave, adr, 1, out);
    vTaskDelay(1000);
    return retVal;
}

TypedefEnum_MasterSates master_readHoldings(uint8_t slave,uint16_t adr, uint16_t len, uint16_t * buff){
    masterRxTxBuf[0] = slave;
    masterRxTxBuf[1] = 0x03;
    masterRxTxBuf[2] = ((uint8_t *)&adr)[1];
    masterRxTxBuf[3] = ((uint8_t *)&adr)[0];
    masterRxTxBuf[4] = ((uint8_t *)&len)[1];
    masterRxTxBuf[5] = ((uint8_t *)&len)[0];
    uint16_t crc = usMBCRC16(masterRxTxBuf, 6);
    masterRxTxBuf[6] = ((uint8_t *)&crc)[0];
    masterRxTxBuf[7] = ((uint8_t *)&crc)[1];

    master_hwWrite(masterRxTxBuf, 8);

    uint16_t expectedSize = 5 + len*2;

    for (size_t i = 0; i < 300; i++)
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if(bytesToRead == expectedSize){
            master_hwBytesToRead(masterRxTxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxTxBuf, expectedSize - 2);
            uint16_t crcReturned = ( uint16_t )( masterRxTxBuf[expectedSize-2] << 8 | masterRxTxBuf[expectedSize-1] );
            if(crcExpected == crcReturned){
                for (uint16_t i = 0; i < len; i++)
                {
                    buff[i] = (( uint16_t *) &masterRxTxBuf[3])[i];
                }
                return MASTERSTATE_NOERROR;
            }else{
                return MASTERSTATE_CRCERROR;
            }
        }
        vTaskDelay(1);
    }
    
    return MASTERSTATE_TIMEOUT;
}


