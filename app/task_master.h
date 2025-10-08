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
    MASTER_STATE_onWAIT = 4,
    MASTER_STATE_onTIMEOUT = 5

} TypedefEnum_MasterSlaveSates;

typedef struct {

    /* NOTE: slaveState[0] doesn't used, slave's numerated by slave modbus ID, started from 1 */
    
    TypedefEnum_MasterSlaveSates slaveStates[5]; 

    bool     fault_source[5];
    uint32_t fault_code[5];

    bool start_req_hw[5];
    bool start_req_rdo[5];


    bool master_wdg[5];

    void (*hw_reboot)();
    void (*hw_save)();
    void (*hw_load)();
    void (*hw_deft)();


}TypeDef_Master;

#endif