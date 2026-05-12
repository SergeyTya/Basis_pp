#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "task_keybord.h"
#include "task_panel.h"

#include "keyboard_port.h"


void keyboard_spi_hw_init();
uint16_t keyboard_spi_hw_rw(uint8_t data);

uint8_t state;
int cnt = 0;

extern TypedefEnum_ButtonStates buttonState;
static TypedefEnum_ButtonStates button_before = 0;

static uint8_t leds = 0x2f;
static uint8_t leds_val = 0;
volatile int leds_pwm = 20;
volatile int leds_pls = 20;

static inline void setLedBit(bool state, uint32_t pos){
     if(state){ 
        leds_val |= (1<<pos);
    } else{ 
        leds_val &= ~(1<<pos);
    }
}

volatile bool test_led_state = false;
volatile uint8_t test_led_pos = 0;

void vTask_keyboard(void * arg){

    keyboard_spi_hw_init();

    uint16_t button_cntr = 0;

    while(1){

     //   setLedBit(test_led_state, test_led_pos);

        leds = leds_val;

       // leds = 0xff;
        if(cnt < leds_pwm){ leds = 0; }
        TypedefEnum_ButtonStates button_now =  keyboard_spi_hw_rw(leds);

        if(button_now == button_before) {
             if(button_cntr<1100) button_cntr++;
        }else{
            
            if(button_cntr > 10 &&  button_cntr < 800){
               buttonState = button_before;
            }

            button_cntr = 0;

        }



        if(button_cntr == 1000){
            buttonState = ~button_now; // long press
        }

        button_before=button_now;
         
        if(cnt++>leds_pls){
            cnt = 0;
        }
        
        vTaskDelay(1);

    }

}



void keyboard_setFaultLedSate(bool state){
    setLedBit(state, LED_FLT_POSITION);  
}
void keyboard_setOnLedSate(bool state){
     setLedBit(state, LED__ON_POSITION);  
}
void keyboard_setAC1LedSate(bool state){
     setLedBit(state, LED_AC1_POSITION);  
}
void keyboard_setAC2LedSate(bool state){
     setLedBit(state, LED_AC2_POSITION);  
}
void keyboard_setDC1LedSate(bool state){
     setLedBit(state, LED_DC1_POSITION);  
}
void keyboard_setDC2LedSate(bool state){
     setLedBit(state, LED_DC2_POSITION);  
}

