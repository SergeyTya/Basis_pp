#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "gd32f4xx_libopt.h"
#ifndef MB_BUF_SIZE_MAX
#define MB_BUF_SIZE_MAX 256
#endif

#define RS_GPIO_TX_CLK RCU_GPIOC
#define RS_GPIO_TX_PORT GPIOC
#define RS_GPIO_TX_AF GPIO_AF_8
#define RS_TX_PIN GPIO_PIN_6

#define RS_GPIO_RX_CLK RCU_GPIOC
#define RS_GPIO_RX_PORT GPIOC
#define RS_GPIO_RX_AF GPIO_AF_8
#define RS_RX_PIN GPIO_PIN_7

#define RS_GPIO_CTL_CLK RCU_GPIOD
#define RS_GPIO_CTL_PORT GPIOD
#define RS_CTL_PIN GPIO_PIN_3

#define RS_USART_CLK RCU_USART5
#define RS_USART USART5
#define RS_USART_IRQHandler USART5_IRQHandler
#define RS_USART_IRQn USART5_IRQn

#define RS_DMA_DCU RCU_DMA1
#define RS_DMA DMA1
#define RS_DMA_TXCH DMA_CH6
#define RS_DMA_RXCH DMA_CH1
#define RS_RX_DMA RS_DMA, RS_DMA_RXCH
#define RS_TX_DMA RS_DMA, RS_DMA_TXCH
#define RS_DMA_SUBPERI DMA_SUBPERI5
#define RS_USATR_DATA_ADR (USART5 + 4U)

#define UART_CHCKFLG(FLG) (USART_REG_VAL(RS_USART, FLG) & BIT(USART_BIT_POS(FLG)))
#define UART_CLEARFLG(FLG) (USART_REG_VAL(RS_USART, FLG) &= ~BIT(USART_BIT_POS(FLG)))
#define RESET_CPIN (GPIO_BC(RS_GPIO_CTL_PORT) = (uint32_t)RS_CTL_PIN)
#define SET_CPIN (GPIO_BOP(RS_GPIO_CTL_PORT) = (uint32_t)RS_CTL_PIN)
#define DISABLE_IRQ __disable_irq()
#define RESET_MCU NVIC_SystemReset()
#define DMAOFF(dma_periph, channelx) (DMA_CHCTL(dma_periph, channelx) &= ~DMA_CHXCTL_CHEN)
#define DMACNTSET(dma_periph, channelx, CNT) (DMA_CHCNT(dma_periph, channelx) = CNT)
#define DMACNTGET(dma_periph, channelx) ((uint32_t)(DMA_CHCNT(dma_periph, channelx)))
#define DMAON(dma_periph, channelx) (DMA_CHCTL(dma_periph, channelx) |= DMA_CHXCTL_CHEN)

volatile char rtx_buffer[MB_BUF_SIZE_MAX];
static volatile int32_t dma_cnt = 0;
static volatile int32_t dma_trans = 0;
const char cpu_id[9] = "GD32F4xxx";
  
void mydmaflgcln(uint32_t dma_periph, dma_channel_enum channelx)
{
  if (channelx < DMA_CH4)
  {
    DMA_INTC0(dma_periph) |= DMA_FLAG_ADD(DMA_FLAG_FTF, channelx);
  }
  else
  {
    channelx -= (dma_channel_enum)4;
    DMA_INTC1(dma_periph) |= DMA_FLAG_ADD(DMA_FLAG_FTF, channelx);
  }
}

static void setRDEstate(bool state){
  if(state==true){
    gpio_bit_set(GPIOD, GPIO_PIN_15);
    gpio_bit_set(RS_GPIO_CTL_PORT, RS_CTL_PIN);
  }else{
    gpio_bit_reset(RS_GPIO_CTL_PORT, RS_CTL_PIN);
    gpio_bit_reset(GPIOD, GPIO_PIN_15);
  }
}

dma_single_data_parameter_struct dma_init_struct;
static void usart_dma_config(void)
{
  /* enable DMA1 */
  rcu_periph_clock_enable(RS_DMA_DCU);
  dma_deinit(RS_RX_DMA);

  dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
  dma_init_struct.memory0_addr = (uint32_t) rtx_buffer;
  dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.number = MB_BUF_SIZE_MAX;
  dma_init_struct.periph_addr = RS_USATR_DATA_ADR;
  dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
  dma_channel_subperipheral_select(RS_RX_DMA, RS_DMA_SUBPERI);
  dma_single_data_mode_init(RS_RX_DMA, &dma_init_struct);

  /* configure DMA mode */
  dma_circulation_disable(RS_RX_DMA);

}

