#include "elink.h"

#define ELINK_CLIENT_PORT_BUFF_SZ 500
#define ELINK_CLIENT_PORT_TX_SZ 100
#define ELINK_CLIENT_PORT_RX_SZ 500
#define ELINK_CLIENT_PORT_REMOTE_IP 
#include "elink_to_panel.h"
#include "string.h"
#include "task_master.h"

uint8_t elink_mst_buff[ELINK_CLIENT_PORT_BUFF_SZ] __attribute__((section(".ARM.__at_0x30004000"), aligned(4))); 

volatile uint16_t link_rcv_cnt = 0;
volatile uint16_t err_diag = 0;

uint8_t hw_ch= 0;

extern uint16_t holdings[256];

extern TypeDef_Master master;

/* Состав пакета для приема 100 байт

1 байт адрес текущего слейва (1-AC1, 2-AC2, 3-DC1, 4-DC2), 
1 байт  = признак запроса записи (если 1 - нужно записать регистр)
2 байта номер регистра для записи,
2 байта значение регистра ,  
1 байт - номер записи лога для чтения (от 1 до 20)
10 байт -команды управления
2 байта счетчик
резерв
*/

// исходящий от клиента к серверу
Typedef_SlaveRequest tcpOutputSlave_req;

#include "task_panel.h"
extern Typedef_PanelConfig panelConfig;
static uint8_t tcp_slave_cnt = 0;

volatile uint16_t wid = 0;

void elink_client_port_get_pld(uint8_t * buff){

    if(!tcpOutputSlave_req.is_write_request){

        tcpOutputSlave_req.slave_id = panelConfig.active_slave;
        tcpOutputSlave_req.is_write_request = 0;
        tcpOutputSlave_req.register_address = 0;

    }else{
        wid = tcpOutputSlave_req.register_address;
    }

    tcpOutputSlave_req.log_index = 0;

    for (size_t i = 0; i < 5; i++)
    {
            tcpOutputSlave_req.start_req[0][i] =  master.slave[i].start_req_hw;
            tcpOutputSlave_req.start_req[1][i] =  master.slave[i].start_req_rdo; 
    }

    tcp_slave_cnt++;

    memcpy(buff, &tcpOutputSlave_req, sizeof(tcpOutputSlave_req));
    tcpOutputSlave_req.is_write_request = 0;
}



/* Состав пакета для отправки 500 байт

 1. 1 байт адрес текущего слейва (1-AC1, 2-AC2, 3-DC1, 4-DC2), 
 2. 10 байт состояние слейвов (0 -мастер 1-AC1, 2-AC2, 3-DC1, 4-DC2 , 5 - метер, 6 логер, 8-10 - резерв)
 3. 256 байт - таблица регистров - массив uint16_t holdings[256] (main.c)   256 bytes
 4. 6 байт - текущее временя   - структура Typedef_RtcTimeStamp (clock.h) 6 bytes  
 5. 20 байт - показания метра   - массив Typedef_MeterEntry[10] (meter.h)  2*10 = 20 bytes
 6. 64 байта - архив событий     - Typedef_LoggerRecord  (task_logger.h) 1*64 = 64 bytes
  -остальное резерв
*/

bool tcp_client_to = true;

#include "mbholding_type.h"
#include "clock.h"
#include "meter.h"

extern Typedef_LoggerRecord rec_disp;

extern TypeDef_MB_Table holdings_table[]; 

void elink_client_port_set_pld(uint8_t * buff){
    link_rcv_cnt++;

    size_t ind=0;
    //1. Skip 
    ind++;
   
    //2. Slave states
    for (size_t i = 0; i < 5; i++)
    {
        TypedefEnum_MasterSlaveSates state = buff[ind++];
        master.slave[i].slaveStates = state;
    }

    // TODO metter + logger states
    clock.state = buff[ind++];
    meter.state = buff[ind++];
    ind += 3;

    // 3. Holdings
    // Проверяем не пишется ли какой нибудь регистр
    bool res = true;
    for (size_t i = 1; i < 5; i++)
    {
      TypeDef_MB_Table * ht = holdings_table + i;
      for (size_t i = 0; i < ht->len; i++)
      {
            if(ht->holdings[i].lock || ht->holdings[i].change_req){
                res = false;
            }
      }      
    }
    
    // обновляем таблицу
    if(res) memcpy(holdings, buff+ind, 256);
    ind += 256;

    //4. часы
    memcpy(&clock.time, buff+ind , sizeof(Typedef_RtcTimeStamp));
    ind += sizeof(Typedef_RtcTimeStamp); 

    //5. метер
    Typedef_Meter * m = (Typedef_Meter *) (buff+ind);
    
    meter.Power_re = m->Power_re;
    meter.Power_im = m->Power_im;
    meter.Power_s = m->Power_s;
    memcpy(meter.I, m->I, 3);
    memcpy(meter.U, m->U, 3);
    ind += sizeof(Typedef_Meter); 

    //6. logger
    memcpy(&rec_disp, buff+ind, sizeof(Typedef_LoggerRecord)); 
    ind += sizeof(Typedef_LoggerRecord); 

    tcp_client_to = false;
}

void elink_client_err_hdl(){
    err_diag++;

    for (size_t i = 1; i < 5; i++)
    {
        /* code */
        tcp_client_to = true; 
    }
}

void elink_client_port_init(){

    ip_addr_t remote_ip;
    IP4_ADDR(&remote_ip, 192, 168, 137, 35);
    uint16_t port = 12855;

    elink_client_start(
        remote_ip, 
        /*port*/port,
        /*polling time, ms*/ 50,
        elink_mst_buff, ELINK_CLIENT_PORT_BUFF_SZ,
        ELINK_CLIENT_PORT_TX_SZ, ELINK_CLIENT_PORT_RX_SZ,
        /*snd callback*/elink_client_port_get_pld, 
        /*rcv callback*/elink_client_port_set_pld,
        /*rcv callback*/elink_client_err_hdl
  );

}

