// fifo_buffer.c
#include "fifo_buffer.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "crc16.h"
#include <stdlib.h>


#ifdef FIFO_DEBUG

#endif

//uint8_t page_buf_tx[FIFO_PAGE_SIZE];
//uint8_t page_buf_rx[FIFO_PAGE_SIZE];

 static void FIFO_copy_to_mem(uint8_t rec[FIFO_DATA_SIZE], size_t index_to_write, FIFO_Buffer * fifo);
 static void FIFO_change_buffer(FIFO_Buffer *fifo);
 static bool FIFO_Peek_From_buffer(size_t index, uint8_t rec[FIFO_DATA_SIZE], FIFO_Buffer * fifo);


// === Инициализация буфера ===
void FIFO_Init(FIFO_Buffer *fifo) {

    FIFO_ASSERT_SIZE();

    //2.1 If false Erase flash
    for (size_t i = 0; i < FIFO_FLASH_SECTOR_CNT_MAX; i++)
    {
        FIFO_ERASE_FLASH_SECTOR(FIFO_FLASH_SECTOR_ADR1+FIFO_SECTOR_SIZE*i);
    }
    
    fifo->count = 0;
    fifo->isCharged = false;


    //3. If  init fifo
    fifo->sector_cnt = 0;
    fifo->sec_cnt_upd = false;

    if(!fifo->page_buf)fifo->page_buf = malloc(fifo->page_size);
  
    
#ifdef FIFO_DEBUG

    // FIFO_DEBUG_PRINT(
    //     "---------\n"
    //     "%x, \n%x \n"
    //     "---------\n",
    //     &FIFO_FLASH_SECTOR_ADR1[0], &FIFO_FLASH_SECTOR_ADR1[sizeof(FIFO_FLASH_SECTOR_ADR1)-1]+sizeof(uint8_t)
    // );


    FIFO_DEBUG_PRINT(
        "Buffer init done\n"
        "Buffer start adr %x \n"
        "Buffer   end adr %x \n"
        "Total %d byte\n"
        "Pointer      adr %x \n"
        "Sector capacity %d \n"
        "Fifo size %d\n",

        (uintptr_t) FIFO_FLASH_SECTOR_ADR1, 
        (uintptr_t) ( (uintptr_t)FIFO_FLASH_SECTOR_ADR1 + FIFO_SECTOR_SIZE*FIFO_FLASH_SECTOR_CNT_MAX), 
        (uintptr_t) (FIFO_SECTOR_SIZE*FIFO_FLASH_SECTOR_CNT_MAX),
        FIFO_FLASH_SECTOR_ADR1, (int) FIFO_SECTOR_CAP, (int) FIFO_SIZE
    );

#endif

}

// === Place record to fifo ===
bool FIFO_Enqueue(FIFO_Buffer *fifo, Typedef_LoggerRecord *record) {
    if (!fifo || !record) {
        return false;
    }

    uint8_t * rp = (uint8_t *) record;
 
    if(fifo->count>FIFO_SIZE) fifo->isCharged = true;
   
    // меняем сектор только непосредственно перед записью
    if (FIFO_IsFull(fifo)){
        FIFO_change_buffer(fifo);
    }

    FIFO_copy_to_mem( rp, fifo->count++, fifo);

    // проверяем , и стираем следующий сектор
    if (FIFO_IsFull(fifo)){
        //FIFO_change_buffer(fifo);
        int32_t sc = fifo->sector_cnt + 1;
        if(sc>=(int32_t)FIFO_FLASH_SECTOR_CNT_MAX) sc =0;
        FIFO_ERASE_FLASH_SECTOR(
            (FIFO_FLASH_SECTOR_ADR1 + sc*FIFO_SECTOR_SIZE)
        );
    }
    


    return true;
}

/*
    0           - oldest
    FIFO_SIZE-1 - newest
*/
bool FIFO_Peek(FIFO_Buffer * fifo, size_t index, Typedef_LoggerRecord *rec){

    int32_t adr = 0;

    if (index >= FIFO_SIZE) {
        return false;
    }

    if(fifo->count == 0){
        return false;
    }

    if(index >= fifo->count){
        if(!fifo->isCharged){
            return false;
        }
    }

    if(fifo->count>=FIFO_SIZE){
        adr = fifo->count - (FIFO_SIZE - index);
    }else{

        if(fifo->sec_cnt_upd){  

            int32_t pos_h = (FIFO_SIZE-1)- fifo->count;
            int32_t d = index-pos_h;
            
            if( d <= 0 ){
                adr = (FIFO_FLASH_CAP) + d -1;
            }else{
                adr = index-pos_h -1;
            }

            //  FIFO_DEBUG_PRINT(
            // "pos_h=%d , d=%d  \n",
            // pos_h, d
            //); 

        }else{
             if(index>fifo->count) return false;
            adr = index;
        }   
    }
    
    //   FIFO_DEBUG_PRINT(
    //     "adr=%d , fifo->count%d , index =%d \n",
    //     adr, fifo->count, index
    //  ); 

     if(adr<0) return false;
    
    
    return FIFO_Peek_From_buffer( adr, (uint8_t*) rec, fifo);
}

