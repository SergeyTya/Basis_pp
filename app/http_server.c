#include <string.h>
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/err.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

#include "task_logger.h"


#define HTTP_PORT 80

struct http_state
{
    uint32_t file_offset; // текущая позиция в отправке
    size_t content_len;

     uint8_t sent_header;  // отправлены ли заголовки?

     uint8_t done;
};

static void http_state_free(struct http_state *hs)
{
    if (hs != NULL) {
        mem_free(hs);
    }
}

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
extern const char * html_head;
extern const char * html_body;
extern const char * html_end;



size_t check_buf(struct tcp_pcb *pcb, size_t len){
    uint16_t http_sent_mss = tcp_mss(pcb);
    return LWIP_MIN(tcp_sndbuf(pcb), LWIP_MIN(http_sent_mss, len));
}


char http_dyndata[1460];

extern uint8_t GD25_ID[16];
extern uint8_t GD25_RDID[3];
extern uint8_t logger_RxBuf[256];
extern Typedef_LoggerRecord * logger_records_rx;

//extern Typedef_Logger logger;

extern volatile uint32_t logger_adr_rx;

err_t http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    struct http_state *hs = (struct http_state *)arg;
   u16_t http_sent_mss ;
   u16_t http_sent_len;
   err_t err = ERR_OK;

    if (hs == NULL) return ERR_ARG;

    while (hs->content_len !=0)
   {
    /* code */
   
    size_t content_sz = 1460;
   

    if(hs->file_offset == 0){
        
        size_t ofst = 0;
        int rc = 0;

        
        for (size_t i = 0; i < 20; i++){

            logger_records_rx = (Typedef_LoggerRecord * ) ( logger_RxBuf + (256-64) - rc*64);

            size_t remaining = sizeof(http_dyndata) - ofst;
            if (remaining == 0) break;

            int n = snprintf(&http_dyndata[ofst], remaining,
                        "<tr>"
                        "<td>%04x</td>"
                        "<td>%03d</td> "
                        "<td>%03d</td>"
                        "<td>%03d</td>"
                        "<td>%03d</td>"
                        "</tr>",
           (int) logger_records_rx->crc16,  logger_records_rx->LiveCounter, rc, Logger_checkRecordCRC16(logger_records_rx), i);

            if (n < 0) break;
            if ((size_t)n >= remaining) { /* truncated */
                ofst = sizeof(http_dyndata) - 1;
                http_dyndata[ofst] = '\0';
                break;
            }
            ofst += (size_t)n;

            rc++;

            if(rc ==4){
                rc = 0;
                if(logger_adr_rx >256)logger_adr_rx -= 256;
                vTaskDelay(50);
            }
        }
        if (ofst < sizeof(http_dyndata)) http_dyndata[ofst] = '\0';
    }



    while (hs->file_offset < content_sz - 1) {
        http_sent_mss = tcp_mss(pcb);
        int csz =  content_sz - hs->file_offset;
        http_sent_len = LWIP_MIN(tcp_sndbuf(pcb), LWIP_MIN(http_sent_mss,csz));
       
        if (http_sent_len != csz) {
            // Буфер полон — выходим, будем ждать следующего вызова
            return ERR_OK;
        }

        err = tcp_write(pcb, &http_dyndata[hs->file_offset], http_sent_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            hs->file_offset += http_sent_len;
           // return ERR_OK;
        } else if (err == ERR_MEM) {
            // Недостаточно памяти — выходим, ждём освобождения
            return ERR_OK;
        } else {
            return err;
        }
    }

        hs->content_len --;
        hs->file_offset = 0;
    }


    size_t szo = 23;
    http_sent_mss = tcp_mss(pcb);
    http_sent_len = LWIP_MIN(tcp_sndbuf(pcb), LWIP_MIN(http_sent_mss, szo));
 
    if(http_sent_len == szo){
        if(hs->done == 0)tcp_write(pcb, "</table></body></html>", szo, TCP_WRITE_FLAG_COPY);
        hs->done = 1;
        return ERR_OK;
    }

    if(hs->done == 1) {
        tcp_sent(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_abort(pcb);
        http_state_free(hs);
    }

    return ERR_OK;
}

extern SemaphoreHandle_t semaphore_MHC;

static char http_header[512];
const char  hd[] = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length:";
char lnp[12];


