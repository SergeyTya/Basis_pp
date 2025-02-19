#ifndef AD779x_OS_H_
#define AD779x_OS_H_

#include "stdint.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void AD7793_Init_async();
uint32_t AD7793_ReadChannel_async(uint32_t chn);

#endif