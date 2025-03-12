#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mbsupport.h"
#include "task_master.h"

void vTask_Master(__attribute__((unused)) void *argument){

    while(1){

        for (size_t i = 1; i < 4; i++)
        {
            TypeDef_MB_Table * table = &holdings_table[i];

            for (size_t i = 0; i < table->len; i++) 
            { 
                TypeDef_MB_Holding * holding = &table->holdings[i];
                if(holding->lock && holding->change_req){
    
                    // TODO send holding value to slave 
                    // master_updateHoldin(holding);
                    holding->lock = false;
                    holding->change_req = false;
                    vTaskDelay(10);
                }
            }
        }

        vTaskDelay(10);
    }

}