#include "clock.h"

Typedef_Clock clock = {
.adr=8,
.enable=1,
.read = Clock_read
};

__attribute__((weak)) void Clock_read(void * c){
   // ((Typedef_Clock *) c)->enable = 1;

    Typedef_Clock * clock =((Typedef_Clock *) c);

    clock->enable = 0;
    clock->time.timestamp_second = 0;
    clock->time.timestamp_minute = 0;
    clock->time.timestamp_hour  = 0;
    
    clock->time.timestamp_hour =  0;
    clock->time.timestamp_day   = 0;
    clock->time.timestamp_month = 0;
    clock->time.timestamp_year  = 0;
}


void Clock_get(Typedef_RtcTimeStamp * time)
{
    *time = clock.time;
}
