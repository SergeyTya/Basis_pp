#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "mb.h"
#include "mbcrc.h"
#include "masterTransport.h"


static uint8_t masterRxTxBuf[256] = { 0 };
static uint8_t masterRxBuf[256] = { 0 };

SemaphoreHandle_t semaphore_MT;
//#define MASTER_TRANSPORT_TIMEOUT_VAL 30 
#define MASTER_TRANSPORT_WAIT() {vTaskDelay(1); }

void master_transport_init(){
    semaphore_MT = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore_MT); 
    MASTER_TRANSPORT_LOCK_GIVE();
}

/**
* @brief Function for writing a value to a slave holding register
*
* @param slave Node address
* @param adr Register address
* @param val Value to write
* @return Transport state
*/
TypedefEnum_MasterTransportSates master_writeHoldingOs(uint8_t slave, uint16_t adr, uint16_t val, int timeout)
{
    MASTER_TRANSPORT_LOCK_TAKE();
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
    if(timeout<=0)timeout=1;
    for (size_t i = 0; i < timeout; i++) // Timeout
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if (bytesToRead == expectedSize) {
            master_hwBytesToRead(masterRxTxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxBuf, expectedSize - 2);
            uint16_t crcReturned = (uint16_t)(masterRxBuf[expectedSize - 1] << 8 | masterRxBuf[expectedSize - 2]);
            if (crcExpected == crcReturned) {
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_NOERROR;
            }
            else {
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_CRCERROR;
            }
        }
        else {
            if (bytesToRead > expectedSize)
            {
                master_hwClearRxTxBuf();
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_CRCERROR;
            }
        }
        MASTER_TRANSPORT_WAIT() 
    }

    MASTER_TRANSPORT_LOCK_GIVE();
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
TypedefEnum_MasterTransportSates master_writeHoldings(uint8_t slave, uint16_t adr, uint16_t val, int timeout)
{
    MASTER_TRANSPORT_LOCK_TAKE();
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
 //   MASTER_TRANSPORT_WAIT()
    if(timeout<=0)timeout=1;
    for (size_t i = 0; i < timeout; i++) // Timeout
    {
        uint8_t bytesToRead = master_hwBytesToRead();
        if (bytesToRead == expectedSize) {
            master_hwBytesToRead(masterRxTxBuf, expectedSize);
            uint16_t crcExpected = usMBCRC16(masterRxBuf, expectedSize - 2);
            uint16_t crcReturned = (uint16_t)(masterRxBuf[expectedSize - 1] << 8 | masterRxBuf[expectedSize - 2]);
            if (crcExpected == crcReturned) {
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_NOERROR;
            }
            else {
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_CRCERROR;
            }
        }
        else {
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
               //MASTER_TRANSPORT_WAIT()
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_CRCERROR;
            }
        }
        MASTER_TRANSPORT_WAIT() 
    }

    MASTER_TRANSPORT_LOCK_GIVE();
    return MASTER_TRANSPORT_TIMEOUT;
}

TypedefEnum_MasterTransportSates master_readHoldingOs(uint8_t slave, uint16_t adr, uint16_t* out, int timeout)
{
   // TypedefEnum_MasterTransportSates retVal =
    return  master_readHoldingsOs(slave, adr, 1, out, timeout);
}

int timeout_err[5] = {0};
int crc_err[5] = {0};
int slave_rx_cnt[5] = {0};
int to_adr = 0;

TypedefEnum_MasterTransportSates master_readHoldingsOs(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout)
{
    MASTER_TRANSPORT_LOCK_TAKE();
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
   // MASTER_TRANSPORT_WAIT()

    if(timeout<=0)timeout=1;
    for (size_t i = 0; i < timeout; i++) // Timeout
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
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_NOERROR;
            }
        }
        else
        {
            if (bytesToRead > expectedSize)
            {
                // master_hwRead(masterRxBuf, expectedSize);
                master_hwClearRxTxBuf();
              // MASTER_TRANSPORT_WAIT()
                crc_err[slave]++;
                MASTER_TRANSPORT_LOCK_GIVE();
                return MASTER_TRANSPORT_CRCERROR;
            }
        }
        MASTER_TRANSPORT_WAIT() 
    }

    timeout_err[slave]++;
    to_adr = adr;
    MASTER_TRANSPORT_LOCK_GIVE();
    return MASTER_TRANSPORT_TIMEOUT;
}