static err_t http_sent_header(void *arg, struct tcp_pcb *pcb, u16_t len){

    struct http_state *hs = (struct http_state *)arg;
    err_t err1 = ERR_OK;
    size_t szb;
    size_t szo;
    
    if (hs == NULL) return ERR_ARG;

    switch (hs->sent_header)
    {
    case 0:
        // Формируем заголовок (используем безопасные snprintf и проверку границ)
        {
            int offset = 0;
            int bpos = 0;
            int remaining = sizeof(http_header) - offset;
            int n = snprintf(&http_header[offset], remaining, "\r\n");
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            remaining = sizeof(http_header) - offset;
            n = snprintf(&http_header[offset], remaining, "Connection: Close\r\n");
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            remaining = sizeof(http_header) - offset;
            n = snprintf(&http_header[offset], remaining, "\r\n");
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            bpos = offset;

            remaining = sizeof(http_header) - offset;
            n = snprintf(&http_header[offset], remaining, "<html><body style='background: #72441eff;color: #fff;font-family:sans-serif'>");
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            remaining = sizeof(http_header) - offset;
            n = snprintf(&http_header[offset], remaining, "<p>FLASH RDID %02x-%02x-%02x ", GD25_RDID[0], GD25_RDID[1], GD25_RDID[2]);
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            remaining = sizeof(http_header) - offset;
            n = snprintf(&http_header[offset], remaining, " FLASH ID %02x", GD25_ID[0]);
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            for (size_t i = 0; i < 15 && offset < (int)sizeof(http_header) - 8; i++)
            {
                remaining = sizeof(http_header) - offset;
                n = snprintf(&http_header[offset], remaining, "-%02x",  GD25_ID[i+1]);
                if (n > 0 && n < remaining) offset += n; else { offset = sizeof(http_header) - 1; break; }
            }

            remaining = sizeof(http_header) - offset;
            n = snprintf(&http_header[offset], remaining, "</p>");
            if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;

            /* добавить html_head, контролируя границы */
            remaining = sizeof(http_header) - offset;
            if (remaining > 0) {
                n = snprintf(&http_header[offset], remaining, "%s", html_head);
                if (n > 0 && n < remaining) offset += n; else offset = sizeof(http_header) - 1;
            }

            int content_len = 1460 * (int)hs->content_len + (offset - bpos) + (int)strlen(html_end);
            snprintf(lnp, sizeof(lnp), "%d", content_len);
            hs->sent_header++;

            szo = strlen(hd);
            szb = check_buf(pcb, szo);
            if (szb == szo) {
                err1 = tcp_write(pcb, hd, szo, TCP_WRITE_FLAG_COPY);
                hs->sent_header++;
            } else {
                return ERR_OK;
            }
        }
        break;

    case 2:
        szo = strlen(lnp);
        szb = check_buf(pcb, szo);
        if( szb == szo ){
            err1 = tcp_write(pcb, lnp, strlen(lnp), TCP_WRITE_FLAG_COPY);
            hs->sent_header++;
        }else{
            return ERR_OK;
        }
        break;

    case 3:
        szo = strlen(http_header);
        szb = check_buf(pcb, szo);
        if( szb == szo ){
            err1 = tcp_write(pcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);
            hs->sent_header++;
        }else{
            return ERR_OK;
        }
        break;
    
    default:
        tcp_sent(pcb, http_sent);     // Устанавливаем callback для отправки тела
        http_sent(arg, pcb, 0);       // Начинаем отправку
        break;
    }
       
    return ERR_OK;
}

/**
 * @brief Callback for receiving HTTP requests
 */
static err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct http_state *hs = (struct http_state *)arg;

    if (hs == NULL) return ERR_ARG;

    // Инициализируем
    hs->file_offset = 0;
    hs->sent_header = 0;
    hs->content_len = 0;
    hs->done = 0;
   // logger_adr_rx = logger.adr_cnt;

    if (p == NULL) {
        // Соединение закрыто клиентом
        tcp_sent(pcb, NULL);
        http_state_free(hs);
        tcp_close(pcb);
        return ERR_OK;
    }

    // Освобождаем полученные данные (нам не нужно их читать)
    pbuf_free(p);

    hs->content_len  = 10;
    tcp_sent(pcb, http_sent_header);
    http_sent_header(arg, pcb, 0);

    return ERR_OK;
}

/**
 * @brief Callback for handling incoming TCP connections
 */
static err_t http_accept(void *arg, struct tcp_pcb *new_pcb, err_t err)
{
    struct http_state *hs;

    if (err != ERR_OK || new_pcb == NULL)
    {
        return ERR_VAL;
    }

    // Выделяем состояние для соединения
    hs = (struct http_state *)mem_malloc(sizeof(struct http_state));
    if (hs == NULL) {
        tcp_abort(new_pcb);
        return ERR_MEM;
    }

    hs->file_offset = 0;
    hs->sent_header = 0;
    hs->content_len = 0;
    hs->done = 0;

    // Настраиваем соединение
    tcp_arg(new_pcb, hs);
    tcp_recv(new_pcb, http_recv);
    tcp_err(new_pcb, NULL); // можно добавить обработчик ошибок

    return ERR_OK;
}


/**
 * @brief Initialize HTTP server
 */
struct tcp_pcb * http_pcb; 
void http_server_init(void)
{
    // Create TCP PCB
    http_pcb = tcp_new();
    if (!http_pcb)
    {
        return; // Failed to create PCB
    }

    // Bind to port 80
    err_t err = tcp_bind(http_pcb, IP_ADDR_ANY, HTTP_PORT);
    if (err != ERR_OK)
    {
        tcp_close(http_pcb);
        return;
    }

    // Start listening
    http_pcb = tcp_listen(http_pcb);
    if (!http_pcb)
    {
        return;
    }

    // Set accept callback
    tcp_accept(http_pcb, http_accept);
}