#ifndef TASK_MASTER_H
#define TASK_MASTER_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

void vTask_Master(__attribute__((unused)) void *argument);

#endif