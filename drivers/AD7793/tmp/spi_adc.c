#include "main.h"
#include "spi_adc.h"
#include "spi_control.h"

#define MAX_SPI_ADC (10)

const u16 adc_t_settle[16] = { 2, 4, 8, 16, 32, 40, 48, 60, 101, 120, 120, 160, 200, 240, 320, 480 }; // время измерения в мс при различных update rate

SPI_ADC_handle_t* spi_adc_list[MAX_SPI_ADC];
static u8 spi_adc_count = 0;

static void SPI_ADC_reset(SPI_ADC_handle_t *spi_adc)
{
	SPI_transaction(&spi_adc->spi, 0, 0, 5, spi_adc->cs_pin, 0xff); // шлем 5 раз 0xff - это сброс
}

static s32 adc_get_status(SPI_ADC_handle_t *spi_adc)
{
	SPI_ADC_comm_t request = { .as_bits.reg = adc_status_reg, .as_bits.r_w = adc_read }; // хотим прочесть статусный регистр
	SPI_ADC_status_t answer;
	SPI_transmit_receive(&spi_adc->spi, &request.as_byte[0], sizeof(request), &answer.as_byte[0], sizeof(answer), spi_adc->cs_pin, 0);
	return answer.as_byte[0];
}

static s32 wait_for_adc(SPI_ADC_handle_t *spi_adc)
{
	s32 timeout = 10;
	vTaskDelay(2);
	SPI_ADC_status_t status;
	status.as_bits.rdy = !adc_status_ready;
	while (timeout--) // Ждем готовности АЦП
	{
		status.as_byte[0] = adc_get_status(spi_adc);
		if (status.as_bits.rdy == adc_status_ready)
			break;
		vTaskDelay(2);
	}
	if (timeout)
		return timeout;
	return -1;
}

static void SPI_ADC_detect(SPI_ADC_handle_t *spi_adc)
{
	const SPI_ADC_comm_t id_request = { .as_bits.reg = adc_id_reg, .as_bits.r_w = adc_read }; // Хотим прочесть регистр ID
	SPI_ADC_id_t id;
	SPI_transmit_receive(&spi_adc->spi, &id_request.as_byte[0], sizeof(id_request), &id.as_byte[0], sizeof(id), spi_adc->cs_pin, 0);
	
	const SPI_ADC_comm_t status_request = { .as_bits.reg = adc_status_reg, .as_bits.r_w = adc_read }; // Хотим прочесть регистр статуса
	SPI_ADC_status_t status;
	SPI_transmit_receive(&spi_adc->spi, &status_request.as_byte[0], sizeof(status_request), &status.as_byte[0], sizeof(status), spi_adc->cs_pin, 0);

	if (status.as_bits.sr4 == 1)
	{
		spi_adc->ADC_type = ADC_UNKNOWN;
		return;
	}
	
	switch (id.as_bits.id) // Определяем типа АЦП согласно даташиту
	{
	case adc_id_xx92:
		spi_adc->ADC_type = ADC_MS5192;
		spi_adc->ADC_bits = 16;
		spi_adc->ADC_channels = 3;
		break;
	case adc_id_xx93:
		spi_adc->ADC_type = ADC_MS5193;
		spi_adc->ADC_bits = 24;
		spi_adc->ADC_channels = 3;
		break;
	case adc_id_xx94_95:
		spi_adc->ADC_channels = 6;
		switch (status.as_bits.sr3)
		{
		case adc_status_xx95:
			spi_adc->ADC_type = ADC_MS5195;
			spi_adc->ADC_bits = 16;
			break;
		case adc_status_xx94:
			spi_adc->ADC_type = ADC_MS5194;
			spi_adc->ADC_bits = 24;
			break;
//		default:
//			break;
		}
		break;
	default:
		spi_adc->ADC_type = ADC_UNKNOWN;
		break;
	}
}

s32 SPI_ADC_calibrate(SPI_ADC_handle_t *spi_adc, u8 channel)
{
	if (xSemaphoreTake(spi_adc->sem, 5000) != pdPASS)
		return -1;
	SPI_ADC_write_request_t request = { 0 };
	
	request.packet.comm.as_bits.reg = adc_config_reg; // Хотим записать регистр конфигурации
	request.packet.config = spi_adc->config;
	request.packet.config.bits.channel = channel;
	
	SPI_transmit(&spi_adc->spi, &request.as_byte[0], sizeof(request), spi_adc->cs_pin);
	
	SPI_ADC_write_request_t calib_zero = { 0 };
	
	calib_zero.packet.comm.as_bits.reg = adc_mode_reg; // Хотим записать регистр режима
	calib_zero.packet.mode = spi_adc->mode;
	calib_zero.packet.mode.bits.mode = adc_mode_int_zero_calibration; // Выбираем режим калибровки
	
	SPI_transmit(&spi_adc->spi, &calib_zero.as_byte[0], sizeof(calib_zero), spi_adc->cs_pin);
	
	u8 ur_code = spi_adc->mode.bits.update_rate;
	u16 wait_time = adc_t_settle[ur_code];
	vTaskDelay(1 + wait_time);

	s32 s1 = wait_for_adc(spi_adc);
	if (s1 < 0)
	{
		return -1;
	}
	
	SPI_ADC_write_request_t calib_full = { 0 };
	
	calib_full.packet.comm.as_bits.reg = adc_mode_reg; // Хотим записать регистр режима
	calib_zero.packet.mode = spi_adc->mode;
	calib_zero.packet.mode.bits.mode = adc_mode_int_full_calibration; // Выбираем режим калибровки
	
	SPI_transmit(&spi_adc->spi, &calib_full.as_byte[0], sizeof(calib_full), spi_adc->cs_pin);
	
	vTaskDelay(1 + 2 * wait_time); // Ждем необходимое время

	s1 = wait_for_adc(spi_adc);
	if (s1 < 0)
	{
		return -1;
	}
	xSemaphoreGive(spi_adc->sem);
	return s1;
}

