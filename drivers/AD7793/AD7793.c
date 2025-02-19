#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"
#include "AD7793.h"

static spi_parameter_struct AD7793_spi_struct;

static void SPI_rw(uint8_t * tx_data, uint8_t * rx_data, size_t sz);

/**
 * @brief MCU pin hard configuration
 *
**/
void AD7793_HardCnfg(){
	rcu_periph_clock_enable(AD7793_SPI_RCU);
	rcu_periph_clock_enable(AD7793_SPI_PORT_RCU);
	rcu_periph_clock_enable(AD7793_NSS_PORT_RCU);

	// SPI CLK
  	AD7793_CONGIG_PIN_AS_AF(AD7793_SPI_CLK_PORT, AD7793_SPI_CLK_PIN_AF, AD7793_SPI_CLK_PIN);
  	// SPI RX
  	AD7793_CONGIG_PIN_AS_AF(AD7793_SPI_MISO_PORT, AD7793_SPI_MISO_PIN_AF, AD7793_SPI_MISO_PIN);
	// SPI TX
  	AD7793_CONGIG_PIN_AS_AF(AD7793_SPI_MOSI_PORT, AD7793_SPI_MOSI_PIN_AF, AD7793_SPI_MOSI_PIN);
  	// SPI NSS
  	AD7793_CONGIG_PIN_AS_OUT(AD7793_SPI_NSS_PORT, AD7793_SPI_NSS_PIN);

	// SPI config
  	spi_i2s_deinit(SPI0);
  	spi_struct_para_init(&AD7793_spi_struct);
  	AD7793_spi_struct.trans_mode 			= SPI_TRANSMODE_FULLDUPLEX;
  	AD7793_spi_struct.device_mode 			= SPI_MASTER;
  	AD7793_spi_struct.frame_size 			= SPI_FRAMESIZE_8BIT;
  	AD7793_spi_struct.clock_polarity_phase 	= SPI_CK_PL_HIGH_PH_2EDGE;
  	AD7793_spi_struct.prescale 				= SPI_PSC_32;
  	AD7793_spi_struct.nss 					= SPI_NSS_SOFT;
  	AD7793_spi_struct.endian 				= SPI_ENDIAN_MSB;

  	spi_init(SPI0, &AD7793_spi_struct);
  	spi_enable(SPI0);
	
}

/**
 * @brief Reads the value of the selected register
 *
 * @param regAddress - The address of the register to read.
 * @param size - The size of the register to read.
 *
 * @return data - The value of the selected register register.
**/
uint32_t AD7793_GetRegisterValue(uint8_t regAddress, uint8_t size)
{
	uint8_t rx_data[4] = { 0x00, 0x00, 0x00, 0x00};
	uint8_t tx_data[4] = { 0x00, 0x00, 0x00, 0x00};
	uint32_t ret_val = 0;
	tx_data[0] = AD7793_COMM_READ |  AD7793_COMM_ADDR(regAddress);
	SPI_rw(tx_data, rx_data, size+1);

	

	if (size == 1) 
	{
		ret_val = rx_data[1];
	}
	if (size == 2) 
	{
		ret_val = (rx_data[1] << 8) + rx_data[2];
	}
    
	return ret_val;
}

/**
 * @brief Writes the value to the register
 *
 * @param -  regAddress - The address of the register to write to.
 * @param -  regValue - The value to write to the register.
 * @param -  size - The size of the register to write.
 *
 * @return  None.
**/
void AD7793_SetRegisterValue(uint8_t regAddress, uint32_t regValue, uint8_t size)
{
	uint8_t rx_data[4] = { 0x00, 0x00, 0x00, 0x00 };
	uint8_t tx_data[4] = { 0x00, 0x00, 0x00, 0x00 };

	tx_data[0] = AD7793_COMM_WRITE | AD7793_COMM_ADDR(regAddress);

	if (size == 1) 
	{
		tx_data[1] = regValue & 255;
	}
	if (size == 2) 
	{
		tx_data[1] = (regValue >> 8) & 255;
		tx_data[2] = regValue & 255;
	}

	SPI_rw(tx_data, rx_data, ++size);
}

static void SPI_rw(uint8_t * tx_data, uint8_t * rx_data, size_t sz){

	gpio_bit_reset(AD7793_SPI_NSS_PORT, AD7793_SPI_NSS_PIN);

	for (size_t i = 0; i < sz; i++)
	{
		spi_i2s_data_transmit(AD7793_SPI, tx_data[i]);
		while(spi_i2s_flag_get(AD7793_SPI, I2S_FLAG_TRANS) == 1);
		rx_data[i] = spi_i2s_data_receive(SPI0);
	}
	
	gpio_bit_set(AD7793_SPI_NSS_PORT, AD7793_SPI_NSS_PIN);
}

