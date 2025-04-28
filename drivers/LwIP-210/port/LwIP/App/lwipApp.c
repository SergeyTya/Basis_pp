#include "lwipApp.h"


#include "ethernetHwInit.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "netconf.h"

uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];

void init_ether(){

    enet_system_setup();
    lwip_stack_init();

}

