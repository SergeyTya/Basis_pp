#include "stdbool.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"


#include "mcu_control.h"

#define CONTROL_FASTLPF(in, out, time) { (out) =(out) + ((in) - (out)) * (time); }

float tmp_sensor_calib_temp[] = {-999, -999.f, -40.f,  -30.f, -20.f, -10.f,   0.f,   10.f,   20.f,   30.f,   40.f,   50.f,   60.f,   70.f,   80.f,   90.f,  100.f,  110.f,  120.f,  130.f,  140.f,  150.f,  999.f,   999.f};
// /*R75 c 10К1*/float tmp_sensor_calib_adc[]  = {   0,   55.f,  87.f,  172.f, 315.f,  537.f, 844.f,  1225.f, 1645.f, 2058.f, 2428.f, 2734.f, 2974.f, 3156.f, 3291.f, 3391.f, 3465.f, 3519.f, 3559.f, 3589.f, 3612.f, 3629.f, 3628.f,  5000.f};
   /*R75 c  5К1*/float tmp_sensor_calib_adc[]  = {   0,   44.f,  45.f,   89.f, 168.f,   294.f,  484.f,   745.f, 1071.f, 1440.f, 1819.f, 2178.f, 2492.f, 2753.f, 2962.f, 3124.f, 3248.f, 3343.f, 3415.f, 3469.f, 3511.f, 3544.f, 3545.f,  5000.f};



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
    int tcnt = 0;

    while (1)
    {
        uint16_t raw = hw_read_AI();

        CONTROL_FASTLPF( ((float) raw), temp_raw, (0.001f*100.f) );
    
        if(tcnt++>2000){
            temp_ext = m_getCustomCurve(tmp_sensor_calib_adc, tmp_sensor_calib_temp, temp_raw, 24);
            tcnt=0;
        }

        vTaskDelay(1);
    }
    
}