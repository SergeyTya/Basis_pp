
#include "stdint.h"
#include "stdlib.h"
#include "usbd_cdc_core_loopback.h"
#include "system_stm32f4xx.h"


void vMCU_init()
{

 //SRAM_Init();
 // TIM_Configuration(10);
 // ECAT_HW_Init();
 // GPIO_Config();
  


  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG , ENABLE);
 	/* configration the LED pin */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_SetBits(GPIOC, GPIO_Pin_13);

  SCB->VTOR = 0x8000000L; // setup by loader
    
 __enable_irq();
 
}

