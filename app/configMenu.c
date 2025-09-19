#include "task_panel.h"
#include "IDisplay.h"
#include "mcu_control.h"

const uint32_t speed_options[] = {9600, 38400, 115200, 230400};
const uint32_t bright_options[] = {25, 50, 75, 100};
const uint32_t onof_options[] = {/*OFF*/0, /*ON*/1};
const uint32_t par_options[] = {/*NONE*/0 , /*ODD*/1, /*EVEN*/2};


void MenuItemBrightnessChangedEvent();

const TypeDef_ConfigMenuItem nullMenuItem = {.label = "Null pointer"};
const size_t configMenuSize = 17;
TypeDef_ConfigMenuItem configMenu[] = {

    {.label="AC1 enable"  , .val=&panelConfig.enableAC1, .options=onof_options, .options_len=2 },
    {.label="AC2 enable"  , .val=&panelConfig.enableAC2, .options=onof_options, .options_len=2  },
    {.label="DC1 enable"  , .val=&panelConfig.enableDC1, .options=onof_options, .options_len=2  },
    {.label="DC2 enable"  , .val=&panelConfig.enableDC2, .options=onof_options, .options_len=2  },
    {.label="MB Address"  , .val=&panelConfig.addres, .limHi = 250, .limLo=1,.enableLim = true },
    {.label="RTU enable"  , .val=&panelConfig.modbus_RTU.enable, .options=onof_options, .options_len=2}, 
    {.label="RTU Speed"   , .val=&panelConfig.modbus_RTU.speed, .options=speed_options, .options_len=4},
    {.label="RTU Parity"  , .val=&panelConfig.modbus_RTU.parity, .options=par_options, .options_len=3},
    {.label="TCP enable"  , .val=&panelConfig.modbus_TCP.enable, .options=onof_options, .options_len=2},
    {.label="TCP IP[0]"   , .val=&panelConfig.modbus_TCP.ip0, .enableLim = true,  .limHi = 255, .limLo=0},
    {.label="TCP IP[1]"   , .val=&panelConfig.modbus_TCP.ip1, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP IP[2]"   , .val=&panelConfig.modbus_TCP.ip2, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP IP[3]"   , .val=&panelConfig.modbus_TCP.ip3, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[0]"   , .val=&panelConfig.modbus_TCP.mask0, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[1]"   , .val=&panelConfig.modbus_TCP.mask1, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[2]"   , .val=&panelConfig.modbus_TCP.mask2, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[3]"   , .val=&panelConfig.modbus_TCP.mask3, .enableLim = true,  .limHi = 255, .limLo=0 },

};

void MenuItemBrightnessChangedEvent(){
    DisplaySetBrightnessLevel((int) *configMenu[0].val);
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
