


#include "port.h"
#include "mbport.h"

#include "usbd_cdc_vcp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

extern __IO uint32_t receive_count;
extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

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
    VCP_SendData(&USB_OTG_dev, (uint8_t * ) ucRTUBuf, usMBPcnt);
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

uint8_t Rxbuffer[256];
/*
 *  Call back from usb IRQ
 */
void usb_modbus_callback() {
  uint16_t cnt = 0;

  //VCP_ReceiveData(&USB_OTG_dev, Rxbuffer, receive_count);

  while (cnt < receive_count) {
    vcMBPrxBuff = (CHAR) Rxbuffer[cnt];
    pxMBFrameCBByteReceived();
    cnt++;
  }

  pxMBPortCBTimerExpired();
}


void events_dummy(){}