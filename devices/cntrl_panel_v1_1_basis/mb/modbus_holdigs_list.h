#ifndef MODBUS_HOLDING_LIST_H_
#define MODBUS_HOLDING_LIST_H_

#define MB_TABLE_ENTRYS_MAX 52

#include "gitversion_autogen.h"
extern const char * build_info;

#include "mbholding_type.h"

#include "dev_cnfg.h"

#include "events.h"

extern uint16_t holdings[];
extern void events_dummy();

#define REG_HOLDINGS_TABLE_LEN 5
#define REG_HOLDINGS_TABLE_INIT {                                 \
    {MB_EXTERNAL_HOLDINGS, MB_EXTERNAL_HOLDINGS_NREGS, 0x0000U},  \
    {MB_SLAVE_AC_HOLDING(MB_SLAVE_AC1_HOLDING_SHIFT, MB_SLAVE_AC1_PNTR_SHIFT), MB_SLAVE_AC_HOLDING_NREGS, MB_SLAVE_AC1_HOLDING_SHIFT}, \
    {MB_SLAVE_AC_HOLDING(MB_SLAVE_AC2_HOLDING_SHIFT, MB_SLAVE_AC2_PNTR_SHIFT), MB_SLAVE_AC_HOLDING_NREGS, MB_SLAVE_AC2_HOLDING_SHIFT}, \
    {MB_SLAVE_DC_HOLDING(MB_SLAVE_DC1_HOLDING_SHIFT, MB_SLAVE_DC1_PNTR_SHIFT), MB_SLAVE_DC_HOLDING_NREGS, MB_SLAVE_DC1_HOLDING_SHIFT}, \
    {MB_SLAVE_DC_HOLDING(MB_SLAVE_DC2_HOLDING_SHIFT, MB_SLAVE_DC2_PNTR_SHIFT), MB_SLAVE_DC_HOLDING_NREGS, MB_SLAVE_DC2_HOLDING_SHIFT}  \
}                                                                 \

#define MB_EXTERNAL_HOLDINGS_NREGS 9
#define MB_EXTERNAL_HOLDINGS {                                                                                           \
                                                                                                                    \
