#ifndef MODBUS_HOLDING_LIST_H_
#define MODBUS_HOLDING_LIST_H_

#define REG_HOLDING_NREGS 2

#include "gitversion_autogen.h"
extern const char * build_info;

#include "mbholding_type.h"

#include "dev_cnfg.h"

#include "events.h"

extern uint16_t holdings[];

#define REG_HOLDINGS_TABLE_LEN 5
#define REG_HOLDINGS_TABLE_INIT {                      \
    {MB_EXTERNAL_HOLDINGS, MB_EXTERNAL_HOLDINGS_NREGS, 0x0000U},  \
    {MB_SLAVE1_HOLDING, MB_SLAVE1_HOLDING_NREGS, 0x1000U},  \
    {MB_SLAVE2_HOLDING, MB_SLAVE2_HOLDING_NREGS, 0x2000U},  \
    {MB_SLAVE3_HOLDING, MB_SLAVE3_HOLDING_NREGS, 0x3000U},  \
    {MB_SLAVE4_HOLDING, MB_SLAVE4_HOLDING_NREGS, 0x4000U}   \
}                                                      \

#define MB_EXTERNAL_HOLDINGS_NREGS 3
#define MB_EXTERNAL_HOLDINGS {                                                                                           \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/50, holdings[0],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/51, holdings[1],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy)                 /*16*/  \
\
}\

#define MB_SLAVE1_HOLDING_NREGS 3
#define MB_SLAVE1_HOLDING {                                                                                            \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x1001, holdings[0],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x1002, holdings[1],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy)                 /*16*/  \
\
}\

#define MB_SLAVE2_HOLDING_NREGS 3
#define MB_SLAVE2_HOLDING {                                                                                           \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x2001, holdings[0],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x2002, holdings[1],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy)                 /*16*/  \
\
}\

#define MB_SLAVE3_HOLDING_NREGS 3
#define MB_SLAVE3_HOLDING {                                                                                           \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x3001, holdings[0],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x3002, holdings[1],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy)                 /*16*/  \
\
}\

#define MB_SLAVE4_HOLDING_NREGS 3
#define MB_SLAVE4_HOLDING {                                                                                           \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x4001, holdings[0],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0x4002, holdings[1],        "Motor type"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy)                 /*16*/  \
\
}\

#endif