#ifndef __SPI_ADC_H
#define __SPI_ADC_H

#include "spi_control.h"

typedef enum
{
	ADC_UNKNOWN,
	ADC_MS5192,
	ADC_MS5193,
	ADC_MS5194,
	ADC_MS5195,
	ADC_AD7792 = ADC_MS5192,
	ADC_AD7793 = ADC_MS5193,
	ADC_AD7794 = ADC_MS5194,
	ADC_AD7795 = ADC_MS5195
} SPI_ADC_code_e; // тип АЦП (определяется при старте)

typedef enum
{
	ADC_temp_channel = 6,
	ADC_AVDD_channel = 7
} SPI_ADC_channels_e;

typedef struct
{
	u8:1;
	u8 amp_cm:1;
	u8:2;
	u8 psw:1;
	u8 mode:3;
	u8 update_rate:4;
	u8 chop_dis:1;
	u8:1; 
	u8 clk_src:2;
} SPI_ADC_mode_bits_t;

typedef enum
{
	adc_mode_off = 0,
	adc_mode_on = 1,
	adc_mode_continuous = 0,
	adc_mode_single = 1,
	adc_mode_idle = 2,
	adc_mode_power_down = 3,
	adc_mode_int_zero_calibration = 4,
	adc_mode_int_full_calibration = 5,
	adc_mode_system_zero_calibration = 6,
	adc_mode_system_full_calibration = 7,
	adc_mode_ur_x = 0,
	adc_mode_ur_470Hz = 1,
	adc_mode_ur_242Hz = 2,
	adc_mode_ur_123Hz = 3,
	adc_mode_ur_62Hz = 4,
	adc_mode_ur_50Hz = 5,
	adc_mode_ur_39Hz = 6,
	adc_mode_ur_33_2Hz = 7,
	adc_mode_ur_19_6Hz = 8,
	adc_mode_ur_16_7Hz = 9,
	adc_mode_ur_16_7_2Hz = 10,
	adc_mode_ur_12_5Hz = 11,
	adc_mode_ur_10Hz = 12,
	adc_mode_ur_8_33Hz = 13,
	adc_mode_ur_6_25Hz = 14,
	adc_mode_ur_4_17Hz = 15,
	adc_mode_internal_64 = 0,	// Internal 64kHz, CLK output is disabled
	adc_mode_internal_64_o = 1,	// Internal 64kHz, CLK output is enabled
	adc_mode_external_64 = 2,	// External 64kHz
	adc_mode_external = 3		// External clock used (divided by 2)
} spi_adc_mode_e;

typedef union
{
	SPI_ADC_mode_bits_t bits;
	u8 as_byte[2];
} SPI_ADC_mode_t;

typedef struct
{
	u8 gain:3;
	u8 boost:1;
	u8 polarity:1;
	u8 burnout:1;
	u8 vbias:2;
	u8 channel:4; // 0..4 для 94/95, 0-2 для 92/93, 6 - temp, 7 - AVdd, 8 - AIN1(-)/AIN1(-) для 94/95
	u8 buf:1;
	u8 ref_det:1; // только у 94/95
	u8 refsel:2; // 
} SPI_ADC_config_bits_t;

typedef enum
{
	adc_config_off = 0,
	adc_config_on = 1,
	adc_config_bipolar = 0,
	adc_config_unipolar = 1,
	adc_config_gain_1 = 0,
	adc_config_gain_2 = 1,
	adc_config_gain_4 = 2,
	adc_config_gain_8 = 3,
	adc_config_gain_16 = 4,
	adc_config_gain_32 = 5,
	adc_config_gain_64 = 6,
	adc_config_gain_128 = 7,

	adc_config_bias_0 = 0,
	adc_config_bias_1 = 1,
	adc_config_bias_2 = 2,
	adc_config_bias_3 = 3,
	
	adc_config_refsel_ext_1 = 0,
	adc_config_refsel_ext_2 = 1, // только для 94/95
	adc_config_refsel_int = 2
	
} spi_adc_config_e;


typedef union
{
	SPI_ADC_config_bits_t bits;
	u8 as_byte[2];
} SPI_ADC_config_t;

typedef struct
{
	u8:2;
	u8 cread:1;
	u8 reg:3;
	u8 r_w:1;
	u8 wen:1;
} SPI_ADC_comm_bits_t;

typedef union
{
	SPI_ADC_comm_bits_t as_bits;
	u8 as_byte[1];
} SPI_ADC_comm_t;

typedef enum
{
	adc_write = 0,
	adc_read = 1,
	adc_cread = 1
} SPI_ADC_comm_e;

typedef struct
{
	u8 channel:3;
	u8 sr3:1;
	u8 sr4:1;
	u8 noxref:1;
	u8 err:1;
	u8 rdy:1;
} SPI_ADC_status_bits_t;

typedef union
{
	SPI_ADC_status_bits_t as_bits;
	u8 as_byte[1];
} SPI_ADC_status_t;