HR_CREATE_UINT16_PARAM  (/*ADR=*/0, holdings[1],        "Freq ref"            ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                \
HR_CREATE_UINT16_PARAM  (/*ADR=*/1, holdings[2],        "Volt ref"            ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                \
HR_CREATE_UINT16_PARAM  (/*ADR=*/2, holdings[0],        "Control word"        ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),                \
HR_CREATE_UINT16_RO     (/*ADR=*/3, holdings[3],        "Volt U"            ),                                                                                               \
HR_CREATE_UINT16_RO     (/*ADR=*/4, holdings[4],        "Volt I"            ),                                                                                               \
HR_CREATE_UINT16_RO     (/*ADR=*/5, holdings[5],        "Volt W"            ),                                                                                               \
HR_CREATE_UINT16_RO     (/*ADR=*/6, holdings[6],        "Curr I"            ),                                                                                               \
HR_CREATE_UINT16_RO     (/*ADR=*/7, holdings[7],        "Curr U"            ),                                                                                               \
HR_CREATE_UINT16_RO     (/*ADR=*/8, holdings[8],        "Curr W"            )                                                                                                       \
}\

#define MB_SLAVE_AC1_HOLDING_SHIFT 0x1000U
#define MB_SLAVE_AC2_HOLDING_SHIFT 0x2000U

#define MB_SLAVE_AC1_PNTR_SHIFT    30U
#define MB_SLAVE_AC2_PNTR_SHIFT    60U

#define MB_SLAVE_AC_HOLDING_NREGS 25
#define MB_SLAVE_AC_HOLDING(hld_sft,pntr_shft) {                                                                                     \
    HR_CREATE_UINT16_PARAM  (/*ADR=*/(hld_sft + 101U), holdings[pntr_shft+0],  "RefF" ,/*min=*/      0,/*max=*/      3, /*def=*/     3,  /*on_change=*/events_dummy),    \
    HR_CREATE_UINT16_PARAM  (/*ADR=*/(hld_sft + 102U), holdings[pntr_shft+1],  "RefU" ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),      \
    HR_CREATE_UINT16_PARAM  (/*ADR=*/(hld_sft + 103U), holdings[pntr_shft+2],  "Ctrl" ,/*min=*/      0,/*max=*/      4, /*def=*/     1,  /*on_change=*/events_dummy),       \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 110U), holdings[pntr_shft+3],  "Ku",  /*def=*/  0, /*on_change=*/events_dummy),      \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 112U), holdings[pntr_shft+5],  "" ,/*def=*/     0, /*on_change=*/events_dummy),                /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 111U), holdings[pntr_shft+4],  "" ,/*def=*/     0, /*on_change=*/events_dummy),      \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 113U), holdings[pntr_shft+6],  "" ,/*def=*/     0, /*on_change=*/events_dummy),                 /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 114U), holdings[pntr_shft+7],  "" ,/*def=*/     0, /*on_change=*/events_dummy),                   /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 121U), holdings[pntr_shft+8],  "" ,/*def=*/     0, /*on_change=*/events_dummy),                 /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 122U), holdings[pntr_shft+9],  "" ,/*def=*/     0, /*on_change=*/events_dummy),              /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 124U), holdings[pntr_shft+10], "" ,/*def=*/     0, /*on_change=*/events_dummy),               /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 125U), holdings[pntr_shft+11], "" ,/*def=*/     0, /*on_change=*/events_dummy),               /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 126U), holdings[pntr_shft+12], "" ,/*def=*/     0, /*on_change=*/events_dummy),                /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 130U), holdings[pntr_shft+13], "" ,/*def=*/     0, /*on_change=*/events_dummy),               /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 131U), holdings[pntr_shft+14], "" ,/*def=*/     0, /*on_change=*/events_dummy),                /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 132U), holdings[pntr_shft+15], "" ,/*def=*/     0, /*on_change=*/events_dummy),               /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 133U), holdings[pntr_shft+16], "" ,/*def=*/     0, /*on_change=*/events_dummy),                /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 140U), holdings[pntr_shft+17], "" ,/*def=*/     0, /*on_change=*/events_dummy),               /*112*/  \
    HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/(hld_sft + 141U), holdings[pntr_shft+18], "" ,/*def=*/     0, /*on_change=*/events_dummy),                /*112*/  \
    HR_CREATE_UINT16_RO     (/*ADR=*/(hld_sft + 240U), holdings[pntr_shft+19], "Uu"), /*112*/  \
    HR_CREATE_UINT16_RO     (/*ADR=*/(hld_sft + 241U), holdings[pntr_shft+20], "Uv"), /*112*/  \
    HR_CREATE_UINT16_RO     (/*ADR=*/(hld_sft + 242U), holdings[pntr_shft+21], "Uw"), /*112*/  \
    HR_CREATE_UINT16_RO     (/*ADR=*/(hld_sft + 243U), holdings[pntr_shft+22], "Iu"), /*112*/  \
    HR_CREATE_UINT16_RO     (/*ADR=*/(hld_sft + 244U), holdings[pntr_shft+23], "Iv"), /*112*/  \
    HR_CREATE_UINT16_RO     (/*ADR=*/(hld_sft + 245U), holdings[pntr_shft+24], "Iw")  /*112*/  \
}\

