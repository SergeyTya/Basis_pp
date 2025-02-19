#include "stdint.h"
#include "string.h"
#include "stdbool.h"

#define APPLICATION_ADDRESS UUB_BAS_ADR // адрес начала программы

#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"

#include "uuboot.h"

// https://habr.com/ru/articles/789598/
// https://habr.com/ru/articles/754216/
// https://habr.com/ru/articles/575014/
// https://hubstub.ru/stm32/159-pishim-svoy-butlouder-dlya-stm32.html
// https://habr.com/ru/articles/275351/

void Go_To_User_App();
uint32_t UUB_crc32(const void *data, size_t len);
uint32_t UUB_crc32_r(const void *data, size_t len);
void mem_copy(size_t len);

volatile uint32_t crc_strg;
volatile uint32_t len_strg;

volatile uint32_t crc_main;
volatile uint32_t len_main;

uint32_t crc_strg_est = 0;
uint32_t crc_main_est = 0;

bool isValid_main;
bool isValid_strg;
bool needUpdate;

// volatile char out;
void main()
{
  // Go_To_User_App();
   
  crc_strg = *((uint32_t *)(UUB_INF1_ADR + 0x10));
  len_strg = *((uint32_t *)(UUB_INF1_ADR));

  crc_main = *((uint32_t *)(UUB_INF2_ADR + 0x10));
  len_main = *((uint32_t *)(UUB_INF2_ADR));

  if (len_strg > 0 && len_strg < 262144U)
  {
    crc_strg_est = UUB_crc32_r((unsigned char *)UUB_STR_ADR, len_strg);
  }

  if (len_main > 0 && len_main < 262144U)
  {
    crc_main_est = UUB_crc32_r((unsigned char *)UUB_BAS_ADR, len_main);
  }

  isValid_main = crc_main == crc_main_est;
  isValid_strg = crc_strg == crc_strg_est;
  needUpdate = isValid_strg && (crc_main_est != crc_strg_est);

  // while(1) ;

  if (needUpdate)
  {
    // change sw
    mem_copy(len_strg);
  }

  if (isValid_main)
  {
    // jump to application
    Go_To_User_App();
  }

  vBootloader_run();
}

void Go_To_User_App()
{
  __disable_irq();
  SCB->VTOR = APPLICATION_ADDRESS;
  uint32_t app_jump_address;

  typedef void (*pFunction)(void);
  pFunction Jump_To_Application;

  app_jump_address = *(uint32_t *)(APPLICATION_ADDRESS + 4);
  Jump_To_Application = (pFunction)app_jump_address;
  __set_MSP(*(volatile uint32_t *)APPLICATION_ADDRESS);
  Jump_To_Application();
}

uint32_t UUB_crc32(const void *data, size_t len)
{
  const unsigned char *buffer = (const unsigned char *)data;
  uint32_t crc = -1;

  while (len--)
  {
    crc = crc ^ (*buffer++ << 24);
    for (int bit = 0; bit < 8; bit++)
    {
      if (crc & (1L << 31))
        crc = (crc << 1) ^ 0x04C11DB7L;
      else
        crc = (crc << 1);
    }
  }
  return ~crc;
}

uint32_t UUB_crc32_r(const void *data, size_t len)
{
  const unsigned char *buffer = (const unsigned char *)data;
  uint32_t crc = -1;

  while (len--)
  {
    crc = crc ^ *buffer++;
    for (int bit = 0; bit < 8; bit++)
    {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xEDB88320L;
      else
        crc = (crc >> 1);
    }
  }
  return ~crc;
}

void mem_copy(size_t len)
{
  fmc_unlock();

  fmc_flag_clear(FMC_FLAG_RDDERR);
  fmc_flag_clear(FMC_FLAG_PGSERR);
  fmc_flag_clear(FMC_FLAG_PGMERR);
  fmc_flag_clear(FMC_FLAG_WPERR);
  fmc_flag_clear(FMC_FLAG_OPERR);
  fmc_flag_clear(FMC_FLAG_END);

  fmc_sector_erase(CTL_SECTOR_NUMBER_3);
  fmc_sector_erase(CTL_SECTOR_NUMBER_5);
  fmc_sector_erase(CTL_SECTOR_NUMBER_6);

  for (size_t i = 0; i < len; i++)
  {
    fmc_flag_clear(FMC_FLAG_RDDERR);
    fmc_flag_clear(FMC_FLAG_PGSERR);
    fmc_flag_clear(FMC_FLAG_PGMERR);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_OPERR);
    fmc_flag_clear(FMC_FLAG_END);

    fmc_byte_program(UUB_BAS_ADR + i, *((uint8_t *)UUB_STR_ADR + i));
  }

  fmc_word_program(UUB_INF2_ADR, *((uint32_t *)UUB_INF1_ADR));
  fmc_word_program(UUB_INF2_ADR + 0x10, *((uint32_t *)(UUB_INF1_ADR + 0x10)));

  fmc_lock();
  NVIC_SystemReset();
}

