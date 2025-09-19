#include "stdint.h"
#include "stdbool.h"

#include "gd32f4xx_libopt.h"

#include "FreeRTOS.h"
#include "task.h"

#include "M204D08AA.h"


#define I2C I2C2


static spi_parameter_struct GD25Q32_spi_struct;
static bool PCF8574_writeByte(uint8_t data);
static void LCD_WriteByte(uint8_t data, uint8_t isData);
static void LCD_WriteByte_v2(uint8_t data, uint8_t isData);
static void LCD_Cmd(uint8_t cmd);
static void LCD_WriteChar(char ch);

void LCD_WriteNibble(uint8_t nibble, uint8_t isData);

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
      //  gpio_bit_reset(GPIOG, GPIO_PIN_14);
    }else{
      //  gpio_bit_set(GPIOG, GPIO_PIN_14);
    }
}

void M204D08AA_STB_WriteWord(uint16_t data){

//    PCF8574_writeByte( (data>>8  ) );
//    PCF8574_writeByte( (data*0xFF) );

  //  LCD_WriteChar('A');

}

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

void M204D08AA_SPI_Config(void)
{

    rcu_periph_clock_enable(RCU_I2C2);
    i2c_clock_config(I2C, 30000, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x1);
    /* enable I2C0 */
    i2c_enable(I2C);
    /* enable acknowledge */
    i2c_ack_config(I2C, I2C_ACK_ENABLE);


    
			vTaskDelay(50); // ждем пока стабилизируется питание

			LCD_WriteByte_v2(0x33, 0); // шлем в одном байте два 0011
			vTaskDelay(1);

			LCD_WriteByte_v2(0x32, 0); // шлем в одном байте  00110010
			vTaskDelay(1);

			LCD_WriteByte_v2(DATA_BUS_4BIT_PAGE0, 0); // включаем режим 4 бит
			vTaskDelay(1);
			LCD_WriteByte_v2(DISPLAY_OFF, 0); // выключаем дисплей
			vTaskDelay(1);
			LCD_WriteByte_v2(CLEAR_DISPLAY, 0); // очищаем дисплей
			vTaskDelay(2);
			LCD_WriteByte_v2(ENTRY_MODE_SET, 0); //ставим режим смещение курсора экран не смещается
			vTaskDelay(1);
			LCD_WriteByte_v2(DISPLAY_ON, 0);// включаем дисплей и убираем курсор
			vTaskDelay(1);

            uint8_t start_address = 0x0;
            LCD_WriteByte_v2(( start_address|= SET_DDRAM_ADDRESS), 0);
            vTaskDelay(5);
            LCD_WriteChar('A');

            LCD_WriteChar('B');


  
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


// PC9 (AF4) i2C_SDA, PA8 (AF4) - I2C_SCL
void M204D08AA_gpio_congig(){

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOE);


    GD32_CONGIG_PIN_AS_AF(GPIOC, GPIO_AF_4, GPIO_PIN_9);
    GD32_CONGIG_PIN_AS_AF(GPIOA, GPIO_AF_4, GPIO_PIN_8);

   // RW pin
   // GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_14);
   // gpio_bit_reset(RS_PIN);
    gpio_mode_set(GPIOG, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_14);    

    // EN pin
   //  GD32_CONGIG_PIN_AS_OUT(GPIOG, GPIO_PIN_11);
   //  gpio_bit_reset(EN_PIN);
    gpio_mode_set(GPIOG, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_11); 

    //BL pin
    GD32_CONGIG_PIN_AS_OUT(GPIOE, GPIO_PIN_1);
  //  gpio_bit_set(GPIOE, GPIO_PIN_1);




}


static bool PCF8574_writeByte(uint8_t data){

     while(i2c_flag_get(I2C, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C, 0x20<<1 , I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while(!i2c_flag_get(I2C, I2C_FLAG_TBE));

    i2c_data_transmit(I2C, data);
    while(!i2c_flag_get(I2C, I2C_FLAG_TBE));
   
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C);
    while(I2C_CTL0(I2C) & I2C_CTL0_STOP);

}

void LCD_Cmd(uint8_t cmd) {
    LCD_WriteByte(cmd, 0);
    vTaskDelay(2); // Command processing delay
}

/**
 * @brief Send a character to the LCD
 */
void LCD_WriteChar(char ch) {
    LCD_WriteByte(ch, 1);
    vTaskDelay(2); // Command processing delay

}


union I2C_DATA{
    uint8_t byte;
    struct{
        uint8_t RS:1;
        uint8_t RW:1;
        uint8_t EN:1;
        uint8_t BL:1;
        uint8_t dtr:4;
    };
} i2cData;

void LCD_WriteByte(uint8_t data, uint8_t isData) {
  
  uint8_t upper_nibble = data >> 4;
  uint8_t lower_nibble = data & 0x0F;
  LCD_WriteNibble(upper_nibble, isData);
  LCD_WriteNibble(lower_nibble, isData);
}

void LCD_WriteByte_v2(uint8_t data, uint8_t isData) {
  
  uint8_t upper_nibble = data >> 4;
  uint8_t lower_nibble = data & 0x0F;

    i2cData.byte = 0;
    PCF8574_writeByte(i2cData.byte);
    i2cData.RS = isData == 1;
    PCF8574_writeByte(i2cData.byte);
    i2cData.EN = 1;
    PCF8574_writeByte(i2cData.byte);
    i2cData.dtr = upper_nibble;
    PCF8574_writeByte(i2cData.byte);
    i2cData.EN = 0;
    PCF8574_writeByte(i2cData.byte);
    i2cData.dtr = 0;
    PCF8574_writeByte(i2cData.byte);
    i2cData.EN = 1;
    PCF8574_writeByte(i2cData.byte);
    i2cData.dtr = lower_nibble;
    PCF8574_writeByte(i2cData.byte);
    i2cData.EN = 0;
    PCF8574_writeByte(i2cData.byte);
    i2cData.byte = 0;
    PCF8574_writeByte(i2cData.byte);

}

void Ldelay(int a)
{
    int i = 0;
    int f = 0;
    while(f < a)
    {
        while(i<60)
            {i++;}
        f++;
    }
}

void LCD_WriteNibble(uint8_t nibble, uint8_t isData) {
  
    i2cData.byte = 0;// Reset union
    PCF8574_writeByte(i2cData.byte);
    i2cData.RS = isData == 1;
    i2cData.EN = 1;
    PCF8574_writeByte(i2cData.byte);
    vTaskDelay(1);
    i2cData.dtr = nibble;
    PCF8574_writeByte(i2cData.byte);
    vTaskDelay(1);
    i2cData.EN = 0;
    PCF8574_writeByte(i2cData.byte);
}