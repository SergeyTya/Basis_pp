#include "stdint.h"
#include "stdbool.h"
#include "gd32f4xx_libopt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "IDisplay_WH2004A.h"


#define I2C I2C2

// display commands

#define CLEAR_DISPLAY 0x1
#define RETURN_HOME 0x2
#define ENTRY_MODE_SET 0x6 // mode cursor shift rihgt, display non shift
#define DISPLAY_ON 0xC // non cursor
#define DISPLAY_OFF 0x8
#define CURSOR_SHIFT_LEFT 0x10
#define CURSOR_SHIFT_RIGHT 0x14
#define DISPLAY_SHIFT_LEFT 0x18
#define DISPLAY_SHIFT_RIGHT 0x1C
#define DATA_BUS_4BIT_PAGE0 0x28
#define DATA_BUS_4BIT_PAGE1 0x2A
#define DATA_BUS_8BIT_PAGE0 0x38
#define SET_CGRAM_ADDRESS 0x40 // usage address |= SET_CGRAM_ADDRESS
#define SET_DDRAM_ADDRESS 0x80

const char WH2004A_Decode2Rus[] = {
    'A',(char)0xA0, 'B',(char)0xA1,(char)0xE0,
    'E', (char)0xA3,(char)0xA4,
    (char)0xA5, (char)0xA6,'K',(char)0xA7,'M','H','O',
    (char)0xA8,'P','C','T', (char)0xA9, (char)0xAA,'X',
    (char)0xE1, (char)0xAB,(char)0xAC,(char)0xE2,
    (char)0xAD,(char)0xAE,'b',(char)0xAF,(char)0xB0,(char)0xB1
};

static void PCF8574_writeByte(uint8_t data);
static void WH2004A_WriteByte(uint8_t data, uint8_t isData);
static void WH2004A_WriteChar(char ch);
void WH2004A_SPI_Config();
void WH2004A_gpio_congig();

static void WH2004A_timer_additional_config(void);


void WH2004A_DisplayInit()
{
    WH2004A_gpio_congig();
    WH2004A_SPI_Config();
    WH2004A_timer_additional_config();

    vTaskDelay(150);
    WH2004A_WriteByte(0x33, 0);
    vTaskDelay(1);
    WH2004A_WriteByte(0x32, 0);
    vTaskDelay(1);

    WH2004A_WriteByte(DATA_BUS_8BIT_PAGE0, 0);
    vTaskDelay(1);
    WH2004A_WriteByte(DISPLAY_OFF, 0);
    vTaskDelay(1);
    WH2004A_WriteByte(CLEAR_DISPLAY, 0);
    vTaskDelay(10);
    WH2004A_WriteByte(ENTRY_MODE_SET, 0);
    vTaskDelay(1);
    WH2004A_WriteByte(DISPLAY_ON, 0);
    vTaskDelay(2);
    WH2004A_WriteByte(CLEAR_DISPLAY, 0);
    vTaskDelay(2);

    uint8_t start_address = 0x0;
    WH2004A_WriteByte((start_address |= SET_DDRAM_ADDRESS), 0);

    // // russian alphabet test
    // for (size_t i = 0; i < 31; i++)
    // {
    //     WH2004A_WriteChar(Decode2Rus2[i]);
    // }

    // bBrightness test

    for (int i = 0; i < 100; i++) {
        WH2004A_DisplaySetBrightnessLevel(100 - i);
        vTaskDelay(3);
    }

    for (int i = 0; i < 100; i++) {
        WH2004A_DisplaySetBrightnessLevel(i);
        vTaskDelay(3);
    }

    WH2004A_DisplaySetBrightnessLevel(25);
}

