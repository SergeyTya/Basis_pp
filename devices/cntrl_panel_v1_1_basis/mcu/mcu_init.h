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

#ifndef GD32_CONGIG_PIN_AS_AF

#define GD32_CONGIG_PIN_AS_AF(PORT, AF, PIN)                                      \
    {                                                                             \
        gpio_mode_set((PORT), GPIO_MODE_AF, GPIO_PUPD_NONE, (PIN));               \
        gpio_output_options_set((PORT), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, (PIN)); \
        gpio_af_set((PORT), AF, (PIN));                                           \
    }

#define GD32_CONGIG_PIN_AS_OUT(PORT, PIN)                                     \
    {                                                                         \
        gpio_mode_set(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN);           \
        gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN); \
    }

#endif

#endif 

