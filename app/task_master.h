#ifndef TASK_MASTER_H
#define TASK_MASTER_H

#include "main.h"

void vTask_Master(__attribute__((unused)) void *argument);

typedef enum
{

    MASTERSTATE_NOERROR = 0,
    MASTERSTATE_ERROR = 1,
    MASTERSTATE_TIMEOUT = 2,
    MASTERSTATE_CRCERROR = 3

} TypedefEnum_MasterSates;

#endif