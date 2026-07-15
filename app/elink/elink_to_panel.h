#ifndef _ELINK_TO_PANEL_H__
#define _ELINK_TO_PANEL_H__

#include "stdint.h"
#include "stddef.h"
#include "clock.h"

// === Структура запроса от слейва ===
typedef struct {
    uint8_t  slave_id;        // 1 байт: адрес слейва (1=AC1, 2=AC2, 3=DC1, 4=DC2)
    uint8_t  is_write_request;// 1 байт: 1 = запрос на запись, 0 = чтение
    uint16_t register_address;// 2 байта: номер регистра ( little-endian )
    uint16_t register_value;  // 2 байта: значение регистра (little-endian)
    uint8_t  log_index;       // 1 байт: номер записи лога (1..20)
    uint8_t  start_req[2][5]; // Cигналы управления для каждого слейва ( [0] - start_req_hw, бит [1] - start_req_do)
    uint8_t  reserved[93];    // остальные байты — резерв (100 - 8 = 92 байта, но явно 8+92=100)
} __attribute__((packed)) Typedef_SlaveRequest;

extern Typedef_SlaveRequest tcpInputSlaveReq;
extern Typedef_SlaveRequest tcpInputSlaveReqRW;
extern Typedef_SlaveRequest tcpOutputSlave_req;
extern Typedef_RtcTimeStamp elink_clock;


#endif