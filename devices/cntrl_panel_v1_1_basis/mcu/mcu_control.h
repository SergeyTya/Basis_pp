#ifndef _MCU_CONTROL_H_
#define _MCU_CONTROL_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "gd32f4xx_libopt.h"

#ifndef INLINE
  #define INLINE inline __attribute__((always_inline))
#endif

bool hw_write_FLASH(uint32_t * src, size_t size);
void hw_read_FLASH (uint32_t * dst, size_t size);

void hw_hours_write();
void hw_hours_read();
uint8_t hw_get_revision();

uint16_t hw_read_AI();


#define HW_START_CRITICAL (__enable_irq())
#define HW_STOP_CRITICAL  (__disable_irq())
#define HW_REBOOT         (NVIC_SystemReset())

#endif




