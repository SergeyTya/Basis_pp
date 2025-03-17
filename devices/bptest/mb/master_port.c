#include <stddef.h>
#include <stdint.h>

#include "usbd_cdc_vcp.h"

extern __IO uint32_t receive_count;
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

void master_hwInit(uint16_t speed){;}
void master_hwRead(uint8_t * buf, size_t len){;}

void master_hwWrite(uint8_t * buf, size_t len){
    VCP_SendData(&USB_OTG_dev, buf, len);
}

void master_hwClearRxTxBuf(){;}
int  master_hwBytesToRead(){return 0;}