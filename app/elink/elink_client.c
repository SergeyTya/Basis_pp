#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "timeouts.h"

// ================= CONFIGURATION =================
#define REMOTE_PORT            12850
#define CLIENT_TASK_PRIORITY   (tskIDLE_PRIORITY + 3U)
#define CLIENT_TASK_STACK_SIZE (5*1024)
#define SENT_RCV_TIMEOUT_MS    500  // 👈 таймаут ответа (5 секунд)
#define RECONNECT_DELAY_MS     30

// Указать свой IP-адрес сервера

#define EXPECTED_RESPONSE_LEN  client.rx_len
#define PAYLOAD_SIZE           client.tx_len


//static uint8_t payload[PAYLOAD_SIZE];

// ================= STATE =================
typedef enum {
    CLIENT_STATE_IDLE,
    CLIENT_STATE_CONNECTING,
    CLIENT_STATE_CONNECTED,
    CLIENT_STATE_SENDING,       // новые: отправка данных
    CLIENT_STATE_RECEIVING,     // новые: ожидание ответа
    CLIENT_STATE_RECEIVED,     // новые: ожидание ответа
    CLIENT_STATE_CLOSING
} client_state_t;

struct client_ctx {
    struct tcp_pcb *pcb;
    ip_addr_t remote_ip;
    u16_t remote_port;
    uint16_t polling_time;
    client_state_t state;
    int remaining_to_send;
    int bytes_received;
    TickType_t last_sent_time;
    uint8_t * buff; size_t buf_sz; // input buffer defenition
    size_t tx_len; size_t rx_len;    // expected snd rcv len
    void (* set_pld)(uint8_t*); // function to set payload to send
    void (* get_pld)(uint8_t*);  // function to get response from pld
    void (* err_hdl)();
};


static struct client_ctx client = {.pcb=NULL, .state=CLIENT_STATE_IDLE, .buff=NULL};

volatile int rcv_err = 0;

// ================= CALLBACKS =================
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t tcp_client_send(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_client_err(void *arg, err_t err);
static void client_connect(void);

// ================= Хэндлеры таймеров =================
static void tcp_recv_timeout_handler(void *arg) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    LWIP_ASSERT("ctx != NULL", ctx != NULL);

    if (
        ctx->state == CLIENT_STATE_RECEIVING
    ) {
           
        if (ctx->pcb) {
            err_t err = tcp_close(ctx->pcb);
            if(err!=ERR_OK){
                tcp_abort(ctx->pcb);
            }
            ctx->pcb = NULL;
        }

         ctx->state = CLIENT_STATE_IDLE;
    }

    rcv_err++;
}

static void tcp_send_timeout_handler(void *arg) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    LWIP_ASSERT("ctx != NULL", ctx != NULL);

    if (
        ctx->state == CLIENT_STATE_SENDING
    ) {
           
        if (ctx->pcb) {
            err_t err = tcp_close(ctx->pcb);
            if(err!=ERR_OK){
                tcp_abort(ctx->pcb);
            }
            ctx->pcb = NULL;
        }

         ctx->state = CLIENT_STATE_IDLE;
    }
}

static void tcp_connect_timeout_handler(void *arg) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    LWIP_ASSERT("ctx != NULL", ctx != NULL);

    if (
        ctx->state == CLIENT_STATE_CONNECTING
    ) {
           
        if (ctx->pcb) {
            err_t err = tcp_close(ctx->pcb);
            if(err!=ERR_OK){
                tcp_abort(ctx->pcb);
            }
            ctx->pcb = NULL;
        }

        ctx->state = CLIENT_STATE_IDLE;
    }

}

