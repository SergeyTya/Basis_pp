#include "mcu_control.h"
#include "stm32f4xx_hal_flash.h"
#include "string.h"


/**
 * Enable hardware PWM outputs
 *
 */
void hw_pwm_enable()
{
   TIM1->BDTR = TIM_BDTR_MOE | TIM_BDTR_OSSR | 50;
}

/**
 * Disable hardware PWM outputs
 *
 */
void hw_pwm_disable()
{
   TIM1->BDTR = TIM_BDTR_OSSR | 50;
}

/**
 * Set hardware PWM duty
 * @param {uint32_t duty[3]} values of CCR registers ready to write to HW
 */
void hw_set_duty(uint32_t duty[3])
{
   TIM1->CCR1 = duty[0];
   TIM1->CCR2 = duty[1];
   TIM1->CCR3 = duty[2];
}

/**
 * Write array or uint16_t to Flash
 * @param {uint16_t * src} pointer to array
 * @param {size_t  size}   array size
 */ 
bool hw_write_FLASH(uint16_t *src, size_t size)
{
   uint32_t SectorError = 0;
   SectorError = 0;
   //__disable_irq();

   HAL_FLASH_Unlock(); // unlock the flash
   FLASH_EraseInitTypeDef FlashErase;
   FlashErase.Sector = FLASH_SECTOR_7;
   FlashErase.NbSectors = 1;
   FlashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
   FlashErase.VoltageRange = VOLTAGE_RANGE_2;
  
   HAL_FLASHEx_Erase(&FlashErase, &SectorError); // The erase function
   if(SectorError == 0){
      return false;
   }
  
   uint32_t FlashMemAddress = 0x08060000;        // Start writing from the 1st address
   for (size_t i = 0; i < size; i++)
   {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FlashMemAddress, src[i]);
      FlashMemAddress += 2; // 2 byte increment
   }

   HAL_FLASH_Lock(); // lock the flash
   return true;

   //__enable_irq();
}

/**
 * Read array or uint16_t to Flas
 * @param {uint16_t * dst} pointer to array
 * @param {size_t  size}   array size
 */
void hw_read_FLASH(uint16_t *dst, size_t size)
{
   //__disable_irq();
   uint16_t * src = (uint16_t *) 0x08060000;
   memcpy(dst, src, sizeof(uint16_t) * size);
   //__enable_irq();
}
