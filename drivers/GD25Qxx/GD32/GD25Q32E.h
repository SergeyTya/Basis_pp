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

#include "GD25Q32E_port.h"

/*
* DMA BUFFER SIZE
*/
#define GD25Q32_BUF_SZ              512

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


#endif