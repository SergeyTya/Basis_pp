/*
 * 		GD25Q32_os.h
 *
 *  GD25Q32 FreeRTOS tasks.
 *
 *  Created on: 21.12.2023 г.
 *      Author: Sergey Tyagushev
 */

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"

#include "gd32f4xx_libopt.h"

#include "GD25Q32E.h"
#include "GD25Q32_os.h"
#include "hoursCounter.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

uint8_t flashID[16] = {0};

volatile bool GD25_flagstatus = false;
volatile bool rw_proc = false;
uint8_t page_gd25_tx[256];
uint8_t page_gd25_rx[256];

volatile uint32_t GD25adr = 0x0;
volatile uint8_t GD25dtr = 0x1;

volatile uint8_t rdsr_num = 1;




void vUsageExampleTask()
{
  vTaskDelay(1);
  vGD25HardWareInit();
  vGD25WriteEnableAsync();

  while (1)
  {
    // vGD25ReadIDAsync();
    for (size_t i = 0; i < 256; i++)
    {
      page_gd25_tx[i++]   = 2;
      page_gd25_tx[i++] = 255;
      page_gd25_tx[i++] = 3;
      page_gd25_tx[i] = 254;
    }
    
    if (rw_proc == true)
    {
      vGD25SectorErase(GD25adr);
      vGD25PageProgramAsync(GD25adr, (uint8_t *)page_gd25_tx);
      rw_proc = false;
    }

    vGD25PageReadAsync(GD25adr, (uint8_t *)page_gd25_rx);


    vTaskDelay(10);
  }
  vTaskDelete(NULL);
}