/**
 * Set the functioning mode
 * 
 * AD7793_MODE_CONT		     Continuous Conversion Mode 
 * 
 * AD7793_MODE_SINGLE		 Single Conversion Mode
 * 
 * AD7793_MODE_IDLE		     Idle Mode
 * 
 * AD7793_MODE_PWRDN		 Power-Down Mode 
 * 
 * AD7793_MODE_CAL_INT_ZERO  Internal Zero-Scale Calibration 
 * 
 * AD7793_MODE_CAL_INT_FULL  Internal Full-Scale Calibration 
 * 
 * AD7793_MODE_CAL_SYS_ZERO  System Zero-Scale Calibration 
 * 
 * AD7793_MODE_CAL_SYS_FULL  System Full-Scale Calibration 
 * 
 * @param mode
 */
void AD7793_SetMode(uint32_t mode) {
	uint32_t command;
	
	command = AD7793_GetRegisterValue(AD7793_REG_MODE, AD7793_REG_MODE_SZ);
	command &= ~AD7793_MODE_SEL(0xFF);
	command |= AD7793_MODE_SEL((uint32_t ) mode);
	AD7793_SetRegisterValue( AD7793_REG_MODE, command, AD7793_REG_MODE_SZ);
	
}

/**
 * Set the differential channel
 * 
 * AD7793_CH_AIN1P_AIN1M	 Select channel AIN1(+) - AIN1(-) 
 * 
 * AD7793_CH_AIN2P_AIN2M	 Select channel AIN2(+) - AIN2(-) 
 * 
 * AD7793_CH_AIN3P_AIN3M	 Select channel AIN3(+) - AIN3(-) 
 * 
 * AD7793_CH_AIN1M_AIN1M	 Select channel AIN1(-) - AIN1(-) 
 * 
 * AD7793_CH_TEMP			 Temp Sensor, gain 1, Internal reference
 * 
 * AD7793_CH_AVDD_MONITOR	 AVDD voltage Monitor
 * 
 * @param channel
 */
void AD7793_SetChannel(uint32_t channel) 
{
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_CONF, AD7793_REG_CONF_SZ);
	command &= ~AD7793_CONF_CHAN(0xFF);
	command |= AD7793_CONF_CHAN((uint32_t ) channel);
	AD7793_SetRegisterValue(AD7793_REG_CONF, command, AD7793_REG_CONF_SZ);
	
}

volatile int32_t AD7793_STATE = 0;
/**
 * Get ready flag from state register
 * @return 1 - Ready, 0 - Not ready
 */
uint8_t AD7793_GetReadyReg() 
{
	uint8_t rdy = 0;
	AD7793_STATE = AD7793_GetRegisterValue(AD7793_REG_STAT, AD7793_REG_STAT_SZ);
	rdy = (( AD7793_STATE& 0x80)==0);
	return rdy;
}

/**
 * Get ready flag from MISO pin
 * @return 1 - Ready, 0 - Not ready
 */
uint8_t AD7793_GetReadyPin() 
{
	uint8_t rdy = 0;
	rdy = (gpio_input_bit_get(AD7793_SPI_MISO_PORT, AD7793_SPI_MISO_PIN) == 0);
	return rdy;
}

/*
 * Chip reset
 * @return
 * 
 */
uint8_t AD7793_Reset() 
{
	gpio_bit_reset(AD7793_SPI_NSS_PORT, AD7793_SPI_NSS_PIN);
	for (size_t i = 0; i < 4; i++)
	{
		spi_i2s_data_transmit(AD7793_SPI, 0xFF);
		while(spi_i2s_flag_get(AD7793_SPI, I2S_FLAG_TRANS) == 1);
	}
	gpio_bit_set(AD7793_SPI_NSS_PORT, AD7793_SPI_NSS_PIN);
}

/**
 * Set direction of current sources select bits
 * 
 * AD7793_DIR_IEXC1_IOUT1_IEXC2_IOUT2	- IEXC1 connect to IOUT1, IEXC2 connect to IOUT2 
 * 
 * AD7793_DIR_IEXC1_IOUT2_IEXC2_IOUT1	- IEXC1 connect to IOUT2, IEXC2 connect to IOUT1 
 * 
 * AD7793_DIR_IEXC1_IEXC2_IOUT1		- Both current sources IEXC1,2 connect to IOUT1  
 * 
 * AD7793_DIR_IEXC1_IEXC2_IOUT2		- Both current sources IEXC1,2 connect to IOUT2 
 * 
 * @param dir
 */
void AD7793_SetCurrentSourcesDirection(uint32_t dir) 
{
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_IO, AD7793_REG_IO_SZ);
	command &= ~AD7793_IEXCDIR(0x0F);
	command |= AD7793_IEXCDIR((uint32_t ) dir);
	//command = command&0x0F;
	AD7793_SetRegisterValue(AD7793_REG_IO, command, AD7793_REG_IO_SZ);
	
}

