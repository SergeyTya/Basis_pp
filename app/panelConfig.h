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


typedef struct
{
    uint32_t addres;
    Typedef_Transport modbus_master;
    Typedef_Transport modbus_RTU;
    Typedef_Transport modbus_TCP;
    Typedef_Transport ethercat;

    uint32_t brightness;

    bool enableAC1;
    bool enableAC2;
    bool enableDC1;
    bool enableDC2;

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



#endif