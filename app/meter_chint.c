#include <stddef.h>
#include <stdint.h>

#include "gd32f4xx_libopt.h"
#include "masterTransport.h"
#include "meter.h"

uint16_t meter_chint_buff[0x20];
float meter_chint_val[0x10];

void meter_read(void *mtr)
{
    Typedef_Meter * m = mtr;
    m->enable = 1;
    union
    {
        float f;
        uint16_t u16[2];
    } fv;

    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_parity_check_coherence_config(USART0, USART_PCM_NONE);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 19200);

    // Read voltage
    m->state = master_readHoldingsOs(m->adr, 0x2000, 0x20, meter_chint_buff);

    size_t j =0;
    for (size_t i = 0; i < 10; i++)
    {
        fv.u16[1] = meter_chint_buff[j++];
        fv.u16[0] = meter_chint_buff[j++];
        meter_chint_val[i] = fv.f;
    }

    m->Power_re.value_disp = 0;
    m->Power_im.value_disp = 0;
    for (size_t i = 0; i < 3; i++)
    {
        m->U[i].value_disp = (uint16_t) meter_chint_val[i+0]/10; 
        m->I[i].value_disp = (uint16_t) meter_chint_val[i+6]/10;
        
        m->Power_re.value_disp += (uint16_t) meter_chint_val[i+9]/10; 
        m->Power_im.value_disp += (uint16_t) meter_chint_val[i+12]/10; 
    }

    usart_word_length_set(USART0, USART_WL_9BIT);
    usart_parity_check_coherence_config(USART0, USART_PCM_EN);
    usart_parity_config(USART0, USART_PM_EVEN);
    usart_baudrate_set(USART0, 115200);
}