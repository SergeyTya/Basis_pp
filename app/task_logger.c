// Betta version
//TODO RTC usage to get absolute time 

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"

#include "gd32f4xx_libopt.h"
#include "GD25Q32E.h"
#include "GD25Q32_os.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "task_logger.h"

#define MOTOR_HOUR_COUNTER_TIME_STEP_SEC 5U
#define MOTOR_HOUR_COUNTER_SECTORS_SIZE  100U

extern uint8_t page_gd25_tx[256];
extern uint8_t page_gd25_rx[256];

SemaphoreHandle_t semaphore_MHC;

Typedef_LoggerRecord time_glob;
Typedef_LoggerRecord * time_rx;
Typedef_LoggerRecord * tmp_records;

uint8_t GD25_ID[16];
uint8_t GD25_RDID[3];

uint8_t logger_RxBuf[256];


volatile uint16_t record_cnt = 0;
volatile uint32_t page_cnt   = 0;
volatile uint32_t sector_cnt = 0;
volatile uint32_t adr_cnt    = 0;
volatile uint32_t adr_rx = 0;

static uint16_t crc16(uint8_t *data, int len);
void vTask_LoggerReader();
void Logger_ErrorHandler();
void Logger_search_valid_record();
bool Logger_checkRecordCRC16(Typedef_LoggerRecord * rec);

