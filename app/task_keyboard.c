#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "task_keybord.h"
#include "task_panel.h"


void keyboard_spi_hw_init();
uint16_t keyboard_spi_hw_rw(uint8_t data);

uint8_t state;

extern TypedefEnum_ButtonStates buttonState;

void vTask_keyboard(void * arg){

    keyboard_spi_hw_init();

    uint8_t button_cntr = 0;
    uint16_t button_before;
    
    while(1){


        uint16_t button_now =  keyboard_spi_hw_rw(1<<state);
        if(button_now == button_before) { if(button_cntr<110) button_cntr++;} else{button_cntr = 0;}
        if(button_cntr == 10){
            
            buttonState = button_now;
        }

        if(button_cntr == 100){
            
            buttonState = ~button_now; // long press

           // button_cntr = 0;
        }

        button_before=button_now;
         

        state++;
        if(state>8) state = 8;
        vTaskDelay(10);

    }

}