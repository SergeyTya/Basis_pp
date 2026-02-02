/*
 * 		GD25Q32E.h
 *
 *  GD25Q32 hardware driver for GD32F4xx MCU
 *
 *  Created on: 21.12.2023 г.
 *      Author: Sergey Tyagushev
 */

#ifndef GD25Q32_PORT_H_
#define GD25Q32_PORT_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#include "gd32f4xx_libopt.h"

/*
* DMA BUFFER SIZE
*/
#define GD25Q32_BUF_SZ              512

/*
* MCU SPI HARDWARE
*/
#define GD25Q32_SPI                SPI4
#define GD25Q32_SPI_RCU            RCU_SPI4
#define GD25Q32_SPI_PORT_RCU       (RCU_GPIOF) 
#define GD25Q32_SPI_CLK_PORT       GPIOF
#define GD25Q32_SPI_CLK_PIN        GPIO_PIN_7
#define GD25Q32_SPI_CLK_PIN_AF     GPIO_AF_5
#define GD25Q32_SPI_MOSI_PORT      GPIOF
#define GD25Q32_SPI_MOSI_PIN       GPIO_PIN_9
#define GD25Q32_SPI_MOSI_PIN_AF    GPIO_AF_5
#define GD25Q32_SPI_MISO_PORT      GPIOF
#define GD25Q32_SPI_MISO_PIN       GPIO_PIN_8
#define GD25Q32_SPI_MISO_PIN_AF    GPIO_AF_5
#define GD25Q32_SPI_NSS_PORT       GPIOF
#define GD25Q32_SPI_NSS_PIN        GPIO_PIN_6
#define GD25Q32_SPI_NSS_PIN_AF     GPIO_AF_5
#define GD25Q32_SPI_DMA_DCU        RCU_DMA1
#define GD25Q32_SPI_DMA            DMA1
#define GD25Q32_SPI_DMA_TXCH       DMA_CH4
#define GD25Q32_SPI_DMA_RXCH       DMA_CH3
#define GD25Q32_SPI_RX_DMA         GD25Q32_SPI_DMA,GD25Q32_SPI_DMA_RXCH
#define GD25Q32_SPI_TX_DMA         GD25Q32_SPI_DMA,GD25Q32_SPI_DMA_TXCH
#define GD25Q32_SPI_DMA_SUBPERI    DMA_SUBPERI2

// #define GD25Q32_SPI_TXDMA_IRQHandler       DMA0_Channel5_IRQHandler
// #define GD25Q32_SPI_TXDMA_IRQn             DMA0_Channel5_IRQn
// #define GD25Q32_SPI_IRQHandler             SPI2_IRQHandler
// #define GD25Q32_SPI_IRQn                   SPI2_IRQn


extern uint8_t GD25Q32_spi_send_array[];
extern uint8_t GD25Q32_spi_receive_array[];

#define GD25Q32_CRITICAL_SECTION_START (__enable_irq())
#define GD25Q32_CRITICAL_SECTION_STOP  (__disable_irq())
/*
* Transmitter SPI DMA buffer
*/
#define GD25Q32_TX_BUF GD25Q32_spi_send_array
/*
* Receiver SPI DMA buffer
*/
#define GD25Q32_RX_BUF GD25Q32_spi_receive_array
/*
* Transaction complete flag
*/
#define GD25Q32_IS_TRANSACTION_DONE (spi_i2s_flag_get(GD25Q32_SPI, I2S_FLAG_TRANS) == 0)
#define GD25Q32_IS_TRANSACTION_RX_DONE (dma_flag_get(GD25Q32_SPI_RX_DMA, DMA_FLAG_FTF) == 1)
#define GD25Q32_IS_TRANSACTION_TX_DONE (dma_flag_get(GD25Q32_SPI_TX_DMA, DMA_FLAG_FTF) == 1)

/*
* Transaction close procedure
*/
#define GD25Q32_TRANSACTION_CLOSE   (GD25Q32_DMA_disable())
/*
* Send data
* @param  {uint8_t* SRC }  pointer to source array,
* @param  {Typedef_gd25q32Cmd cmd }  Command structure
*/
#define GD25Q32_SEND(SRC, CMD)      (GD25Q32_sendData(SRC, CMD.req_sz, CMD.res_sz))
/*
* Read data from receive buffer
* @param  {uint8_t* DST }  pointer to destination array,
* @param  {size_t OFFSET }  data offset size,
* @param  {size_t   SIZE }  data size
*/
#define GD25Q32_READ_BUFF(DST, OFFSET, SIZE) (memcpy(DST, GD25Q32_RX_BUF + OFFSET, SIZE))

void GD25Q32_hard_init();
uint8_t *  GD25Q32_sendData(uint8_t * src, size_t req_sz, size_t res_sz );
void GD25Q32_DMA_disable();
void GD25Q32_GetDataFromRxBuff(uint8_t * dst, size_t offset, size_t size);
bool GD25Q32_await_transaction_end();


#endif