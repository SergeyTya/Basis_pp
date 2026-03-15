#include <stddef.h>
#include <stdint.h>

#include "gd32f4xx_libopt.h"
#include "masterTransport.h"
#include "meter.h"

#define CHINT_HR_CNT 20

uint16_t meter_chint_buff[CHINT_HR_CNT];
float meter_chint_val[CHINT_HR_CNT/2];

//IrAr = 750
//UrAr = 1


void meter_read(void *mtr)
{
    Typedef_Meter * m = mtr;

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
    m->state = master_readHoldingsOs(m->adr, 0x2000, CHINT_HR_CNT, meter_chint_buff);

    size_t j =0;
    for (size_t i = 0; i < CHINT_HR_CNT/2; i++)
    {
        fv.u16[1] = meter_chint_buff[j++];
        fv.u16[0] = meter_chint_buff[j++];
        meter_chint_val[i] = fv.f;
    }

    float St = 0;
    for (size_t i = 0; i < 3; i++)
    {
        float Uf = meter_chint_val[i+0]*0.1f;
        float If = meter_chint_val[i+6]*0.75f;
        m->U[i].value_disp = (uint16_t) Uf; 
        m->I[i].value_disp = (uint16_t) If;
        St += Uf*If;
    }

    m->Power_s.value_disp = (uint16_t) ( St/1000.f);

    usart_word_length_set(USART0, USART_WL_9BIT);
    usart_parity_check_coherence_config(USART0, USART_PCM_EN);
    usart_parity_config(USART0, USART_PM_EVEN);
    usart_baudrate_set(USART0, 115200);
}