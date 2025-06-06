#define KBD_SPI                 SPI2
#define KBD_SPI_RCU             RCU_SPI2
#define KBD_SPI_PORT_RCU        RCU_GPIOB
#define KBD_SPI_CLK_PORT        GPIOB
#define KBD_SPI_CLK_PIN         GPIO_PIN_3
#define KBD_SPI_CLK_PIN_AF      GPIO_AF_6
#define KBD_SPI_MOSI_PORT       GPIOC
#define KBD_SPI_MOSI_PIN        GPIO_PIN_12
#define KBD_SPI_MOSI_PIN_AF     GPIO_AF_6
#define KBD_SPI_MISO_PORT       GPIOB
#define KBD_SPI_MISO_PIN        GPIO_PIN_4
#define KBD_SPI_MISO_PIN_AF     GPIO_AF_6
#define KBD_SPI_NSS1_PORT_RCU   RCU_GPIOA
#define KBD_SPI_NSS1_PORT       GPIOA
#define KBD_SPI_NSS1_PIN        GPIO_PIN_15
#define KBD_SPI_NSS2_PORT_RCU   RCU_GPIOG
#define KBD_SPI_NSS2_PORT       GPIOG
#define KBD_SPI_NSS2_PIN        GPIO_PIN_15

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


#include "gd32f4xx_libopt.h"
#include <stdbool.h>

void keyboard_spi_hw_init(){

    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(KBD_SPI_PORT_RCU);
    rcu_periph_clock_enable(KBD_SPI_RCU);
    rcu_periph_clock_enable(KBD_SPI_NSS1_PORT_RCU);
    rcu_periph_clock_enable(KBD_SPI_NSS2_PORT_RCU);

    // SPI CLK
    GD32_CONGIG_PIN_AS_AF(KBD_SPI_CLK_PORT, KBD_SPI_CLK_PIN_AF, KBD_SPI_CLK_PIN);
    // SPI RX
    GD32_CONGIG_PIN_AS_AF(KBD_SPI_MISO_PORT, KBD_SPI_MISO_PIN_AF, KBD_SPI_MISO_PIN);
    // SPI TX
    GD32_CONGIG_PIN_AS_AF(KBD_SPI_MOSI_PORT, KBD_SPI_MOSI_PIN_AF, KBD_SPI_MOSI_PIN);
    // SPI NSS
     GD32_CONGIG_PIN_AS_OUT(KBD_SPI_NSS1_PORT, KBD_SPI_NSS1_PIN);
     GD32_CONGIG_PIN_AS_OUT(KBD_SPI_NSS2_PORT, KBD_SPI_NSS2_PIN);

    // gpio_mode_set(KBD_SPI_NSS1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, KBD_SPI_NSS1_PIN);          
    // gpio_output_options_set(KBD_SPI_NSS1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, KBD_SPI_NSS1_PIN);

    // gpio_mode_set(KBD_SPI_NSS2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, KBD_SPI_NSS2_PIN);          
    // gpio_output_options_set(KBD_SPI_NSS2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, KBD_SPI_NSS2_PIN);



    // SPI config
    spi_parameter_struct KBD_spi_struct;
    spi_i2s_deinit(KBD_SPI);
    spi_struct_para_init(&KBD_spi_struct);
    KBD_spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    KBD_spi_struct.device_mode = SPI_MASTER;
    KBD_spi_struct.frame_size = SPI_FRAMESIZE_8BIT;
    KBD_spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    KBD_spi_struct.prescale = SPI_PSC_256;
    KBD_spi_struct.nss = SPI_NSS_HARD;
    KBD_spi_struct.endian = SPI_ENDIAN_MSB;

    spi_init(KBD_SPI, &KBD_spi_struct);
    spi_nss_output_enable(KBD_SPI);

    spi_enable(KBD_SPI);

}

static int keyboardSpiPos = 0;
static uint16_t spi_rx=0;

uint16_t keyboard_spi_hw_rw(uint8_t data){

    gpio_bit_set(KBD_SPI_NSS1_PORT, KBD_SPI_NSS1_PIN);

    gpio_bit_reset(KBD_SPI_NSS2_PORT, KBD_SPI_NSS2_PIN);
    gpio_bit_reset(KBD_SPI_NSS2_PORT, KBD_SPI_NSS2_PIN);
    gpio_bit_set(KBD_SPI_NSS2_PORT, KBD_SPI_NSS2_PIN);

    gpio_bit_reset(KBD_SPI_NSS1_PORT, KBD_SPI_NSS1_PIN);

    // SPI send data
    spi_i2s_data_transmit(KBD_SPI, data);
    while (spi_i2s_flag_get(KBD_SPI, SPI_FLAG_TRANS) == SET) {
        ;
    }

    ((uint8_t *) &spi_rx)[0] = spi_i2s_data_receive(KBD_SPI);

    spi_i2s_data_transmit(KBD_SPI, data);
    while (spi_i2s_flag_get(KBD_SPI, SPI_FLAG_TRANS) == SET) {
        ;
    }

    ((uint8_t *) &spi_rx)[1] = spi_i2s_data_receive(KBD_SPI);

    return spi_rx;

}


