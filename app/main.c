#include "stdint.h"
#include "stdbool.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

volatile int16_t int16 = 0;

int main() {

    vMCU_init();
    
    xTaskCreate(vTask_modbus, "Modbus", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
   
    vTaskStartScheduler();
    return 0;
}