#define MB_SLAVE_DC1_HOLDING_SHIFT 0x3000U
#define MB_SLAVE_DC2_HOLDING_SHIFT 0x4000U
#define MB_SLAVE_DC1_PNTR_SHIFT 100
#define MB_SLAVE_DC2_PNTR_SHIFT 160
#define MB_SLAVE_DC_HOLDING_NREGS 52
#define MB_SLAVE_DC_HOLDING(hld_sft,pntr_shft) {\
HR_CREATE_UINT16_PARAM      (/*ADR=*/hld_sft+100, holdings[pntr_shft+1],  "Source" ,/*min=*/      0,/*max=*/        2, /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM      (/*ADR=*/hld_sft+101, holdings[pntr_shft+2],  "Idref"  ,/*min=*/      0,/*max=*/      100, /*def=*/     0,  /*on_change=*/events_dummy),                   \
HR_CREATE_UINT16_PARAM      (/*ADR=*/hld_sft+102, holdings[pntr_shft+3],  "Udref"  ,/*min=*/      0,/*max=*/      100, /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM      (/*ADR=*/hld_sft+103, holdings[pntr_shft+4],  "Fwd"    ,/*min=*/      0,/*max=*/        1, /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM_NL   (/*ADR=*/hld_sft+104, holdings[pntr_shft+5],  "Control",                                   /*def=*/     1,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+110, holdings[pntr_shft+6],  "Id 0"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+111, holdings[pntr_shft+7],  "Ud 0"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+112, holdings[pntr_shft+8],  "AIN2"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+113, holdings[pntr_shft+9],  "Id K"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+114, holdings[pntr_shft+10], "Ud K"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+115, holdings[pntr_shft+11], "Ain2 K" ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+120, holdings[pntr_shft+12], "Idlim"  ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+121, holdings[pntr_shft+13], "UDlim"  ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+122, holdings[pntr_shft+14], "Idflt"  ),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+123, holdings[pntr_shft+15], "PWMmax" ,/*min=*/   0,/*max=*/      1800, /*def=*/     1750,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+124, holdings[pntr_shft+16], "Udp"    ,/*min=*/      0,/*max=*/         2, /*def=*/        1,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+125, holdings[pntr_shft+17], "Idp"    ,/*min=*/      0,/*max=*/         2, /*def=*/        0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+126, holdings[pntr_shft+18], "Udr"    ,/*min=*/      1,/*max=*/      9999, /*def=*/     1000,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+127, holdings[pntr_shft+19], "Idr"    ,/*min=*/      1,/*max=*/      9999, /*def=*/     1000,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+128, holdings[pntr_shft+20], "Pn"     ,/*min=*/      1,/*max=*/      9999, /*def=*/     1000,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+129, holdings[pntr_shft+21], "Pt"     ,/*min=*/      1,/*max=*/      9999, /*def=*/     1000,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+134, holdings[pntr_shft+22], "Pwmfb"  ,/*min=*/      1,/*max=*/      9999, /*def=*/      250,   /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+135, holdings[pntr_shft+23], "Id0li"  ,/*min=*/      5,/*max=*/       200, /*def=*/       20,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+130, holdings[pntr_shft+24], "Idgain" ,/*min=*/      0,/*max=*/       999, /*def=*/       10,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+131, holdings[pntr_shft+25], "Udgain" ,/*min=*/      0,/*max=*/       999, /*def=*/       20,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+132, holdings[pntr_shft+26], "Idtime" ,/*min=*/      0,/*max=*/       999, /*def=*/       20,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+133, holdings[pntr_shft+27], "Udtime" ,/*min=*/      0,/*max=*/       999, /*def=*/       20,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/hld_sft+140, holdings[pntr_shft+28], "Fault" ,                                  /*def=*/        1,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+141, holdings[pntr_shft+29],         "" ,/*min=*/      0,/*max=*/       3, /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/hld_sft+150, holdings[pntr_shft+30], "Din" ,                                    /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/hld_sft+160, holdings[pntr_shft+31], "Dout" ,                                   /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/hld_sft+161, holdings[pntr_shft+32], "Dout" ,                                   /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM_NL  (/*ADR=*/hld_sft+170, holdings[pntr_shft+33], "Pcntl",                                   /*def=*/     0,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+171, holdings[pntr_shft+34], "P1spd" ,/*min=*/          1,/*max=*/      7, /*def=*/     7,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+172, holdings[pntr_shft+35], "P1par" ,/*min=*/          1,/*max=*/      3, /*def=*/     2,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+173, holdings[pntr_shft+36], "P2spd" ,/*min=*/          1,/*max=*/      7,  /*def=*/    7,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+174, holdings[pntr_shft+37], "P2par" ,/*min=*/          1,/*max=*/      3, /*def=*/     2,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_PARAM  (/*ADR=*/hld_sft+175, holdings[pntr_shft+38], "Adr"   ,/*min=*/          1,/*max=*/    240, /*def=*/     1,  /*on_change=*/events_dummy),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+180, holdings[pntr_shft+39], "Idraw"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+181, holdings[pntr_shft+40], "Udraw"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+182, holdings[pntr_shft+41], "Ain2raw" ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+190, holdings[pntr_shft+42], "State"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+200, holdings[pntr_shft+43], "Dout"    ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+210, holdings[pntr_shft+44], "Id"      ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+211, holdings[pntr_shft+45], "Ud"      ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+212, holdings[pntr_shft+46], "Idref"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+213, holdings[pntr_shft+47], "Udref"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+214, holdings[pntr_shft+48], "Pd"      ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+220, holdings[pntr_shft+49], "State"   ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+230, holdings[pntr_shft+50], "FON"     ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+231, holdings[pntr_shft+51], "PWM"     ),                  \
HR_CREATE_UINT16_RO  (/*ADR=*/hld_sft+240, holdings[pntr_shft+52], "Fault"   ),                  \
\
}\

// #if MB_TABLE_ENTRYS_MAX < MB_SLAVE_DC_HOLDING_NREGS
// #error Number entries doesn't much
// #endif

// #if MB_TABLE_ENTRYS_MAX < MB_SLAVE_AC_HOLDING_NREGS
// #error Number entries doesn't much
// #endif

#endif