// use block 1 as work hour counter
void vTask_MotorHoursCounter(){
  semaphore_MHC = xSemaphoreCreateBinary();
  xSemaphoreGive(semaphore_MHC);

  memset(page_gd25_tx, 0xFF, 256);
  vTaskDelay(1);
  vGD25HardWareInit();
  vGD25WriteEnableAsync();
  vGD25ReadIDAsync(GD25_ID);
  vGD25ReadRDIDAsync(GD25_RDID);
  vGD25PageReadAsync(4096*(MOTOR_HOUR_COUNTER_SECTORS_SIZE + 1), page_gd25_rx);

  bool check_mem_id = true;
  for (size_t i = 0; i < 16; i++)
  {
    if(GD25_ID[i] != page_gd25_rx[i]){
      check_mem_id = false;
      break;
    }
  }
  
  if(check_mem_id == false){
    vGD25SectorErase(0);
    vGD25SectorErase(MOTOR_HOUR_COUNTER_SECTORS_SIZE + 1);
    memset(page_gd25_tx, 0xFF, sizeof(page_gd25_tx));
    memcpy(page_gd25_tx, GD25_ID, 16);
    vGD25PageProgramAsync(MOTOR_HOUR_COUNTER_SECTORS_SIZE + 1, page_gd25_tx);
  }else{
    Logger_search_valid_record();
  }

  xTaskCreate(vTask_LoggerReader, "", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
  
  while(1){

    /*    */
    if(xSemaphoreTake(semaphore_MHC, 1000) != pdPASS){
      continue; // TODO error handler
    }

    time_glob.LiveCounter += MOTOR_HOUR_COUNTER_TIME_STEP_SEC;
 //   if(INVERTER_isRUN(inverter_now)) time_glob.motorHoursCounter += MOTOR_HOUR_COUNTER_TIME_STEP_SEC;
    // time_glob.state[0] = inverter_now->state_flags.value[0];
    // time_glob.state[1] = inverter_now->state_flags.value[1];
    // time_glob.current  = INVERTER_GET_CURRENT(inverter_now);
    // time_glob.volt_DC  = INVERTER_GET_BUS_VOLT(inverter_now);
    // time_glob.volt_out = INVERTER_GET_OUTVOLT(inverter_now);



    time_glob.crc16 = 0; //reset crc
    volatile uint16_t crc = crc16( (uint8_t *) &time_glob, sizeof(Typedef_LoggerRecord) );
    time_glob.crc16 = crc;
    adr_cnt = 4096*sector_cnt+page_cnt*256;
    memcpy(&page_gd25_tx[sizeof(time_glob)*record_cnt], (uint8_t *) &time_glob, sizeof(time_glob) );
    vGD25PageProgramAsync(adr_cnt, page_gd25_tx);
    record_cnt++;
    if(record_cnt==4){ // change page
      record_cnt=0;
      page_cnt++;
      memset(page_gd25_tx, 0xFF, sizeof(page_gd25_tx));
      if(page_cnt==16){
        page_cnt = 0;
        sector_cnt++;
        if(sector_cnt > MOTOR_HOUR_COUNTER_SECTORS_SIZE) {
          sector_cnt = 0;
        }
        vGD25SectorErase(0x1000U*(sector_cnt)); // erase sector
        uint32_t next_sector = sector_cnt+1;
        if(next_sector > MOTOR_HOUR_COUNTER_SECTORS_SIZE){
          next_sector = 0;
        }
        vGD25SectorErase(0x1000U*(next_sector)); // erase sector
      }
    }
    xSemaphoreGive(semaphore_MHC);
     

    vTaskDelay( MOTOR_HOUR_COUNTER_TIME_STEP_SEC*1000U );
    
  }
  vTaskDelete(NULL);
}

void vTask_LoggerReader(){
  vTaskDelay(1000);
  while(1){

    if(xSemaphoreTake(semaphore_MHC, 1000) != pdPASS){
      continue; // TODO error handler
    }

    vGD25PageReadAsync(adr_rx, logger_RxBuf);
    time_rx = (Typedef_LoggerRecord *) logger_RxBuf;
 
    xSemaphoreGive(semaphore_MHC);
    
    vTaskDelay(100);

  }
  vTaskDelete(NULL);
}


void Logger_search_valid_record(){
  while (1)
  {
    if(xSemaphoreTake(semaphore_MHC, 1000) != pdPASS){
      continue; // TODO error handler
    }
    adr_cnt = 4096U*sector_cnt+page_cnt*256U;
    vGD25PageReadAsync(adr_cnt, page_gd25_rx);
    xSemaphoreGive(semaphore_MHC);
    tmp_records = (Typedef_LoggerRecord *) page_gd25_rx;
    record_cnt = 0;
    for (size_t i = 0; i < 4; i++)
    {
      if(tmp_records[i].crc16 == 0xFFFF){
         if(i==0){ // need to be tested
              uint32_t tmp_adr;
              if(page_cnt==0 && sector_cnt==0){
                tmp_adr = MOTOR_HOUR_COUNTER_SECTORS_SIZE*16U*256U - 256U;
              }else{
                tmp_adr = adr_cnt - 256U;  
              }
              vGD25PageReadAsync(tmp_adr, page_gd25_rx);
              i = 3;
              if(MHC_checkRecordCRC16(&(tmp_records[i])) == false){
                Logger_ErrorHandler();
              }
          }else{
            i=i-1;  
          }
          time_glob = tmp_records[i];
          return;
      }
      if(MHC_checkRecordCRC16(&(tmp_records[i])) == false){
        Logger_ErrorHandler();
      }
      record_cnt++;
    }

    page_cnt++;
    if(page_cnt==16){
      page_cnt = 0;
      sector_cnt++;
      if(sector_cnt > MOTOR_HOUR_COUNTER_SECTORS_SIZE) {
        sector_cnt = 0;
      }
    }

    vTaskDelay(1);
  }
}


static uint16_t crc16(uint8_t *data, int len) {
    uint16_t crc = 0xFFFF;
    int i, j;

    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }

    return crc;
}

void MHC_ErrorHandler(){
  while (1)
  {
    vTaskDelay(1000);
  }
}

bool Logger_checkRecordCRC16(Typedef_LoggerRecord * rec){
  volatile bool ret_val = true;
  Typedef_LoggerRecord tmp_rec = * rec;
  tmp_rec.crc16 = 0;
  uint16_t tmp_crc = crc16( (uint8_t *) &tmp_rec, sizeof(Typedef_LoggerRecord));
  if(tmp_crc != rec->crc16){
    ret_val = false;
  }
  return ret_val;
}
