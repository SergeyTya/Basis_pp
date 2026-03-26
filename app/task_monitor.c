#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "GD25Q32_os.h"
#include "mbsupport.h"

#include "timers.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "meter.h"
#include "task_master.h"

extern TypeDef_Master master;
extern Typedef_Meter meter;

#define MONITOR_STOP_HW()              \
    {                                  \
        master.slave[i].start_req_hw = true; \
    }

#define MONITOR_SET_FAULT(code){ \
        master.slave[i].fault_source_pm = true; \
        master.slave[i].fault_code_pm = code; \
        MONITOR_STOP_HW(); \
}


// Use it to get process values from holding tables
uint16_t *(*foos[])(uint16_t adr) = {NULL, GetHoldingPntrByAdrFromAC1, GetHoldingPntrByAdrFromAC2, GetHoldingPntrByAdrFromDC1, GetHoldingPntrByAdrFromDC2};

void vTask_monitor(void *p)
{
    vTaskDelay(5000);

    while (1)
    {
        for (size_t i = 1; i < 5; i++)
        {
            // READ PROCESS VALUES
            if (master.slave[i].slaveStates == MASTER_STATE_onRUN)
            {

                // Session continued
                // get pointer getter function
                uint16_t *(*foo)(uint16_t adr) = foos[i];

                // get process values
                if (i < 3)
                { // AC

                    // GET holding values from holding table
                    uint16_t U[3] = {*foo(240) / 10, *foo(241) / 10U, *foo(242) / 10}; // 0.1V
                    uint16_t Us = 0;
                    for (size_t j = 0; j < 3; i++)
                    {
                        master.slave[i].Uac[j];
                        Us += U[j];
                    }
                    master.slave[i].Uav = (master.slave[i].Uav + Us/3)/2;


                    uint16_t I[3] = {*foo(243), *foo(244), *foo(245)};
                    uint16_t Is=0;
                    for (size_t j = 0; j < 3; j++)
                    {
                        master.slave[i].Iac[j] = I[j];
                        Is += I[j];
                    }
                    master.slave[i].Iav = (master.slave[i].Iav + Is/3)/2;

                    master.slave[i].F = *foo(101) / 10;
                    master.slave[i].Fav = ( master.slave[i].Fav + master.slave[i].F)/2; 
                }
                else
                { // DC

                    master.slave[i].Udc = *foo(211) / 10; 
                    master.slave[i].Idc = *foo(210);

                    master.slave[i].Iav = (master.slave[i].Iav + master.slave[i].Idc)/2;
                    master.slave[i].Uav = (master.slave[i].Uav +  master.slave[i].Udc)/2;

                    master.slave[i].Imax = master.slave[i].Imax< master.slave[i].Idc ? master.slave[i].Idc :  master.slave[i].Imax;
                }

                // Evaluate values
                // uint16_t Isma= sma_add(&filterI[i], Is);
            }

            // check ERRORS
            if (
                (master.slave[i].slaveStates == MASTER_STATE_onRUN) && (master.slave[i].fault_source_pm == false))
            {

                master.slave[i].fault_source_pm = false;
                // 1. [CODE 2] Load current is low
                if (master.PM_enable.CODE2)
                {
                    if(master.slave[i].delay++ >=200){
                        if( master.slave[i].Idc < 1 ) { //NEED DELAY?;
                            MONITOR_SET_FAULT(2);
                            master.slave[i].delay = 200;
                        }
                    }
                }
                // 2. [CODE 4] Grid voltage phase FAIL
                if (master.PM_enable.CODE4)
                {
                    for (size_t j = 0; j < 3; j++)
                    {
                        if (meter.U[j].value_disp < 50)
                        {
                            MONITOR_SET_FAULT(4);
                        }
                    }
                }
                // 3. [CODE 5] Grid voltage LOW
                if (master.PM_enable.CODE5)
                {
                    for (size_t j = 0; j < 3; j++)
                    {
                        if (meter.U[j].value_disp < 340)
                        {
                            MONITOR_SET_FAULT(5);
                        }
                    }
                }
                // 4. [CODE 6] Grid voltage HIGH
                if (master.PM_enable.CODE6)
                {
                    if (master.PM_enable.CODE5)
                    {
                        for (size_t j = 0; j < 3; j++)
                        {
                            if (meter.U[j].value_disp > 420)
                            {
                                MONITOR_SET_FAULT(6);
                            }
                        }
                    }
                }

                if ( (i == 3) || (i == 4) ) //DC
                {

                    // 5. [CODE 7] DC source output LOW (< 20V)
                    if (master.PM_enable.CODE7)
                    {
                        if(master.slave[i].Udc < 20)
                        {
                            MONITOR_SET_FAULT(7);
                          //  master.slave[i].start_req_hw = false;
                          //  master_stop_dc(i);

                        }
                    }
                    // 6. [CODE 8] DC source output HIGH (> 32V)
                    if (master.PM_enable.CODE8)
                    {
                        if(master.slave[i].Udc > 32 )
                        {
                            MONITOR_SET_FAULT(8);
                           // master.slave[i].start_req_hw = false;
                          //  master_stop_dc(i);

                        }
                    }
                    // 7. [CODE 9] DC source output OV (> 40V)
                    if (master.PM_enable.CODE9)
                    {
                        if(master.slave[i].Udc > 40 )
                        {
                            MONITOR_SET_FAULT(9);
                            //master.slave[i].start_req_hw = false;
                           // master_stop_dc(i);
                        }
                    }
                }
            }
            else
            {
                master.slave[i].Iav = 0;
                master.slave[i].Uav = 0;
                master.slave[i].delay = 0;
            }
        }

        vTaskDelay(100);
    }
}