#include "stdbool.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"


#include "mcu_control.h"

#define CONTROL_FASTLPF(in, out, time) { (out) =(out) + ((in) - (out)) * (time); }

volatile float temp_ext = 0;
void vTask_TemperatureControl() {

    while (1)
    {
        uint16_t raw = hw_read_AI();
        float temp_raw = raw*0.1f;

        CONTROL_FASTLPF(temp_raw, temp_ext, (0.001f*50.f) );

        vTaskDelay(1);
    }
    
}