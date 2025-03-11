#ifndef MBSUPPORT_H_
#define MB_SUPPORT_H


#include <stdint.h>
#include "modbus_holdigs_list.h"

extern TypeDef_MB_Table holdings_table[];

#define AC1_TABLE holdings_table[1]
#define AC2_TABLE holdings_table[2]
#define DC1_TABLE holdings_table[3]
#define DC2_TABLE holdings_table[4]

#define GET_HOLDING_PNTR_BY_ADR_FROM_AC1(adr) (GetHoldingPntrByAdrFromTable(adr, &AC1_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_AC1(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_AC1(adr))

#define GET_HOLDING_PNTR_BY_ADR_FROM_AC2(adr) (GetHoldingPntrByAdrFromTable(adr, &AC2_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_AC2(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_AC2(adr))

#define GET_HOLDING_PNTR_BY_ADR_FROM_DC1(adr) (GetHoldingPntrByAdrFromTable(adr, &DC1_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_DC1(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_DC1(adr))

#define GET_HOLDING_PNTR_BY_ADR_FROM_DC2(adr) (GetHoldingPntrByAdrFromTable(adr, &DC2_TABLE))
#define GET_HOLDING_VALUE_BY_ADR_FROM_DC2(adr) (*GET_HOLDING_PNTR_BY_ADR_FROM_DC2(adr))

uint16_t * GetHoldingPntrByAdrFromTable(uint8_t adr, TypeDef_MB_Table * table );


#endif