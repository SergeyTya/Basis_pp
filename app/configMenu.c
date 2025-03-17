#include "task_panel.h"

const uint32_t speed_options[] = {9600, 38400, 115200, 230400};
const uint32_t brigh_options[] = {25, 50, 75, 1000};
const uint32_t onof_options[] = {0,1};
const uint32_t par_options[] = {0,1};

const TypeDef_ConfigMenuItem nullMenuItem = {.label = "Null pointer"};
size_t configMenuSize = 10;
TypeDef_ConfigMenuItem configMenu[] = {
    {.label="Brightness", .val=&panelConfig.brightness, .options=brigh_options, .options_len=sizeof(brigh_options)},
    {.label="Addres", .val=&panelConfig.addres, .limHi = 250, .limLo=1,.enableLim = true},
    {.label="RTU speed", .val=&panelConfig.modbus_RTU.speed, .options=speed_options, .options_len=sizeof(speed_options)},
    {.label="RTU parity", .val=&panelConfig.modbus_RTU.parity, .options=par_options, .options_len=sizeof(par_options)},
    {.label="Master Spd", .val=&panelConfig.modbus_master.speed, .options=speed_options, .options_len=sizeof(speed_options)},
    {.label="Master Par", .val=&panelConfig.modbus_master.parity, .options=par_options, .options_len=sizeof(par_options)},
    {.label="AC1 enbl", .val=&panelConfig.enableAC1, .options=onof_options, .options_len=sizeof(onof_options) },
    {.label="AC2 enbl", .val=&panelConfig.enableAC2, .options=onof_options, .options_len=sizeof(onof_options)},
    {.label="DC1 enbl", .val=&panelConfig.enableDC1, .options=onof_options, .options_len=sizeof(onof_options)},
    {.label="DC2 enbl", .val=&panelConfig.enableDC2, .options=onof_options, .options_len=sizeof(onof_options)}
};
