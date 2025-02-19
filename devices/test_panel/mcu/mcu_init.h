//
// Created by Sergey Tyagushev on 05.01.2023.
//

#ifndef ESC6_STM32F4XX_H
#define ESC6_STM32F4XX_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "dev_cnfg.h"


#ifndef __RAMFUNC
#define __RAMFUNC __attribute__((section(".RamFunc")))
#endif

#define SET_REDLED HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
#define SET_GRNLED HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET) ;

#define RESET_REDLED HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
#define RESET_GRNLED HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

#define    DWT_CYCCNT    *(volatile unsigned long *)0xE0001004
#define    DWT_CONTROL   *(volatile unsigned long *)0xE0001000
#define    SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC

  void vMCU_init();
  void runAlivePin();
  void vEnableIRQ();


#endif 