// === Read Fifo element (0) - oldest (fifo->count-1) - newest ===
static bool FIFO_Peek_From_buffer( size_t index, uint8_t rec[FIFO_DATA_SIZE], FIFO_Buffer * fifo) {

    // Find page we need

       // Total page written
    int pg_tot = (index/(FIFO_PAGE_CAP));
    // Page index start in record buffer
    int pg_ind = (FIFO_PAGE_CAP)*(pg_tot);
    // The index of new record in page
    int pg_in_tw = index-pg_ind;

    // Read page

    FIFO_PORT_READ_PAGE_FROM_FLASH(
        fifo->page_buf, (FIFO_FLASH_SECTOR_ADR1 + pg_ind*FIFO_DATA_SIZE) 
    );

    //Read record
    memcpy( rec, 
        (uint8_t *)(fifo->page_buf + pg_in_tw*FIFO_DATA_SIZE),
        FIFO_DATA_SIZE);

    //  FIFO_DEBUG_PRINT(
    //     "                   record[%d]= %d \n",
    //     index,
    //     ((Typedef_LoggerRecord *)rec)->LiveCounter
    //  ); 
     

    return true;
}


static void FIFO_copy_to_mem( uint8_t rec[FIFO_DATA_SIZE], size_t index_to_write, FIFO_Buffer * fifo ){


    // clear page buffer
    memset(fifo->page_buf, 0xFF, FIFO_PAGE_SIZE);

    // 1. Read page 256 b
    
    // Total page written
    uint32_t pg_tot = (index_to_write/(FIFO_PAGE_CAP));
    // Page start index in record buffer
    uint32_t pg_ind = (FIFO_PAGE_CAP)*(pg_tot);

    // The index of new record in page
    uint32_t pg_in_tw = index_to_write-pg_ind;
       
    // 1. Read page from flash 
    FIFO_PORT_READ_PAGE_FROM_FLASH(
        fifo->page_buf, 
        (FIFO_FLASH_SECTOR_ADR1 + pg_ind*FIFO_DATA_SIZE)
    );
    // 2. Add new record to page
    memcpy(
        (fifo->page_buf + pg_in_tw*FIFO_DATA_SIZE),
        rec, FIFO_DATA_SIZE);

    // 3. Write  updated page to flash
    FIFO_PORT_WRITE_PAGE_TO_FLASH( 
        (FIFO_FLASH_SECTOR_ADR1 + pg_ind*FIFO_DATA_SIZE),
         fifo->page_buf
    );

//     // #ifdef FIFO_DEBUG       
//     // FIFO_DEBUG_PRINT(
//     //     "Index to write %d, addres to write %x buf_pointer %x\n", 
        
//     //     index_to_write, fifo_w_adr, rec_buf
//     //  ); 
// #endif
    
}


static void FIFO_change_buffer(FIFO_Buffer *fifo)
{

    // FIFO_DEBUG_PRINT(
    //     " Sector changed "
    //         "Index % 4d , Sector cnt % 4d, Addres % 10x,\n",
    //     fifo->count, fifo->sector_cnt, FIFO_FLASH_SECTOR_ADR1
    // );

    fifo->sector_cnt++;
    if(fifo->sector_cnt >=FIFO_FLASH_SECTOR_CNT_MAX) {

        fifo->sector_cnt =0;
        fifo->sec_cnt_upd = true;
        fifo->count = 0;
    }else{
    }

    
}