typedef enum
{
	adc_status_ready = 0,
	adc_status_err = 1,
	adc_status_xx95 = 0,
	adc_status_xx94 = 1
} SPI_ADC_status_e;

typedef enum
{
	adc_comm_reg = 0,
	adc_status_reg = 0,
	adc_mode_reg = 1,
	adc_config_reg = 2,
	adc_data_reg = 3,
	adc_id_reg = 4,
	adc_io_reg = 5,
	adc_offset_reg = 6,
	adc_fullscale_reg = 7
} SPI_ADC_registers_e;

typedef struct
{
	u8 id:4;
	u8:4;
} SPI_ADC_id_bits_t;

typedef union
{
	SPI_ADC_id_bits_t as_bits;
	u8 as_byte[1];
} SPI_ADC_id_t;

typedef enum
{
	adc_id_xx92 = 0x0a,
	adc_id_xx93 = 0x0b,
	adc_id_xx94_95 = 0x0f
} SPI_ADC_id_e;

typedef struct
{
	union
	{
		struct
		{
			SPI_ADC_comm_t comm;
			union
			{
				SPI_ADC_mode_t mode;
				SPI_ADC_config_t config;
				u8 as_byte[2];
			};
		} packet;
		u8 as_byte[3];
	};
	
} SPI_ADC_write_request_t;

typedef struct
{
	SemaphoreHandle_t sem;
	SPI_ADC_config_t config;
	SPI_ADC_mode_t mode;
	spi_handle_t spi;
	u8 cs_pin;
	u8 ADC_bits;
	u8 ADC_channels;
	SPI_ADC_code_e ADC_type;
} SPI_ADC_handle_t;

#define MS51xx_COMM_W                      0x00
#define MS51xx_COMM_R                      0x40

#define MS51xx_COMM_REG_COMM_W             0x00 // Communication register write
#define MS51xx_COMM_REG_STAT_R             0x00 // Status register read
#define MS51xx_COMM_REG_MODE               0x08 // Mode register
#define MS51xx_COMM_REG_CONF               0x10 // Configuration register
#define MS51xx_COMM_REG_DATA               0x18 // Data register (RO)
#define MS51xx_COMM_REG_ID                 0x20 // ID register (RO)
#define MS51xx_COMM_REG_IO                 0x28 // IO register
#define MS51xx_COMM_REG_OFFSET             0x30 // Offset register
#define MS51xx_COMM_REG_FS                 0x38 // Full-scale register

//Mode register HIGH
#define MS51xx_MODEH_CONTINUOUS            0x00 // Continuous Conversion Mode
#define MS51xx_MODEH_SINGLE                0x20 // Single Conversion Mode
#define MS51xx_MODEH_IDLE                  0x40 // Idle Mode
#define MS51xx_MODEH_POWER_DOWN            0x60 // Power-Down Mode
#define MS51xx_MODEH_INT_ZERO_CALIB        0x80 // Internal Zero-Scale Calibration
#define MS51xx_MODEH_INT_FULL_CALIB        0xA0 // Internal Full-Scale Calibration
#define MS51xx_MODEH_SYS_ZERO_CALIB        0xC0 // System Zero-Scale Calibration
#define MS51xx_MODEH_SYS_FULL_CALIB        0xE0 // System Full-Scale Calibration

#define MS51xx_MODEH_PSW                   0x10 // Power Swith Control
#define MS51xx_MODEH_AMP_CM                0x02 // Instrumentation Amplifier Common-Mode

//Mode register LOW
#define MS51xx_MODEL_CLK_INT               0x00 // Internal 64kHz clock
#define MS51xx_MODEL_CLK_INT_PIN           0x40 // Internal 64kHz clock, available at the CLK pin
#define MS51xx_MODEL_CLK_EXT_64            0x80 // External 64kHz clock
#define MS51xx_MODEL_CLK_EXT_DIV2          0xC0 // External clock divided by 2

#define MS51xx_MODEL_CHOP_DIS              0x20 // Disable chop

#define MS51xx_MODEL_UR_X                  0x00 // Update rate X (?)
#define MS51xx_MODEL_UR_470                0x01 // Update rate 470Hz
#define MS51xx_MODEL_UR_242                0x02 // Update rate 242Hz
#define MS51xx_MODEL_UR_123                0x03 // Update rate 123Hz
#define MS51xx_MODEL_UR_62                 0x04 // Update rate 62Hz
#define MS51xx_MODEL_UR_50                 0x05 // Update rate 50Hz
#define MS51xx_MODEL_UR_39                 0x06 // Update rate 39Hz
#define MS51xx_MODEL_UR_33_2               0x07 // Update rate 33.2Hz
#define MS51xx_MODEL_UR_19_6               0x08 // Update rate 19.6Hz, 90dB(60Hz) rejection
#define MS51xx_MODEL_UR_16_7               0x09 // Update rate 16.7Hz, 80dB(50Hz) rejection
#define MS51xx_MODEL_UR_16_7_2             0x0A // Update rate 16.7Hz, 65dB(50&60Hz) rejection
#define MS51xx_MODEL_UR_12_5               0x0B // Update rate 12.5Hz, 66dB(50&60Hz) rejection
#define MS51xx_MODEL_UR_10                 0x0C // Update rate 10Hz,   69dB(50&60Hz) rejection
#define MS51xx_MODEL_UR_8_33               0x0D // Update rate 8.33Hz, 70dB(50&60Hz) rejection
#define MS51xx_MODEL_UR_6_25               0x0E // Update rate 6.25Hz, 72dB(50&60Hz) rejection
#define MS51xx_MODEL_UR_4_17               0x0F // Update rate 4.17Hz, 74dB(50&60Hz) rejection

