#include <string.h>
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/err.h"

#define HTTP_PORT 80

struct http_state
{
    uint32_t file_offset; // текущая позиция в отправке
    uint8_t sent_header;  // отправлены ли заголовки?
    size_t content_len;
};

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
extern const char * html_head;
extern const char * html_body;
extern const char * html_end;
static char http_header[256];

void http_state_free(struct http_state *hs)
{
    if (hs != NULL)
    {
        mem_free(hs);
    }
}

err_t http_close(void *arg, struct tcp_pcb *pcb, struct pbuf *buf, int8_t len)
{
    tcp_close(pcb);
    tcp_recv(pcb, NULL);
    return ERR_OK;
}


char http_dyndata[1460];
err_t http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{

   struct http_state *hs = (struct http_state *)arg;
   u16_t http_sent_mss ;
   u16_t http_sent_len;

   // char *data = (char *)mem_malloc(1460);
  //  if(data ==  NULL) return ERR_MEM;

  //  while (hs->file_offset < hs->content_len - 1)

    size_t content_sz = 1460;

    while (hs->file_offset < content_sz - 1) {
        http_sent_mss = tcp_mss(pcb);
        http_sent_len = LWIP_MIN(tcp_sndbuf(pcb), LWIP_MIN(http_sent_mss, content_sz - hs->file_offset));
        err_t err;

        if (http_sent_len == 0) {
            // Буфер полон — выходим, будем ждать следующего вызова
            return ERR_OK;
        }

        err = tcp_write(pcb, &http_dyndata[hs->file_offset], http_sent_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            hs->file_offset += http_sent_len;
        } else if (err == ERR_MEM) {
            // Недостаточно памяти — выходим, ждём освобождения
            return ERR_OK;
        } else {
            return err;
        }
    }

    
   // tcp_write(pcb, html_end, strlen(html_end), TCP_WRITE_FLAG_COPY);

    tcp_sent(pcb, NULL);
    //tcp_recv(pcb, http_close);
    http_state_free(hs);
    //  tcp_abort(pcb);
    return ERR_OK;
}

/**
 * @brief Callback for receiving HTTP requests
 */
static err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
      struct http_state *hs = (struct http_state *)arg;

    if (err != ERR_OK)
    {
        return err;
    }

    if (p != NULL)
    {
        // Send HTTP response

        // Освобождаем полученные данные (нам не нужно их читать)
        pbuf_free(p);

        hs->content_len = 1;
        // Генерируем заголовок
        int content_len = 1460*hs->content_len + strlen(html_head) + strlen(html_end);

        // Формируем заголовок
        int offset = 0;
        offset += sprintf(&http_header[offset], "HTTP/1.1 200 OK\r\n");
        offset += sprintf(&http_header[offset], "Content-Type: text/html\r\n");
        offset += sprintf(&http_header[offset], "Content-Length: %d\r\n", content_len);
        offset += sprintf(&http_header[offset], "Connection: Close\r\n");
        offset += sprintf(&http_header[offset], "\r\n");

        // Отправляем заголовок
        err_t err1 = tcp_write(pcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);
        if (err1 == ERR_OK)
        {
            err_t err1 = tcp_write(pcb, html_head, strlen(html_head), TCP_WRITE_FLAG_COPY);
            if (err1 == ERR_OK){
                hs->sent_header = 1;


            for (size_t i = 0; i < 20; i++){

                snprintf(&http_dyndata[i*73], 73,
                        "<tr >"
                        "<td>%5d</td>"
                        "<td>%3d</td> "
                        "<td>%3d</td>"
                        "<td>%3d</td>"
                        "<td>%3d</td>"
                        "< /tr>"
                        ,
                65000, 100, 101, i, hs->content_len
                );
                /* code */
            }

                tcp_sent(pcb, http_sent); // Устанавливаем callback для отправки тела
                http_sent(arg, pcb, 0);   // Начинаем отправку
            }

        }
    }
    else
    {
        // End of stream
        tcp_close(pcb);
        return ERR_OK;
    }

    // Free received buffer
    if (p != NULL)
    {
        pbuf_free(p);
    }
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

    // Выделяем состояние
    hs = (struct http_state *)mem_malloc(sizeof(struct http_state));
    if (hs == NULL)
    {
        tcp_abort(new_pcb);
        return ERR_MEM;
    }

    // Инициализируем
    hs->file_offset = 0;
    hs->sent_header = 0;
    hs->content_len = 0;

    // Настраиваем соединение
    tcp_arg(new_pcb, hs);
    tcp_recv(new_pcb, http_recv);
    tcp_err(new_pcb, NULL); // можно добавить обработчик ошибок

    return ERR_OK;
}


/**
 * @brief Initialize HTTP server
 */
void http_server_init(void)
{
    // Create TCP PCB
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        return; // Failed to create PCB
    }

    // Bind to port 80
    err_t err = tcp_bind(pcb, IP_ADDR_ANY, HTTP_PORT);
    if (err != ERR_OK)
    {
        tcp_close(pcb);
        return;
    }

    // Start listening
    pcb = tcp_listen(pcb);
    if (!pcb)
    {
        return;
    }

    // Set accept callback
    tcp_accept(pcb, http_accept);
}