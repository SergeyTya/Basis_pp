#ifndef TASK_LOGGER_H_
#define TASK_LOGGER_H_

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
    uint32_t OnlineCounter;
    uint32_t LiveCounter;
    uint32_t mem1;
    uint32_t mem2;
    uint32_t mem3;
    uint32_t state[2];
    uint32_t spare[5];
    uint16_t crc16;
}Typedef_LoggerRecord;


void vTask_logger_writer();
void vTask_logger_reader();


#endif