void WH2004A_DisplayUpdateFromBuffer(char buff[80]) {

    WH2004A_WriteByte(CLEAR_DISPLAY, 0);
    vTaskDelay(2);
    int start_address = 0x0;
    WH2004A_WriteByte((start_address |= SET_DDRAM_ADDRESS), 0);

    for (size_t i = 0; i < 80; i++)
    {
        char c = buff[i];
        if (c >= -64 && c < -32) {
            WH2004A_WriteChar(WH2004A_Decode2Rus[64 + c]);
        }
        else {
            if (c == 0) { c = ' '; }
            WH2004A_WriteChar(c);
        }
    }
}

void WH2004A_DisplaySetBrightnessLevel(int lvl) {

    TIMER_CH1CV(TIMER0) = TIMER_CAR(TIMER0) * (100 - lvl) / 100;
}

void WH2004A_SPI_Config(void)
{

    rcu_periph_clock_enable(RCU_I2C2);
    i2c_clock_config(I2C, 300000, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x1);
    /* enable I2C0 */
    i2c_enable(I2C);
    /* enable acknowledge */
    i2c_ack_config(I2C, I2C_ACK_ENABLE);
}

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

#define RS_PIN  GPIOG,GPIO_PIN_14
#define EN_PIN  GPIOG,GPIO_PIN_11
#define I2C_8BIT_ENPIN(state) { if(state){gpio_bit_set(EN_PIN);}else{gpio_bit_reset(EN_PIN);} }
#define I2C_8BIT_RSPIN(state) { if(state){gpio_bit_set(RS_PIN);}else{gpio_bit_reset(RS_PIN);} }
void WH2004A_gpio_congig() {

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOG);


    // PC9 (AF4) i2C_SDA, PA8 (AF4) - I2C_SCL
    GD32_CONGIG_PIN_AS_AF(GPIOC, GPIO_AF_4, GPIO_PIN_9);
    GD32_CONGIG_PIN_AS_AF(GPIOA, GPIO_AF_4, GPIO_PIN_8);

    // RW pin
    GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_14);
    // EN pin
    GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_11);
    //BL pin
    rcu_periph_clock_enable(RCU_GPIOE);
    GD32_CONGIG_PIN_AS_OUT(GPIOE, GPIO_PIN_1);
    gpio_bit_set(GPIOE, GPIO_PIN_1);
}


static void PCF8574_writeByte(uint8_t data) {

    while (i2c_flag_get(I2C, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(I2C, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C, 0x20 << 1, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while (!i2c_flag_get(I2C, I2C_FLAG_TBE));

    i2c_data_transmit(I2C, data);
    while (!i2c_flag_get(I2C, I2C_FLAG_TBE));

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C);
    while (I2C_CTL0(I2C) & I2C_CTL0_STOP);

}

static timer_parameter_struct timer_initpara;
static timer_oc_parameter_struct timer_ocintpara;

static void WH2004A_timer_additional_config(void)
{

    rcu_periph_clock_enable(RCU_GPIOE);
    GD32_CONGIG_PIN_AS_AF(GPIOE, GPIO_AF_1, GPIO_PIN_1);

    int timer_max = SystemCoreClock / (2 * 10000);
    rcu_periph_clock_enable(RCU_TIMER0);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    timer_deinit(TIMER0);
    timer_initpara.prescaler = 100;
    timer_initpara.alignedmode = TIMER_COUNTER_DOWN;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = timer_max;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;


    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0); // PWM MODE
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);


    timer_enable(TIMER0);
    timer_primary_output_config(TIMER0, ENABLE);
    timer_auto_reload_shadow_enable(TIMER0);

}

void WH2004A_Cmd(uint8_t cmd) {
    WH2004A_WriteByte(cmd, 0);
}

/**
 * @brief Send a character to the WH2004A
 */
void WH2004A_WriteChar(char ch) {
    WH2004A_WriteByte(ch, 1);
}

void WH2004A_WriteByte(uint8_t data, uint8_t isData) {
    I2C_8BIT_RSPIN(isData);
    I2C_8BIT_ENPIN(1);
    PCF8574_writeByte(data);
    I2C_8BIT_ENPIN(0);
}
