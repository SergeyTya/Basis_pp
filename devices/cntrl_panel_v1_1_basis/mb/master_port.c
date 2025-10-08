#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gd32f4xx_libopt.h" 

// USART 0 PB6 PB7

static uint8_t master_tx_buffer[256];
static uint8_t master_rx_buffer[256];
int rx_cnt = 0;

#define RSM_GPIO_TX_CLK            RCU_GPIOB
#define RSM_GPIO_TX_PORT           GPIOB
#define RSM_GPIO_TX_AF             GPIO_AF_7
#define RSM_TX_PIN                 GPIO_PIN_6

#define RSM_GPIO_RX_CLK            RCU_GPIOB
#define RSM_GPIO_RX_PORT           GPIOB
#define RSM_GPIO_RX_AF             GPIO_AF_7
#define RSM_RX_PIN                 GPIO_PIN_7

#define RSM_GPIO_CTL_CLK           RCU_GPIOB
#define RSM_GPIO_CTL_PORT          GPIOB
#define RSM_CTL_PIN                GPIO_PIN_9

#define RSM_USART_CLK              RCU_USART0
#define RSM_USART                  USART0
#define RSM_USART_IRQHandler       USART0_IRQHandler
#define RSM_USART_IRQn             USART0_IRQn

#define RSM_DMA_DCU                RCU_DMA1
#define RSM_DMA                    DMA1
#define RSM_DMA_TXCH               DMA_CH7
#define RSM_DMA_RXCH               DMA_CH5
#define RSM_RX_DMA                 RSM_DMA,RSM_DMA_RXCH
#define RSM_TX_DMA                 RSM_DMA,RSM_DMA_TXCH
#define RSM_DMA_SUBPERI            DMA_SUBPERI4
#define RSM_USATR_DATA_ADR         (USART0 + 4U)
/************/

#define RSM_RDIO_PORT             GPIOE
#define RSM_RDIO_OUT1_PIN         GPIO_PIN_3
#define RSM_RDIO_OUT2_PIN         GPIO_PIN_2
#define RSM_RDIO_OUT3_PIN         GPIO_PIN_6
#define RSM_RDIO_OUT4_PIN         GPIO_PIN_5
#define RSM_RDIO_OUT5_PIN         GPIO_PIN_4
#define RSM_RDIO_DISABLE_ALL      gpio_bit_set(GPIOE, RSM_RDIO_OUT1_PIN|RSM_RDIO_OUT2_PIN|RSM_RDIO_OUT3_PIN|RSM_RDIO_OUT4_PIN|RSM_RDIO_OUT5_PIN )
#define RSM_RDIO_SET_STATE(RDIO_GPIO_PIN, state) {if(state!=0) {gpio_bit_reset(RSM_RDIO_PORT, RDIO_GPIO_PIN);}else{ gpio_bit_set(RSM_RDIO_PORT,RDIO_GPIO_PIN);}}
#define RSM_RDIO_OUT1_SET(state) RSM_RDIO_SET_STATE((RSM_RDIO_OUT1_PIN), state )
#define RSM_RDIO_OUT2_SET(state) RSM_RDIO_SET_STATE((RSM_RDIO_OUT2_PIN), state )
#define RSM_RDIO_OUT3_SET(state) RSM_RDIO_SET_STATE((RSM_RDIO_OUT3_PIN), state )
#define RSM_RDIO_OUT4_SET(state) RSM_RDIO_SET_STATE((RSM_RDIO_OUT4_PIN), state )
#define RSM_RDIO_OUT5_SET(state) RSM_RDIO_SET_STATE((RSM_RDIO_OUT5_PIN), state )



extern void keyboard_setFaultLedSate(bool state);
extern void keyboard_setOnLedSate(bool state);
extern void keyboard_setAC1LedSate(bool state);
extern void keyboard_setAC2LedSate(bool state);
extern void keyboard_setDC1LedSate(bool state);
extern void keyboard_setDC2LedSate(bool state);

int slaveResponseExpectedSize = 0;
static inline void Master_txDMA_set_state(bool state, int32_t cnt);
static inline void Master_rxDMA_set_state(bool state, int32_t cnt);
static inline void Master_setRDEstate(bool state);

bool recive_complete = false;

