#ifndef MBSUPPORT_H_
#define MB_SUPPORT_H


#include <stdint.h>
#include "modbus_holdigs_list.h"

extern TypeDef_MB_Table holdings_table[];
extern TypeDef_MB_Holding mbsupportNullHolding;

#define CONFIG_SLAVE_NONE 0
#define CONFIG_SLAVE_AC1  1
#define CONFIG_SLAVE_AC2  2
#define CONFIG_SLAVE_DC1  3
#define CONFIG_SLAVE_DC2  4

#define AC1_TABLE holdings_table[CONFIG_SLAVE_AC1]
#define AC2_TABLE holdings_table[CONFIG_SLAVE_AC2]
#define DC1_TABLE holdings_table[CONFIG_SLAVE_DC1]
#define DC2_TABLE holdings_table[CONFIG_SLAVE_DC2]

#define GET_HOLDING_PNTR_BY_ADR_FROM_AC1(adr) (GetHoldingPntrByAdrFromAC1(adr))
#define GET_HOLDING_VALUE_BY_ADR_FROM_AC1(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_AC1(adr))

#define GET_HOLDING_PNTR_BY_ADR_FROM_AC2(adr) (GetHoldingPntrByAdrFromTable(adr, &AC2_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_AC2(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_AC2(adr))

#define GET_HOLDING_PNTR_BY_ADR_FROM_DC1(adr) (GetHoldingPntrByAdrFromTable(adr, &DC1_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_DC1(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_DC1(adr))

#define GET_HOLDING_PNTR_BY_ADR_FROM_DC2(adr) (GetHoldingPntrByAdrFromTable(adr, &DC2_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_DC2(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_DC2(adr))

uint16_t * GetHoldingPntrByAdrFromTable(uint8_t adr, TypeDef_MB_Table * table );
uint16_t * GetHoldingPntrByAdrFromAC1(uint8_t adr );
uint16_t * GetHoldingPntrByAdrFromAC2(uint8_t adr );
uint16_t * GetHoldingPntrByAdrFromDC1(uint8_t adr );
uint16_t * GetHoldingPntrByAdrFromDC2(uint8_t adr );

TypeDef_MB_Holding * GetHoldingByAdrFromTable(uint8_t adr, TypeDef_MB_Table * table );
TypeDef_MB_Holding * GetHoldingByAdrFromAC1(uint8_t adr );
TypeDef_MB_Holding * GetHoldingByAdrFromAC2(uint8_t adr );
TypeDef_MB_Holding * GetHoldingByAdrFromDC1(uint8_t adr );
TypeDef_MB_Holding * GetHoldingByAdrFromDC2(uint8_t adr );

#define IS_HOLDING_NULL_POINTER( adr )  (adr == &mbsupportNullHolding)||(adr==0)?true:false

#endif