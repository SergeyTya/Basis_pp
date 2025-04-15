#include "mcu_control.h"
#include "string.h"
#include "gd32f4xx_libopt.h"
#include "dev_cnfg.h"


/**
 * Write array or uint16_t to Flash
 * @param {uint16_t * src} pointer to array
 * @param {size_t  size}   array size
 */
bool hw_write_FLASH(uint16_t *src, size_t size)
{
   uint32_t scEEP_base_adr = 0x080E0000U;

   /* WDG -DISABLE */
   // vDisableWDG();
   __disable_irq();

   fmc_unlock();
   fmc_flag_clear(FMC_FLAG_RDDERR);
   fmc_flag_clear(FMC_FLAG_PGSERR);
   fmc_flag_clear(FMC_FLAG_PGMERR);
   fmc_flag_clear(FMC_FLAG_WPERR);
   fmc_flag_clear(FMC_FLAG_OPERR);
   fmc_flag_clear(FMC_FLAG_END);

   fmc_sector_erase(CTL_SECTOR_NUMBER_11);
   fmc_flag_clear(FMC_FLAG_RDDERR);
   fmc_flag_clear(FMC_FLAG_PGSERR);
   fmc_flag_clear(FMC_FLAG_PGMERR);
   fmc_flag_clear(FMC_FLAG_WPERR);
   fmc_flag_clear(FMC_FLAG_OPERR);
   fmc_flag_clear(FMC_FLAG_END);

   size_t len = size / 2;
   if ((size & 1) == 1)
   {
      len += 1;
   }

   for (int i = 0; i < len; i++)
   {
      fmc_flag_clear(FMC_FLAG_RDDERR);
      fmc_flag_clear(FMC_FLAG_PGSERR);
      fmc_flag_clear(FMC_FLAG_PGMERR);
      fmc_flag_clear(FMC_FLAG_WPERR);
      fmc_flag_clear(FMC_FLAG_OPERR);
      fmc_flag_clear(FMC_FLAG_END);
      uint32_t data = ((uint32_t *)src)[i];
      fmc_word_program(scEEP_base_adr, data);
      scEEP_base_adr += 4;
   }

   fmc_lock();
   /* WDG -ENABLE*/
   __enable_irq();
   // vEnableWDG();

   return true;
}

/**
 * Read array or uint16_t to Flas
 * @param {uint16_t * dst} pointer to array
 * @param {size_t  size}   array size
 */
void hw_read_FLASH(uint16_t *dst, size_t size)
{
   uint16_t *src = (uint16_t *)0x080E0000;
   memcpy(dst, src, sizeof(uint16_t) * size);
}
