
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "gd32f4xx_libopt.h"
#include "masterTransport.h"

#include "clock.h"


uint16_t clock_buffer[6];

void Clock_read(void * p){

    
	Typedef_Clock * c = (Typedef_Clock *)p;

    c->adr = 16;

    memset(clock_buffer, 0, 12);

    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_parity_check_coherence_config(USART0, USART_PCM_NONE);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200);


    c->state = master_readHoldingsOs(c->adr, 0x400, 6, clock_buffer);

    c->time.timestamp_year   = clock_buffer[5];
    c->time.timestamp_month  = clock_buffer[4];
    c->time.timestamp_day    = clock_buffer[3];
    c->time.timestamp_hour   = clock_buffer[2];
    c->time.timestamp_minute = clock_buffer[1];
    c->time.timestamp_second = clock_buffer[0];


    usart_word_length_set(USART0, USART_WL_9BIT);
    usart_parity_check_coherence_config(USART0, USART_PCM_EN);
    usart_parity_config(USART0, USART_PM_EVEN);
    usart_baudrate_set(USART0, 115200);

}