bool FIFO_Scan(FIFO_Buffer * fifo){

    Typedef_LoggerRecord rec;
    
    int32_t ind_fl=-1; // index where crc good end
    int32_t ind_rs=-1; // index where crc goods start

    if(!fifo->page_buf)fifo->page_buf = malloc(fifo->page_size);

    bool trig = true;

    size_t i;
    for (i = 0; i < (FIFO_FLASH_CAP); i++)
    {
        bool st = true;
        FIFO_Peek_From_buffer(i, (uint8_t *) &rec, fifo);
        if(!FIFO_check_crc(&rec)){
            st = false;
        }

        if( trig && !st) {
            if(ind_fl != -1) {
                FIFO_DEBUG_PRINT("ERROR FALLING ");
                FIFO_DEBUG_PRINT(
                    "ind_rs=%d, ind_fl=%d, cycles read i=%d \n", 
                    ind_rs, ind_fl, i
                );
                return false;
                break;
            }
            ind_fl= i;
        }
        if(!trig &&  st) {
            if(ind_rs != -1) {
                FIFO_DEBUG_PRINT("ERROR RASSING\n");
                return false;
                break;
            }
            ind_rs= i;
        } 

        trig = st;

    }


    FIFO_DEBUG_PRINT(
             "ind_rs=%d, ind_fl=%d, cycles read i=%d \n", 
               ind_rs, ind_fl, i
            );
    
    if((ind_fl<ind_rs) && (ind_fl!=-1) && (ind_rs!=-1) ){
        //fr case   
        int32_t cur_rec = ind_fl-1;
        if(cur_rec<0){
            cur_rec = FIFO_FLASH_CAP - 1;
            fifo->isCharged = true;
        }
        uint32_t cur_sec = ind_fl/FIFO_SECTOR_CAP;

        FIFO_Peek_From_buffer(cur_rec, (uint8_t *) &rec, fifo);
        bool crcvalid1 = FIFO_check_crc(&rec);
        bool crcvalid2 = ind_rs >(int) (FIFO_SECTOR_CAP*(cur_sec)); //check is sector clean 

        if(crcvalid1 && crcvalid2 ){
            fifo->sec_cnt_upd =1;

            if(ind_fl == 0){  // если сразу нет записей значит фифо полное но сектор не сменился
                fifo->count = FIFO_SIZE;
                fifo->sector_cnt = FIFO_FLASH_SECTOR_CNT_MAX;
            }else{
                fifo->count = ind_fl;
                fifo->sector_cnt = ind_fl/FIFO_SECTOR_CAP;
            }
         
            FIFO_DEBUG_PRINT(
             " FIFO FR-INIT DONE\n"
             " sec_cnt_upd=%d, fifo->count=%d, sector_cnt=%d, isCharged=%d\n",
             fifo->sec_cnt_upd, fifo->count,fifo->sector_cnt, fifo->isCharged
            );

        }else{
             FIFO_DEBUG_PRINT(" FIFO FR-INIT ERROR\n");
             return false;
        }
    
    }else if( (ind_rs ==-1) && (ind_fl>=0)){

        uint32_t cur_rec = ind_fl-1;
        if(ind_fl!=0){
            cur_rec = ind_fl-1;
        }else{
            cur_rec = FIFO_FLASH_CAP-1;
        }
        FIFO_Peek_From_buffer(cur_rec, (uint8_t *) &rec, fifo);
        if(FIFO_check_crc(&rec)){

            // check if where record behind
            FIFO_Peek_From_buffer(FIFO_FLASH_CAP-1, (uint8_t *) &rec, fifo);
            fifo->sec_cnt_upd = FIFO_check_crc(&rec);

            fifo->count = ind_fl;
            fifo->sector_cnt = ind_fl/FIFO_SECTOR_CAP;

            FIFO_DEBUG_PRINT(
             " FIFO F-INIT DONE\n"
             " sec_cnt_upd=%d, fifo->count=%d, sector_cnt=%d,\n",
             fifo->sec_cnt_upd, fifo->count,fifo->sector_cnt
            );

        }else{
             FIFO_DEBUG_PRINT(" FIFO F-INIT ERROR\n");
             return false;
        }
    
    }else{
        fifo->sec_cnt_upd =1;
        fifo->count = 0;
        fifo->sector_cnt = 0;

        FIFO_DEBUG_PRINT("Init from 0 index\n");
    }

    
    FIFO_DEBUG_PRINT("Done ind_fl=%d ind_rs=%d \n", ind_fl, ind_rs );

    return true;

}

size_t FIFO_Count(const FIFO_Buffer *fifo) {
    size_t ret_val = fifo->count;
    if(ret_val > FIFO_SIZE) ret_val = FIFO_SIZE;
    if(fifo->isCharged) ret_val = FIFO_SIZE;
    return ret_val;
}

void FIFO_write_crc(Typedef_LoggerRecord * rec){
    rec->crc16 = crc16((uint8_t *)rec, sizeof(Typedef_LoggerRecord) - 2);
}

bool FIFO_check_crc(Typedef_LoggerRecord * rec){
    
    return (rec->crc16 == crc16((uint8_t *)rec, sizeof(Typedef_LoggerRecord) - 2)) && (rec->crc16 != 0);
}


size_t FIFO_last_indx(const FIFO_Buffer *fifo){ 
    return FIFO_Count(fifo) -1;

}