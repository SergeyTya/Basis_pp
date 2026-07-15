#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
void ethernetif_input( void * pvParameters );

// В ethernetif.h добавьте:
typedef struct {
    uint8_t *buffer;
    uint16_t len;
} rx_frame_t;

#endif 
