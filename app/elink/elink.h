#ifndef LINK_MASTER_H
#define LINK_MASTER_H

#include <stdbool.h> 
#include <stdint.h>
#include "lwip/tcp.h"

// Master waiting for connection
void elink_server_start();
void elink_server_get_pld(uint8_t * buf_rx, size_t szrx, uint8_t * buf_tx, uint8_t sztx );

// Client connect to remote IP send request and waiting for response
void elink_client_start(
    ip_addr_t remote_ip,   //Remote IP
    uint16_t remote_port,  //Remote port
    uint16_t polling_time, // Polling time
    uint8_t * buff,  // rx/tx buffer pointer
    size_t buf_sz,   // rx/tx buffer size 
    size_t tx_len,   // byte len to transmit
    size_t rx_len,   // expected byte to rcv
    void (* set_pld)(uint8_t*),  // function to set payload to send
    void (* get_pld)(uint8_t*),  // function to get response from pld
    void (* err_hdl)()           // error callback
);

// Client call back function
void elink_client_port_get_pld(uint8_t *);
void elink_client_port_set_pld(uint8_t *);
void elink_client_err_hdl();

void elink_client_port_init();




#endif // LINK_MASTER_H