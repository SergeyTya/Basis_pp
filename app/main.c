#include "stdint.h"
#include "stdbool.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

#include "task_panel.h"
#include "task_modbus.h"

#include "mb.h"
#include "mbport.h"

#include "task_master.h"
#include "task_keybord.h"

#include "lwipApp.h"

uint16_t holdings[256];


void vTask_ethercat(__attribute__((unused)) void *argument);
void vTask_VCP(__attribute__((unused)) void *argument);
void vTask_blinkBlakpill(__attribute__((unused)) void *argument);

void vTask_1s(void * arg){
    while(1){
        hwDriveHartBit_led1();
        vTaskDelay(1000);
    }
}

int main() {

    vMCU_init();
    
    init_ether();

    StructureInit_PanelConfig(&panelConfig);
    ConfigMenuReadAll();
  
    xTaskCreate(vTask_Master    , "Master"        , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(vTask_Panel     , "Panel"         , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate(vTask_modbus    , "ModbusSlave"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);
    xTaskCreate(vTask_keyboard  , "Keyboard"      , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);

    vTaskStartScheduler();
    return 0;
}