//Configuration register HIGH
#define MS51xx_CONFH_VBIAS_DIS             0x00 // Bias Voltage Generator disable
#define MS51xx_CONFH_VBIAS_AIN1            0x40 // Bias Voltage Generator connected to AIN1(-)
#define MS51xx_CONFH_VBIAS_AIN2            0x80 // Bias Voltage Generator connected to AIN2(-)
#define MS51xx_CONFH_VBIAS_AIN3            0xC0 // Bias Voltage Generator connected to AIN3(-)

#define MS51xx_CONFH_BO                    0x20 // Burnout Current Enable

#define MS51xx_CONFH_UNIPOLAR              0x10 // Unipolar if 1 (0-0xFFFFFF), else bipolar (0 at 0x800000)

#define MS51xx_CONFH_BOOST                 0x08 // Increase current of Bias Voltage Generator

#define MS51xx_CONFH_GAIN_1                0x00 // 2.5V Input Range
#define MS51xx_CONFH_GAIN_2                0x01 // 1.25V Input Range
#define MS51xx_CONFH_GAIN_4                0x02 // 625mV Input Range
#define MS51xx_CONFH_GAIN_8                0x03 // 312.5mV Input Range
#define MS51xx_CONFH_GAIN_16               0x04 // 156.2mV Input Range
#define MS51xx_CONFH_GAIN_32               0x05 // 78.125mV Input Range
#define MS51xx_CONFH_GAIN_64               0x06 // 39.06mV Input Range
#define MS51xx_CONFH_GAIN_128              0x07 // 19.53mV Input Range

//Configuration register LOW
#define MS51xx_CONFL_REFSEL_EXT1           0x00 // External reference between REFIN1(+) and REFIN1(-)
#define MS51xx_CONFL_REFSEL_EXT2           0x40 // External reference between REFIN2(+) and REFIN2(-)
#define MS51xx_CONFL_REFSEL_INT            0x80 // Internal 1.17V reference

#define MS51xx_CONFL_REF_DET               0x20 // Reference detect

#define MS51xx_CONFL_BUF                   0x10 // Enable ADC buffer

#define MS51xx_CONFL_CH_MASK               0x0F // CHannel select mask

#define MS51xx_CONFL_CH1                   0x00 // Select Channel AIN1
#define MS51xx_CONFL_CH2                   0x01 // Select Channel AIN2
#define MS51xx_CONFL_CH3                   0x02 // Select Channel AIN3
#define MS51xx_CONFL_CH4                   0x03 // Select Channel AIN4
#define MS51xx_CONFL_CH5                   0x04 // Select Channel AIN5
#define MS51xx_CONFL_CH6                   0x05 // Select Channel AIN6
#define MS51xx_CONFL_CHT                   0x07 // Select Temp Sensor
#define MS51xx_CONFL_CHM                   0x07 // Select AV Monitor

// Функция инициализации внешнего АЦП
// Выполняет сброс АЦП и пытается определить его тип
void SPI_ADC_init(SPI_ADC_handle_t *spi_adc);

// Функция выполняет калибровку АЦП
s32 SPI_ADC_calibrate(SPI_ADC_handle_t *spi_adc, u8 channel);

// Функция запускает измерение канала, выжидает время,
// считывает результат и возвращает его
s32 SPI_ADC_read_channel(SPI_ADC_handle_t *spi_adc, u8 channel);

// Функция посылает команду "начать измерение" в АЦП
void SPI_ADC_start_measurement(SPI_ADC_handle_t *spi_adc, u8 channel);

// Функция возвращает результат измерения, начатого ранее АЦП
s32 SPI_ADC_read_data(SPI_ADC_handle_t *spi_adc);

// Функция возвращает указатель на структуру АЦП по номеру
SPI_ADC_handle_t *SPI_ADC_get_ptr(u8 num);

// Функция возвращает количество проинициализированных АЦП
u8 SPI_ADC_count();

// Функция возвращает строку-описатель порта АЦП в параметре name,
// например, "SPI0 (CS=A12)"
s32 SPI_ADC_get_name(const SPI_ADC_handle_t *spi_adc, char* name);

#endif
