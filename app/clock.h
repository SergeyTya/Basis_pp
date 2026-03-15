#ifndef TASK_CLOCK_H
#define TASK_CLOCK_H

#include <stdint.h>
#include "task_logger.h"
#include "masterTransport.h"



typedef struct
{

    uint16_t adr;
    uint32_t enable;

    TypedefEnum_MasterTransportSates state;   
    Typedef_RtcTimeStamp time;
 
    void (*read) (void * );

} Typedef_Clock;


void Clock_read(void*);

void vTask_Clock();
void Clock_get(Typedef_RtcTimeStamp * time);

#endif