// 🚀 Подключение
static void client_connect(void) {
    client.state = CLIENT_STATE_CONNECTING;
    if (client.pcb != NULL) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("client_connect: pcb already exists\n"));
        tcp_close(client.pcb);
        client.pcb = NULL;
        client.state = CLIENT_STATE_IDLE;
        return;
    }

    client.pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!client.pcb) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("client_connect: tcp_new failed\n"));
        tcp_close(client.pcb);
        client.pcb = NULL;
        client.state = CLIENT_STATE_IDLE;
        return;
    }

    tcp_arg(client.pcb, &client);
    tcp_sent(client.pcb, NULL);
    tcp_recv(client.pcb, tcp_client_recv); 
    tcp_err(client.pcb, tcp_client_err);
    
    client.bytes_received = 0;

    err_t err = tcp_connect(client.pcb, &client.remote_ip, client.remote_port, tcp_client_connected);
    if (err != ERR_OK) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("tcp_connect failed: %d\n", err));
        tcp_close(client.pcb);
        client.pcb = NULL;
        client.state = CLIENT_STATE_IDLE;
    }

    // Таймаут подключения
    sys_untimeout(tcp_connect_timeout_handler, &client);
    sys_timeout(SENT_RCV_TIMEOUT_MS, tcp_connect_timeout_handler, &client); 
}

// ✅ Подключено
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    client.state = CLIENT_STATE_CONNECTED;
    // Сброс таймаута подключения
    sys_untimeout(tcp_connect_timeout_handler, &ctx);
    return ERR_OK;
}

volatile int sent_cnt=0;
// 📤 sent callback — отправляет данные, затем переходит к приёму
static err_t tcp_client_send(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    struct client_ctx *ctx = (struct client_ctx *)arg;

    sent_cnt++;

    if(ctx->state == CLIENT_STATE_CONNECTED){
        ctx->state = CLIENT_STATE_SENDING;
        ctx->bytes_received = 0;
    }

    
    while (
        tcp_sndbuf(tpcb) >= PAYLOAD_SIZE 
        && 
        ctx->remaining_to_send > 0) 
    {
        size_t to_send = PAYLOAD_SIZE;
        if ((int)to_send > ctx->remaining_to_send) {
            to_send = ctx->remaining_to_send;
        }
        err_t err = tcp_write(tpcb, ctx->buff, to_send, TCP_WRITE_FLAG_COPY);
        
        if (err != ERR_OK) {
            return err; 
        }

        tcp_output(tpcb);
       
        if (err != ERR_OK) {
            return err; 
        }

        ctx->remaining_to_send -= (int)to_send;     
    }

    // while (1) {
    // u16_t free_space = tcp_sndbuf(tpcb);  // ← БЕЗОПАСНО: проверяем "на лету"
    // if (free_space == 0 || ctx->remaining_to_send <= 0) break;

    // size_t to_send = (free_space >= PAYLOAD_SIZE) ? PAYLOAD_SIZE : free_space;
    // if (to_send > ctx->remaining_to_send) to_send = ctx->remaining_to_send;

    // err_t err = tcp_write(tpcb, payload, to_send, TCP_WRITE_FLAG_COPY);
    // if (err != ERR_OK) {
    //     //LWIP_DEBUGF(LWIP_DBG_ON, ("tcp_write failed: err=%d, free_space=%d\n", err, free_space));
    //     break;
    // }

    // ctx->remaining_to_send -= (int)to_send;
    // }

    tcp_output(tpcb);


    if (
        ctx->remaining_to_send <= 0
    ) {
        // Все данные отправлены — ожидаем ответ
        ctx->state = CLIENT_STATE_RECEIVING;
        ctx->last_sent_time = xTaskGetTickCount();
        // сброс таймаута посылки
        sys_untimeout(tcp_send_timeout_handler, ctx);
        //⏳ таймер таймаута приема
        sys_untimeout(tcp_recv_timeout_handler, ctx);
        sys_timeout(SENT_RCV_TIMEOUT_MS, tcp_recv_timeout_handler, ctx);
        // сброс колбэка
        
    }else{
        
    }

    return ERR_OK;
}

