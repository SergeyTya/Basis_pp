/*
 * 		GD25Q32_os.c
 *
 *  GD25Q32 FreeRTOS driver.
 *
 *  Created on: 21.12.2023 г.
 *      Author: Sergey Tyagushev
 */


#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"


#include "GD25Q32E.h"
#include "GD25Q32E_port.h"
#include "GD25Q32_os.h"

#include "FreeRTOS.h"
#include "task.h"

extern uint8_t flashID[16];

static uint8_t ui8GD25ReadRDSRAsync(uint8_t num);
void gd32_send_await(uint8_t* req, Typedef_gd25q32Cmd* cmd);

#define GD23Q32_STATUS_BYTE1 1
#define GD23Q32_STATUS_BYTE2 2
#define GD23Q32_STATUS_BYTE3 3
\
#define GD25Q32_SEND_AWAIT(REQ, CMD) gd32_send_await(&REQ, &CMD);

#define GD25Q32_FLAG_AWAIT_RESET(READ_FLAG_ASYNC) \
   {                                              \
      while (READ_FLAG_ASYNC == true)             \
      {                                           \
         ;                                        \
      }                                           \
   }
#define GD25Q32_FLAG_AWAIT_SET(READ_FLAG_ASYNC) \
   {                                            \
      while (READ_FLAG_ASYNC == false)          \
      {                                         \
         ;                                      \
      }                                         \
   }

void gd32_send_await(uint8_t* req, Typedef_gd25q32Cmd* cmd) {
   GD25Q32_sendData(req, cmd->req_sz, cmd->res_sz);
   while ( !GD25Q32_IS_TRANSACTION_RX_DONE ) {
      vTaskDelay(1);
   }
   GD25Q32_DMA_disable();
}

/**
 * MCU Hardware init
 */
void vGD25HardWareInit()
{
   GD25Q32_hard_init();
}

/**
 * Read Unique 128 bit chip ID
 * @param {uint8_t * id[16]}  - pointer to dest array
 */
void vGD25ReadIDAsync(uint8_t id[16])
{
   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_READ_ID;
   uint8_t req[5] = { cmd.id, 0, 0, 0, 0 };
   GD25Q32_SEND_AWAIT(req, cmd);
   GD25Q32_READ_BUFF(id, cmd.req_sz, cmd.res_sz);
}
/**
 * Read Identification (RDID) (9FH)
 *
 * Manufacture ID/Memory type/Memory capacity
 *
 * C8/40/16 for GD25Q32
 *
 * @param {uint8_t * id[2]}  - pointer to dest array
 */
void vGD25ReadRDIDAsync(uint8_t id[3])
{
   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_READ_RDID;
   uint8_t req[1] = { cmd.id };
   GD25Q32_SEND_AWAIT(req, cmd);
   GD25Q32_READ_BUFF(id, cmd.req_sz, cmd.res_sz);
}
/**
 * Write enable (06H)
 */
void  vGD25WriteEnableAsync()
{
   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_WREN;
   uint8_t req[1] = { cmd.id };
   GD25Q32_SEND_AWAIT(req, cmd);
}
/**
 * Write disable (04H)
 */
void vGD25WriteDisableAsync()
{
   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_WRDI;
   uint8_t req[1] = { cmd.id };
   GD25Q32_SEND_AWAIT(req, cmd);
}

/**
 * Read status flag byte
 * @param  {uint8_t num} options:
 * @param  GD23Q32_STATUS_BYTE1
 * @param  GD23Q32_STATUS_BYTE2
 * @param  GD23Q32_STATUS_BYTE3
 * @retval {bool} flag status
 */
static uint8_t ui8GD25ReadRDSRAsync(uint8_t num)
{
   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_RDSR1;
   switch (num)
   {
   case 2:
      cmd.id = gd25q32Cmd_RDSR2;
      break;
   case 3:
      cmd.id = gd25q32Cmd_RDSR3;
      break;

   default:
      break;
   }
   uint8_t req[2] = { cmd.id, 0 };
   GD25Q32_SEND_AWAIT(req, cmd);
   uint8_t res = 0;
   GD25Q32_READ_BUFF(&res, 1, 1);
   return res;
}
/**
 * Read "Write in Progress" flag
 *
 * @retval {bool} flag status
 */
bool blGD25ReadWIPFlagAsync()
{
   uint8_t res = ui8GD25ReadRDSRAsync(GD23Q32_STATUS_BYTE1);
   bool ret_val = (res & 1) != 0;
   return ret_val;
}

/**
 * Read Write Enable Latch flag
 *
 * @retval {bool} flag status
 */
bool blGD25ReadWELFlagAsync()
{
   uint8_t res = ui8GD25ReadRDSRAsync(GD23Q32_STATUS_BYTE1);
   bool ret_val = (res & 2) != 0;
   return ret_val;
}

/**
 * Program 256 byte  (write/disable included)
 * @param  {uint32_t adr}  24-bit address
 * @param  {uint8_t* } pointer to 256 byte data
 */
void vGD25PageProgramAsync(uint32_t adr, uint8_t page[256])
{
   GD25Q32_FLAG_AWAIT_RESET(blGD25ReadWIPFlagAsync());
   vGD25WriteEnableAsync();

   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_PW;
   GD25Q32_TX_BUF[0] = cmd.id;
   GD25Q32_TX_BUF[1] = ((uint8_t*)&adr)[2];
   GD25Q32_TX_BUF[2] = ((uint8_t*)&adr)[1];
   GD25Q32_TX_BUF[3] = ((uint8_t*)&adr)[0];
   memcpy(&GD25Q32_TX_BUF[4], page, 256);
   GD25Q32_SEND_AWAIT(GD25Q32_TX_BUF, cmd);

   GD25Q32_FLAG_AWAIT_RESET(blGD25ReadWIPFlagAsync());
   vGD25WriteDisableAsync();
}
/**
 * Read 256 byte page
 *
 * @param  {uint32_t adr }  24-bit address
 * @param  {uint8_t* page } pointer to data page
 *
 */
void vGD25PageReadAsync(uint32_t adr, uint8_t page[256])
{
   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_PR;

   GD25Q32_TX_BUF[0] = cmd.id;
   GD25Q32_TX_BUF[1] = ((uint8_t*)&adr)[2];
   GD25Q32_TX_BUF[2] = ((uint8_t*)&adr)[1];
   GD25Q32_TX_BUF[3] = ((uint8_t*)&adr)[0];
   GD25Q32_SEND_AWAIT(GD25Q32_TX_BUF, cmd);
   GD25Q32_READ_BUFF(page, 4, 256);
}
/**
 * Erase 4096 byte sector
 * @param  {uint32_t adr }  24-bit address
 */
void vGD25SectorErase(uint32_t adr)
{
   GD25Q32_FLAG_AWAIT_RESET(blGD25ReadWIPFlagAsync());
   vGD25WriteEnableAsync();

   Typedef_gd25q32Cmd cmd = GD25Q32_COMMAND_SE;

   GD25Q32_TX_BUF[0] = cmd.id;
   GD25Q32_TX_BUF[1] = ((uint8_t*)&adr)[2];
   GD25Q32_TX_BUF[2] = ((uint8_t*)&adr)[1];
   GD25Q32_TX_BUF[3] = ((uint8_t*)&adr)[0];

   GD25Q32_SEND_AWAIT(GD25Q32_TX_BUF, cmd);

   GD25Q32_FLAG_AWAIT_RESET(blGD25ReadWIPFlagAsync());
   vGD25WriteDisableAsync();
}


