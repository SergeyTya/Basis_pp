#include <stdint.h>

#define SMA_SIZE 3

// Структура для SMA-фильтра
typedef struct {
    uint16_t buffer[SMA_SIZE];  // Буфер последних значений
    uint8_t index;              // Текущая позиция в буфере
    uint8_t count;              // Сколько значений уже накоплено (до заполнения)
} SMA_Filter;

void sma_init(SMA_Filter *f);
uint16_t sma_add(SMA_Filter *f, uint16_t value);