#include "stdint.h"
#include "stdbool.h"

#include "gd32f4xx_libopt.h"

#include "FreeRTOS.h"
#include "task.h"

#include "M204D08AA.h"


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


static spi_parameter_struct GD25Q32_spi_struct;
static bool PCF8574_writeByte(uint8_t data);
static void LCD_WriteByte(uint8_t data, uint8_t isData);
static void LCD_WriteByte_v2(uint8_t data, uint8_t isData);
static void LCD_Cmd(uint8_t cmd);
static void LCD_WriteChar(char ch);

static void LCD_SetBright(uint8_t brightness) {

    TIMER_CH1CV(TIMER0) = TIMER_CAR(TIMER0) * (100 - brightness) / 100;
}

void LCD_WriteByte_8bit(uint8_t data, uint8_t isData);

void LCD_WriteNibble(uint8_t nibble, uint8_t isData);

void M204D08AA_SPI_Config();
void M204D08AA_gpio_congig();

static void timer_additional_config(void);

extern const char Decode2Rus2[];

void M204D08AA_HardInit()
{
    M204D08AA_gpio_congig();
    vTaskDelay(300);
    M204D08AA_SPI_Config();
    timer_additional_config();


    // // russian alphabet test
    // for (size_t i = 0; i < 31; i++)
    // {
    //     LCD_WriteChar(Decode2Rus2[i]);
    // }

    // bBrightness test

    for (int i = 0; i < 100; i++) {
        LCD_SetBright(100 - i);
        vTaskDelay(3);
    }

    for (int i = 0; i < 100; i++) {
        LCD_SetBright(i);
        vTaskDelay(3);
    }

    LCD_SetBright(25);
}

void M204D08AA_STB_SetState(bool state) {
}

volatile uint32_t xxcarr = 9000;
void M204D08AA_STB_WriteWord(uint16_t data) {

    uint8_t cmd = data >> 8;
    uint8_t dtr = data & 0xFF;

    if (cmd == 250) {
        if (dtr == 0) dtr = ' ';
        LCD_WriteChar(dtr);
    }

    uint8_t start_address = 0x0;
    if (cmd == 248) {

        switch (dtr)
        {
        case 1: // clear    
            LCD_WriteByte(CLEAR_DISPLAY, 0); // очищаем дисплей
            vTaskDelay(2);
            break;

        case 128:
            start_address = 0x0;
            break;
        case 192:
            start_address = 0x40;
            break;
        case 148:
            start_address = 0x14;
            break;
        case 212:
            start_address = 0x54;
            break;

        case 56:
            LCD_SetBright(25);
            break;
        case 57:
            LCD_SetBright(50);
            break;
        case 58:
            LCD_SetBright(75);
            break;
        case 59:
            LCD_SetBright(100);
            break;

        default:
            break;
        }
        LCD_WriteByte((start_address |= SET_DDRAM_ADDRESS), 0);
    }

}



void M204D08AA_SPI_Config(void)
{

    rcu_periph_clock_enable(RCU_I2C2);
    i2c_clock_config(I2C, 300000, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x1);
    /* enable I2C0 */
    i2c_enable(I2C);
    /* enable acknowledge */
    i2c_ack_config(I2C, I2C_ACK_ENABLE);


    vTaskDelay(150);
    LCD_WriteByte(0x33, 0);
    vTaskDelay(1);
    LCD_WriteByte(0x32, 0);
    vTaskDelay(1);

    LCD_WriteByte(DATA_BUS_8BIT_PAGE0, 0);
    vTaskDelay(1);
    LCD_WriteByte(DISPLAY_OFF, 0);
    vTaskDelay(1);
    LCD_WriteByte(CLEAR_DISPLAY, 0);
    vTaskDelay(10);
    LCD_WriteByte(ENTRY_MODE_SET, 0);
    vTaskDelay(1);
    LCD_WriteByte(DISPLAY_ON, 0);
    vTaskDelay(2);
    LCD_WriteByte(CLEAR_DISPLAY, 0);
    vTaskDelay(2);

    uint8_t start_address = 0x0;
    LCD_WriteByte((start_address |= SET_DDRAM_ADDRESS), 0);
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
void M204D08AA_gpio_congig() {

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


static bool PCF8574_writeByte(uint8_t data) {

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
static timer_break_parameter_struct timer_breakpara;

static void timer_additional_config(void)
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

void LCD_Cmd(uint8_t cmd) {
    LCD_WriteByte(cmd, 0);
}

/**
 * @brief Send a character to the LCD
 */
void LCD_WriteChar(char ch) {
    LCD_WriteByte(ch, 1);
}


void LCD_WriteByte(uint8_t data, uint8_t isData) {
    LCD_WriteByte_8bit(data, isData);
}

void LCD_WriteByte_8bit(uint8_t data, uint8_t isData) {

    I2C_8BIT_RSPIN(isData);
    I2C_8BIT_ENPIN(1);
    PCF8574_writeByte(data);
    I2C_8BIT_ENPIN(0);
}