/**
 * Set value  of current sources select bits
 * 
 * AD7793_EN_IXCEN_DISABLE	- Disable excitation current
 * 
 * AD7793_EN_IXCEN_10uA		- Excitation Current 10uA 
 * 
 * AD7793_EN_IXCEN_210uA	- Excitation Current 210uA 
 * 
 * AD7793_EN_IXCEN_1mA		- Excitation Current 1mA 
 * 
 */
void AD7793_SetCurrentSourcesValue(uint32_t val) 
{
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_IO, AD7793_REG_IO_SZ);
	command &= ~AD7793_IEXCEN(0x0F);
	command |= AD7793_IEXCEN((uint32_t ) val);
	//command = command&0x0F;
	AD7793_SetRegisterValue(AD7793_REG_IO, command, AD7793_REG_IO_SZ);
	
}

/**
 * @brief Set Unipolar/Bipolar Bit. 
 * 
 *  
 *  AD7793_UNIPOLAR  
 *  
 *  AD7793_BIPOLAR 
 */
void AD7793_SetPolarity(uint32_t state){
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_CONF, AD7793_REG_CONF_SZ);
	if(state == true){
		command |=  (AD7793_CONF_UNIPOLAR);
	}else{
		command &= ~(AD7793_CONF_UNIPOLAR);
	}
	AD7793_SetRegisterValue(AD7793_REG_CONF, command, AD7793_REG_CONF_SZ);
	
}


/**
 * @brief Set input buffer amplifier state 
 * 
 *  
 *  1- enable  
 *  
 *  0- disable 
 */
void AD7793_SetBuf(uint32_t state){
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_CONF, AD7793_REG_CONF_SZ);
	if(state == true){
		command |=  (AD7793_CONF_BUF);
	}else{
		command &= ~(AD7793_CONF_BUF);
	}
	AD7793_SetRegisterValue(AD7793_REG_CONF, command, AD7793_REG_CONF_SZ);
	
}

/**
 * @brief Set gain 
 * 
* AD7793_GAIN_1

* AD7793_GAIN_2 

* AD7793_GAIN_4  

* AD7793_GAIN_8  
    
* AD7793_GAIN_16 

* AD7793_GAIN_32    

* AD7793_GAIN_64   

* AD7793_GAIN_128   

*/
void AD7793_SetGain(uint32_t gain){
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_CONF, AD7793_REG_CONF_SZ);
	command &= ~AD7793_CONF_GAIN(0xFF);
	command |= AD7793_CONF_GAIN((uint32_t ) gain);
	AD7793_SetRegisterValue(AD7793_REG_CONF, command, AD7793_REG_CONF_SZ);
	
}


/**
 * @brief Set gain 
 * 
 * AD7793_RATE_NIL    
 * 	
 * AD7793_RATE_470    	
 * 	
 * AD7793_RATE_242    	
 * 
 * AD7793_RATE_123    	
 * 	
 * AD7793_RATE_62    
 * 		
 * AD7793_RATE_50  		
 * 
 * AD7793_RATE_39		
 * 
 * AD7793_RATE_33_2		
 * 
 * AD7793_RATE_19_6		
 * 
 * AD7793_RATE_16_7_50		
 * 
 * AD7793_RATE_16_7_50_60  
 * 
 * AD7793_RATE_12_5  		
 * 
 * AD7793_RATE_10    	
 * 
 * AD7793_RATE_8_33  	
 * 	
 * AD7793_RATE_6_25  	
 * 	
 * AD7793_RATE_4_17  		
 */
void AD7793_SetRate(uint32_t rate) {
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_MODE, AD7793_REG_MODE_SZ);
	command &= ~AD7793_MODE_RATE(0xFF);
	command |= AD7793_MODE_RATE((uint32_t ) rate);
	AD7793_SetRegisterValue(AD7793_REG_MODE, command, AD7793_REG_MODE_SZ);
	
}


/**
 * @brief SetClock
 * 
 * AD7793_CLK_INT	
 * 	
 * AD7793_CLK_INT_CO	
 * 
 * AD7793_CLK_EXT		
 * 
 * AD7793_CLK_EXT_DIV2	
 */
 
void AD7793_SetClock(uint32_t rate) {
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_MODE, AD7793_REG_MODE_SZ);
	command &= ~AD7793_MODE_CLKSRC(0xFF);
	command |= AD7793_MODE_CLKSRC((uint32_t ) rate);
	AD7793_SetRegisterValue(AD7793_REG_MODE, command, AD7793_REG_MODE_SZ);
	
}

/**
 * Set refrence Bit. 
 *  
 * AD7793_REFSEL_INT  
 * 
 * AD7793_REFSEL_EXT  
 * 
 */
void AD7793_SetReference(uint32_t state){
	
	uint32_t command;
	command = AD7793_GetRegisterValue(AD7793_REG_CONF, AD7793_REG_CONF_SZ);
	command &= ~AD7793_CONF_REFSEL(0xFF);
	command |= AD7793_CONF_REFSEL((uint32_t ) state);
	AD7793_SetRegisterValue(AD7793_REG_CONF, command, AD7793_REG_CONF_SZ);
	
}


