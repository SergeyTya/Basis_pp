/*
 * 			FSESC 6.7
 *
 * MCUinit.h
 *
 */



#ifndef SRC_MCUINIT_H_BUILD_NAME_MPCH1k5_435221005
#define SRC_MCUINIT_H_BUILD_NAME_MPCH1k5_435221005

#include "stdint.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "mcu_control.h"


#define DEVICE_INFO        "FSEC6_7_mini"

#define MCU_CLOCK           SystemCoreClock
#define RT_FREQ            20000U

#define PWM_MAX             (TIM1->ARR)

/*Form Vesc 6.0 SW HW_60*/
#define V_REG               (   3.3f)
#define CURRENT_SHUNT_RES   (0.0005f)
#define CURRENT_AMP_GAIN	(  -20.f)
#define VIN_R1				(39000.f)
#define VIN_R2				( 2200.f)
/*****/

#define DEV_CONFIG_CURRENT_SCALE  ((float) ((V_REG / 4095.f) / (CURRENT_SHUNT_RES * CURRENT_AMP_GAIN)))
#define DEV_CONFIG_BUS_SCALE      ( 0.0154f       )
#define DEV_CONFIG_BUS_OFFSET_RAW ( 0             )

#define DEV_CONFIG_ADC_CH_I1   0
#define DEV_CONFIG_ADC_CH_I2   5
#define DEV_CONFIG_ADC_CH_I3   1
#define DEV_CONFIG_ADC_CH_UDC  6

#define DEV_CONFIG_SHORTCURRENT_LIMIT 60.f

#define DEV_CONFIG_BUS_RATED  24.f//24.f

#define DEV_CONFIG_BUS_LO     20.f
#define DEV_CONFIG_BUS_HI     26.f

/*********************/

#define DEV_CONFIG_CURRENT_LOOP_FREQ (10000.f)
#define DEV_CONFIG_INERTIA           (2E-8   )


#endif
