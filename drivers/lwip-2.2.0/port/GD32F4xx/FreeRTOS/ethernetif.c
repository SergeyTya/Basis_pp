/**
* @file
* Ethernet Interface Skeleton
*
*/

/*


    НАСТРОЙКА lwip для режима NO_SYS=0 и встроенного  tcp_thread
    
    #define LWIP_TCPIP_CORE_LOCKING 0
    #define SYS_LIGHTWEIGHT_PROT    0  

    lwiopts.h - добавить для запуска встроенного потока 

    #define TCPIP_MBOX_SIZE           16 
    #define TCPIP_THREAD_NAME         "tcpip"
    #define TCPIP_THREAD_STACKSIZE    1024
    #define TCPIP_THREAD_PRIO         (7 - 3)

    в netconf.c смотри измененный lwip_stack_init

*
*/

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "err.h"
#include "ethernetif.h"

#include "main.h"
#include "gd32f4xx_enet.h"
#include <string.h>
#include "semphr.h"


#define ETHERNETIF_INPUT_TASK_STACK_SIZE          (200)
#define ETHERNETIF_INPUT_TASK_PRIO                (configMAX_PRIORITIES - 3)
/* The time to block waiting for input */

/* define those to better describe your network interface */
#define IFNAME0 'G'
#define IFNAME1 'D'

/* ENET RxDMA/TxDMA descriptor */
extern enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM], txdesc_tab[ENET_TXBUF_NUM];

/* ENET receive buffer  */
extern uint8_t rx_buff[ENET_RXBUF_NUM][ENET_RXBUF_SIZE]; 

/* ENET transmit buffer */
extern uint8_t tx_buff[ENET_TXBUF_NUM][ENET_TXBUF_SIZE]; 

/*global transmit and receive descriptors pointers */
extern enet_descriptors_struct  *dma_current_txdesc;
extern enet_descriptors_struct  *dma_current_rxdesc;

/* preserve another ENET RxDMA/TxDMA ptp descriptor for normal mode */
enet_descriptors_struct  ptp_txstructure[ENET_TXBUF_NUM];
enet_descriptors_struct  ptp_rxstructure[ENET_RXBUF_NUM];


static struct netif *low_netif = NULL;
QueueHandle_t g_rx_message_queue;
QueueHandle_t g_rx_pbuf_queue;

static void ethernetif_input_task(void *arg);

/**
* In this function, the hardware should be initialized.
* Called from ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void low_level_init(struct netif *netif)
{
    uint32_t i;
    low_netif =netif;

    /* set netif MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set netif MAC hardware address */
    netif->hwaddr[0] =  MAC_ADDR0;
    netif->hwaddr[1] =  MAC_ADDR1;
    netif->hwaddr[2] =  MAC_ADDR2;
    netif->hwaddr[3] =  MAC_ADDR3;
    netif->hwaddr[4] =  MAC_ADDR4;
    netif->hwaddr[5] =  MAC_ADDR5;

    /* set netif maximum transfer unit */
    netif->mtu = 1500;

    /* accept broadcast address and ARP traffic */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;


    /* initialize MAC address in ethernet MAC */ 
    enet_mac_address_set(ENET_MAC_ADDRESS0, netif->hwaddr);
  

    /* initialize descriptors list: chain/ring mode */
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_ptp_enhanced_descriptors_chain_init(ENET_DMA_TX);
    enet_ptp_enhanced_descriptors_chain_init(ENET_DMA_RX);
#else

    // enet_descriptors_chain_init(ENET_DMA_TX);
    // enet_descriptors_chain_init(ENET_DMA_RX);
    
   enet_descriptors_ring_init(ENET_DMA_TX);
   enet_descriptors_ring_init(ENET_DMA_RX);  
    
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */  

    /* enable ethernet Rx interrrupt */
    {   int i;
        for(i=0; i<ENET_RXBUF_NUM; i++){ 
            enet_rx_desc_immediate_receive_complete_interrupt(&rxdesc_tab[i]);
            //enet_rx_desc_delay_receive_complete_interrupt(&rxdesc_tab[i], 1);
            
        }
    }

    enet_flowcontrol_feature_enable(ENET_ZERO_QUANTA_PAUSE);
   


#ifdef CHECKSUM_BY_HARDWARE
    /* enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for(i=0; i < ENET_TXBUF_NUM; i++){
        enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
    }
#endif /* CHECKSUM_BY_HARDWARE */

    /* enable MAC and DMA transmission and reception */

    g_rx_message_queue = xQueueCreate(10, sizeof(rx_frame_t));

    xTaskCreate(ethernetif_input_task, "ETHERNETIF_INPUT", ETHERNETIF_INPUT_TASK_STACK_SIZE, NULL,
                ETHERNETIF_INPUT_TASK_PRIO,NULL);

    enet_enable();  

}


