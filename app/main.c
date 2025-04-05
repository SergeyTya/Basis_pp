#include "stdint.h"
#include "stdbool.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

#include "task_panel.h"

#include "mb.h"
#include "mbport.h"

#include "task_master.h"

uint16_t holdings[256];


void vTask_ethercat(__attribute__((unused)) void *argument);
void vTask_VCP(__attribute__((unused)) void *argument);
void vTask_blinkBlakpill(__attribute__((unused)) void *argument);

int main() {

    vMCU_init();
  
    xTaskCreate(vTask_Master  , "Master"  , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(vTask_Panel   , "Panel"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();
    return 0;
}

QueueHandle_t xQueue = NULL;
eMBErrorCode eStatus;



