#ifndef MODBUS_HOLDING_LIST_H_
#define MODBUS_HOLDING_LIST_H_

#define REG_HOLDING_NREGS 110

#include "gitversion_autogen.h"
extern const char * build_info;

#include "inverter.h"   
extern TypeDef_Inverter inverter;

#include "MotorControl_interface.h"
extern MotorControlInitStructure_TypeDef motorInit;
extern MotorControl_TypeDef motor;

#include "mbholding_type.h"

#include "dev_cnfg.h"

#include "events.h"


#define MB_HOLDING_REGISTERS {                                                                                           \
                                                                                                                         \
HR_CREATE_UINT16_PARAM (inverter.control_command       , "control"  ,/*min=*/ -32768,/*max=*/32768, /*def=*/   0, /*on_change=*/events_commandPoll  ),             /*0*/ \
HR_CREATE_UINT32_RO (inverter.state_flags.value                            ,"state"                ),             /*1*/ \
HR_CREATE_FLOAT_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I1].rms_value      ,"Iu RMS  A"            ),             /*2*/ \
HR_CREATE_FLOAT_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_UDC].rms_value     ,"Udc V"                 ),             /*2*/ \
HR_CREATE_FLOAT_RO  (inverter.OUT_VAL.out                                  ,"out, pu"               ),             /*5*/ \
HR_CREATE_FLOAT_RO  (inverter.OUT_VAL.out_freq                             ,"out freq, Hz"          ),             /*7*/ \
HR_CREATE_INT16_RO  (inverter.OUT_VAL.speed_rpm                            ,"speed, rpm"            ),             /*7*/ \
HR_CREATE_FLOAT_RO  (inverter.OUT_VAL.torque                               ,"IQ (trq), pu"            ),             /*6*/ \
HR_CREATE_FLOAT_RO  (inverter.OUT_VAL.flux                                 ,"ID (flx), pu"            ),             /*6*/ \
HR_CREATE_FLOAT_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I1].value          ,"Iu, A"                 ),             /*2*/ \
HR_CREATE_FLOAT_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I2].value          ,"Iv, A"                 ),             /*2*/ \
HR_CREATE_FLOAT_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I3].value          ,"Iw, A"                 ),             /*2*/ \
HR_CREATE_INT16_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I1].RAW_unival     ,"IuRaw, A"              ),             /*2*/ \
HR_CREATE_INT16_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I2].RAW_unival     ,"IvRaw, A"              ),             /*2*/ \
HR_CREATE_INT16_RO  (inverter.sensors[DEV_CONFIG_ADC_CH_I3].RAW_unival     ,"IwRaw, A"              ),             /*2*/ \
HR_CREATE_INT16_RO  (motor.OUTPUTS.CCR[0]                                       ,"CCR1"                  ),             /*8*/ \
HR_CREATE_INT16_RO  (motor.OUTPUTS.CCR[1]                                       ,"CCR2"                  ),             /*8*/ \
HR_CREATE_INT16_RO  (motor.OUTPUTS.CCR[2]                                       ,"CCR3"                  ),             /*8*/ \
\
\
\
HR_CREATE_FLOAT_PARAM   (inverter.REF_VAL.speed_ref,  "ref speed, rpm"    ,/*min=*/-9000.f,/*max=*/9000.f, /*def=*/   0.f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motor.CONTROLS.fTorqueRef,   "trq ref, pu"       ,/*min=*/    -1.f,/*max=*/  1.f, /*def=*/  0.0f,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_FLOAT_PARAM   (inverter.REF_VAL.ramp_ref,   "ref ramp, sec"     ,/*min=*/    0.f,/*max=*/ 600.f, /*def=*/   1.f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (inverter.REF_VAL.OC_value,   "OC value, A"       ,/*min=*/    1.f,/*max=*/ 150.f, /*def=*/  20.f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (inverter.REF_VAL.OC_time,    "OC time, sec"      ,/*min=*/   0.1f,/*max=*/ 600.f, /*def=*/   1.f, /*on_change=*/events_dummy),                 /*16*/ \
\
HR_CREATE_UINT16_PARAM  (motorInit.motor_type,        "motor type"        ,/*min=*/      0,/*max=*/      3, /*def=*/     3,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_UINT16_PARAM  (motorInit.control_type,      "control type"      ,/*min=*/      0,/*max=*/      1, /*def=*/     0,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_FLOAT_PARAM   (motorInit.rated_speed,       "mtr rtd espd, 1/s" ,/*min=*/   0.1f,/*max=*/6000.f, /*def=*/ 3036.f,  /*on_change=*/events_dummy),            \
HR_CREATE_FLOAT_PARAM   (motorInit.maximum_current ,  "mtr cur max, A"    ,/*min=*/   0.1f,/*max=*/ DEV_CONFIG_SHORTCURRENT_LIMIT, /*def=*/ DEV_CONFIG_SHORTCURRENT_LIMIT/2, /*on_change=*/events_dummy),\
HR_CREATE_FLOAT_PARAM   (motorInit.maximum_speed  ,   "max speed, pu"     ,/*min=*/   0.1f,/*max=*/   3.0f, /*def=*/   1.f,  /*on_change=*/events_dummy), \
HR_CREATE_FLOAT_PARAM   (motorInit.maximum_torque,    "trq lim, pu"       ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/  0.9f,  /*on_change=*/events_dummy),                 /*16*/  \
HR_CREATE_FLOAT_PARAM   (motorInit.flux_ref,          "flx ref, pu"       ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/  0.1f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.ID_loop_gain,      "ID loop gain"      ,/*min=*/    0.f,/*max=*/  100.f, /*def=*/  0.3f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.ID_loop_time,      "ID loop time"      ,/*min=*/    0.f,/*max=*/10000.f, /*def=*/1000.f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.IQ_loop_gain,      "IQ loop gain"      ,/*min=*/    0.f,/*max=*/  100.f, /*def=*/  0.1f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.IQ_loop_time,      "IQ loop time"      ,/*min=*/    0.f,/*max=*/10000.f, /*def=*/1000.f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.speed_loop_gain,   "spd loop gain"     ,/*min=*/    0.f,/*max=*/  100.f, /*def=*/  0.1f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.speed_loop_time,   "spd loop time"     ,/*min=*/    0.f,/*max=*/10000.f, /*def=*/  10.f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.mras_loop_gain,    "Observer gain"     ,/*min=*/    0.f,/*max=*/  100.f, /*def=*/  0.8f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.mras_loop_time,    "Observer time"     ,/*min=*/    0.f,/*max=*/10000.f, /*def=*/1000.f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.pp,                "motor 2P"          ,/*min=*/    1.f,/*max=*/   16.f, /*def=*/  10.f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Rs,                "motor Rs, Ohm"     ,/*min=*/  1e-7f,/*max=*/  100.f, /*def=*/ 77e-3f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.L1,                "motor Ld, Ohm"     ,/*min=*/  1e-7f,/*max=*/  100.f, /*def=*/ 52e-6f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.L2,                "motor Lq, Ohm"     ,/*min=*/  1e-7f,/*max=*/  100.f, /*def=*/ 78e-6f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.L3,                "motor Ke, Wb"      ,/*min=*/  1e-7f,/*max=*/  100.f, /*def=*/ 50e-4f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Freq_pnt[0],       "Sclr Fr[0], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.01f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Freq_pnt[1],       "Sclr Fr[1], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.10f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Freq_pnt[2],       "Sclr Fr[2], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.7f,  /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Freq_pnt[3],       "Sclr Fr[3], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   1.f,   /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Volt_pnt[0],       "Sclr Vl[0], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.05f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Volt_pnt[1],       "Sclr Vl[1], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.05f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Volt_pnt[2],       "Sclr Vl[2], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.05f, /*on_change=*/events_dummy),                 /*16*/ \
HR_CREATE_FLOAT_PARAM   (motorInit.Volt_pnt[3],       "Sclr Vl[3], pu"    ,/*min=*/    0.f,/*max=*/    1.f, /*def=*/   0.05f, /*on_change=*/events_dummy),                 /*16*/ \
\
HR_CREATE_UINT16_PARAM  (inverter.PRM.MB_adr,         "modbus adr"         ,/*min=*/      1,/*max=*/   255, /*def=*/      1, events_dummy),               /*22*/\
HR_CREATE_UINT32_PARAM  (inverter.PRM.MB_bdr,         "modbus bdr"         ,/*min=*/ 230400,/*max=*/230400, /*def=*/ 230400, events_dummy),               /*22*/\
\
HR_CREATE_BOOL_PARAM    (inverter.PRM.CAN1_wdg_enbl,  "CAN1 WDe"  , FALSE, events_dummy             ),      /*24*/\
HR_CREATE_BOOL_PARAM    (inverter.PRM.CAN2_wdg_enbl,  "CAN2 WDe"  , FALSE, events_dummy             ),      /*25*/\
HR_CREATE_BOOL_PARAM    (inverter.PRM.MB_wdg_enbl,    "modbus WDe", FALSE, events_dummy             ),    /*26*/\
HR_CREATE_UINT16_PARAM  (inverter.PRM.wdg_time,       "WD time"   , 0 ,1000, 100, events_dummy      ),       /*27*/\
\
HR_CREATE_FLOAT_RO  (motor.puMrasValues.th_est     ,"Estim Th"                ),             /*2*/ \
HR_CREATE_FLOAT_RO  (motor.CONTROLS.fThmes         ,"Sens Th"                 ),             /*2*/ \
HR_CREATE_FLOAT_RO  (motor.Wre                     ,"Estim Wr"                ),             /*2*/ \
HR_CREATE_FLOAT_RO  (motor.CONTROLS.fWmes          ,"Sens Wr"                 ),             /*2*/ \
}\

#endif