#ifndef SRC_MCUINIT_H_BUILD_NAME_M
#define SRC_MCUINIT_H_BUILD_NAME_M

#include "stdint.h"
#include "stdlib.h"

void hwDriveHartBit_led1();
void hwDriveHartBit_led2();


#define DEVICE_INFO        "basis_pp"

#define MCU_CLOCK           SystemCoreClock
#define RT_FREQ             20000U



#endif
