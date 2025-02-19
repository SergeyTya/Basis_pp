#ifndef AD779x_H_
#define AD779x_H_

#include "stdint.h"
#include "stdlib.h"
#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"

/*GD32F407 hardware */
#define AD7793_SPI                SPI0
#define AD7793_SPI_RCU            RCU_SPI0
#define AD7793_SPI_PORT_RCU       (RCU_GPIOA) 
#define AD7793_SPI_CLK_PORT       GPIOA
#define AD7793_SPI_CLK_PIN        GPIO_PIN_5
#define AD7793_SPI_CLK_PIN_AF     GPIO_AF_5
#define AD7793_SPI_MOSI_PORT      GPIOA
#define AD7793_SPI_MOSI_PIN       GPIO_PIN_7
#define AD7793_SPI_MOSI_PIN_AF    GPIO_AF_5
#define AD7793_SPI_MISO_PORT      GPIOA
#define AD7793_SPI_MISO_PIN       GPIO_PIN_6
#define AD7793_SPI_MISO_PIN_AF    GPIO_AF_5
#define AD7793_SPI_NSS_PORT       GPIOE
#define AD7793_NSS_PORT_RCU       (RCU_GPIOE)  
#define AD7793_SPI_NSS_PIN        GPIO_PIN_14

#define AD7793_CONGIG_PIN_AS_OUT(PORT, PIN)                                  \
  {                                                                       \
    gpio_mode_set(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN);           \
    gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN); \
  }

#define AD7793_CONGIG_PIN_AS_IN(PORT, PIN)                        \
  {                                                            \
    gpio_mode_set(PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PIN); \
  }

#define AD7793_CONGIG_PIN_AS_AF(PORT, AF, PIN)                                   \
  {                                                                           \
    gpio_mode_set((PORT), GPIO_MODE_AF, GPIO_PUPD_NONE, (PIN));               \
    gpio_output_options_set((PORT), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, (PIN)); \
    gpio_af_set((PORT), AF, (PIN));                                           \
  }

/*AD7793 Registers*/
#define AD7793_REG_COMM		  0 /* Communications Register(WO, 8-bit) */
#define AD7793_REG_STAT	    0 /* Status Register	    (RO, 8-bit) */
#define AD7793_REG_MODE	    1 /* Mode Register	     	(RW, 16-bit */
#define AD7793_REG_CONF	    2 /* Configuration Register (RW, 16-bit)*/
#define AD7793_REG_DATA	    3 /* Data Register	     	(RO, 16-/24-bit) */
#define AD7793_REG_ID	      4 /* ID Register	     	(RO, 8-bit) */
#define AD7793_REG_IO	      5 /* IO Register	     	(RO, 8-bit) */
#define AD7793_REG_OFFSET   6 /* Offset Register	    (RW, 24-bit */
#define AD7793_REG_FULLSCAL	7 /* Full-Scale Register	(RW, 24-bit */

/*AD7793 Register size*/
#define AD7793_REG_COMM_SZ		  1 /* Communications Register(WO, 8-bit) */
#define AD7793_REG_STAT_SZ	    1 /* Status Register	    (RO, 8-bit) */
#define AD7793_REG_MODE_SZ	    2 /* Mode Register	     	(RW, 16-bit */
#define AD7793_REG_CONF_SZ	    2 /* Configuration Register (RW, 16-bit)*/

#ifdef AD7793_CHIP
#define AD7793_REG_DATA_SZ	    3 /* Data Register	     	(RO, 24-bit for AD7792) */
#else
#define AD7793_REG_DATA_SZ	    2 /* Data Register	     	(RO, 16-bit for AD7792) */  
#endif

#define AD7793_REG_ID_SZ	      1 /* ID Register	     	(RO, 8-bit) */
#define AD7793_REG_IO_SZ	      1 /* IO Register	     	(RO, 8-bit) */
#define AD7793_REG_OFFSET_SZ    2 /* Offset Register	    (RW, 24-bit */
#define AD7793_REG_FULLSCAL_SZ	2 /* Full-Scale Register	(RW, 24-bit */

/* Communications Register Bit Designations (AD7793_REG_COMM) */
#define AD7793_COMM_WEN		  (1 << 7) 			/* Write Enable */
#define AD7793_COMM_WRITE	  (0 << 6) 			/* Write Operation */
#define AD7793_COMM_READ    (1 << 6) 			/* Read Operation */
#define AD7793_COMM_ADDR(x)	(((x) & 0x7) << 3)	/* Register Address */
#define AD7793_COMM_CREAD	  (1 << 2) 			/* Continuous Read of Data Register */

