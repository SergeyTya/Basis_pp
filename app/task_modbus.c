#include "mb.h"
#include "mbport.h"
#include "task_modbus.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

QueueHandle_t xQueue = NULL;
eMBErrorCode eStatus;

volatile uint8_t data = 0;
void vTask_modbus(__attribute__((unused)) void *argument)
{

    eStatus = eMBInit(MB_RTU, 1, 0, 115200, MB_PAR_NONE);
    eStatus = eMBEnable();

    xQueue = xQueueCreate(6, sizeof(uint32_t));

    while (1)
    {
        eMBPoll();

        uint16_t ToCounter = eMBGetTimeoutCounter();
        vTaskDelay(1);
    }
}