#ifndef MASTERTRANSPORT_H
#define MASTERTRANSPORT_H

#include <stdint.h>
#include <stddef.h>


typedef enum
{

    MASTER_TRANSPORT_NOERROR = 0,
    MASTER_TRANSPORT_ERROR = 1,
    MASTER_TRANSPORT_TIMEOUT = 2,
    MASTER_TRANSPORT_CRCERROR = 3,

} TypedefEnum_MasterTransportSates;


TypedefEnum_MasterTransportSates master_writeHoldingOs(uint8_t slave, uint16_t adr, uint16_t val);
TypedefEnum_MasterTransportSates master_readHoldingOs(uint8_t slave, uint16_t adr, uint16_t* out);
TypedefEnum_MasterTransportSates master_readHoldingsOs(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff);

extern void master_LEDonWaitState(int dev);
extern void master_hwClearRxTxBuf();
extern int  master_hwBytesToRead();
extern void master_expectedByteCnt(int size);
extern void master_hwInit(uint16_t speed);
extern void master_hwRead(uint8_t* buf, size_t len);
extern void master_hwWrite(uint8_t* buf, size_t len);


#endif