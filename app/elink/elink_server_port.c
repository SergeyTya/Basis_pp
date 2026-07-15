#include "stdint.h"
#include "stddef.h"
#include <string.h>

#include "elink_to_panel.h"

/* Состав пакета для приема 100 байт

1 байт адрес текущего слейва (1-AC1, 2-AC2, 3-DC1, 4-DC2), 
1 байт  = признак запроса записи (если 1 - нужно записать регистр)
2 байта номер регистра для записи,
2 байта значение регистра ,  
1 байт - номер записи лога для чтения (от 1 до 20)
резерв
*/


// входящий для сервера от клиента
Typedef_SlaveRequest tcpInputSlaveReq;

// копируем для записи
Typedef_SlaveRequest tcpInputSlaveReqRW;

/* Состав пакета для отправки 500 байт

 1. 1 байт адрес текущего слейва (1-AC1, 2-AC2, 3-DC1, 4-DC2), 
 2. 10 байта состояние слейвов (0 -мастер 1-AC1, 2-AC2, 3-DC1, 4-DC2 , 5 - метер, 6 логер, 8-10 - резерв)
 3. 256 байт - таблица регистров - массив uint16_t holdings[256] (main.c)   256 bytes
 4. 6 байт - текущее временя   - структура Typedef_RtcTimeStamp (clock.h) 6 bytes  
 5. 20 байт - показания метра   - массив Typedef_MeterEntry[10] (meter.h)  2*10 = 20 bytes
 6. 64 байта - архив событий     - Typedef_LoggerRecord  (task_logger.h) 1*64 = 64 bytes
  -остальное резерв
*/

#include "task_master.h"
#include "meter.h"
#include "clock.h"

extern TypeDef_Master master;
extern uint16_t holdings[256];
extern Typedef_Clock clock;
extern Typedef_Meter meter;
extern Typedef_LoggerRecord rec_disp_remote;

void elink_server_get_pld(uint8_t * buf_rx, size_t szrx, uint8_t * buf_tx, uint8_t sztx ){

    
    /* разбор посыvлки */
    memcpy(&tcpInputSlaveReq, buf_rx, sizeof(tcpInputSlaveReq));  

    if(tcpInputSlaveReq.is_write_request){
        memcpy(&tcpInputSlaveReqRW, &tcpInputSlaveReq, sizeof(tcpInputSlaveReq));
    }

    /* формирование ответа */
    size_t ind =0;
    // 1.
    buf_tx[ind++] = tcpInputSlaveReq.slave_id;
    // 2.
    for (size_t i = 0; i < 5; i++)
    {
        buf_tx[ind++] = master.slave[i].slaveStates;
    }
    buf_tx[ind++] = meter.state;
    buf_tx[ind++] = clock.state;
    buf_tx[ind++] = 0;
    buf_tx[ind++] = 0;
    buf_tx[ind++] = 0;

    //3. holdings
    memcpy(buf_tx+ind, holdings, 256);
    ind += 256;

    //4. clock
    clock.time.timestamp_minute ++;
    if(clock.time.timestamp_minute> 60) clock.time.timestamp_minute =0;
    memcpy(buf_tx+ind, &clock.time, sizeof(Typedef_RtcTimeStamp));
    ind += sizeof(Typedef_RtcTimeStamp);
    
    //45. meter
    memcpy(buf_tx+ind , &meter, sizeof(Typedef_Meter));
    ind += sizeof(Typedef_Meter);

    //6. logger
    memcpy(buf_tx+ind ,&rec_disp_remote, sizeof(Typedef_LoggerRecord));
    ind += sizeof(Typedef_LoggerRecord); 
}
    