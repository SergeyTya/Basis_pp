/*
 * 		GD25Q32E.h
 *
 *  GD25Q32 hardware driver for GD32F4xx MCU
 *
 *  Created on: 21.12.2023 г.
 *      Author: Sergey Tyagushev
 */

#ifndef GD25Q32_H_
#define GD25Q32_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

/*
* DMA BUFFER SIZE
*/
#define GD25Q32_BUF_SZ              512

/*
* MCU SPI HARDWARE
*/
#define GD25Q32_SPI                SPI2
#define GD25Q32_SPI_RCU            RCU_SPI2
#define GD25Q32_SPI_PORT_RCU       (RCU_GPIOB) 
#define GD25Q32_SPI_CLK_PORT       GPIOB
#define GD25Q32_SPI_CLK_PIN        GPIO_PIN_3
#define GD25Q32_SPI_CLK_PIN_AF     GPIO_AF_6
#define GD25Q32_SPI_MOSI_PORT      GPIOB
#define GD25Q32_SPI_MOSI_PIN       GPIO_PIN_5
#define GD25Q32_SPI_MOSI_PIN_AF    GPIO_AF_6
#define GD25Q32_SPI_MISO_PORT      GPIOB
#define GD25Q32_SPI_MISO_PIN       GPIO_PIN_4
#define GD25Q32_SPI_MISO_PIN_AF    GPIO_AF_6
#define GD25Q32_SPI_NSS_PORT       GPIOA
#define GD25Q32_SPI_NSS_PIN        GPIO_PIN_15
#define GD25Q32_SPI_NSS_PIN_AF     GPIO_AF_6
#define GD25Q32_SPI_DMA_DCU                RCU_DMA0
#define GD25Q32_SPI_DMA                    DMA0
#define GD25Q32_SPI_DMA_TXCH               DMA_CH5
#define GD25Q32_SPI_DMA_RXCH               DMA_CH0
#define GD25Q32_SPI_RX_DMA                 GD25Q32_SPI_DMA,GD25Q32_SPI_DMA_RXCH
#define GD25Q32_SPI_TX_DMA                 GD25Q32_SPI_DMA,GD25Q32_SPI_DMA_TXCH
#define GD25Q32_SPI_DMA_SUBPERI            DMA_SUBPERI0

// #define GD25Q32_SPI_TXDMA_IRQHandler       DMA0_Channel5_IRQHandler
// #define GD25Q32_SPI_TXDMA_IRQn             DMA0_Channel5_IRQn
// #define GD25Q32_SPI_IRQHandler             SPI2_IRQHandler
// #define GD25Q32_SPI_IRQn                   SPI2_IRQn

typedef enum gd25q32Cmd{
    gd25q32Cmd_WREN  = 0x06, //WRITE ENABLE
    gd25q32Cmd_WRDI  = 0x04, //WRITE DISABLE
    gd25q32Cmd_RDSR1  = 0x05, //READ STATUS REGISTER
    gd25q32Cmd_RDSR2  = 0x35, //READ STATUS REGISTER
    gd25q32Cmd_RDSR3  = 0x15, //READ STATUS REGISTER
    gd25q32Cmd_WRSR  = 0x01, //WRITE STATUS REGISTER
    gd25q32Cmd_READ  = 0x03, //READ DATA BYTES 
    gd25q32Cmd_FREAD = 0x0B, //READ DATA BYTES AT HIGHER SPEED
    gd25q32Cmd_SBW   = 0x77, //SET BURST WITH WRAP (77H)
    gd25q32Cmd_PP    = 0x02, //PAGE PROGRAM (PP) (02H) 
    gd25q32Cmd_FPP   = 0x02, //FAST PAGE PROGRAM (FPP) (F2H)
    gd25q32Cmd_SE    = 0x20, //SECTOR ERASE (SE) (20H)
    gd25q32Cmd_BE32  = 0x52, //32KB BLOCK ERASE (BE) (52H)
    gd25q32Cmd_BE64  = 0xD8, //64KB BLOCK ERASE (BE) (D8H)
    gd25q32Cmd_CE    = 0x60, //CHIP ERASE (CE) (60/C7H)
    gd25q32Cmd_DP    = 0xB9, //DEEP POWER-DOWN (DP) (B9H)
    gd25q32Cmd_RDI   = 0xAB, //RELEASE FROM DEEP POWER-DOWN OR HIGH PERFORMANCE MODE AND READ DEVICE ID
    gd25q32Cmd_REMS  = 0x90, //READ MANUFACTURE ID/ DEVICE ID (REMS) (90H)
    gd25q32Cmd_RDID  = 0x9F, //READ IDENTIFICATION (RDID) (9FH)
    gd25q32Cmd_HPM   = 0xA3, //HIGH PERFORMANCE MODE (HPM) (A3H)
    gd25q32Cmd_ID    = 0x4B, //READ UNIQUE ID (4BH)
    gd25q32Cmd_PES   = 0x75, //PROGRAM/ERASE SUSPEND (PES) 
    gd25q32Cmd_PER   = 0x7A, //PROGRAM/ERASE RESUME (PER)
    gd25q32Cmd_ESR   = 0x44, //ERASE SECURITY REGISTERS (44H)
    gd25q32Cmd_PSR   = 0x42,  //PROGRAM SECURITY REGISTERS (42H)
    gd25q32Cmd_RSR   = 0x48,  //READ SECURITY REGISTERS (48H)
    gd25q32Cmd_RESET = 0x66,  //ENABLE RESET (66H) RESET (99H)
    gd25q32Cmd_RSFDP = 0x5A   //READ SERIAL FLASH DISCOVERABLE PARAMETER (5AH)
}Typedef_gd25q32CmdID;


