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
int cnt = 0;

extern TypedefEnum_ButtonStates buttonState;
static uint16_t button_before = 0;

uint8_t leds = 0x2f;
volatile int leds_pwm = 20;
volatile int leds_pls = 20;

void vTask_keyboard(void * arg){

    keyboard_spi_hw_init();

    uint16_t button_cntr = 0;

    while(1){

        leds = 0xff;
        if(cnt < leds_pwm){ leds = 0; }
        uint16_t button_now =  keyboard_spi_hw_rw(leds);
        if(button_now == button_before) { if(button_cntr<1100) button_cntr++;} else{button_cntr = 0;}
        if(button_cntr == 100){
            
            buttonState = button_now;
        }

        if(button_cntr == 1000){
            
            buttonState = ~button_now; // long press

           // button_cntr = 0;
        }

        button_before=button_now;
         
        if(cnt++>leds_pls){
            cnt = 0;
        }
        
        vTaskDelay(1);

    }

}