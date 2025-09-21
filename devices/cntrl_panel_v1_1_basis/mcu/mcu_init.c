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

  rcu_periph_clock_enable(RCU_GPIOG);

  //LEDS
  GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_4);
  GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_5);
  GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_6);
  GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_7);

  gpio_bit_set(GPIOG, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);


  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

  // hw revision
  rcu_periph_clock_enable(RCU_GPIOC);
  gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
  gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);





  //Temp AI
  rcu_periph_clock_enable(RCU_ADC0);
  /* config ADC clock */
  adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
  adc_deinit();
  /* ADC mode config */
  adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
  /* ADC contineous function disable */
  adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
  /* ADC scan mode disable */
  adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
  /* ADC data alignment config */
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  /* ADC channel length config */
  adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1U);

  /* ADC trigger config */
  adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
  adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

  /* enable ADC interface */
  adc_enable(ADC0);
  /* ADC calibration and reset calibration */
  adc_calibration_enable(ADC0);
  __enable_irq();

  rcu_periph_clock_enable(RCU_GPIOE);
  //RDO 1-5
  gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

  // all RDO disabled
  gpio_bit_set(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

}


void hwDriveHartBit_led1() {
  gpio_bit_toggle(HART_BIT_PIN1);
}

void hwDriveHartBit_led2() {
  gpio_bit_toggle(HART_BIT_PIN2);
}

void delay() {
  for (int i = 0; i < SystemCoreClock / 10; i++) {
    __NOP();
  }
}

void HardFault_Handler(void)
{
  /* if Hard Fault exception occurs, go to infinite loop */
  while (1) {
    delay();
    gpio_bit_toggle(HART_BIT_PIN1);
    gpio_bit_toggle(HART_BIT_PIN2);
    gpio_bit_toggle(HART_BIT_PIN3);
    gpio_bit_toggle(HART_BIT_PIN4);
  }
}


void hw_hbl_set(int led, uint8_t state) {

  int leds[4][2] = { {HART_BIT_PIN1}, {HART_BIT_PIN2}, {HART_BIT_PIN3}, {HART_BIT_PIN4} };

  if (state != 0) {
    gpio_bit_set(leds[led][0], leds[led][1]);
  }
  else {
    gpio_bit_reset(leds[led][0], leds[led][1]);
  }
}

