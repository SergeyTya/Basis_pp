#ifndef HOURS_COUNTER_H
#define HOURS_COUNTER_H

#include "stdint.h"

typedef struct
{
    uint8_t timestamp_month;                                                    /*!< RTC time-stamp month value */
    uint8_t timestamp_date;                                                     /*!< RTC time-stamp date value: 0x1 - 0x31(BCD format) */
    uint8_t timestamp_day;                                                      /*!< RTC time-stamp weekday value */
    uint8_t timestamp_hour;                                                     /*!< RTC time-stamp hour value */
    uint8_t timestamp_minute;                                                   /*!< RTC time-stamp minute value: 0x0 - 0x59(BCD format) */
    uint8_t timestamp_second;                                                   /*!< RTC time-stamp second value: 0x0 - 0x59(BCD format) */
    uint32_t am_pm;                                                             /*!< RTC time-stamp AM/PM value */

}Typedef_RtcTimeStamp;

typedef struct{
    Typedef_RtcTimeStamp time;
    uint32_t motorOnlineCounter;
    uint32_t motorHoursCounter;
    uint16_t current;
    uint16_t volt_DC;
    uint16_t volt_out;
    uint16_t inv_temp;
    uint16_t mtr_temp;
    uint32_t state[2];
    uint32_t spare[5];
    uint16_t crc16;
}Typedef_MotorHoursCounterRecord;


void vTask_MotorHoursCounter();


#endif