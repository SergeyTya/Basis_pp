#ifndef PANEL_CONFIG_H_
#define PANEL_CONFIG_H_


#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    
    uint32_t adr;
    uint32_t speed;
    uint32_t parity;
    uint32_t bits;
    uint32_t ip0, ip1, ip2, ip3;
    uint32_t port;
    uint32_t mask0,mask1,mask2,mask3;
    uint32_t enable;

    /* data */
}Typedef_Transport;

void ConfigMenuSaveAll();
void ConfigMenuReadAll();

typedef struct
{
    uint32_t addres;
    Typedef_Transport modbus_master;
    Typedef_Transport modbus_RTU;
    Typedef_Transport modbus_TCP;
    Typedef_Transport ethercat;

    uint32_t brightness;

    uint32_t enableAC1;
    uint32_t enableAC2;
    uint32_t enableDC1;
    uint32_t enableDC2;

    // PANEL_CONFIG_ACTIVESLAVE
    uint32_t active_slave;

 
}Typedef_PanelConfig;

/**
 *  @brief use it like null pointer for hw functions
 */
void PanelConfigHwDummy();

void StructureInit_PanelConfig(Typedef_PanelConfig * panelConfig);
void MenuItemGeneralChangedEvent();


#endif