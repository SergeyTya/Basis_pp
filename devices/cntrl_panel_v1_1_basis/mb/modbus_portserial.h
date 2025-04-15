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
#define RS_GPIO_TX_AF             GPIO_AF_7
#define RS_TX_PIN                 GPIO_PIN_10

#define RS_GPIO_RX_CLK            RCU_GPIOC
#define RS_GPIO_RX_PORT           GPIOC
#define RS_GPIO_RX_AF             GPIO_AF_7
#define RS_RX_PIN                 GPIO_PIN_11

#define RS_GPIO_CTL_CLK           RCU_GPIOB
#define RS_GPIO_CTL_PORT          GPIOB
#define RS_CTL_PIN                GPIO_PIN_8

#define RS_USART_CLK              RCU_USART2
#define RS_USART                  USART2
#define RS_USART_IRQHandler       USART2_IRQHandler
#define RS_USART_IRQn             USART2_IRQn

#define RS_DMA_DCU                RCU_DMA0
#define RS_DMA                    DMA0
#define RS_DMA_TXCH               DMA_CH3
#define RS_DMA_RXCH               DMA_CH1
#define RS_RX_DMA                 RS_DMA,RS_DMA_RXCH
#define RS_TX_DMA                 RS_DMA,RS_DMA_TXCH
#define RS_DMA_SUBPERI            DMA_SUBPERI4
#define RS_USATR_DATA_ADR         (USART2 + 4U)



void usb_modbus_callback(const uint8_t* Buf, const uint32_t *Len);

#endif
