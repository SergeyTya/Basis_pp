#include "stdint.h"
#include "stdbool.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

#include "applInterface.h"

#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"


uint16_t holdings[10];


void vTask_ethercat(__attribute__((unused)) void *argument);
void vTask_VCP(__attribute__((unused)) void *argument);

int main() {

    vMCU_init();
    
    xTaskCreate(vTask_ethercat, "EtherCAT", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTask_VCP     , "VCP"     , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
   
    vTaskStartScheduler();
    return 0;
}


void vTask_ethercat(__attribute__((unused)) void *argument)
{
    MainInit();
    while(1){
        MainLoop();
        vTaskDelay(1);
    }
}


extern __IO uint32_t receive_count;
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;
uint8_t Rxbuffer[64];

void vTask_VCP(__attribute__((unused)) void *argument){
  while (1)
  {
    /* wait data reception */
    while (VCP_CheckDataReceived() == 0) vTaskDelay(1);

    /* receive one character */
    VCP_ReceiveData(&USB_OTG_dev, Rxbuffer, receive_count);

    /* Check to see if we have data yet */
    if (receive_count != 0)
    {
      /* wait data sent */
      while (VCP_CheckDataSent() == 1) vTaskDelay(1);

      /* send myTxBuffer */
      VCP_SendData(&USB_OTG_dev, Rxbuffer, receive_count);

      /* INIT received byte count */
      receive_count = 0;
    }
  }
}
