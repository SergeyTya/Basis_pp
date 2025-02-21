


#include "port.h"
#include "mbport.h"

#include "usbd_cdc_if.h"
#include "usb_device.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define MB_EVENT_BIT_RESET ( 0      )
#define MB_EVENT_BIT_0	   ( 1 << 0 )

static BOOL         sblModbusTxEnable         = FALSE;
static TaskHandle_t sTaskHandle_modbus_writer = NULL ;
extern CHAR         ucRTUBuf[]                       ;
uint32_t            usMBPcnt                  = 0    ;
CHAR                vcMBPrxBuff                      ;

_Noreturn static TaskHandle_t svTask_modbus_writer(void *pvParametrs){

  osEvent event;

  while(TRUE){

    while (
        sblModbusTxEnable == TRUE
        ) {
      pxMBFrameCBTransmitterEmpty();
    }
    event = osSignalWait( MB_EVENT_BIT_0 , osWaitForever);
  }
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    MX_USB_DEVICE_Init();
    return TRUE;
}

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{

  if(sTaskHandle_modbus_writer == NULL){

    xTaskCreate(  svTask_modbus_writer,
                "",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 10,
                &sTaskHandle_modbus_writer);
  }

  if(xTxEnable == TRUE)
  {
    sblModbusTxEnable = TRUE;
    osSignalSet( sTaskHandle_modbus_writer, MB_EVENT_BIT_0 );
  }
  else
  {
    sblModbusTxEnable = FALSE;
    osSignalSet( sTaskHandle_modbus_writer, MB_EVENT_BIT_RESET );
    CDC_Transmit_FS((uint8_t * ) ucRTUBuf, usMBPcnt );
    usMBPcnt = 0;
  }
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    usMBPcnt++;
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
   *pucByte =vcMBPrxBuff;
    return TRUE;
}

/*
 *  Call back from usb IRQ
 */
void usb_modbus_callback(const uint8_t *Buf, const uint32_t *Len) {
  uint16_t cnt = 0;
  while (cnt < *Len) {
    vcMBPrxBuff = (CHAR) Buf[cnt];
    pxMBFrameCBByteReceived();
    cnt++;
  }
  pxMBPortCBTimerExpired();
}
