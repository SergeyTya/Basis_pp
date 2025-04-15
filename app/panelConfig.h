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
    uint32_t ip;
    uint32_t port;
    uint32_t mask;


    bool enable;

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

    bool fault_source[5];
    uint32_t fault_code[5];

    bool start_req[5];

    bool master_wdg[5];

    void (*hw_reboot)();
    void (*hw_save)();
    void (*hw_load)();
    void (*hw_deft)();

}Typedef_PanelConfig;

/**
 *  @brief use it like null pointer for hw functions
 */
void PanelConfigHwDummy();

void StructureInit_PanelConfig(Typedef_PanelConfig * panelConfig);
void MenuItemGeneralChangedEvent();


#endif