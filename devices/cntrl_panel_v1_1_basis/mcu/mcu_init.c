//
// Created by Sergey Tyagushev on 11.11.2022.
//

#include "stdint.h"
#include "stdlib.h"
#include "mcu_init.h"


#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"

void vMCU_init()
{
  __disable_irq();


  __enable_irq();
}
