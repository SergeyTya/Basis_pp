#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "GD25Q32_os.h"
#include "mbsupport.h"
#include "task_master.h"

#include "panelConfig.h"
#include "masterTransport.h"

#include "timers.h"

#include "meter.h"
#include "clock.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "crc16.h"

#include "../fifo_buffer/fifo_buffer.h"

#define LOGGER_INFO_ADR 0x0000U

extern uint8_t page_gd25_tx[256];
extern uint8_t page_gd25_rx[256];

uint8_t GD25_ID[16];
uint8_t GD25_RDID[3];

FIFO_Buffer fifo;

Typedef_LoggerRecord rec_disp;
volatile uint8_t log_fifo_rx_pointer = FIFO_SIZE;

extern Typedef_Clock clock;

uint16_t reslc[5];

SemaphoreHandle_t semaphore_MHC;

volatile uint32_t lc_res = 0;
volatile bool lc_res_bl = false;

// use block 1 as work hour counter

volatile uint32_t fiind = 0;

Typedef_LoggerRecord slave_null = {.type="NON"};

void vTask_logger_reader()
{
  vTaskDelay(100);
  while (1)
  {

    while (xSemaphoreTake(semaphore_MHC, portMAX_DELAY) != pdPASS) ;
    
    if(!FIFO_Peek(&fifo, log_fifo_rx_pointer, &rec_disp))
    {
      rec_disp = slave_null;
    }

    xSemaphoreGive(semaphore_MHC);
    vTaskDelay(300);
  }

}

#include "../fifo_buffer/sma.h"

// Use it to reset clock
const Typedef_RtcTimeStamp null_time = {.timestamp_day = 0, .timestamp_hour = 0, .timestamp_minute = 0, .timestamp_month = 0, .timestamp_second = 0, .timestamp_year = 0};
// Logger records for every channel
Typedef_LoggerRecord slave_rec[5] = {{.type = {0, 0}}, {.type = "AC1"}, {.type = "AC2"}, {.type = "DC1"}, {.type = "DC2"}};

// Global clock
extern Typedef_Clock clock;

// SMA filters
SMA_Filter filterI[5];


//
static Typedef_LoggerRecord test_rec1 = {.type="AC1", .F=400 };
static Typedef_LoggerRecord test_rec2 = {.type="AC2", .F=400 };
static Typedef_LoggerRecord test_rec3 = {.type="DC1", .U=17, .Im=40 };
static Typedef_LoggerRecord test_rec4 = {.type="DC2", .U=20 };

extern TypeDef_Master master;
void vTask_logger_clear(){

    while (xSemaphoreTake(semaphore_MHC, portMAX_DELAY) != pdPASS);
        FIFO_Init(&fifo);
        vTaskDelay(300);
    xSemaphoreGive(semaphore_MHC);

}

void vTask_tes_writer(){

  vTaskDelay(300);
  while(1)
  {
    while (xSemaphoreTake(semaphore_MHC, portMAX_DELAY) != pdPASS);
    Clock_get(&test_rec1.timeStart);
    test_rec1.timeStop.timestamp_minute =  test_rec1.timeStart.timestamp_second;
    FIFO_write_crc(&test_rec1);
    FIFO_Enqueue(&fifo, &test_rec1); 
    vTaskDelay(1);
    xSemaphoreGive(semaphore_MHC);
    vTaskDelay(5000);

  }

}


extern void  master_RDO4_setstate(bool state);
void vTask_logger()
{
  log_fifo_rx_pointer = 0;
  TypeDef_Master *const m = &master;

  semaphore_MHC = xSemaphoreCreateBinary();
  xSemaphoreGive(semaphore_MHC); 
  while (xSemaphoreTake(semaphore_MHC, portMAX_DELAY) != pdPASS);

  memset(page_gd25_tx, 0xFF, 256);
  vTaskDelay(1);
  vGD25HardWareInit();
  vGD25WriteEnableAsync();
  vGD25ReadIDAsync(GD25_ID);
  vGD25ReadRDIDAsync(GD25_RDID);
  vGD25PageReadAsync(LOGGER_INFO_ADR, page_gd25_rx);

  if (/* Search valid record */!FIFO_Scan(&fifo))
  {
    FIFO_Init(&fifo);
    vTaskDelay(1);
  }

  
  log_fifo_rx_pointer = FIFO_last_indx(&fifo)>=0 ? FIFO_last_indx(&fifo) : 0;

  xTaskCreate(vTask_logger_reader, "", configMINIMAL_STACK_SIZE*3, NULL, tskIDLE_PRIORITY + 2, NULL);
 // xTaskCreate(vTask_tes_writer,  "", configMINIMAL_STACK_SIZE*3, NULL, tskIDLE_PRIORITY + 1, NULL);

  xSemaphoreGive(semaphore_MHC); 
  while (1)
  {

    for (size_t i = 1; i < 5; i++)
    {
      if (
        /* Session started */
        (m->slave[i].slaveStates == MASTER_STATE_onRUN) && (slave_rec[i].slave_state != MASTER_STATE_onRUN))
      {
        
        // Save time
        slave_rec[i].timeStart = clock.time;
        // Reset values
        slave_rec[i].I = 0;
        slave_rec[i].U = 0;
        slave_rec[i].F = 0;
        slave_rec[i].Im = 0;
        slave_rec[i].LiveCounter = 0;
        slave_rec[i].timeStop = null_time;
        slave_rec[i].crc16 = 0;
        slave_rec[i].code  = 0;

        // Init filters
        sma_init(&filterI[i]);
      }

      if (
        /* Session stoped */
        (m->slave[i].slaveStates != MASTER_STATE_onRUN) && (slave_rec[i].slave_state == MASTER_STATE_onRUN)

      )
      {
        // Save time
        slave_rec[i].timeStop = clock.time;
        // Fault code
        if(m->slave[i].fault_source_pm){
          slave_rec[i].code  = m->slave[i].fault_code_pm;
        }else{
          slave_rec[i].code  = m->slave[i].fault_code;
        }
        // write crc
        uint16_t crc = crc16((uint8_t *)&slave_rec[i], sizeof(Typedef_LoggerRecord) - 2);
        slave_rec[i].crc16 = crc;
        // Place to fifo
         while (xSemaphoreTake(semaphore_MHC, portMAX_DELAY) != pdPASS) ;
        FIFO_Enqueue(&fifo, &slave_rec[i]);
        xSemaphoreGive(semaphore_MHC);
      }

      if ( /* Session in progress */
          (m->slave[i].slaveStates == MASTER_STATE_onRUN))
      {
     
        slave_rec[i].I   = m->slave[i].Iav;
        slave_rec[i].U   = m->slave[i].Uav;
        slave_rec[i].F   = m->slave[i].F;
        slave_rec[i].Im  = m->slave[i].Imax;
        slave_rec[i].LiveCounter++;  
      }
      // save status
      slave_rec[i].slave_state = m->slave[i].slaveStates;
    } // slave for


    // // HVIL to RDO4
    // if(
    //   m->HVIL[0]||m->HVIL[1]||m->HVIL[2]||m->HVIL[3]||m->HVIL[4] 
    // ){
    //   master_RDO4_setstate(1);
    // }else{
    //   master_RDO4_setstate(0);
    // }

    vTaskDelay(300);
  } // RTOS while
}