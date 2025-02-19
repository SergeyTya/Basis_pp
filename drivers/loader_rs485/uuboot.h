//
// Created by Sergey on 03.12.2022.
//

#ifndef UUBOOT_H
#define UUBOOT_H

#include "stdlib.h"
#include <stdint.h>
#include <stdbool.h>


#define UUB_INF_CHAR 'I'
#define UUB_ERS_CHAR 'E'
#define UUB_PRG_CHAR 'P'
#define UUB_VRF_CHAR 'V'
#define UUB_AQN_CHAR 'A'
#define UUB_RST_CHAR 'R'
#define UUB_BAD_CHAR 'B'
#define UUB_CID_CHAR 'M'
#define UUB_CRC_CHAR 'X'

void vBootloader_run(void);

// Portable base address for fw storing
extern const char cpu_id[9];
extern void UUBport_ConfigHw();

/**
 * Read one byte from transport. 
 * @retval {uint8_t}  data
*/
extern  uint8_t UUBport_ReceiveByte(void);

/**
 * Put one byte to transport. 
 * @param {uint8_t} data
 * @param {size_t} len
*/
extern void UUBport_Send(const unsigned char * data, size_t len);

/**
 * Write byte array to EEPROM.
 * @param {uint32_t} memory address value
 * @param {uint32_t} data
 * @param {uint32_t} array size
 */
extern void UUBport_WriteArrayToEEPROM(uint32_t adr, uint8_t * data, size_t len);

/**
* Erase EEPROM. Function do not have to use any flash stored functions
*/
extern bool UUBport_EraseEEPROM();

/**
 * Read one byte from transport. 
 * @retval {uint8_t}  data
*/
extern  void UUBport_RebootHW(void);


extern  unsigned char * UUBport_GetID(void);

#endif
