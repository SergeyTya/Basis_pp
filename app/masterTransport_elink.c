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
#include "mbsupport.h"
#include "elink/elink.h"
#include "meter.h"
#include "clock.h"

#include "string.h"

#include "elink/elink_to_panel.h"

extern Typedef_SlaveRequest tcpInputSlaveReq;
extern Typedef_SlaveRequest tcpOutputSlave_req;
extern bool tcp_client_to;

TypedefEnum_MasterTransportSates master_writeHoldingOs_Elink(uint8_t slave, uint16_t adr, uint16_t val, int timeout)
{
   
    if(tcp_client_to) return MASTER_TRANSPORT_TIMEOUT;

   //while(tcpOutputSlave_req.is_write_request) 


   switch (slave)
   {
        case CONFIG_SLAVE_AC1:
        case CONFIG_SLAVE_AC2:
        case CONFIG_SLAVE_DC1:
        case CONFIG_SLAVE_DC2:

        tcpOutputSlave_req.is_write_request = 1;
        tcpOutputSlave_req.slave_id = slave;
        tcpOutputSlave_req.register_address = adr;
        tcpOutputSlave_req.register_value = val;

        vTaskDelay(102); // >= client delay

        break;
    
        case CONFIG_METER_ADR:
        case CONFIG_CLOCK_ADR:
        default:
        return MASTER_TRANSPORT_TIMEOUT;
        
   }
   
   
    return MASTER_TRANSPORT_TIMEOUT;
}


extern uint16_t holdings[256];

TypedefEnum_MasterTransportSates master_readHoldingOs_Elink(uint8_t slave, uint16_t adr, uint16_t* out, int timeout)
{
    TypeDef_MB_Table *table;
    TypeDef_MB_Holding *holding;

    if(tcp_client_to) return MASTER_TRANSPORT_TIMEOUT;

   

    switch (slave)
   {
        case CONFIG_SLAVE_AC1:
        case CONFIG_SLAVE_AC2:
        case CONFIG_SLAVE_DC1:
        case CONFIG_SLAVE_DC2:
        
            table = &holdings_table[slave];
            holding = GetHoldingByAdrFromTable(adr, table);
            if(holding != &mbsupportNullHolding){
                *out = *holding->pntr;
                return MASTER_TRANSPORT_NOERROR;
                
            }else{
                return  MASTER_TRANSPORT_TIMEOUT;
            }

        break;

        case CONFIG_METER_ADR:  
        break;

        case CONFIG_CLOCK_ADR:
        break;
   
        default:
        return MASTER_TRANSPORT_TIMEOUT;
        
   }
    return  MASTER_TRANSPORT_TIMEOUT;
}

TypedefEnum_MasterTransportSates master_readHoldingsOs_Elink(uint8_t slave, uint16_t adr, uint16_t len, uint16_t* buff, int timeout)
{
    switch (slave)
   {
        case CONFIG_SLAVE_AC1:
    
        break;

        case CONFIG_SLAVE_AC2:

        break;

        case CONFIG_SLAVE_DC1:
    
        break;

        case CONFIG_SLAVE_DC2:
    
        break;

        default:
        return MASTER_TRANSPORT_TIMEOUT;
        
   }
    return MASTER_TRANSPORT_TIMEOUT;
}