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
#include "panelConfig.h"

extern Typedef_PanelConfig panelConfig;

SemaphoreHandle_t semaphore_MT;
//#define MASTER_TRANSPORT_TIMEOUT_VAL 30 
#define MASTER_TRANSPORT_WAIT() {vTaskDelay(1); }

Typedef_MasterTransport transport = {.read=NULL, .readOpt=NULL, .write=NULL};

void master_transport_init(){
    semaphore_MT = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore_MT); 
    MASTER_TRANSPORT_LOCK_GIVE();

    // RTU
    

    switch (panelConfig.elinkSetup)
  {
  case 0:
  case 1:
    transport.read    = master_readHoldingOs_RTU;
    transport.readOpt = master_readHoldingsOs_RTU;
    transport.write   = master_writeHoldingOs_RTU;
    break;
  case 2:
    transport.read    = master_readHoldingOs_Elink;
    transport.readOpt = master_readHoldingsOs_Elink;
    transport.write   = master_writeHoldingOs_Elink;
  default: // Disabled
    break;
  }
}

TypedefEnum_MasterTransportSates master_writeHoldingOs(uint8_t slave, uint16_t adr, uint16_t val, int timeout)
{
    if(transport.write != NULL) return transport.write(slave, adr, val, timeout);
    return MASTER_TRANSPORT_TIMEOUT;
}

TypedefEnum_MasterTransportSates master_readHoldingOs(uint8_t slave, uint16_t adr, uint16_t* out, int timeout)
{
    if(transport.read != NULL) return transport.read(slave, adr, out, timeout); 
    return  MASTER_TRANSPORT_TIMEOUT;
}

TypedefEnum_MasterTransportSates master_readHoldingsOs(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout)
{
    if(transport.readOpt != NULL) return transport.readOpt(slave, adr, len, buff, timeout);
    return MASTER_TRANSPORT_TIMEOUT;
}
