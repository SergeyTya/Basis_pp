#include "task_panel.h"

const uint32_t speed_options[] = {9600, 38400, 115200, 230400};

const TypeDef_ConfigMenuItem nullMenuItem = {.label = "Null pointer"};
size_t configMenuSize = 10;
TypeDef_ConfigMenuItem configMenu[] = {
    {.label="Brightness", .val=&panelConfig.brightness},
    {.label="Addres", .val=&panelConfig.addres},
    {.label="RTU speed", .val=&panelConfig.modbus_RTU.speed, .options=speed_options, .options_len=sizeof(speed_options)},
    {.label="RTU parity", .val=&panelConfig.modbus_RTU.parity},
    {.label="Master Spd", .val=&panelConfig.modbus_master.speed},
    {.label="Master Par", .val=&panelConfig.modbus_master.parity},
    {.label="AC1 enbl", .val=&panelConfig.enableAC1},
    {.label="AC2 enbl", .val=&panelConfig.enableAC2},
    {.label="DC1 enbl", .val=&panelConfig.enableDC1},
    {.label="DC2 enbl", .val=&panelConfig.enableDC2}
};