/**
* @brief Function for initializing the hardware
*
* @param speed Baud rate for the USART
*/
void master_hwInit(uint16_t speed) {

  rcu_periph_clock_enable(RSM_GPIO_RX_CLK);
  rcu_periph_clock_enable(RSM_GPIO_TX_CLK);
  rcu_periph_clock_enable(RSM_GPIO_CTL_CLK);


  /* enable GPIO clock */
  rcu_periph_clock_enable(RSM_GPIO_RX_CLK);
  rcu_periph_clock_enable(RSM_GPIO_TX_CLK);
  rcu_periph_clock_enable(RSM_GPIO_CTL_CLK);

  /* enable USART clock */
  rcu_periph_clock_enable(RSM_USART_CLK);

  /* connect port to USARTx_Tx */
  gpio_af_set(RSM_GPIO_TX_PORT, RSM_GPIO_TX_AF, RSM_TX_PIN);
  /* connect port to USARTx_Rx */
  gpio_af_set(RSM_GPIO_RX_PORT, RSM_GPIO_RX_AF, RSM_RX_PIN);

  /* configure USART Tx as alternate function push-pull */
  gpio_mode_set(RSM_GPIO_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RSM_TX_PIN);
  gpio_output_options_set(RSM_GPIO_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RSM_TX_PIN);

  /* configure USART Rx as alternate function push-pull */
  gpio_mode_set(RSM_GPIO_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RSM_RX_PIN);
  gpio_output_options_set(RSM_GPIO_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RSM_RX_PIN);

  /* USART configure */
  usart_deinit(RSM_USART);

  usart_oversample_config(RSM_USART, USART_OVSMOD_8);


  usart_word_length_set(RSM_USART, USART_WL_9BIT);
  usart_parity_check_coherence_config(RSM_USART, USART_PCM_EN);
  usart_parity_config(RSM_USART, USART_PM_EVEN);


  // usart_word_length_set(RSM_USART, USART_WL_8BIT);
  // usart_parity_check_coherence_config(RSM_USART, USART_PCM_NONE);
  // usart_parity_config(RSM_USART, USART_PM_NONE);


  usart_stop_bit_set(RSM_USART, 1);
  usart_baudrate_set(RSM_USART, 115200); /// ????????????
  usart_receive_config(RSM_USART, USART_RECEIVE_ENABLE);
  usart_transmit_config(RSM_USART, USART_TRANSMIT_ENABLE);
  usart_enable(RSM_USART);

  gpio_mode_set(RSM_GPIO_CTL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RSM_CTL_PIN);
  gpio_output_options_set(RSM_GPIO_CTL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RSM_CTL_PIN);


  dma_single_data_parameter_struct dma_init_struct;
  /* enable DMA1 */
  rcu_periph_clock_enable(RSM_DMA_DCU);
  /* deinitialize DMA channe7(RS_USART TX) */
  dma_deinit(RSM_TX_DMA);
  dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
  dma_init_struct.memory0_addr = (uint32_t)master_tx_buffer;
  dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.number = 512;
  dma_init_struct.periph_addr = RSM_USATR_DATA_ADR;
  dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
  dma_single_data_mode_init(RSM_TX_DMA, &dma_init_struct);
  dma_channel_subperipheral_select(RSM_TX_DMA, RSM_DMA_SUBPERI);
  /* configure DMA mode */
  dma_circulation_disable(RSM_TX_DMA);

  /* deinitialize DMA channe2(USART0 RX) */
  dma_deinit(RSM_RX_DMA);
  dma_init_struct.number = 0;
  dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
  dma_init_struct.memory0_addr = (uint32_t)master_rx_buffer;
  dma_single_data_mode_init(RSM_RX_DMA, &dma_init_struct);
  dma_channel_subperipheral_select(RSM_RX_DMA, RSM_DMA_SUBPERI);
  /* configure DMA mode */
  dma_circulation_disable(RSM_RX_DMA);

  // nvic_irq_enable(RSM_USART_IRQn, 10, 0);
  // NVIC_EnableIRQ(RSM_USART_IRQn);

  Master_setRDEstate(false);

  Master_txDMA_set_state(false, 0);
  Master_rxDMA_set_state(false, 0);

  //nvic_irq_enable(DMA0_Channel7_IRQn, 10 ,0);
  usart_interrupt_enable(RSM_USART, USART_INT_RBNE);
  nvic_irq_enable(RSM_USART_IRQn, 10, 0);


   RSM_RDIO_DISABLE_ALL;
}

