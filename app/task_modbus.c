#include "mb.h"
#include "mbport.h"
#include "task_modbus.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "panelConfig.h"

extern Typedef_PanelConfig panelConfig;

QueueHandle_t xQueue = NULL;
eMBErrorCode eStatus;

volatile uint8_t data = 0;
void vTask_modbus(__attribute__((unused)) void *argument)
{
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