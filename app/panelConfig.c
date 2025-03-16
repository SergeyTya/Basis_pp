#include "panelConfig.h"


void PanelConfigHwDummy(){
    ;
}

void StructureInit_PanelConfig(Typedef_PanelConfig * panelConfig){

    panelConfig->brightness = 100;
    
    panelConfig->hw_deft = PanelConfigHwDummy;
    panelConfig->hw_load = PanelConfigHwDummy;
    panelConfig->hw_reboot = PanelConfigHwDummy;
    panelConfig->hw_reboot = PanelConfigHwDummy;

    panelConfig->modbus_master.enable = true;
    panelConfig->modbus_master.speed  = 9600;
    panelConfig->modbus_master.bits = 8;
    panelConfig->modbus_master.parity = 0;
    
    panelConfig->modbus_RTU.adr = 1;
    panelConfig->modbus_RTU.enable = true;
    panelConfig->modbus_RTU.speed  = 9600;
    panelConfig->modbus_RTU.bits = 8;
    panelConfig->modbus_RTU.parity = 0;
}