/**
* @brief Function for reading data from the hardware
*
* @param buf Pointer to the buffer where the data will be stored
* @param len Length of the data to be read
*/
void master_hwRead(uint8_t* buf, size_t len) {
  slaveResponseExpectedSize = 0;
  //  Master_rxDMA_set_state(false, 0);
  memcpy(buf, master_rx_buffer, len);
}

/**
* @brief Function for setting the expected byte count
*
* @param expectedSize Expected byte count
*/
void master_expectedByteCnt(int expectedSize) {
  //  Master_rxDMA_set_state(false, 0);
  slaveResponseExpectedSize = expectedSize;
  rx_cnt = 0;
  //  Master_rxDMA_set_state(true, slaveResponseExpectedSize);
}

/**
* @brief Function for writing data to the hardware
*
* @param buf Pointer to the data buffer
* @param len Length of the data buffer
*/
void master_hwWrite(uint8_t* buf, size_t len) {

  Master_setRDEstate(true);
  memcpy(master_tx_buffer, buf, len);
  Master_txDMA_set_state(true, len);
  while (true) {
    if (usart_flag_get(RSM_USART, USART_FLAG_TC) == SET) {
      usart_flag_clear(RSM_USART, USART_FLAG_TC); // clear flag
      Master_txDMA_set_state(DISABLE, 0);
      Master_setRDEstate(false);
      return;
    }
  }

}

/**
* @brief Function for clearing the receive and transmit buffers
*
* @details This function clears the receive and transmit buffers by setting the expected size of the slave response to 0 and the receive complete flag to false.
*/
void master_hwClearRxTxBuf() {
  slaveResponseExpectedSize = 0;
  recive_complete = false;
}


/**
* @brief Function for getting the number of bytes available to read
*
* @return Number of bytes available to read for Transport
*/
int  master_hwBytesToRead() {
  return rx_cnt;
}

/**
* @brief Function for setup master HW Leds to fault state
*
* @param dev Device modbus ID
*/
void master_LEDonFaultReset() {
  keyboard_setFaultLedSate(false);
}

/**
* @brief Function for setup master HW Leds to fault state
*
* @param dev Device modbus ID
*/
void master_LEDonFaultState() {
  keyboard_setFaultLedSate(true);
  RSM_RDIO_DISABLE_ALL;
}


/**
* @brief Function for setup master HW Rdio to onRun state
*
* @param dev Device modbus ID
*/
void  master_RDIOonRUNstate(int dev) {

  // TODO RDO ENABLE
  switch (dev)
  {
  case 1:
  RSM_RDIO_OUT1_SET(1);
  break; 
  case 2:
  RSM_RDIO_OUT2_SET(1);
  break;
  case 3:
  RSM_RDIO_OUT3_SET(1);
  break;
  case 4:
  RSM_RDIO_OUT4_SET(1);
  break;

  default:
    break;
  }
}

bool master_ToggleHeaterState(){

  gpio_bit_toggle(RSM_RDIO_PORT,RSM_RDIO_OUT5_PIN);
  return (gpio_input_bit_get(RSM_RDIO_PORT,RSM_RDIO_OUT5_PIN) == 0 );
}


/**
* @brief Function for setup master HW Leds to onFAULT state
*
* @param dev Device modbus ID
*/
void  master_LEDonRUNstate(int dev) {

  // TODO RDO ENABLE
  switch (dev)
  {
  case 1:
  keyboard_setAC1LedSate(true);
  RSM_RDIO_OUT1_SET(1);
  break; 
  case 2:
  keyboard_setAC2LedSate(true);
  RSM_RDIO_OUT2_SET(1);
  break;
  case 3:
  keyboard_setDC2LedSate(true);
  RSM_RDIO_OUT3_SET(1);
  break;
  case 4:
  keyboard_setDC1LedSate(true); 
  RSM_RDIO_OUT4_SET(1);
  break;

  default:
    break;
  }
}

void  master_LEDonGlobRunState(bool state) {

  // TODO RDO ENABLE
  keyboard_setOnLedSate(state);
}

/**
* @brief Function for setup master HW Leds to onSTOP state
*
* @param dev Device modbus ID
*/
void master_LEDonReadyState(int dev) {
  // TODO RDO ENABLE
  switch (dev)
  {
  case 1:
  keyboard_setAC1LedSate(false);
  RSM_RDIO_OUT1_SET(0);
  break; 
  case 2:
  keyboard_setAC2LedSate(false);
  RSM_RDIO_OUT2_SET(0);
  break;
  case 3:
  keyboard_setDC2LedSate(false);
  RSM_RDIO_OUT3_SET(0);
  break;
  case 4:
  keyboard_setDC1LedSate(false); 
  RSM_RDIO_OUT4_SET(0);
  break;

  default:
    break;
  }
}

