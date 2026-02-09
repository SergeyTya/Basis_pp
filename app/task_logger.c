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

#define LOGGER_TIME_STEP_SEC 5U
#define LOGGER_SECTORS_SIZE  100U  // total sectors in use (5200 records)

#define LOGGER_INFO_SECTOR 101U
#define LOGGER_INFO_ADR    0x1000U*LOGGER_INFO_SECTOR

extern uint8_t page_gd25_tx[256];
extern uint8_t page_gd25_rx[256];

SemaphoreHandle_t semaphore_MHC;

Typedef_LoggerRecord time_glob;
Typedef_LoggerRecord * logger_records_rx;
Typedef_LoggerRecord * tmp_records;

uint8_t GD25_ID[16];
uint8_t GD25_RDID[3];

uint8_t logger_RxBuf[256];

Typedef_Logger logger = {
.record_cnt = 0,
.page_cnt   = 0,
.sector_cnt = 0,
.adr_cnt    = 0,
};

volatile uint32_t logger_adr_rx  = 0;


static uint16_t crc16(uint8_t *data, int len);

void Logger_ErrorHandler();
void Logger_search_valid_record();
bool Logger_checkRecordCRC16(Typedef_LoggerRecord * rec);

// use block 1 as work hour counter
void vTask_logger_writer(){
  semaphore_MHC = xSemaphoreCreateBinary();
  xSemaphoreGive(semaphore_MHC);

  memset(page_gd25_tx, 0xFF, 256);
  vTaskDelay(1);
  vGD25HardWareInit();
  vGD25WriteEnableAsync();
  vGD25ReadIDAsync(GD25_ID);
  vGD25ReadRDIDAsync(GD25_RDID);
  vGD25PageReadAsync(LOGGER_INFO_ADR, page_gd25_rx);

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
    vGD25SectorErase(LOGGER_INFO_ADR);
    memset(page_gd25_tx, 0xFF, sizeof(page_gd25_tx));
    memcpy(page_gd25_tx, GD25_ID, 16);
    vGD25PageProgramAsync(LOGGER_INFO_ADR, page_gd25_tx);
    vGD25PageReadAsync(LOGGER_INFO_ADR, page_gd25_rx);
  }else{
    Logger_search_valid_record();
  }

  xTaskCreate(vTask_logger_reader, "", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
  
  while(1){

    /*    */
    if(xSemaphoreTake(semaphore_MHC, 1000) != pdPASS){
      continue; // TODO error handler
    }

    time_glob.LiveCounter += LOGGER_TIME_STEP_SEC;
 //   if(INVERTER_isRUN(inverter_now)) time_glob.motorHoursCounter += LOGGER_TIME_STEP_SEC;
    // time_glob.state[0] = inverter_now->state_flags.value[0];
    // time_glob.state[1] = inverter_now->state_flags.value[1];
    // time_glob.current  = INVERTER_GET_CURRENT(inverter_now);
    // time_glob.volt_DC  = INVERTER_GET_BUS_VOLT(inverter_now);
    // time_glob.volt_out = INVERTER_GET_OUTVOLT(inverter_now);



    time_glob.crc16 = 0; //reset crc
    volatile uint16_t crc = crc16( (uint8_t *) &time_glob, sizeof(Typedef_LoggerRecord) );
    time_glob.crc16 = crc;
    logger.adr_cnt = 4096*logger.sector_cnt+logger.page_cnt*256;
    memcpy(&page_gd25_tx[sizeof(time_glob)*logger.record_cnt], (uint8_t *) &time_glob, sizeof(time_glob) );
    vGD25PageProgramAsync(logger.adr_cnt, page_gd25_tx);
    logger.record_cnt++;
    if(logger.record_cnt==4){ // change page
      logger.record_cnt=0;
      logger.page_cnt++;
      memset(page_gd25_tx, 0xFF, sizeof(page_gd25_tx));
      if(logger.page_cnt==16){
        logger.page_cnt = 0;
        logger.sector_cnt++;
        if(logger.sector_cnt > LOGGER_SECTORS_SIZE) {
          logger.sector_cnt = 0;
        }
        vGD25SectorErase(0x1000U*(logger.sector_cnt)); // erase sector
        uint32_t next_sector = logger.sector_cnt+1;
        if(next_sector > LOGGER_SECTORS_SIZE){
          next_sector = 0;
        }
        vGD25SectorErase(0x1000U*(next_sector)); // erase sector
      }
    }
    xSemaphoreGive(semaphore_MHC);
     

    vTaskDelay( LOGGER_TIME_STEP_SEC*1000U );
    
  }
  vTaskDelete(NULL);
}

void vTask_logger_reader(){
  vTaskDelay(1000);
  while(1){

    if(xSemaphoreTake(semaphore_MHC, 1000) != pdPASS){
      continue; // TODO error handler
    }

    vGD25PageReadAsync(logger_adr_rx, logger_RxBuf);
    logger_records_rx = (Typedef_LoggerRecord *) logger_RxBuf;
 
    xSemaphoreGive(semaphore_MHC);
    
    vTaskDelay(30);

  }
  vTaskDelete(NULL);
}


void Logger_search_valid_record(){
  while (1)
  {
    if(xSemaphoreTake(semaphore_MHC, 1000) != pdPASS){
      continue; // TODO error handler
    }
    logger.adr_cnt = 4096U*logger.sector_cnt+logger.page_cnt*256U;
    vGD25PageReadAsync(logger.adr_cnt, page_gd25_rx);
    xSemaphoreGive(semaphore_MHC);
    tmp_records = (Typedef_LoggerRecord *) page_gd25_rx;
    logger.record_cnt = 0;
    for (size_t i = 0; i < 4; i++)
    {
      if(tmp_records[i].crc16 == 0xFFFF){
         if(i==0){ // need to be tested
              uint32_t tmp_adr;
              if(logger.page_cnt==0 && logger.sector_cnt==0){
                tmp_adr = LOGGER_SECTORS_SIZE*16U*256U - 256U;
              }else{
                tmp_adr = logger.adr_cnt - 256U;  
              }
              vGD25PageReadAsync(tmp_adr, page_gd25_rx);
              i = 3;
              if(Logger_checkRecordCRC16(&(tmp_records[i])) == false){
                Logger_ErrorHandler();
              }
          }else{
            i=i-1;  
          }
          time_glob = tmp_records[i];
          return;
      }
      if(Logger_checkRecordCRC16(&(tmp_records[i])) == false){
        Logger_ErrorHandler();
      }
      logger.record_cnt++;
    }

    logger.page_cnt++;
    if(logger.page_cnt==16){
      logger.page_cnt = 0;
      logger.sector_cnt++;
      if(logger.sector_cnt > LOGGER_SECTORS_SIZE) {
        logger.sector_cnt = 0;
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

void Logger_ErrorHandler(){
  vGD25SectorErase(0);
  vGD25SectorErase(LOGGER_INFO_ADR);
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
