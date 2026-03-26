#ifndef TASK_MASTER_H
#define TASK_MASTER_H

#include "main.h"

void vTask_Master(__attribute__((unused)) void *argument);

typedef enum
{
    MASTER_STATE_EMPTY = 0,
    MASTER_STATE_onREADY = 1,
    MASTER_STATE_onFAULT = 2,
    MASTER_STATE_onRUN = 3,
    MASTER_STATE_onWAIT_FOR_AC_OK = 4,
    MASTER_STATE_onWAIT_FOR_MAIN_RELAY = 5,
    MASTER_STATE_onWAIT_FOR_STOP,
    MASTER_STATE_onTIMEOUT,
    MASTER_STATE_TAIL = 0xff

} TypedefEnum_MasterSlaveSates;

typedef struct{

    //TODO Typedef_SlaveType type;
    //TODO uint16_t adr; 

    TypedefEnum_MasterSlaveSates slaveStates;

    bool     fault_source;   // fault flag from device
    bool     fault_source_pm;// fault flag from panel monitor

    uint32_t fault_code;     // fault code from device
    uint32_t fault_code_pm;  // fault code from panel monitor

    bool start_req_hw;
    bool start_req_rdo;

    volatile  bool master_wdg;

    int16_t Iac[3];
    int16_t Uac[3];

    int16_t Idc;
    int16_t Udc;

    int16_t Iav;
    int16_t Uav;
    
    int16_t Imax;
    int16_t Umax;

    int16_t F;
    int16_t Fav;

    uint32_t delay;

}Typedef_Slave;

typedef struct {

    /* NOTE: slave[0] doesn't used, slave's numerated by slave modbus ID, started from 1 */

    Typedef_Slave slave[5];
    
    union{
        struct {
            uint16_t CODE2:1; // Load current low 0
            uint16_t CODE4:1; // Grid Phase break 1
            uint16_t CODE5:1; // Grid voltage low 2
            uint16_t CODE6:1; // Grid voltage High 3
            uint16_t CODE7:1; // DC voltage low 4
            uint16_t CODE8:1; // DC voltage high 5
            uint16_t CODE9:1; // DC voltage overvoltage 6
        };

        uint16_t value;
    } PM_enable; // Enable flags for panel monitor check


    void (*hw_reboot)();
    void (*hw_save)();
    void (*hw_load)();
    void (*hw_deft)();

    

}TypeDef_Master;

void master_stop_dc(uint8_t slaveAdr);

#endif