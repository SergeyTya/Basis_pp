#include "stdint.h"
#include "stdbool.h"
#include "M204D08AA.h"
#include "stm32f4xx.h"


#define M204D08AA_SPIx                           SPI1
#define M204D08AA_SPIx_CLK                       RCC_APB2Periph_SPI1
#define M204D08AA_SPIx_CLK_INIT                  RCC_APB2PeriphClockCmd
#define M204D08AA_SPIx_IRQn                      SPI1_IRQn
#define M204D08AA_SPIx_IRQHANDLER                SPI1_IRQHandler

#define M204D08AA_SPIx_SCK_PIN                   GPIO_Pin_3
#define M204D08AA_SPIx_SCK_GPIO_PORT             GPIOB
#define M204D08AA_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define M204D08AA_SPIx_SCK_SOURCE                GPIO_PinSource3
#define M204D08AA_SPIx_SCK_AF                    GPIO_AF_SPI1

#define M204D08AA_SPIx_MISO_PIN                  GPIO_Pin_4
#define M204D08AA_SPIx_MISO_GPIO_PORT            GPIOB
#define M204D08AA_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define M204D08AA_SPIx_MISO_SOURCE               GPIO_PinSource4
#define M204D08AA_SPIx_MISO_AF                   GPIO_AF_SPI1

#define M204D08AA_SPIx_MOSI_PIN                  GPIO_Pin_5
#define M204D08AA_SPIx_MOSI_GPIO_PORT            GPIOB
#define M204D08AA_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define M204D08AA_SPIx_MOSI_SOURCE               GPIO_PinSource5
#define M204D08AA_SPIx_MOSI_AF                   GPIO_AF_SPI1

#define M204D08AA_SPIx_STB_PIN                  GPIO_Pin_15
#define M204D08AA_SPIx_STB_GPIO_PORT            GPIOA
#define M204D08AA_SPIx_STB_GPIO_CLK             RCC_AHB1Periph_GPIOA

SPI_InitTypeDef SPI_InitStructure;

static void SPI_Config(void);

void M204D08AA_HardInit()
{
    SPI_Config();
}

void M204D08AA_STB_SetState(bool state){
    if(state == true){
        GPIO_ResetBits(M204D08AA_SPIx_STB_GPIO_PORT, M204D08AA_SPIx_STB_PIN);
    }else{
        GPIO_SetBits(M204D08AA_SPIx_STB_GPIO_PORT, M204D08AA_SPIx_STB_PIN);
    }
}

void M204D08AA_STB_WriteWord(uint16_t data){
   SPI_SendData(M204D08AA_SPIx, data);
   while(SPI_I2S_GetFlagStatus(M204D08AA_SPIx, SPI_FLAG_BSY));
}

static void SPI_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Peripheral Clock Enable -------------------------------------------------*/
    /* Enable the SPI clock */
    M204D08AA_SPIx_CLK_INIT(M204D08AA_SPIx_CLK, ENABLE);

    /* Enable GPIO clocks */
    RCC_AHB1PeriphClockCmd(M204D08AA_SPIx_SCK_GPIO_CLK | M204D08AA_SPIx_MISO_GPIO_CLK | M204D08AA_SPIx_MOSI_GPIO_CLK, ENABLE);

    /* SPI GPIO Configuration --------------------------------------------------*/
    /* GPIO Deinitialisation */
   GPIO_DeInit(M204D08AA_SPIx_SCK_GPIO_PORT);
   GPIO_DeInit(M204D08AA_SPIx_MISO_GPIO_PORT);
   GPIO_DeInit(M204D08AA_SPIx_MOSI_GPIO_PORT);
   SPI_I2S_DeInit(M204D08AA_SPIx);

    /* Connect SPI pins to AF5 */
    GPIO_PinAFConfig(M204D08AA_SPIx_SCK_GPIO_PORT , M204D08AA_SPIx_SCK_SOURCE , M204D08AA_SPIx_SCK_AF);
    GPIO_PinAFConfig(M204D08AA_SPIx_MISO_GPIO_PORT, M204D08AA_SPIx_MISO_SOURCE, M204D08AA_SPIx_MISO_AF);
    GPIO_PinAFConfig(M204D08AA_SPIx_MOSI_GPIO_PORT, M204D08AA_SPIx_MOSI_SOURCE, M204D08AA_SPIx_MOSI_AF);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

    /* SPI SCK pin configuration */
    GPIO_InitStructure.GPIO_Pin = M204D08AA_SPIx_SCK_PIN;
    GPIO_Init(M204D08AA_SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

    /* SPI  MISO pin configuration */
    GPIO_InitStructure.GPIO_Pin = M204D08AA_SPIx_MISO_PIN;
    GPIO_Init(M204D08AA_SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

    /* SPI  MOSI pin configuration */
    GPIO_InitStructure.GPIO_Pin = M204D08AA_SPIx_MOSI_PIN;
    GPIO_Init(M204D08AA_SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

    RCC_AHB1PeriphClockCmd(M204D08AA_SPIx_STB_GPIO_CLK , ENABLE);
    /* configration the LED pin */
    GPIO_InitStructure.GPIO_Pin = M204D08AA_SPIx_STB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(M204D08AA_SPIx_STB_GPIO_PORT, &GPIO_InitStructure);

    /* SPI configuration -------------------------------------------------------*/

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(M204D08AA_SPIx, &SPI_InitStructure);

    SPI_Cmd(M204D08AA_SPIx, ENABLE);
}