#ifndef METER_H_
#define METER_H_

#include <stddef.h>
#include <stdint.h>

#include "masterTransport.h"

#define CONFIG_METER_ADR 7

typedef struct __attribute__((packed)) 
{
    int16_t value_disp; 
}Typedef_MeterEntry;


typedef struct
{

    uint16_t adr;
    uint32_t enable;

    TypedefEnum_MasterTransportSates state;
    
    Typedef_MeterEntry Power_re;
    Typedef_MeterEntry Power_im;
    Typedef_MeterEntry Power_s;
    Typedef_MeterEntry U[3];
    Typedef_MeterEntry I[3];

    void (*read) (void * );

} Typedef_Meter;

extern Typedef_Meter meter;

void meter_read(void*);
void meter_read_elink(void * m);

#endif