/**
* @brief Function for setup master HW Leds when slave connection lost
*
* @param dev Device modbus ID
*/
void master_LEDonTimeoutState() {
  static bool FLblinker = false;
  static uint16_t FLblinker_cntr = 0;

  if (FLblinker_cntr == 0) {
    FLblinker_cntr = 3;
    FLblinker = !FLblinker;
  } else {
    FLblinker_cntr--;
    return;
  }

  keyboard_setFaultLedSate(FLblinker);
}
/**
* @brief Function for blinking slave state LED util start delay
*
* @param dev Device modbus ID
*/
extern bool blinker;
void master_LEDonWaitState(int dev) {


  switch (dev)
  {
  case 1: 
    keyboard_setAC1LedSate(blinker);
    RSM_RDIO_OUT1_SET(0);
     break;
  case 2:
    keyboard_setAC2LedSate(blinker);
    RSM_RDIO_OUT2_SET(0);
   break;   
  case 3: 
    keyboard_setDC1LedSate(blinker);
    RSM_RDIO_OUT3_SET(0);
    break;
  case 4:
    keyboard_setDC2LedSate(blinker);
    RSM_RDIO_OUT4_SET(0);
    break;

  default:
    break;
  }
}

static inline void Master_txDMA_set_state(bool state, int32_t cnt) {
  if (state == true) {
    dma_transfer_number_config(RSM_TX_DMA, cnt); // setup dma max transfers
    usart_dma_transmit_config(RSM_USART, USART_DENT_ENABLE); // enable uart receiver DMA
    dma_flag_clear(RSM_TX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_channel_enable(RSM_TX_DMA);  // enable DMA
    return;
  }
  usart_dma_transmit_config(RSM_USART, USART_DENT_DISABLE); // enable uart receiver DMA
  dma_channel_disable(RSM_TX_DMA);           // enable DMA
}

static inline void Master_rxDMA_set_state(bool state, int32_t cnt) {
  if (state == ENABLE) {
    dma_transfer_number_config(RSM_RX_DMA, cnt); // setup dma max transfers
    dma_circulation_disable(RSM_RX_DMA);
    dma_channel_subperipheral_select(RSM_RX_DMA, DMA_SUBPERI4);
    usart_dma_receive_config(RSM_USART, USART_DENR_ENABLE); // enable uart receiver DMA
    dma_interrupt_enable(RSM_RX_DMA, DMA_CHXCTL_FTFIE);
    dma_channel_enable(RSM_RX_DMA);  // enable DMA
    return;
  }
  dma_interrupt_disable(RSM_RX_DMA, DMA_CHXCTL_FTFIE);
  usart_dma_receive_config(RSM_USART, USART_DENR_DISABLE); // enable uart receiver DMA
  dma_channel_disable(RSM_RX_DMA);           // enable DMA
}

static inline void Master_setRDEstate(bool state) {
  if (state == true) {
    gpio_bit_set(RSM_GPIO_CTL_PORT, RSM_CTL_PIN);
  }
  else {
    gpio_bit_reset(RSM_GPIO_CTL_PORT, RSM_CTL_PIN);
  }
}


/*!
    \brief      this function handles DMA0_Channel4_IRQHandler interrupt
*/
void DMA0_Channel7_IRQHandler(void)
{
  if (dma_interrupt_flag_get(RSM_RX_DMA, DMA_INT_FLAG_FTF)) {
    dma_interrupt_flag_clear(RSM_RX_DMA, DMA_INT_FLAG_FTF);
    recive_complete = true;
    Master_rxDMA_set_state(false, 0);
  }

  NVIC_ClearPendingIRQ(DMA0_Channel7_IRQn);
}


void RSM_USART_IRQHandler() {

  if (usart_flag_get(RSM_USART, USART_FLAG_RBNE) == SET) {
    usart_flag_clear(RSM_USART, USART_FLAG_RBNE);
    master_rx_buffer[rx_cnt++] = usart_data_receive(RSM_USART);
  }

  NVIC_ClearPendingIRQ(RSM_USART_IRQn);

}
