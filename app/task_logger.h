#ifndef TASK_LOGGER_H_
#define TASK_LOGGER_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct __attribute__((packed))
{
    uint8_t timestamp_month;                                                    /*!< RTC time-stamp month value */
    uint8_t timestamp_year;                                                     /*!< RTC time-stamp date value: 0x1 - 0x31(BCD format) */
    uint8_t timestamp_day;                                                      /*!< RTC time-stamp weekday value */
    uint8_t timestamp_hour;                                                     /*!< RTC time-stamp hour value */
    uint8_t timestamp_minute;                                                   /*!< RTC time-stamp minute value: 0x0 - 0x59(BCD format) */
    uint8_t timestamp_second;                                                   /*!< RTC time-stamp second value: 0x0 - 0x59(BCD format) */
}Typedef_RtcTimeStamp;

typedef struct __attribute__((packed)){ 
    Typedef_RtcTimeStamp timeStart; // 6b
    Typedef_RtcTimeStamp timeStop;  // 6b
    char type[3];
    uint8_t slave_state;
    int16_t I;
    int16_t U;  
    int16_t F; 
    int16_t Im; 
    int16_t code; 
    uint32_t LiveCounter; //4b
    uint8_t sp[32];
    uint16_t crc16; //2b
}Typedef_LoggerRecord;  // 64 byte size



void vTask_logger_clear();
void vTask_logger_writer();
void vTask_logger_reader();
void vTask_logger();

void vTask_logger_read_AC1(Typedef_LoggerRecord * rec, int num);
void vTask_logger_read_AC2(Typedef_LoggerRecord * rec, int num);
void vTask_logger_read_DC1(Typedef_LoggerRecord * rec, int num);
void vTask_logger_read_DC2(Typedef_LoggerRecord * rec, int num);

uint32_t vTask_logger_get_count_AC1();
uint32_t vTask_logger_get_count_AC2();
uint32_t vTask_logger_get_count_DC1();
uint32_t vTask_logger_get_count_DC3();



#endif