// ✅ recv callback — приём данных от сервера
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    LWIP_ASSERT("ctx != NULL", ctx != NULL);

    if (err != ERR_OK || p == NULL) {
        if (p != NULL) {
            pbuf_free(p); // в случае ошибки
        }
        if (err == ERR_CLSD) {
            ctx->state = CLIENT_STATE_CLOSING;
        }
        return ERR_OK;
    }

    u16_t bytes_to_copy = p->tot_len;
    if (ctx->bytes_received + bytes_to_copy > (int) EXPECTED_RESPONSE_LEN) {
        bytes_to_copy = EXPECTED_RESPONSE_LEN - ctx->bytes_received;
    }
    if (bytes_to_copy > 0) {
        u16_t copied = pbuf_copy_partial(p, ctx->buff + ctx->bytes_received, bytes_to_copy, 0);
        LWIP_ASSERT("copied == bytes_to_copy", copied == bytes_to_copy);
        ctx->bytes_received += copied;
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p); // обязательно — иначе утечка памяти

    if (ctx->bytes_received >= (int) EXPECTED_RESPONSE_LEN) {
        ctx->state = CLIENT_STATE_RECEIVED;
         // Останавливаем таймаут приёма
        sys_untimeout(tcp_recv_timeout_handler, ctx);
    }

    return ERR_OK;
}

// error callback
static err_t tcp_client_err(void *arg, err_t err) {
    struct client_ctx *ctx = (struct client_ctx *)arg;
    LWIP_ASSERT("ctx != NULL", ctx != NULL);

    if (err != ERR_ABRT) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("tcp_client_err: %d\n", err));
    }

    if (ctx->pcb != NULL) {
            sys_untimeout(tcp_recv_timeout_handler, ctx);
        ctx->pcb = NULL;
    }
    ctx->state = CLIENT_STATE_IDLE;
    return ERR_OK;
}

// ================= FreeRTOS TASK =================
volatile int poll_cnt =0;
static void client_task(void *arg) {
    LWIP_UNUSED_ARG(arg);

    while (1) {
        
        while(
            (client.state != CLIENT_STATE_IDLE)
         && (client.state != CLIENT_STATE_RECEIVED)
         && (client.state != CLIENT_STATE_CONNECTED)
        ){
            vTaskDelay(pdMS_TO_TICKS(1)); // small delay to let tcp_connect start
        }
        
        switch (client.state)
        {
        case CLIENT_STATE_IDLE:
            /* New conection*/
            if(client.err_hdl!=NULL) client.err_hdl();
            vTaskDelay(pdMS_TO_TICKS(SENT_RCV_TIMEOUT_MS*2));
            client_connect();
            break;

        case CLIENT_STATE_RECEIVED:
        case CLIENT_STATE_CONNECTED:
            vTaskDelay(pdMS_TO_TICKS(client.polling_time));
            struct client_ctx *ctx = &client;
        
            if(client.state == CLIENT_STATE_RECEIVED) {
                /* Read input data here */
                 if(ctx->get_pld!=NULL) ctx->get_pld(ctx->buff);
            }

             /* Prepare output data here */
            if(ctx->set_pld!=NULL) ctx->set_pld(ctx->buff);
            
            /*Send new request*/
            
            ctx->remaining_to_send = PAYLOAD_SIZE;
            ctx->bytes_received = 0;
            sys_untimeout(tcp_send_timeout_handler, ctx);
            sys_timeout(SENT_RCV_TIMEOUT_MS, tcp_send_timeout_handler, ctx);
            tcp_client_send(ctx, ctx->pcb, ctx->remaining_to_send);
            poll_cnt++;

            break;
        
        default:
            break;
        }

       
    }
}

// Public init function
void elink_client_start(
    ip_addr_t remote_ip,
    uint16_t remote_port,
    uint16_t polling_time,
    uint8_t * buff, size_t buf_sz, // input buffer defenition
    size_t tx_len, size_t rx_len,    // expected snd rcv len
    void (* set_pld)(uint8_t*), // function to set payload to send
    void (* get_pld)(uint8_t*),  // function to get response from pld
    void (* err_hdl)()
) 
{
    client.polling_time = polling_time;
    client.remote_ip = remote_ip;
    client.remote_port = remote_port;
    client.buff = buff;
    client.buf_sz = buf_sz;
    client.tx_len = tx_len;
    client.rx_len = rx_len;
    client.set_pld = set_pld;
    client.get_pld = get_pld;
    client.err_hdl = err_hdl;
    
    xTaskCreate(
        client_task,
        "tcp_client",
        CLIENT_TASK_STACK_SIZE,
        NULL,
        CLIENT_TASK_PRIORITY,
        NULL
    );
}