#include "task_panel.h"
#include "IDisplay.h"
#include "mcu_control.h"
#include "task_logger.h"

const uint32_t speed_options[] = {9600, 38400, 115200, 230400};
const uint32_t bright_options[] = {25, 50, 75, 100};
const uint32_t onof_options[] = {/*OFF*/0, /*ON*/1};
const uint32_t par_options[] = {/*NONE*/0 , /*ODD*/1, /*EVEN*/2};
uint32_t no_val = 0;
uint32_t  configmenu_heater_state = 0;
uint32_t  configmenu_erase_mem = 0;

#include "meter.h"
#include "clock.h"

extern Typedef_Clock clock;
extern Typedef_Meter meter;


void MenuItemBrightnessChangedEvent();
void MenuItemRebootChangedEvent();
void MenuItemHeaterEvent();
void MenuItemClearMemeEvent();

const TypeDef_ConfigMenuItem nullMenuItem = {.label = "Null pointer"};
const size_t configMenuSize = 23;
TypeDef_ConfigMenuItem configMenu[] = {

    {.label="Reboot"      , .val=&no_val,  .disabled = true,  .itemChangedEvent = MenuItemRebootChangedEvent },
    {.label="Heater"      , .val=&configmenu_heater_state,  .disabled = true,  .itemChangedEvent = MenuItemHeaterEvent },
    {.label="Clock"       , .val=&clock.enable,  .options=onof_options, .options_len=2},
    {.label="Meter"       , .val=&meter.enable,  .options=onof_options, .options_len=2},
    {.label="AC1 enable"  , .val=&panelConfig.enableAC1, .options=onof_options, .options_len=2 },
    {.label="AC2 enable"  , .val=&panelConfig.enableAC2, .options=onof_options, .options_len=2  },
    {.label="DC1 enable"  , .val=&panelConfig.enableDC1, .options=onof_options, .options_len=2  },
    {.label="DC2 enable"  , .val=&panelConfig.enableDC2, .options=onof_options, .options_len=2  },
    {.label="MB Address"  , .val=&panelConfig.addres, .limHi = 250, .limLo=1,.enableLim = true },
    {.label="RTU enable"  , .val=&panelConfig.modbus_RTU.enable, .options=onof_options, .options_len=2}, 
    {.label="RTU Speed"   , .val=&panelConfig.modbus_RTU.speed, .options=speed_options, .options_len=4},
    {.label="RTU Parity"  , .val=&panelConfig.modbus_RTU.parity, .options=par_options, .options_len=3},
    {.label="TCP enable"  , .val=&panelConfig.modbus_TCP.enable, .options=onof_options, .options_len=2},
    {.label="ECT enable"  , .val=&panelConfig.ethercat.enable, .options=onof_options, .options_len=2},
    {.label="TCP IP[0]"   , .val=&panelConfig.modbus_TCP.ip0, .enableLim = true,  .limHi = 255, .limLo=0},
    {.label="TCP IP[1]"   , .val=&panelConfig.modbus_TCP.ip1, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP IP[2]"   , .val=&panelConfig.modbus_TCP.ip2, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP IP[3]"   , .val=&panelConfig.modbus_TCP.ip3, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[0]"   , .val=&panelConfig.modbus_TCP.mask0, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[1]"   , .val=&panelConfig.modbus_TCP.mask1, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[2]"   , .val=&panelConfig.modbus_TCP.mask2, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="TCP MS[3]"   , .val=&panelConfig.modbus_TCP.mask3, .enableLim = true,  .limHi = 255, .limLo=0 },
    {.label="CLR MEM"     , .val=&configmenu_erase_mem,  .disabled = true , .itemChangedEvent = MenuItemClearMemeEvent}

};

void MenuItemClearMemeEvent(){
    vTask_logger_clear();
     HW_REBOOT;

}

void MenuItemBrightnessChangedEvent(){
    DisplaySetBrightnessLevel((int) *configMenu[0].val);
    ConfigMenuSaveAll();
}

void MenuItemGeneralChangedEvent(){
    ConfigMenuSaveAll();
    HW_REBOOT;
}

void MenuItemRebootChangedEvent(){
    HW_REBOOT;
}

bool master_ToggleHeaterState();
void MenuItemHeaterEvent(){
    uint8_t val = master_ToggleHeaterState();
    configmenu_heater_state = val;
}

void ConfigMenuSaveAll(){
  
    size_t len = configMenuSize;
    uint32_t data[len];
    
    for (size_t i = 0; i < configMenuSize; i++)
    {
        data[i] =  *configMenu[i].val;
    }
    
   hw_write_FLASH( data, len); // sent to hw
}

void ConfigMenuReadAll(){

    size_t len = configMenuSize;
    uint32_t data[len];
    
    hw_read_FLASH(data, len);

    for (size_t i = 0; i < configMenuSize; i++)
    {
        if(data[i] == 0xFFFFFFFF){ 
            return;
        };
        *configMenu[i].val = data[i];
    }
}
