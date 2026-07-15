// fifo_buffer.h
#ifndef FIFO_BUFFER_H
#define FIFO_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "task_logger.h"
//#define FIFO_DEBUG




#define FIFO_SIZE_DEF                   (400)  // Максимум записей в буфере
#define FIFO_SECTOR_SIZE_DEF            ( 4096UL )
#define FIFO_PAGE_SIZE_DEF              (  256UL )
#define FIFO_FLASH_SECTOR_CNT_MAX_DEF   (   10UL ) // MIN 2!
#define FIFO_FLASH_SECTOR_ADR1_DEF      ((uint8_t *) 0x3000U)


#define FIFO_SIZE                       fifo->size
#define FIFO_SECTOR_SIZE                fifo->sector_size
#define FIFO_PAGE_SIZE                  fifo->page_size
#define FIFO_FLASH_SECTOR_CNT_MAX       fifo->sector_count_max
#define FIFO_FLASH_SECTOR_ADR1          fifo->fifo_start_adr


#ifdef FIFO_DEBUG

//#define FIFO_PTYPE uintptr_t
#define FIFO_DEBUG_PRINT(...) printf(__VA_ARGS__)
#define FIFO_PORT_READ_PAGE_FROM_FLASH(buf, pg_adr)     memcpy( (uint8_t *) (buf)    , (uint8_t *) (pg_adr)  , FIFO_PAGE_SIZE)
#define FIFO_PORT_WRITE_PAGE_TO_FLASH(pg_adr, buf)      memcpy( (uint8_t *) (pg_adr) , (uint8_t *) (buf)     , FIFO_PAGE_SIZE)
#define FIFO_ERASE_FLASH_SECTOR(sec_adr)                memset( (uint8_t *) (sec_adr), 0                     , FIFO_SECTOR_SIZE);

#define FIFO_BUFFER_BYTE_SZ                             (FIFO_SECTOR_SIZE*FIFO_FLASH_SECTOR_CNT_MAX)

//extern uint8_t fifo_sector1[FIFO_BUFFER_BYTE_SZ];
//#define FIFO_FLASH_SECTOR_ADR1  fifo_sector1

#include "LoggerRecord.h"

#else
#define FIFO_PORT_READ_PAGE_FROM_FLASH(buf, pg_adr) vGD25PageReadAsync(( (uint32_t) (pg_adr)), buf)
#define FIFO_PORT_WRITE_PAGE_TO_FLASH(pg_adr, buf)  vGD25PageProgramAsync(((uint32_t) (pg_adr)),buf )
#define FIFO_ERASE_FLASH_SECTOR(sec_adr)            vGD25SectorErase( (uint32_t) (sec_adr))
#define FIFO_PTYPE volatile uint32_t

#define FIFO_DEBUG_PRINT(...) ;

#include "task_logger.h"
#include "GD25Q32_os.h"
#endif

#define FIFO_DATA_SIZE      (sizeof(Typedef_LoggerRecord))
#define FIFO_SECTOR_CAP     (FIFO_SECTOR_SIZE/FIFO_DATA_SIZE)
#define FIFO_PAGE_CAP       (FIFO_PAGE_SIZE/FIFO_DATA_SIZE)
#define FIFO_FLASH_CAP      (FIFO_FLASH_SECTOR_CNT_MAX*FIFO_SECTOR_CAP)

#define FIFO_ASSERT_SIZE() { \
FIFO_DEBUG_PRINT("FIFO_ASSERT_SIZE \n"); \
FIFO_DEBUG_PRINT("FIFO_SECTOR_CAP  %d\n" , FIFO_SECTOR_CAP ); \
FIFO_DEBUG_PRINT("FIFO_SIZE %d\n" , FIFO_SIZE); \
FIFO_DEBUG_PRINT("FIFO_FLASH_CAP %d\n" , FIFO_FLASH_CAP); \
\
    while ( (FIFO_FLASH_CAP - FIFO_SECTOR_CAP)  < FIFO_SIZE){;} \
}\


typedef struct {
    uint32_t count;         // elements in fifo (ind = count - 1)!
    uint32_t sector_cnt;    // sector index c
    bool sec_cnt_upd;       // sectors rewrote (have values behind 0 fifo index)
    bool isCharged;         // fifo charged count == FIFOMAX

   const size_t size;         // records
   const size_t sector_size;       // byte
   const size_t page_size;         // byte
   const size_t sector_count_max;  // sectors 
   const uint8_t * fifo_start_adr;  // memory adr

   uint8_t * page_buf; // page size
    
} FIFO_Buffer;

// Инициализация
void FIFO_Init(FIFO_Buffer *fifo);

// Добавить запись: при переполнении удаляется половина
bool FIFO_Enqueue(FIFO_Buffer *fifo, Typedef_LoggerRecord *record);

// Получить запись по индексу: 0 = последняя, 1 = предпоследняя...
bool FIFO_Peek(FIFO_Buffer * fifo, size_t index, Typedef_LoggerRecord *rec);

bool FIFO_Scan(FIFO_Buffer * fifo);

// Получить количество элементов
size_t FIFO_Count(const FIFO_Buffer *fifo);

static inline bool FIFO_IsEmpty(const FIFO_Buffer *fifo) {
    return fifo->count == 0;
}

static inline bool FIFO_IsFull(const FIFO_Buffer *fifo) {
   size_t c = fifo->count-fifo->sector_cnt*FIFO_SECTOR_CAP;
   return c >= (uintptr_t) (FIFO_SECTOR_CAP);
   
}

void FIFO_write_crc(Typedef_LoggerRecord * rec);
bool FIFO_check_crc(Typedef_LoggerRecord * rec);

size_t FIFO_last_indx(const FIFO_Buffer *fifo);


#endif // FIFO_BUFFER_H