void UUBport_ConfigHw()
{
  //DISABLE_IRQ;
  rcu_periph_clock_enable(RCU_GPIOD);
  gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

  gpio_bit_set(GPIOD, GPIO_PIN_12);
  gpio_bit_set(GPIOD, GPIO_PIN_13);
  gpio_bit_set(GPIOD, GPIO_PIN_14);

  /* enable GPIO clock */
  // rcu_periph_clock_enable(RS_GPIO_CTL_CLK);
  rcu_periph_clock_enable(RS_GPIO_RX_CLK);
  rcu_periph_clock_enable(RS_GPIO_TX_CLK);

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
  usart_baudrate_set(RS_USART, 460800); /// ????????????
  usart_parity_config(RS_USART, USART_PM_NONE);
  usart_stop_bit_set(RS_USART, 1);
  usart_receive_config(RS_USART, USART_RECEIVE_ENABLE);
  usart_transmit_config(RS_USART, USART_TRANSMIT_ENABLE);
  usart_enable(RS_USART);

  gpio_mode_set(RS_GPIO_CTL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RS_CTL_PIN);
  gpio_output_options_set(RS_GPIO_CTL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS_CTL_PIN);

  // tx alive pin
  gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

  //nvic_irq_enable(RS_USART_IRQn, 2, 1);

  usart_receiver_timeout_enable(RS_USART);
  usart_receiver_timeout_threshold_config(RS_USART, 1);

  /* configure USART DMA */
  usart_dma_config();
  usart_dma_receive_config(RS_USART, USART_DENR_DISABLE); // disable uart receiver DMA

  setRDEstate(false);

  usart_dma_transmit_config(RS_USART, USART_DENT_DISABLE); // disable uart tx DMA
  dma_channel_disable(RS_TX_DMA);                          // enable DMA
  usart_dma_receive_config(RS_USART, USART_DENR_ENABLE); 
  usart_data_transmit(RS_USART, 0);
  //while ( UART_CHCKFLG(USART_FLAG_TC) == 0);
  DMAOFF(RS_DMA, RS_DMA_RXCH);
  DMACNTSET(RS_DMA, RS_DMA_RXCH, MB_BUF_SIZE_MAX);
  mydmaflgcln(RS_RX_DMA);
  DMAON(RS_DMA, RS_DMA_RXCH);
  
}

/**
 * Read one byte from transport.
 * @retval {uint8_t}  data
 */

uint8_t UUBport_ReceiveByte(void)
{
  char res;
  if (dma_cnt < dma_trans)
  {
    res = rtx_buffer[dma_cnt++];
    return res;
  }
  RESET_CPIN;
  while (UART_CHCKFLG(USART_FLAG_RT) == 0)
  {
    FWDGT_CTL = FWDGT_KEY_RELOAD; // reload fwdg
  }
  DMAOFF(RS_DMA, RS_DMA_RXCH);
  UART_CLEARFLG(USART_FLAG_RT);
  dma_cnt = 0;
  dma_trans =  MB_BUF_SIZE_MAX - dma_transfer_number_get(RS_RX_DMA); 
  res = rtx_buffer[dma_cnt++];
  DMACNTSET(RS_DMA, RS_DMA_RXCH, MB_BUF_SIZE_MAX);
  mydmaflgcln(RS_RX_DMA);
  DMAON(RS_DMA, RS_DMA_RXCH);
  return res;
}

/**
 * Put one byte to transport.
 * @param {uint8_t} data
 */

void UUBport_Send(const unsigned char *data, size_t len)
{

  for (size_t i = 0; i < len; i++)
  {
    while (UART_CHCKFLG(USART_FLAG_TC) == 0)
    {
      ;
    }

    SET_CPIN;
    gpio_bit_set(GPIOD, GPIO_PIN_12);
    USART_DATA(RS_USART) = ((uint16_t)USART_DATA_DATA & data[i]);
    while (UART_CHCKFLG(USART_FLAG_TC) == 0)
    {
      ;
    }
    RESET_CPIN;
    gpio_bit_reset(GPIOD, GPIO_PIN_12);
  }
}

/**
 * Write byte array to EEPROM.
 * @param {uint32_t} memory address value
 * @param {uint32_t} data
 * @param {uint32_t} array size
 */
void UUBport_WriteArrayToEEPROM(uint32_t adr, uint8_t *data, size_t len)
{
  fmc_flag_clear(FMC_FLAG_RDDERR);
  fmc_flag_clear(FMC_FLAG_PGSERR);
  fmc_flag_clear(FMC_FLAG_PGMERR);
  fmc_flag_clear(FMC_FLAG_WPERR);
  fmc_flag_clear(FMC_FLAG_OPERR);
  fmc_flag_clear(FMC_FLAG_END);

  for (size_t i = 0; i < len; i++)
  {
    fmc_byte_program(adr++, data[i]);
  }
}

/**
 * Erase EEPROM.
 */
bool UUBport_EraseEEPROM()
{

  fmc_unlock();
  fmc_flag_clear(FMC_FLAG_RDDERR);
  fmc_flag_clear(FMC_FLAG_PGSERR);
  fmc_flag_clear(FMC_FLAG_PGMERR);
  fmc_flag_clear(FMC_FLAG_WPERR);
  fmc_flag_clear(FMC_FLAG_OPERR);
  fmc_flag_clear(FMC_FLAG_END);

  fmc_sector_erase(CTL_SECTOR_NUMBER_8);
  fmc_sector_erase(CTL_SECTOR_NUMBER_7);

  return true;
}

void UUBport_RebootHW()
{
  NVIC_SystemReset();
}

unsigned char id2_str[12] = "UUB_LDR_";
unsigned char * UUBport_GetID(void){
  return id2_str;
}

size_t UUBport_GetIDlen(){
  return sizeof(id2_str);
}