#ifndef MBHTABLE_H_
#define MBHTABLE_H_

#include "stdbool.h"

#ifndef REG_HOLDING_START
#define REG_HOLDING_START 1
#endif

#ifndef REG_INPUT_START
#define REG_INPUT_START 1
#endif

#ifndef REG_INPUT_NREGS
#define REG_INPUT_NREGS 1
#endif


#include "mbholding_type.h"



extern uint16_t shreg[2]; // "mb.c"

#define MB_GET_HOLDING_BASE_ADR(adr)           mb_getHoldingBaseArd(adr)

#define MB_GET_HOLDING_VALUE_PNTR(base_pntr)   base_pntr->pntr

#define MB_GET_HOLDING_VALUE_PNTR_BASE(base_pntr)   base_pntr->pntr_base

#define MB_GET_HOLDING_MAX(base_pntr)          base_pntr->max

#define MB_GET_HOLDING_MIN(base_pntr)          base_pntr->min

#define MB_GET_HOLDING_DEF(base_pntr)          base_pntr->def

#define MB_GET_HOLDING_LIMENBL(base_pntr)      base_pntr->lim_enbl

#define MB_GET_HOLDING_DSCRP(base_pntr)        base_pntr->desc

#define MB_GET_HOLDING_INDEX(base_pntr)        base_pntr->index

#define MB_GET_HOLDING_TYPE(base_pntr)         base_pntr->type

#define MB_GET_HOLDING_isRO(base_pntr)         base_pntr->readOnly

#define HCHECk_LIMITS(type_h, base_pntr)                         \
  {                                                              \
    type_h min_val = (type_h)MB_GET_HOLDING_MIN(base_pntr);      \
    type_h max_val = (type_h)MB_GET_HOLDING_MAX(base_pntr);      \
    type_h new_val = * ((type_h *) shreg);                       \
    if (new_val > max_val){ new_val = max_val; }                 \
    if (new_val < min_val){new_val = min_val;}                   \
    * ((type_h *) (MB_GET_HOLDING_VALUE_PNTR_BASE(base_pntr))) = new_val;  \
    (void) base_pntr->on_change( MB_GET_HOLDING_VALUE_PNTR(base_pntr) );   \
  }

 void mb_setHoldingValue(TypeDef_MB_Holding * reg_base_adr, uint16_t value);
 TypeDef_MB_Holding * mb_getHoldingBaseArd(uint16_t holding_adr);
        
#endif

