#include "stdbool.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"


#include "mcu_control.h"

#define CONTROL_FASTLPF(in, out, time) { (out) =(out) + ((in) - (out)) * (time); }

float tmp_sensor_calib_temp[] = {-999, -999.f, -40.f,  -30.f, -20.f, -10.f,   0.f,   10.f,   20.f,   30.f,   40.f,   50.f,   60.f,   70.f,   80.f,   90.f,  100.f,  110.f,  120.f,  130.f,  140.f,  150.f,  999.f,   999.f};
float tmp_sensor_calib_adc[]  = {   0,   55.f,  54.f,  107.f, 199.f, 347.f, 569.f,  871.f, 1242.f, 1650.f, 2054.f, 2419.f, 2724.f, 2966.f, 3150.f, 3288.f, 3390.f, 3465.f, 3520.f, 3561.f, 3591.f, 3614.f, 3615.f,  5000.f};



float m_getlin(float x0, float x1, float y0, float y1, float x ){
        float ret_val;
	if( (x>x1) || (x<x0) ){
		ret_val = 0.f;
	}else{
          ret_val = (float)((float)((y1 - y0) * (x - x0)) / (x1 - x0)) + y0;
        }
 	return ret_val;
}


float m_getCustomCurve(float arr_x[], float arr_y[], float curr_x, size_t len)
{
    float ret_val = 0.f;
    if (curr_x <= arr_x[0])
    {
        ret_val = m_getlin(0.f, arr_x[0], 0.f, arr_y[0], curr_x);
    }else if(curr_x > arr_x[len-1U]){
        ret_val = arr_y[len-1U];
    }else{
        for (size_t i = 0U; i < len-1U; i++){
            if ( (curr_x <= arr_x[i + 1U]) && (curr_x > arr_x[i]) ){
                ret_val = m_getlin(arr_x[i], arr_x[i + 1U], arr_y[i], arr_y[i + 1U], curr_x);
            }
        }
    }
    return ret_val;
}



volatile float temp_ext = 0;
float temp_raw = 0;
void vTask_TemperatureControl() {

    while (1)
    {
        uint16_t raw = hw_read_AI();

        CONTROL_FASTLPF( ((float) raw), temp_raw, (0.001f*10.f) );
        temp_ext = m_getCustomCurve(tmp_sensor_calib_adc, tmp_sensor_calib_temp, temp_raw, 24);
        
        vTaskDelay(1);
    }
    
}