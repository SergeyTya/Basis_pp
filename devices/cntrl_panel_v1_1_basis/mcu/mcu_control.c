#include "mcu_control.h"
#include "string.h"
#include "gd32f4xx_libopt.h"
#include "dev_cnfg.h"
#include "mcu_init.h"


/**
 * Write array or uint16_t to Flash
 * @param {uint16_t * src} pointer to array
 * @param {size_t  size}   array size
 */
bool hw_write_FLASH(uint32_t *src, size_t size)
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

   size_t len = size;

   for (size_t i = 0; i < len; i++)
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
void hw_read_FLASH(uint32_t *dst, size_t size)
{
   uint32_t *src = (uint32_t *)0x080E0000;
   memcpy(dst, src, sizeof(uint32_t) * size);
}


uint8_t hw_get_revision(){


   volatile uint8_t bit1 = gpio_input_bit_get(GPIOG, GPIO_PIN_8); 
   volatile uint8_t bit2 = gpio_input_bit_get(GPIOC, GPIO_PIN_7);
   volatile uint8_t bit3 = gpio_input_bit_get(GPIOC, GPIO_PIN_8);

   uint8_t ret_val =  bit1+(bit2<<1)+(bit3<<2);

   return ret_val;
}

uint16_t hw_read_AI(){
       /* ADC routine channel config */
    //adc_routine_channel_config(ADC, 0U, ADC_CHANNEL_0, ADC_SAMPLETIME_15);
  
    uint32_t ret_val = 0;
    int k = 10;
    for(int i = 0; i < k; i++){

       /* ADC software trigger enable */
   adc_software_trigger_enable(ADC, ADC_ROUTINE_CHANNEL);

    /* wait the end of conversion flag */
    while(!adc_flag_get(ADC, ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    
    /* return regular channel sample value */
    ret_val +=   adc_routine_data_read(ADC);

    adc_flag_clear(ADC, ADC_FLAG_EOC);


    }

    ret_val /= k;
   
    return  ret_val;

}


