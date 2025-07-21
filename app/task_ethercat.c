

#include "applInterface.h"


void vTask_ethercat(__attribute__((unused)) void *argument)
{
    __disable_irq();
        ECAT_HW_Init();
        MainInit();
    __enable_irq();
    while(1){
        MainLoop();
        vTaskDelay(100);
    }
}