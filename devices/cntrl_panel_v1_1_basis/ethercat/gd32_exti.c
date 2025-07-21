#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"


//EXTI0 PC0 - LINT (IRQ), EXTI2 PC2 - SYNC1, EXTI3 PC3 - SYNC2
void EXTI0_Configuration(){

    rcu_periph_clock_enable(RCU_GPIOC); // Adjust if using a different port
    rcu_periph_clock_enable(RCU_SYSCFG);
    // Configure the pin as input
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    nvic_irq_enable(EXTI0_IRQn, 2, 0); // Adjust IRQn accordingly
    // Connect the EXTI line to the selected GPIO pin
    syscfg_exti_line_config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN0);
    
    // Configure the EXTI line
    exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING); //! only falling edge
    // Enable and set priority for the EXTI interrupt
    exti_interrupt_flag_clear(EXTI_0);
}

void EXTI2_Configuration(){

    rcu_periph_clock_enable(RCU_GPIOC); // Adjust if using a different port

    // Configure the pin as input
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_2); 

    // Connect the EXTI line to the selected GPIO pin
    syscfg_exti_line_config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN2);
   
    // Configure the EXTI line
    exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_RISING); // Adjust EXTI_LINE accordingly
    exti_interrupt_disable(EXTI_0);
    
    exti_interrupt_flag_clear(EXTI_0);
    // Enable and set priority for the EXTI interrupt
    nvic_irq_enable(EXTI2_IRQn, 3, 2); // Adjust IRQn accordingly
   // exti_interrupt_flag_clear(EXTI_2);
}

void EXTI3_Configuration(){

    rcu_periph_clock_enable(RCU_GPIOC); // Adjust if using a different port

    // Configure the pin as input
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3); 

    // Connect the EXTI line to the selected GPIO pin
    syscfg_exti_line_config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN3);
   
   // gpio_exti_source_select(gpio_pin_to_port_source(gpio_port), gpio_pin_to_pin_source(gpio_pin));

    // Configure the EXTI line
    exti_init(EXTI_3, EXTI_INTERRUPT, EXTI_TRIG_RISING); // Adjust EXTI_LINE accordingly

    // Enable and set priority for the EXTI interrupt
    nvic_irq_enable(EXTI3_IRQn, 3, 2); // Adjust IRQn accordingly
    exti_interrupt_flag_clear(EXTI_3);
}


volatile uint8_t escIntState  = 0;
void set_escint_state( uint8_t state){
    if(state != 0){
        exti_interrupt_flag_clear(EXTI_0);
        exti_interrupt_enable(EXTI_0);
    }else{
        exti_interrupt_disable(EXTI_0);
    }

    escIntState = state;
}