/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    uint8_t *buffer ;
    uint16_t framelength = 0;
    ErrStatus reval = ERROR;
  
   //SYS_ARCH_DECL_PROTECT(sr);

    // ✅ Ждём, пока DMA освободит дескриптор — но с проверкой на зависание (например, timeout)
    // Можно добавить watchdog или таймер, но для простоты — бесконечный wait:
    while (RESET != (dma_current_txdesc->status & ENET_TDES0_DAV)) {
        // Optional: можно добавить тик-задержку, если ISR не сработала
    }

    buffer = (uint8_t *)(enet_desc_information_get(dma_current_txdesc, TXDESC_BUFFER_1_ADDR));

    for (q = p; q != NULL; q = q->next) {
        memcpy(&buffer[framelength], q->payload, q->len);
        framelength += q->len;
    }

    // Отправка
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    reval = ENET_NOCOPY_PTPFRAME_TRANSMIT_ENHANCED_MODE(framelength, NULL);
#else
    reval = ENET_NOCOPY_FRAME_TRANSMIT(framelength);
#endif

    // ✅ Возвращаем ошибку, а не зависаем:
    if (SUCCESS == reval) {
        return ERR_OK;
    } else {
        return ERR_IF;  // вместо while(1)
    }
    
}


/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

#include "lwip/tcpip.h"



//сообщает о пакете
void ENET_IRQHandler(void)
{
    enet_interrupt_flag_clear(ENET_DMA_INT_FLAG_RS_CLR);
    enet_interrupt_flag_clear(ENET_DMA_INT_FLAG_NI_CLR);
    
   // enet_txfifo_flush();

    gpio_bit_toggle(GPIOG,GPIO_PIN_6);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  while(1){
    uint32_t size = enet_rxframe_size_get();
    if(size == 0) break;

    rx_frame_t frame;
    frame.buffer = (uint8_t *)enet_desc_information_get(dma_current_rxdesc, RXDESC_BUFFER_1_ADDR);
    frame.len = enet_desc_information_get(dma_current_rxdesc, RXDESC_FRAME_LENGTH);

  

    if (!xQueueSendFromISR(g_rx_message_queue, &frame, &xHigherPriorityTaskWoken)) {
              
    }

    ENET_NOCOPY_FRAME_RECEIVE();
     
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   

     

}

#ifdef USE_ENET_INTERRUPT
// передает данные в стек
static void ethernetif_input_task(void *arg)
{
   rx_frame_t frame; 
    for(;;) {
        if (xQueueReceive(g_rx_message_queue, &frame, portMAX_DELAY) == pdTRUE) {
            if (frame.len == 0) continue;  // безопасность

            struct pbuf *p = pbuf_alloc(PBUF_RAW, frame.len, PBUF_POOL);
            if (p != NULL) {
                memcpy(p->payload, frame.buffer, frame.len);
                //tcpip_callback(tcpip_input_callback, p);
               err_t ret = tcpip_input(p, low_netif);  // ← прямо здесь!
               if (ret != ERR_OK) {
                 if(p!=NULL) pbuf_free(p);  // если tcpip_input() не забрал pbuf, освобождаем сами
                    // Можно: g_tcpip_input_errors++; // счётчик ошибок для отладки
                }
            }
            ENET_NOCOPY_FRAME_RECEIVE();
        }
    }
}
#else
static void ethernetif_input_task(void *arg){

    struct pbuf *p;
    u16_t len;
    uint8_t *buffer;
   
    while(1){

        if(enet_rxframe_size_get()) {

           
            p = NULL;
    
            /* obtain the size of the packet and put it into the "len" variable. */
            len = enet_desc_information_get(dma_current_rxdesc, RXDESC_FRAME_LENGTH);
            buffer = (uint8_t *)(enet_desc_information_get(dma_current_rxdesc, RXDESC_BUFFER_1_ADDR));
    
            /* we allocate a pbuf chain of pbufs from the Lwip buffer pool */
            p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

            if (p != NULL) {
               memcpy(p->payload, buffer, len);
                //tcpip_callback(tcpip_input_callback, p);
               tcpip_input(p, low_netif);  // ← прямо здесь!
            }
            ENET_NOCOPY_FRAME_RECEIVE();

             gpio_bit_toggle(GPIOG,GPIO_PIN_6);

        }
        
        vTaskDelay(2);
    }
}

#endif


