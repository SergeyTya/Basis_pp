#ifndef _MCU_CONTROL_H_
#define _MCU_CONTROL_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"

#ifndef INLINE
  #define INLINE inline __attribute__((always_inline))
#endif

extern TIM_HandleTypeDef htim1;

void hw_pwm_enable ();
void hw_pwm_disable();
void hw_set_duty(uint32_t duty[3]);

#define HW_REBOOT         (NVIC_SystemReset())

INLINE void hw_reboot(){
    NVIC_SystemReset();
}


bool hw_write_FLASH(uint16_t * src, size_t size);
void hw_read_FLASH (uint16_t * dst, size_t size);


void hw_hours_write();
void hw_hours_read();

void hw_jump_to_loader();

INLINE void hw_start_critical(){
  //__disable_irq();
}

INLINE void hw_stop_critical(){
  //__enable_irq();
}




#endif




