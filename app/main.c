#include "stdint.h"
#include "stdbool.h"

#include  "task_list.h"
#include  "mcu_init.h"
#include  "dev_cnfg.h"
#include  "main.h"

#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "task_panel.h"

#include "mb.h"
#include "mbport.h"

uint16_t holdings[256];


void vTask_ethercat(__attribute__((unused)) void *argument);
void vTask_VCP(__attribute__((unused)) void *argument);
void vTask_blinkBlakpill(__attribute__((unused)) void *argument);

int main() {

    vMCU_init();
    
    xTaskCreate(vTask_VCP     , "VCP"     , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTask_Panel   , "Panel"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);

    vTaskStartScheduler();
    return 0;
}

QueueHandle_t xQueue = NULL;
eMBErrorCode eStatus;

extern __IO uint32_t receive_count;
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;
extern uint8_t Rxbuffer[64];
// uint8_t Txbuffer[64] = "hello my friend \n";

void vTask_VCP(__attribute__((unused)) void *argument){
  
    eStatus = eMBInit(MB_RTU, 1, 0, 115200, MB_PAR_NONE);
    eStatus = eMBEnable();

    xQueue = xQueueCreate(6, sizeof(uint32_t));

  while (1)
  {
    eMBPoll(); 

    if( VCP_CheckDataReceived() != 0){
      VCP_ReceiveData(&USB_OTG_dev, Rxbuffer, receive_count);
      usb_modbus_callback();
      receive_count = 0;
    }

    vTaskDelay(2);
  }
}


