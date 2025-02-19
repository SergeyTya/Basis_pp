#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "AD7793.h"
#include "AD7793_os.h"

extern uint32_t ad7793_ChT;
extern volatile uint32_t ad7793_Ch1;
extern volatile uint32_t ad7793_Ch2;
extern volatile uint32_t ad7793_Ch3;
extern volatile uint32_t ad7793_ChV;

volatile uint32_t AD7793_RATE_GLOB = AD7793_RATE_242;
volatile int16_t wait_time = 0;



void AD7793_Init_async(){

    AD7793_Reset();
    vTaskDelay(3);

    while(AD7793_GetReadyReg() == 0){
        vTaskDelay(1);
    }

    AD7793_SetRate(AD7793_RATE_GLOB);

    //Setup current source
    AD7793_SetCurrentSourcesDirection(AD7793_DIR_IEXC1_IOUT1_IEXC2_IOUT2);
    AD7793_SetCurrentSourcesValue(AD7793_EN_IXCEN_210uA);
   
    AD7793_SetReference(AD7793_REFSEL_INT);
    AD7793_SetGain(AD7793_GAIN_1);
    AD7793_SetBuf(1);
    AD7793_SetPolarity(AD7793_UNIPOLAR);

    //Calibration
    uint32_t chnls[] = {AD7793_CH_AIN1P_AIN1M, AD7793_CH_AIN2P_AIN2M,AD7793_CH_AIN3P_AIN3M};

    for (size_t i = 0; i < 3; i++)
    {
        AD7793_SetChannel(chnls[i]);
        AD7793_SetMode(AD7793_MODE_CAL_INT_ZERO);
        while(AD7793_GetReadyReg() == 0){
            vTaskDelay(1);
        }

        AD7793_SetMode(AD7793_MODE_CAL_INT_FULL);
         while(AD7793_GetReadyReg() == 0){
            vTaskDelay(1);
        }
    }

    vTaskDelay(10);

}


uint32_t AD7793_ReadChannel_async(uint32_t chn){
    AD7793_SetChannel(chn);
    AD7793_SetMode(AD7793_MODE_SINGLE);
    while(AD7793_GetReadyReg() == 0){
            vTaskDelay(1);
    }
    return AD7793_GetRegisterValue(AD7793_REG_DATA, AD7793_REG_DATA_SZ);
}



