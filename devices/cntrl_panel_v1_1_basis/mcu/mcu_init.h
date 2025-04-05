//
// Created by Sergey Tyagushev on 05.01.2023.
//

#ifndef PANEL_GD32F470_H
#define PANEL_GD32F470_H

/* Includes ------------------------------------------------------------------*/

#include "dev_cnfg.h"

#ifndef __RAMFUNC
#define __RAMFUNC __attribute__((section(".RamFunc")))
#endif

  void vMCU_init();
  void runAlivePin();
  void vEnableIRQ();
  uint8_t spi_drv_read();


#endif 