/* Status Register Bit Designation masks(AD7793_REG_STAT) */
#define AD7793_STAT_RDY		(1 << 7) /* Ready */
#define AD7793_STAT_ERR		(1 << 6) /* Error (Overrange, Underrange) */
#define AD7793_STAT_CH3		(1 << 2) /* Channel 3 */
#define AD7793_STAT_CH2		(1 << 1) /* Channel 2 */
#define AD7793_STAT_CH1		(1 << 0) /* Channel 1 */

/* Mode Register Bit Designations (AD7793_REG_MODE) */
#define AD7793_MODE_SEL(x)		(((x) & 0x7) << 13)	/* Operation Mode Select */
#define AD7793_MODE_CLKSRC(x)	(((x) & 0x3) << 6) 	/* ADC Clock Source Select */
#define AD7793_MODE_RATE(x)		((x) & 0xF) 		/* Filter Update Rate Select */

/* AD7793_MODE_SEL(x) options */
#define AD7793_MODE_CONT		          0 /* Continuous Conversion Mode */
#define AD7793_MODE_SINGLE		        1 /* Single Conversion Mode */
#define AD7793_MODE_IDLE		          2 /* Idle Mode */
#define AD7793_MODE_PWRDN		          3 /* Power-Down Mode */
#define AD7793_MODE_CAL_INT_ZERO      4 /* Internal Zero-Scale Calibration */
#define AD7793_MODE_CAL_INT_FULL      5 /* Internal Full-Scale Calibration */
#define AD7793_MODE_CAL_SYS_ZERO      6 /* System Zero-Scale Calibration */
#define AD7793_MODE_CAL_SYS_FULL      7 /* System Full-Scale Calibration */

/* AD7793_MODE_CLKSRC(x) options */
#define AD7793_CLK_INT		            0 /* Internal 64 kHz Clk not available at the CLK pin */
#define AD7793_CLK_INT_CO	            1 /* Internal 64 kHz Clk available at the CLK pin */
#define AD7793_CLK_EXT		            2 /* External 64 kHz Clock */
#define AD7793_CLK_EXT_DIV2	          3 /* External Clock divided by 2 */

/* AD7793_MODE_RATE(x) options */
#define AD7793_RATE_NIL    		         0
#define AD7793_RATE_470    		         1
#define AD7793_RATE_242    		         2
#define AD7793_RATE_123    		         3
#define AD7793_RATE_62    		         4
#define AD7793_RATE_50  		           5
#define AD7793_RATE_39			           6
#define AD7793_RATE_33_2		           7
#define AD7793_RATE_19_6		           8 	/* 60 Hz only  */
#define AD7793_RATE_16_7_50		         9 	/* 50 Hz only  */
#define AD7793_RATE_16_7_50_60        10	/* 50 and 60 Hz */
#define AD7793_RATE_12_5  		        11 	/* 50 and 60 Hz */
#define AD7793_RATE_10    	          12 	/* 50 and 60 Hz */
#define AD7793_RATE_8_33  		        13 	/* 50 and 60 Hz */
#define AD7793_RATE_6_25  		        14 	/* 50 and 60 Hz */
#define AD7793_RATE_4_17  		        15 	/* 50 and 60 Hz */

/* Configuration Register Bit Designations (AD7793_REG_CONF) */
#define AD7793_CONF_VBIAS(x)  (((x) & 0x3) << 14) 	/* Bias Voltage Generator Enable */
#define AD7793_CONF_BO_EN	    (1 << 13) 			    /* Burnout Current Enable */
#define AD7793_CONF_UNIPOLAR  (1 << 12) 			    /* Unipolar/Bipolar Enable */
#define AD7793_CONF_BOOST	    (1 << 11) 			    /* Boost Enable */
#define AD7793_CONF_GAIN(x)	  (((x) & 0x7) << 8) 	/* Gain Select */
#define AD7793_CONF_REFSEL(x) (((x) & 0x1) << 7) 	/* INT/EXT Reference Select */
#define AD7793_CONF_BUF		    (1 << 4) 				    /* Buffered Mode Enable */
#define AD7793_CONF_CHAN(x)	  ((x) & 0x7) 			  /* Channel select */

/* AD7793_CONF_VBIAS(x) options */
#define AD7793_VBIAS_GEN_DISABL    	0	/* Bias voltage generator disabled */
#define AD7793_VBIAS_AIN1_NEG    	  1 	/* Bias voltage generator to AIN1(-) */
#define AD7793_VBIAS_AIN2_NEG      	2 	/* Bias voltage generator to AIN2(-) */

