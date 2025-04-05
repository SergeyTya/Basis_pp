/* -----------------------MODBUS----------------------------------*/
#ifndef _PORTSERIAL_FSESC6_7_MINI_H
#define _PORTSERIAL_FSESC6_7_MINI_H

#include "stdint.h"

#define ENTER_CRITICAL_SECTION() ;
#define EXIT_CRITICAL_SECTION()  ;

#define MB_BUF_SIZE_MAX     (256U*4U)     /*!< Maximum size of a Modbus RTU frame. */

#ifndef BOOL
#define BOOL uint8_t
#define TRUE 1
#define FALSE 0
#endif

/* -----------------------MODBUS----------------------------------*/
#define RS_GPIO_TX_CLK            RCU_GPIOC
#define RS_GPIO_TX_PORT           GPIOC
#define RS_GPIO_TX_AF             GPIO_AF_8
#define RS_TX_PIN                 GPIO_PIN_6

#define RS_GPIO_RX_CLK            RCU_GPIOC
#define RS_GPIO_RX_PORT           GPIOC
#define RS_GPIO_RX_AF             GPIO_AF_8
#define RS_RX_PIN                 GPIO_PIN_7

#define RS_GPIO_CTL_CLK             RCU_GPIOD
#define RS_GPIO_CTL_PORT            GPIOD
#define RS_CTL_PIN                  GPIO_PIN_3

#define RS_USART_CLK              RCU_USART5
#define RS_USART                  USART5
#define RS_USART_IRQHandler       USART5_IRQHandler
#define RS_USART_IRQn             USART5_IRQn

#define RS_DMA_DCU                RCU_DMA1
#define RS_DMA                    DMA1
#define RS_DMA_TXCH               DMA_CH6
#define RS_DMA_RXCH               DMA_CH1
#define RS_RX_DMA                 RS_DMA,RS_DMA_RXCH
#define RS_TX_DMA                 RS_DMA,RS_DMA_TXCH
#define RS_DMA_SUBPERI            DMA_SUBPERI5
#define RS_USATR_DATA_ADR         (USART5 + 4U)

/************/

void usb_modbus_callback(const uint8_t* Buf, const uint32_t *Len);

#endif
