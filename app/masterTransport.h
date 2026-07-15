#ifndef MASTERTRANSPORT_H
#define MASTERTRANSPORT_H

#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#define MASTER_TRANSPORT_WAIT() {vTaskDelay(1); }

typedef enum
{
    MASTER_TRANSPORT_NOERROR = 0,
    MASTER_TRANSPORT_ERROR = 1,
    MASTER_TRANSPORT_TIMEOUT = 2,
    MASTER_TRANSPORT_CRCERROR = 3,

} TypedefEnum_MasterTransportSates;

typedef struct{

    TypedefEnum_MasterTransportSates ( *write  ) (uint8_t slave, uint16_t adr, uint16_t val, int timeout);
    TypedefEnum_MasterTransportSates ( *read   ) (uint8_t slave, uint16_t adr, uint16_t* out, int timeout);
    TypedefEnum_MasterTransportSates ( *readOpt)(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout);

}Typedef_MasterTransport;


TypedefEnum_MasterTransportSates master_writeHoldingOs(uint8_t slave, uint16_t adr, uint16_t val, int timeout);
TypedefEnum_MasterTransportSates master_readHoldingOs(uint8_t slave, uint16_t adr, uint16_t* out, int timeout);
TypedefEnum_MasterTransportSates master_readHoldingsOs(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout);
void master_transport_init();

extern void master_LEDonWaitForAcOkState(int dev);
extern void master_LEDonWaitForMainRelayState(int dev);
extern void master_hwClearRxTxBuf();
extern int  master_hwBytesToRead();
extern void master_expectedByteCnt(int size);
extern void master_hwInit(uint16_t speed);
extern void master_hwRead(uint8_t* buf, size_t len);
extern void master_hwWrite(uint8_t* buf, size_t len);

extern SemaphoreHandle_t semaphore_MT;
#define  MASTER_TRANSPORT_LOCK_TAKE() {    while (xSemaphoreTake(semaphore_MT, portMAX_DELAY) != pdPASS) {;}}
#define  MASTER_TRANSPORT_LOCK_GIVE() {    xSemaphoreGive(semaphore_MT);}
#define MASTER_TRANSPORT_WITH_LOCK(foo){ \
    MASTER_TRANSPORT_LOCK_TAKE()\
    foo; \
    MASTER_TRANSPORT_LOCK_GIVE()\
}

TypedefEnum_MasterTransportSates master_writeHoldingOs_RTU(uint8_t slave, uint16_t adr, uint16_t val, int timeout);
TypedefEnum_MasterTransportSates master_readHoldingOs_RTU(uint8_t slave, uint16_t adr, uint16_t* out, int timeout);
TypedefEnum_MasterTransportSates master_readHoldingsOs_RTU(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout);

TypedefEnum_MasterTransportSates master_writeHoldingOs_Elink(uint8_t slave, uint16_t adr, uint16_t val, int timeout);
TypedefEnum_MasterTransportSates master_readHoldingOs_Elink(uint8_t slave, uint16_t adr, uint16_t* out, int timeout);
TypedefEnum_MasterTransportSates master_readHoldingsOs_Elink(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout);


#endif