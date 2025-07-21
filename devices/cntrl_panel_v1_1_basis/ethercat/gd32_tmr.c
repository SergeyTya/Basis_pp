#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"
#include "ecat_def.h"
void     Timer2_Init(uint8_t period);

void TMR_Init(uint8_t period){

    Timer2_Init(period);
}


void Timer2_Init(uint8_t period) {


    // Enable the clock for Timer2
    rcu_periph_clock_enable(RCU_TIMER2);

    // Configure the base timer parameters
    timer_parameter_struct timer_initpara;
    timer_initpara.prescaler         = 42 - 1;       // Prescaler value to get 10 kHz from 84 MHz system clock
    timer_initpara.alignedmode       = TIMER_COUNTER_UP;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = period*200- 1;      // 1 ms
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    // Enable the update interrupt
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    // Start the timer
    timer_enable(TIMER2);


    #if ECAT_TIMER_INT
						/* Configure NVIC Interrupt  -------------------------*/	
	 nvic_irq_enable(TIMER2_IRQn, 3, 2);
	#endif
}