#ifndef MODBUS_HOLDING_LIST_H_
#define MODBUS_HOLDING_LIST_H_

#define REG_HOLDING_NREGS 2

#include "gitversion_autogen.h"
extern const char * build_info;

#include "mbholding_type.h"

#include "dev_cnfg.h"

#include "events.h"

extern uint16_t holdings[];

#define REG_HOLDINGS_TABLE_LEN 1
#define REG_HOLDINGS_TABLE_INIT {                    \
    {MB_HOLDING_x00, REG_HOLDING_NREGS_x00, 0x0000U} \
}                                                    \

#define REG_HOLDING_NREGS_x00 113
#define MB_HOLDING_x00 {                                                                                           \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/50, holdings[0],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/51, holdings[1],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy)                 /*16*/  \
\
}\

#endif