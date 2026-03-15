#include "sma.h"

// Инициализация фильтра
void sma_init(SMA_Filter *f) {
    f->index = 0;
    f->count = 0;
    for (int i = 0; i < SMA_SIZE; i++) {
        f->buffer[i] = 0;
    }
}

// Добавить новое значение и получить среднее
uint16_t sma_add(SMA_Filter *f, uint16_t value) {
    f->buffer[f->index] = value;
    f->index = (f->index + 1) % SMA_SIZE;

    if (f->count < SMA_SIZE) {
        f->count++;
    }

    // Суммируем активные значения
    uint32_t sum = 0;
    for (int i = 0; i < f->count; i++) {
        sum += f->buffer[i];
    }

    return (uint16_t)(sum / f->count);
}