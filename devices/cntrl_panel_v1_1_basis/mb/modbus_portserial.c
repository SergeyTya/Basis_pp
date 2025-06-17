#include <stdint.h>
#include <stdbool.h>

#include "dev_cnfg.h"

#include "port.h"
#include "mbport.h"

// include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"


#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define MB_EVENT_BIT_RESET ( 0      )
#define MB_EVENT_BIT_0	   ( 1 << 0 )

extern CHAR         ucRTUBuf[];
uint32_t            usMBPcnt = 0;
CHAR                vcMBPrxBuff;

int32_t rtx_cnt = 0;
volatile char rtx_buffer[MB_BUF_SIZE_MAX];

void setRDEstate(bool state) {
  if (state == true) {
    gpio_bit_set(RS_GPIO_CTL_PORT, RS_CTL_PIN);
  }
  else {
    gpio_bit_reset(RS_GPIO_CTL_PORT, RS_CTL_PIN);
  }

  hwDriveHartBit_led2();
}

static inline void txDMA_set_state(BOOL state, int32_t cnt) {
  if (state == ENABLE) {
    dma_transfer_number_config(RS_TX_DMA, cnt); // setup dma max transfers
    usart_dma_transmit_config(RS_USART, USART_DENT_ENABLE); // enable uart receiver DMA
    dma_flag_clear(RS_TX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_channel_enable(RS_TX_DMA);  // enable DMA
    return;
  }
  usart_dma_transmit_config(RS_USART, USART_DENT_DISABLE); // enable uart receiver DMA
  dma_channel_disable(RS_TX_DMA);           // enable DMA
}

static inline void rxDMA_set_state(BOOL state) {
  if (state == ENABLE) {
    dma_transfer_number_config(RS_RX_DMA, MB_BUF_SIZE_MAX); // setup dma max transfers
    usart_dma_receive_config(RS_USART, USART_DENR_ENABLE); // enable uart receiver DMA
    dma_flag_clear(RS_RX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_channel_enable(RS_RX_DMA);  // enable DMA
    return;
  }
  usart_dma_receive_config(RS_USART, USART_DENR_DISABLE); // enable uart receiver DMA
  dma_channel_disable(RS_RX_DMA);           // enable DMA
}

static void usart_dma_config(void)
{
  dma_single_data_parameter_struct dma_init_struct;
  /* enable DMA1 */
  rcu_periph_clock_enable(RS_DMA_DCU);
  /* deinitialize DMA channe7(RS_USART TX) */
  dma_deinit(RS_TX_DMA);
  dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
  dma_init_struct.memory0_addr = (uint32_t)rtx_buffer;
  dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.number = MB_BUF_SIZE_MAX;
  dma_init_struct.periph_addr = RS_USATR_DATA_ADR;
  dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
  dma_single_data_mode_init(RS_TX_DMA, &dma_init_struct);
  dma_channel_subperipheral_select(RS_TX_DMA, RS_DMA_SUBPERI);
  /* configure DMA mode */
  dma_circulation_disable(RS_TX_DMA);

  /* deinitialize DMA channe2(USART0 RX) */
  dma_deinit(RS_RX_DMA);
  dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
  dma_init_struct.memory0_addr = (uint32_t)rtx_buffer;
  dma_single_data_mode_init(RS_RX_DMA, &dma_init_struct);
  dma_channel_subperipheral_select(RS_RX_DMA, RS_DMA_SUBPERI);
  /* configure DMA mode */
  dma_circulation_disable(RS_RX_DMA);
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
  /* enable GPIO clock */
  rcu_periph_clock_enable(RS_GPIO_RX_CLK);
  rcu_periph_clock_enable(RS_GPIO_TX_CLK);
  rcu_periph_clock_enable(RS_GPIO_CTL_CLK);



  /* enable USART clock */
  rcu_periph_clock_enable(RS_USART_CLK);

  /* connect port to USARTx_Tx */
  gpio_af_set(RS_GPIO_TX_PORT, RS_GPIO_TX_AF, RS_TX_PIN);
  /* connect port to USARTx_Rx */
  gpio_af_set(RS_GPIO_RX_PORT, RS_GPIO_RX_AF, RS_RX_PIN);

  /* configure USART Tx as alternate function push-pull */
  gpio_mode_set(RS_GPIO_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS_TX_PIN);
  gpio_output_options_set(RS_GPIO_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS_TX_PIN);

  /* configure USART Rx as alternate function push-pull */
  gpio_mode_set(RS_GPIO_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS_RX_PIN);
  gpio_output_options_set(RS_GPIO_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS_RX_PIN);

  /* USART configure */
  usart_deinit(RS_USART);
  usart_baudrate_set(RS_USART, ulBaudRate); /// ????????????

  switch (eParity)
  {
  case MB_PAR_NONE: default:
    usart_word_length_set(RS_USART, USART_WL_8BIT);
    usart_parity_check_coherence_config(RS_USART, USART_PCM_NONE);
    usart_parity_config(RS_USART, USART_PM_NONE);
    break;
  case MB_PAR_ODD:
    usart_word_length_set(RS_USART, USART_WL_9BIT);
    usart_parity_check_coherence_config(RS_USART, USART_PCM_EN);
    usart_parity_config(RS_USART, USART_PM_ODD);
  case MB_PAR_EVEN:
    usart_parity_config(RS_USART, USART_PM_EVEN);
    break;
  }

  usart_stop_bit_set(RS_USART, 1);
  usart_receive_config(RS_USART, USART_RECEIVE_ENABLE);
  usart_transmit_config(RS_USART, USART_TRANSMIT_ENABLE);
  usart_enable(RS_USART);

  gpio_mode_set(RS_GPIO_CTL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RS_CTL_PIN);
  gpio_output_options_set(RS_GPIO_CTL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS_CTL_PIN);

  nvic_irq_enable(RS_USART_IRQn, 10, 0);
  NVIC_EnableIRQ(RS_USART_IRQn);

  usart_receiver_timeout_enable(RS_USART);
  usart_receiver_timeout_threshold_config(RS_USART, 1);

  /* configure USART DMA */
  usart_dma_config();
  usart_dma_receive_config(RS_USART, USART_DENR_DISABLE); // enable uart receiver DMA

  setRDEstate(false);

  return TRUE;
}
static BOOL sblStopTx = 0;
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{

  if (xRxEnable == TRUE)
  {
    usart_flag_clear(RS_USART, USART_FLAG_RT); // clear timeout flag
    usart_interrupt_enable(RS_USART, USART_INT_RT); // enable timeout interrupt
    rxDMA_set_state(ENABLE);
  }
  else
  {
    usart_interrupt_disable(RS_USART, USART_INT_RT); // disable timeout interrupt
    rxDMA_set_state(DISABLE);
  }

  if (xTxEnable == TRUE)
  {
    sblStopTx = 1;
    while (sblStopTx == 1) {
      pxMBFrameCBTransmitterEmpty();
    }
    setRDEstate(true);
    usart_interrupt_enable(RS_USART, USART_INT_TC); // enable transition complete IRq
    txDMA_set_state(ENABLE, rtx_cnt);
    rtx_cnt = 0;
  }
  else
  {
    sblStopTx = 0;
  }
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
  rtx_buffer[rtx_cnt++] = ucByte;
  return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR* pucByte)
{
  *pucByte = rtx_buffer[rtx_cnt++];
  return TRUE;
}

/*
 *  Call back from usb IRQ
 */
void usb_modbus_callback(const uint8_t* Buf, const uint32_t* Len) {
}

void RS_USART_IRQHandler() {

  // Receiver timeout
  if (usart_flag_get(RS_USART, USART_FLAG_RT) == SET) {
    rxDMA_set_state(DISABLE);
    usart_flag_clear(RS_USART, USART_FLAG_RT); // clear timeout flag
    int32_t cnt = MB_BUF_SIZE_MAX - dma_transfer_number_get(RS_RX_DMA); // get received data count
    while (rtx_cnt < cnt) {
      pxMBFrameCBByteReceived(); // put data to modbus frame
    }
    rtx_cnt = 0;
    pxMBPortCBTimerExpired(); // finishing
    rxDMA_set_state(ENABLE);
    //Dev_Cntrl.MBWDG_CNT = 0;
  }

  // Transition complete
  if (usart_flag_get(RS_USART, USART_FLAG_TC) == SET) {
    setRDEstate(false);
    usart_interrupt_disable(RS_USART, USART_INT_TC); // disable interrupt
    usart_flag_clear(RS_USART, USART_FLAG_TC); // clear flag
    txDMA_set_state(DISABLE, 0);
  }

  NVIC_ClearPendingIRQ(RS_USART_IRQn);
}