/* AD7793_CONF_GAIN(x) options  Gain and ADC input range (2.5V reference)*/
#define AD7793_GAIN_1       0	/* 1X (In-amp nor used)	2.5 */
#define AD7793_GAIN_2       1	/* 2X (In-amp nor used)	1.25 */
#define AD7793_GAIN_4       2	/* 4X 					625mV */
#define AD7793_GAIN_8       3	/* 8X 					312.5 mV*/
#define AD7793_GAIN_16      4	/* 16X					156.2 mV*/
#define AD7793_GAIN_32      5	/* 32X 					78.125 mV */
#define AD7793_GAIN_64      6	/* 64X					39.06 mV  */
#define AD7793_GAIN_128     7	/* 128X					19.53 mV */

/* AD7793 Polarity options */
#define AD7793_BIPOLAR      0 /** BIPOLAR */
#define AD7793_UNIPOLAR     1 /** UNIPOLAR */

/* AD7793_CONF_REFSEL(x) options */
#define AD7793_REFSEL_INT   1	/* Internal reference selected. */
#define AD7793_REFSEL_EXT   0	/* External reference applied between REFIN(+) and REFIN(–). */

/* AD7793_CONF_CHAN(x) options */
#define AD7793_CH_AIN1P_AIN1M	0 /* Select channel AIN1(+) - AIN1(-) */
#define AD7793_CH_AIN2P_AIN2M	1 /* Select channel AIN2(+) - AIN2(-) */
#define AD7793_CH_AIN3P_AIN3M	2 /* Select channel AIN3(+) - AIN3(-) */
#define AD7793_CH_AIN1M_AIN1M	3 /* Select channel AIN1(-) - AIN1(-) */
#define AD7793_CH_TEMP			6 /* Temp Sensor, gain 1, Internal reference */
#define AD7793_CH_AVDD_MONITOR	7 /* AVDD voltage Monitor, gain 1/6, Internal 1.17V reference */

/* ID Register Bit Designations (AD7793_REG_ID) */
#define AD7793_ID			0xB
#define AD7793_ID_MASK		0xF

/* IO (Excitation Current Sources) Register Bit Designations (AD7793_REG_IO) */
#define AD7793_IEXCDIR(x)	(((x) & 0x3) << 2)
#define AD7793_IEXCEN(x)	(((x) & 0x3) << 0)

/* AD7793_IEXCDIR(x) options*/
#define AD7793_DIR_IEXC1_IOUT1_IEXC2_IOUT2	0  /* IEXC1 connect to IOUT1, IEXC2 connect to IOUT2 */
#define AD7793_DIR_IEXC1_IOUT2_IEXC2_IOUT1	1  /* IEXC1 connect to IOUT2, IEXC2 connect to IOUT1 */
#define AD7793_DIR_IEXC1_IEXC2_IOUT1		2  /* Both current sources IEXC1,2 connect to IOUT1  */
#define AD7793_DIR_IEXC1_IEXC2_IOUT2		3  /* Both current sources IEXC1,2 connect to IOUT2 */

/* AD7793_IEXCEN(x) options*/
#define AD7793_EN_IXCEN_DISABLE			0  /* Disable excitation current*/
#define AD7793_EN_IXCEN_10uA				1  /* Excitation Current 10uA */
#define AD7793_EN_IXCEN_210uA				2  /* Excitation Current 210uA */
#define AD7793_EN_IXCEN_1mA					3  /* Excitation Current 1mA */

    void AD7793_HardCnfg();
    void AD7793_Init();
uint32_t AD7793_GetRegisterValue(uint8_t regAddress, uint8_t size);
    void AD7793_SetRegisterValue(uint8_t regAddress, uint32_t regValue, uint8_t size);
    void AD7793_SetMode(uint32_t mode);
    void AD7793_SetChannel(uint32_t channel) ;
    void AD7793_SetPolarity(uint32_t state);
 uint8_t AD7793_GetReadyReg();
 uint8_t AD7793_GetReadyPin();
 uint8_t AD7793_Reset();
    void AD7793_SetCurrentSourcesDirection(uint32_t dir);
    void AD7793_SetCurrentSourcesValue(uint32_t val);
    void AD7793_SetRate(uint32_t rate);
    void AD7793_SetReference(uint32_t state);
    void AD7793_SetBuf(uint32_t state);
    void AD7793_SetClock(uint32_t rate);
    void AD7793_SetGain(uint32_t gain);



#endif
