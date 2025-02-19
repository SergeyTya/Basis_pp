/*
 * 		GD25Q32_os.h
 *
 *  GD25Q32 FreeRTOS driver.
 *
 *  Created on: 21.12.2023 г.
 *      Author: Sergey Tyagushev
 */


#ifndef GD25Q32_H_OS
#define GD25Q32_H_OS

#include "stdbool.h"

#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"


void vGD25HardWareInit();
void vGD25ReadIDAsync(uint8_t id[16]);
void vGD25ReadRDIDAsync(uint8_t id[3]);
void vGD25WriteEnableAsync();
void vGD25WriteDisableAsync();
bool blGD25ReadWIPFlagAsync();
bool blGD25ReadWELFlagAsync();
void vGD25PageProgramAsync(uint32_t adr, uint8_t page[256]);
void vGD25PageReadAsync(uint32_t adr, uint8_t page[256]);
void vGD25SectorErase(uint32_t adr);

#endif