typedef struct {
    Typedef_gd25q32CmdID id; // Command ID
    size_t req_sz;              // Request size
    size_t res_sz;              // Response size
                                /* total SPI frame size  =  req_sz + res_sz */
}Typedef_gd25q32Cmd;


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


/*
* Read Unique ID (4BH)
*/
#define  GD25Q32_COMMAND_READ_RDID {.id=gd25q32Cmd_RDID, .req_sz=1, .res_sz=3 } 
/*
* Read Unique ID (4BH)
*/
#define  GD25Q32_COMMAND_READ_ID {.id=gd25q32Cmd_ID, .req_sz=5, .res_sz=16 } 
/*
* Write enable (06H)
*/
#define  GD25Q32_COMMAND_WREN {.id=gd25q32Cmd_WREN, .req_sz=1,  .res_sz=0 } 
/*
* Write disable (04H)
*/
#define  GD25Q32_COMMAND_WRDI {.id=gd25q32Cmd_WRDI, .req_sz=1,  .res_sz=0 } 
/*
* Read Status Register (RDSR) 05H
*/
#define  GD25Q32_COMMAND_RDSR1 {.id=gd25q32Cmd_RDSR1, .req_sz=1,  .res_sz=1 } 
/*
* Read Status Register (RDSR) 35H
*/
#define  GD25Q32_COMMAND_RDSR2 {.id=gd25q32Cmd_RDSR2, .req_sz=1,  .res_sz=1 } 
/*
* Read Status Register (RDSR) 15H
*/
#define  GD25Q32_COMMAND_RDSR3 {.id=gd25q32Cmd_RDSR3, .req_sz=1,  .res_sz=1 } 
/*
* 32KB Block Erase (BE) (52H) 
*/
#define  GD25Q32_COMMAND_BE32 {.id=gd25q32Cmd_BE32, .req_sz=4,  .res_sz=0 } 
/*
*  64KB Block Erase (BE) (D8H)
*/
#define  GD25Q32_COMMAND_BE64 {.id=gd25q32Cmd_BE64, .req_sz=4,  .res_sz=0 } 
/*
*  Page Program (PP) (02H) 
*/
#define  GD25Q32_COMMAND_PW {.id=gd25q32Cmd_PP, .req_sz=260,   .res_sz=0 } 
/*
*  Page Read (PP) (03H) 
*/
#define  GD25Q32_COMMAND_PR {.id=gd25q32Cmd_READ, .req_sz=4,  .res_sz=256} 
/*
* SECTOR ERASE (SE) (20H)
*/
#define GD25Q32_COMMAND_SE  {.id=gd25q32Cmd_SE, .req_sz=4,  .res_sz=0} 

void GD25Q32_hard_init();
uint8_t *  GD25Q32_sendData(uint8_t * src, size_t req_sz, size_t res_sz );
void GD25Q32_DMA_disable();
void GD25Q32_GetDataFromRxBuff(uint8_t * dst, size_t offset, size_t size);
bool GD25Q32_await_transaction_end();


#endif