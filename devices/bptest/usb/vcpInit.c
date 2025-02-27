
#include "usbd_cdc_core_loopback.h"
#include "usbd_desc.h"
#include "usbd_req.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

__IO uint32_t receive_count = 1;

void  USBD_USR_Init();
void  USBD_USR_DeviceReset(uint8_t speed);
void  USBD_USR_DeviceConfigured();
void  USBD_USR_DeviceSuspended();
void  USBD_USR_DeviceResumed();
void  USBD_USR_DeviceConnected();
void  USBD_USR_DeviceDisconnected();

USBD_Usr_cb_TypeDef USR_cb = {
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,
};

void vcpInit(){

    USBD_Init(
        &USB_OTG_dev, 
        USB_OTG_FS_CORE_ID, 
        &USR_desc, 
        &USBD_CDC_cb, 
        &USR_cb
    );

}

volatile int cntr = 0;
void  USBD_USR_Init(){
   cntr++;
}
void  USBD_USR_DeviceReset(uint8_t speed){
    cntr++;
}
void  USBD_USR_DeviceConfigured(){
  cntr++;
}
void  USBD_USR_DeviceSuspended(){
  cntr++;
}
void  USBD_USR_DeviceResumed(){
  cntr++;
}
void  USBD_USR_DeviceConnected(){
    cntr++;
}
void  USBD_USR_DeviceDisconnected(){
  cntr++;
}