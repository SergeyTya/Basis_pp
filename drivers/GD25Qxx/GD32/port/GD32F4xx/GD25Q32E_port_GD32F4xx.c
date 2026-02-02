/*
 * 		GD25Q32E.c
 *
 *  GD25Q32 hardware driver for GD32F4xx MCU
 *
 *  Created on: 21.12.2023 г.
 *      Author: Sergey Tyagushev
 */

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"

#include "gd32f4xx_libopt.h"

#include "GD25Q32E_port.h"

uint8_t GD25Q32_spi_send_array[GD25Q32_BUF_SZ] = {0};
uint8_t GD25Q32_spi_receive_array[GD25Q32_BUF_SZ] = {0};

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

spi_parameter_struct GD25Q32_spi_struct;
dma_single_data_parameter_struct dma_init_struct;

static void GD25Q32_DMA_enable(int32_t cnt);

static void GD25Q32_DMA_enable(int32_t cnt)
{
    // memset(GD25Q32_spi_receive_array, 0, GD25Q32_BUF_SZ);
    
    gpio_bit_reset(GD25Q32_SPI_NSS_PORT, GD25Q32_SPI_NSS_PIN);
    spi_enable(GD25Q32_SPI);

    dma_transfer_number_config(GD25Q32_SPI_TX_DMA, cnt); // setup dma max transfers
    dma_transfer_number_config(GD25Q32_SPI_RX_DMA, cnt);

    dma_flag_clear(GD25Q32_SPI_TX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_flag_clear(GD25Q32_SPI_RX_DMA, DMA_FLAG_FTF);

    spi_dma_enable(GD25Q32_SPI, SPI_DMA_RECEIVE);
    spi_dma_enable(GD25Q32_SPI, SPI_DMA_TRANSMIT);

    dma_channel_enable(GD25Q32_SPI_RX_DMA);
    dma_channel_enable(GD25Q32_SPI_TX_DMA);
}

void GD25Q32_DMA_disable()
{
    gpio_bit_set(GD25Q32_SPI_NSS_PORT, GD25Q32_SPI_NSS_PIN);

    spi_dma_disable(GD25Q32_SPI, SPI_DMA_RECEIVE);
    spi_dma_disable(GD25Q32_SPI, SPI_DMA_TRANSMIT);

    dma_channel_disable(GD25Q32_SPI_RX_DMA); // enable DMA
    dma_channel_disable(GD25Q32_SPI_TX_DMA);

    dma_flag_clear(GD25Q32_SPI_TX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_flag_clear(GD25Q32_SPI_RX_DMA, DMA_FLAG_FTF);

    spi_disable(GD25Q32_SPI);
}

/**
 * @brief Init GD32F4xx hardware
 */
void GD25Q32_hard_init()
{

    rcu_periph_clock_enable(GD25Q32_SPI_PORT_RCU);
    rcu_periph_clock_enable(RCU_GPIOA);

    rcu_periph_clock_enable(GD25Q32_SPI_RCU);

    // SPI CLK
    GD32_CONGIG_PIN_AS_AF(GD25Q32_SPI_CLK_PORT, GD25Q32_SPI_CLK_PIN_AF, GD25Q32_SPI_CLK_PIN);
    // SPI RX
    GD32_CONGIG_PIN_AS_AF(GD25Q32_SPI_MISO_PORT, GD25Q32_SPI_MISO_PIN_AF, GD25Q32_SPI_MISO_PIN);
    // SPI TX
    GD32_CONGIG_PIN_AS_AF(GD25Q32_SPI_MOSI_PORT, GD25Q32_SPI_MOSI_PIN_AF, GD25Q32_SPI_MOSI_PIN);
    // SPI NSS
    GD32_CONGIG_PIN_AS_OUT(GD25Q32_SPI_NSS_PORT, GD25Q32_SPI_NSS_PIN);

    // SPI config
    spi_i2s_deinit(GD25Q32_SPI);
    spi_struct_para_init(&GD25Q32_spi_struct);
    GD25Q32_spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    GD25Q32_spi_struct.device_mode = SPI_MASTER;
    GD25Q32_spi_struct.frame_size = SPI_FRAMESIZE_8BIT;
    GD25Q32_spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    GD25Q32_spi_struct.prescale = SPI_PSC_128  ;
    GD25Q32_spi_struct.nss = SPI_NSS_HARD;
    GD25Q32_spi_struct.endian = SPI_ENDIAN_MSB;
    

    spi_init(GD25Q32_SPI, &GD25Q32_spi_struct);
    spi_nss_output_enable(GD25Q32_SPI);
    spi_enable(GD25Q32_SPI);


    //nvic_irq_enable(GD25Q32_SPI_IRQn, 5, 1);

    dma_single_data_para_struct_init(&dma_init_struct);
    rcu_periph_clock_enable(GD25Q32_SPI_DMA_DCU);
    /* configure SPI transmit DMA */
    dma_deinit(GD25Q32_SPI_TX_DMA);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr = (uint32_t)GD25Q32_spi_send_array;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(GD25Q32_SPI);
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number = GD25Q32_BUF_SZ;
    dma_single_data_mode_init(GD25Q32_SPI_TX_DMA, &dma_init_struct);
    dma_channel_subperipheral_select(GD25Q32_SPI_TX_DMA, GD25Q32_SPI_DMA_SUBPERI);
    /* configure DMA mode */
    dma_circulation_disable(GD25Q32_SPI_TX_DMA);

    /* configure SPI receive DMA */
    dma_deinit(GD25Q32_SPI_RX_DMA);
    dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(GD25Q32_SPI);
    dma_init_struct.memory0_addr = (uint32_t)GD25Q32_spi_receive_array;
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(GD25Q32_SPI_RX_DMA, &dma_init_struct);
    dma_channel_subperipheral_select(GD25Q32_SPI_RX_DMA, GD25Q32_SPI_DMA_SUBPERI);
    /* configure DMA mode */
    dma_circulation_disable(GD25Q32_SPI_RX_DMA);

    GD25Q32_DMA_disable();
}

/**
 * Send data to chip
 * @param  {   uint8_t * src }  pointer to request array,
 * @param  {   size_t req_sz }  request size,
 * @param  {   size_t res_sz }  response size,
 * @retval { uint8_t *       }  pointer to response
 */
uint8_t * GD25Q32_sendData(uint8_t *src, size_t req_sz, size_t res_sz)
{

    if (GD25Q32_spi_send_array != src)
    {
        memcpy(GD25Q32_spi_send_array, src, req_sz);
    }
    GD25Q32_DMA_enable(req_sz + res_sz);
    return (GD25Q32_spi_receive_array + req_sz);
}
/**
 * @brief 
 * @param dst uint8_t * Pointer to destination buffer
 * @param offset size_t Data offset in SPI_RX Buffer. Usually it is Tx data size
 * @param size size_t Rx data size.
 */
void GD25Q32_GetDataFromRxBuff(uint8_t *dst, size_t offset, size_t size)
{
    uint8_t *src = GD25Q32_RX_BUF + offset;
    for (size_t i = 0; i < size; i++)
    {
        dst[i] = src[i];
    }
}


/**
 * @brief Read transaction end flag, if done reset NSS pin
 * @return bool Transaction end flag
 */
bool GD25Q32_await_transaction_end()
{

    bool ret_val = false;
    if (spi_i2s_flag_get(GD25Q32_SPI, I2S_FLAG_TRANS) == RESET)
    {

        gpio_bit_set(GD25Q32_SPI_NSS_PORT, GD25Q32_SPI_NSS_PIN);
        ret_val = true;
    }
    return ret_val;
}
