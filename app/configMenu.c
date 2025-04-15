#include "task_panel.h"
#include "M204D08AA.h"
#include "mcu_control.h"

const uint32_t speed_options[] = {9600, 38400, 115200, 230400};
const uint32_t brigh_options[] = {25, 50, 75, 100};
const uint32_t onof_options[] = {0,1};
const uint32_t par_options[] = {0,1};

void MenuItemBrightnessChangedEvent();

const TypeDef_ConfigMenuItem nullMenuItem = {.label = "Null pointer"};
const size_t configMenuSize = 10;
TypeDef_ConfigMenuItem configMenu[] = {
    {.label="Brightness", .val=&panelConfig.brightness, .options=brigh_options, .options_len=4, .itemChangedEvent = MenuItemBrightnessChangedEvent },
    {.label="Addres", .val=&panelConfig.addres, .limHi = 250, .limLo=1,.enableLim = true },
    {.label="RTU speed", .val=&panelConfig.modbus_RTU.speed, .options=speed_options, .options_len=4},
    {.label="RTU parity", .val=&panelConfig.modbus_RTU.parity, .options=par_options, .options_len=2},
    {.label="Master Spd", .val=&panelConfig.modbus_master.speed, .options=speed_options, .options_len=4},
    {.label="Master Par", .val=&panelConfig.modbus_master.parity, .options=par_options, .options_len=2},
    {.label="AC1 enbl", .val=&panelConfig.enableAC1, .options=onof_options, .options_len=2 },
    {.label="AC2 enbl", .val=&panelConfig.enableAC2, .options=onof_options, .options_len=2  },
    {.label="DC1 enbl", .val=&panelConfig.enableDC1, .options=onof_options, .options_len=2  },
    {.label="DC2 enbl", .val=&panelConfig.enableDC2, .options=onof_options, .options_len=2  }
};

void MenuItemBrightnessChangedEvent(){
    M204D08AA_SetBrightnessLevel((int) *configMenu[0].val);
    ConfigMenuSaveAll();
}

void MenuItemGeneralChangedEvent(){
    ConfigMenuSaveAll();
    HW_REBOOT;
}

void ConfigMenuSaveAll(){
  
    size_t len = configMenuSize*2;
    uint16_t data[len];
    uint32_t * pntr = (uint32_t *) data;

    for (size_t i = 0; i < configMenuSize; i++)
    {
        pntr[i] =  *configMenu[i].val;
    }
    
   hw_write_FLASH((uint16_t*) data, len); // sent to hw
}

void ConfigMenuReadAll(){

    size_t len = configMenuSize*2;
    uint16_t data[len];
    uint32_t * pntr = (uint32_t *) data;

    hw_read_FLASH((uint16_t*)pntr, len);

    for (size_t i = 0; i < configMenuSize; i++)
    {
        if(pntr[i] == 0xFFFFFFFF) return;
        *configMenu[i].val = pntr[i];
    }
}
