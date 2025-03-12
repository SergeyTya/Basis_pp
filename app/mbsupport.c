#include <stdint.h>
#include "mbsupport.h"
#include "modbus_holdigs_list.h"

static uint16_t mbsupportNullPointer = 0;


uint16_t * GetHoldingPntrByAdrFromTable(uint8_t adr, TypeDef_MB_Table * table ){

    uint16_t * ret_val = &mbsupportNullPointer;

    for (size_t i = 0; i < table->len; i++) 
    { 
    if( (table->holdings[i].reg_adr & 0x00FF) == (uint16_t) adr){   
        ret_val = table->holdings[i].pntr;      
        break;                                          
        }
    };

    return ret_val;
}

uint16_t * GetHoldingPntrByAdrFromAC1(uint8_t adr ){
    return GetHoldingPntrByAdrFromTable(adr,  &AC1_TABLE);
}

uint16_t * GetHoldingPntrByAdrFromAC2(uint8_t adr ){
    return GetHoldingPntrByAdrFromTable(adr,  &AC2_TABLE);
}

uint16_t * GetHoldingPntrByAdrFromDC1(uint8_t adr ){
    return GetHoldingPntrByAdrFromTable(adr,  &DC1_TABLE);
}

uint16_t * GetHoldingPntrByAdrFromDC2(uint8_t adr ){
    return GetHoldingPntrByAdrFromTable(adr,  &DC2_TABLE);
}