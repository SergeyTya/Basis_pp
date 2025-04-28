//
// Created by Sergey Tyagushev on 11.11.2022.
//

#include "stdint.h"
#include "stdlib.h"
#include "mcu_init.h"


#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"

#define HART_BIT_PIN1 GPIOG,GPIO_PIN_4
#define HART_BIT_PIN2 GPIOG,GPIO_PIN_5



void vMCU_init()
{
  __disable_irq();

  rcu_periph_clock_enable(RCU_GPIOG);
  GD32_CONGIG_PIN_AS_OUT( GPIOG, GPIO_PIN_4 );
  GD32_CONGIG_PIN_AS_OUT( GPIOG, GPIO_PIN_5 );
  GD32_CONGIG_PIN_AS_OUT( GPIOG, GPIO_PIN_6 );
  GD32_CONGIG_PIN_AS_OUT( GPIOG, GPIO_PIN_7 );

  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

  __enable_irq();
}


void hwDriveHartBit_led1(){
  gpio_bit_toggle(HART_BIT_PIN1);
}

void hwDriveHartBit_led2(){
  gpio_bit_toggle(HART_BIT_PIN2);
}

void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}
