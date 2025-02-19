
#ifndef _TASK_MEM_H_
#define _TASK_MEM_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "mcu_control.h"

bool save_holdings();
void load_holdings();
void default_holdings();

#endif