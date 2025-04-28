#include "mb.h"
#include "mbport.h"
#include "task_modbus.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "panelConfig.h"

extern Typedef_PanelConfig panelConfig;
extern bool isTCP;


volatile uint8_t data = 0;
void vTask_modbusRTU(__attribute__((unused)) void *argument)
{
    QueueHandle_t xQueue = NULL;
    eMBErrorCode eStatus;

    eStatus = eMBInit(MB_RTU, panelConfig.modbus_RTU.adr, 0, panelConfig.modbus_RTU.speed, MB_PAR_NONE);
    eStatus = eMBEnable();

    xQueue = xQueueCreate(6, sizeof(uint32_t));

    while (1)
    {
        eMBPoll();

        uint16_t ToCounter = eMBGetTimeoutCounter();
        vTaskDelay(1);
    }
}


void vTask_modbusTCP(void* arg){

    isTCP = true;
    
    eMBErrorCode  xStatus;
    vTaskDelay(2000);
   
    for( ;; )
    {

        eMBErrorCode init = eMBTCPInit( 7 );
        if( init != MB_ENOERR )
        {
            vTaskDelay(1000);
        }
        else if( eMBEnable(  ) != MB_ENOERR )
        {
            vTaskDelay(1000);
        }
        else
        {
            do
            {
                xStatus = eMBPoll(  );
                vTaskDelay(30);
            }
            while( xStatus == MB_ENOERR );
        }
        /* An error occured. Maybe we can restart. */
        ( void )eMBDisable(  );
        ( void )eMBClose(  );
    }
}


void vTask_modbus(__attribute__((unused)) void *argument)
{
    vTask_modbusTCP(argument);
}