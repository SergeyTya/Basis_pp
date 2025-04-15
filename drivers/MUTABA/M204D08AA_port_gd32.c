#include "stdint.h"
#include "stdbool.h"

#include "gd32f4xx_libopt.h"

#include "FreeRTOS.h"
#include "task.h"

#include "M204D08AA.h"

#define M204D08AA_SPIx                           SPI3
#define M204D08AA_SPIx_CLK                       RCU_SPI3

#define M204D08AA_SPIx_SCK_PORT                  GPIOG
#define M204D08AA_SPIx_SCK_PIN                   GPIO_PIN_11
#define M204D08AA_SPIx_SCK_PIN_AF                GPIO_AF_6

#define M204D08AA_SPIx_MOSI_PORT                 GPIOG
#define M204D08AA_SPIx_MOSI_PIN                  GPIO_PIN_13
#define M204D08AA_SPIx_MOSI_PIN_AF               GPIO_AF_6

#define M204D08AA_SPIx_STB_PORT                  GPIOG
#define M204D08AA_SPIx_STB_PIN                   GPIO_PIN_14


#ifndef GD32_CONGIG_PIN_AS_AF

#define GD32_CONGIG_PIN_AS_AF(PORT, AF, PIN)                                      \
    {                                                                             \
        gpio_mode_set((PORT), GPIO_MODE_AF, GPIO_PUPD_NONE, (PIN));               \
        gpio_output_options_set((PORT), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, (PIN)); \
        gpio_af_set((PORT), AF, (PIN));                                           \
    }

#define GD32_CONGIG_PIN_AS_OUT(PORT, PIN)                                     \
    {                                                                         \
        gpio_mode_set(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN);           \
        gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN); \
    }

#endif



static spi_parameter_struct GD25Q32_spi_struct;

void M204D08AA_SPI_Config();
void M204D08AA_gpio_congig();

void M204D08AA_HardInit()
{
  M204D08AA_gpio_congig();
  vTaskDelay(300);
  M204D08AA_SPI_Config();
}

void M204D08AA_STB_SetState(bool state){
    if(state == true){
        gpio_bit_reset(M204D08AA_SPIx_STB_PORT, M204D08AA_SPIx_STB_PIN);
    }else{
        gpio_bit_set(M204D08AA_SPIx_STB_PORT, M204D08AA_SPIx_STB_PIN);
    }
}

void M204D08AA_STB_WriteWord(uint16_t data){
   spi_i2s_data_transmit(M204D08AA_SPIx, data);
  while(spi_i2s_flag_get(M204D08AA_SPIx, SPI_FLAG_TRANS));
}

void M204D08AA_SPI_Config(void)
{

  rcu_periph_clock_enable(RCU_GPIOG);
  rcu_periph_clock_enable(M204D08AA_SPIx_CLK);
 
   // SPI config
 
   spi_i2s_deinit(M204D08AA_SPIx);
   spi_struct_para_init(&GD25Q32_spi_struct);
   GD25Q32_spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
   GD25Q32_spi_struct.device_mode = SPI_MASTER;
   GD25Q32_spi_struct.frame_size = SPI_FRAMESIZE_16BIT;
   GD25Q32_spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
   GD25Q32_spi_struct.prescale = SPI_PSC_128;
   GD25Q32_spi_struct.nss = SPI_NSS_HARD;
   GD25Q32_spi_struct.endian = SPI_ENDIAN_MSB;

   spi_init(M204D08AA_SPIx, &GD25Q32_spi_struct);
   spi_nss_output_enable(M204D08AA_SPIx);


    spi_enable(M204D08AA_SPIx);
  

}

void M204D08AA_gpio_congig(){
         // SPI CLK
         GD32_CONGIG_PIN_AS_AF(M204D08AA_SPIx_SCK_PORT, M204D08AA_SPIx_SCK_PIN_AF, M204D08AA_SPIx_SCK_PIN);
         // SPI TX
         GD32_CONGIG_PIN_AS_AF(M204D08AA_SPIx_MOSI_PORT, M204D08AA_SPIx_MOSI_PIN_AF, M204D08AA_SPIx_MOSI_PIN);
         // SPI NSS
         GD32_CONGIG_PIN_AS_OUT(M204D08AA_SPIx_STB_PORT, M204D08AA_SPIx_STB_PIN);

         gpio_bit_set(M204D08AA_SPIx_STB_PORT, M204D08AA_SPIx_STB_PIN);
}