void SPI_ADC_init(SPI_ADC_handle_t *spi_adc)
{
	if (spi_adc_count >= MAX_SPI_ADC)
	{
		uart_printf("SPI_ADC_init(): ошибка инициализации - слишком много АЦП\r\n");
		return;
	}
	spi_adc->sem = xSemaphoreCreateMutex();
	SPI_driver_init(&spi_adc->spi);
	SPI_cs_pin_init(spi_adc->cs_pin);
	SPI_ADC_reset(spi_adc); // Сбрасываем АПЦ (иначе он работает в неподходящем нам режиме)
	vTaskDelay(3);
	SPI_ADC_detect(spi_adc); // Определяем тип АЦП
	vTaskDelay(3);
	if (spi_adc->ADC_type != ADC_UNKNOWN)
	{
		u8 ch_count = 3;
		if ((spi_adc->ADC_type == ADC_AD7794) || (spi_adc->ADC_type == ADC_AD7795))
			ch_count = 6;
		for (u8 i = 0; i < ch_count; i++)
		{
			s32 result = SPI_ADC_calibrate(spi_adc, i); // Выполняем калибровку АЦП
			if (result < 0)
				uart_printf("SPI_ADC_init(): Ошибка калибровки АЦП на SPI%d, канал %d\r\n", spi_adc->spi.number, i);
		}
	}
	else
		uart_printf("SPI_ADC_init(): АЦП на SPI%d не распознан\r\n", spi_adc->spi.number);
	spi_adc_list[spi_adc_count++] = spi_adc;
}

void SPI_ADC_start_measurement(SPI_ADC_handle_t *spi_adc, u8 channel)
{
	SPI_ADC_write_request_t adc_config = { 0 };
	
	adc_config.packet.comm.as_bits.reg = adc_config_reg; // Хотим записать режим конфигурации
	adc_config.packet.config.bits.polarity = spi_adc->config.bits.polarity;
	adc_config.packet.config.bits.channel = channel; // Указываем номер канала
	
	SPI_transmit(&spi_adc->spi, &adc_config.as_byte[0], sizeof(adc_config), spi_adc->cs_pin);
	
	SPI_ADC_write_request_t adc_start = { 0 };
	
	adc_start.packet.comm.as_bits.reg = adc_mode_reg; // Хотим записать регистр режима
	adc_start.packet.mode.bits.mode = spi_adc->mode.bits.mode;
	adc_start.packet.mode.bits.update_rate = spi_adc->mode.bits.update_rate; // Задаем частоту измерений
	
	SPI_transmit(&spi_adc->spi, &adc_start.as_byte[0], sizeof(adc_start), spi_adc->cs_pin);
	
}

s32 SPI_ADC_read_data(SPI_ADC_handle_t *spi_adc)
{
	s32 result = wait_for_adc(spi_adc); // Ждем готовности АЦП
	if (result < 0)
		return result;
	
	u8 adc_answer[3] = { 0 };
	SPI_ADC_comm_t read_request = { 0 };
	read_request.as_bits.r_w = adc_read;
	read_request.as_bits.reg = adc_data_reg; // Хотим прочесть регистр данных
	
	SPI_transmit_receive(&spi_adc->spi, &read_request.as_byte[0], sizeof(read_request), &adc_answer[0], spi_adc->ADC_bits / 8, spi_adc->cs_pin, 0);
	
	result = adc_answer[0] << 8;
	result |= adc_answer[1];
	if (spi_adc->ADC_bits == 24)
		result = (result << 8) | adc_answer[2];
	return result;
}

s32 SPI_ADC_read_channel(SPI_ADC_handle_t *spi_adc, u8 channel)
{
	if (xSemaphoreTake(spi_adc->sem, 100) != pdPASS)
		return -9;
	SPI_ADC_start_measurement(spi_adc, channel); // Запускаем измерения

	u8 ur_code = spi_adc->mode.bits.update_rate;
	u16 wait_time = adc_t_settle[ur_code]; // Определяем время измерения
	vTaskDelay(1 + wait_time); // Ждем

	s32 result = SPI_ADC_read_data(spi_adc); // Считываем результат измерения
	xSemaphoreGive(spi_adc->sem);
	return result;
}

SPI_ADC_handle_t *SPI_ADC_get_ptr(u8 num)
{
	if (num >= spi_adc_count)
		return 0;
	return spi_adc_list[num];
}

u8 SPI_ADC_count()
{
	return spi_adc_count;
}

u8 SPI_ADC_get_port_name(const SPI_ADC_handle_t *spi_adc) // возвращает имя порта ноги cs_pin ('A'..'E')
{
	return 'A' + CONFIG_TO_PORT(spi_adc->cs_pin);
}

u8 SPI_ADC_get_port_pin(const SPI_ADC_handle_t *spi_adc)
{
	return CONFIG_TO_PIN_NUMBER(spi_adc->cs_pin);
}

s32 SPI_ADC_get_name(const SPI_ADC_handle_t *spi_adc, char* name)
{
	if (!spi_adc)
		return -1;
	return sprintf(name, "SPI%d (CS=%c%d)", spi_adc->spi.number, SPI_ADC_get_port_name(spi_adc), SPI_ADC_get_port_pin(spi_adc));
}
