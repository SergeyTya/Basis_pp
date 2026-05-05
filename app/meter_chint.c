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

/*

Нажимаем menu -> просит пароль (по умолчанию 701) -> conn
prot - абсолютно мне непонятные символы скрываются под этим меню, я оставил по умолчанию, а именно n.2
bAud - скорость передачи, тут опять таки в инструкции не слова, а внутри меню просто цифры от 1 до 5, методом проб я выбрал 3, что соответствует 9600
addr - адрес slave устройства, по умолчанию 1

bAud - 4
addr - 7
prot - n1 

*/

uint8_t chint_wdg=0;
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
    TypedefEnum_MasterTransportSates res
     = master_readHoldingsOs(m->adr, 0x2000, CHINT_HR_CNT, meter_chint_buff, 50);

    usart_word_length_set(USART0, USART_WL_9BIT);
    usart_parity_check_coherence_config(USART0, USART_PCM_EN);
    usart_parity_config(USART0, USART_PM_EVEN);
    usart_baudrate_set(USART0, 115200);

    vTaskDelay(10); // NEED Delay

    if(res!=MASTER_TRANSPORT_NOERROR){
        chint_wdg++;
    }else{
        chint_wdg = 0;
    }

    if(chint_wdg > 3){
        chint_wdg = 4;
        m->state = MASTER_TRANSPORT_TIMEOUT;
    }else{
        m->state = MASTER_TRANSPORT_NOERROR;
    }

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
        float Uf = meter_chint_val[i+3]*0.1f;
        float If = meter_chint_val[i+6]*0.75f;
        m->U[i].value_disp = (uint16_t) Uf; 
        m->I[i].value_disp = (uint16_t) If;
        St += Uf*If/1.73; // ULL
    }

    m->Power_s.value_disp = (uint16_t) ( St/1000.f);


}