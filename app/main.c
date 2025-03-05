#include "stdint.h"
#include "stdbool.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "task_panel.h"

uint16_t holdings[10];


void vTask_ethercat(__attribute__((unused)) void *argument);
void vTask_VCP(__attribute__((unused)) void *argument);
void vTask_blinkBlakpill(__attribute__((unused)) void *argument);

int main() {

    vMCU_init();
    
    //xTaskCreate(vTask_VCP     , "VCP"     , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTask_Panel   , "Panel"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);

    vTaskStartScheduler();
    return 0;
}


// uint8_t Rxbuffer[64];
// uint8_t Txbuffer[64] = "hello my friend \n";

// void vTask_VCP(__attribute__((unused)) void *argument){
  
//   while(VCP_CheckDataReceived() == 0){
//     vTaskDelay(1);
//   }
  
//   while (1)
//   {
    
//     GPIO_SetBits(GPIOC, GPIO_Pin_13);

//     if( VCP_CheckDataReceived() != 0){
//       VCP_ReceiveData(&USB_OTG_dev, Rxbuffer, receive_count);
//     }

//     while (VCP_CheckDataSent() == 1) vTaskDelay(1);

//     VCP_SendData(&USB_OTG_dev, Txbuffer, 20);
//     vTaskDelay(5000);   
//     GPIO_ResetBits(GPIOC, GPIO_Pin_13); 
//     vTaskDelay(5000);
//   }
// }


