#ifndef METER_H_
#define METER_H_

#include <stddef.h>
#include <stdint.h>

#include "masterTransport.h"

typedef struct meter
{
    int16_t value_disp; 
}Typedef_MeterEntry;


typedef struct
{

    uint16_t adr;
    uint16_t enable;

    TypedefEnum_MasterTransportSates state;
    
    Typedef_MeterEntry Power_re;
    Typedef_MeterEntry Power_im;
    Typedef_MeterEntry Power_s;
    Typedef_MeterEntry U[3];
    Typedef_MeterEntry I[3];

    void (*read) (void * );

} Typedef_Meter;


void meter_read(void*);

#endif