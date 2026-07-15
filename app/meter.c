#include "meter.h"

Typedef_Meter meter = { 
    .adr=7, 
    .enable=1, 
    .read = meter_read
};


__attribute__((weak)) void meter_read(void * m){
    ((Typedef_Meter *)m)->enable = 0;
}

