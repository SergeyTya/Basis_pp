#ifndef M204D08AA__ 
#define M204D08AA__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

void M204D08AA_HardInit();
void M204D08AA_STB_SetState(bool state);
void M204D08AA_STB_WriteWord(uint16_t data);

#endif