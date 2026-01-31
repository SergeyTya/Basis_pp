#include <string.h>
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/err.h"

// HTTP server port (HTTP = 80)
#define HTTP_PORT 80

//static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

// Simple HTTP response
// static const char *http_response =
//     "HTTP/1.1 200 OK\r\n"
//     "Content-Type: text/html\r\n"
//     "Content-Length: 57\r\n"
//     "\r\n"
//     "Welcome to <a href=\"https://bazisaero.ru\">Bazis Aero</a> power source!";

//extern const char *http_response;

// HTML-страница (пример большого контента)
static const char html_content[] =
"<html><head><title>LwIP HTTP Server</title></head><body>"
"<h1>Large Page from LwIP</h1>"
"<p>This is a very long HTML page to test chunked sending.</p>"
"<p>Line 1</p><p>Line 2</p><p>Line 3</p>"
// Добавим много строк, чтобы превысить MSS
#define LINE "<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut imperdiet ex nec dolor fermentum, vel tincidunt nisl ultricies. </p>\r\n"
LINE 
LINE 
LINE 
LINE 
LINE
LINE 
LINE 
LINE 
LINE 
LINE
"--------10-------------"
LINE 
LINE
LINE
LINE 
LINE 
LINE 
LINE 
LINE 
LINE 
LINE
"--------20-------------"
LINE 
LINE 
LINE
LINE 
LINE 
LINE 
LINE 
LINE
LINE 
LINE
"--------30-------------"

"<p>End of content.</p></body></html>";

// HTTP-заголовок с динамическим Content-Length
static char http_header[256];

// Состояние соединения
struct http_state {
    uint32_t file_offset;   // текущая позиция в отправке
    uint8_t sent_header;    // отправлены ли заголовки?
    uint8_t sent_done;    // отправлены ли заголовки?
};

// Освобождение состояния
void http_state_free(struct http_state *hs) {
    if (hs != NULL) {
        mem_free(hs);
    }
}

err_t http_close(void *arg,struct tcp_pcb *pcb, struct pbuf * buf, int8_t len) {
    tcp_close(pcb);
    tcp_recv(pcb, NULL);
    return ERR_OK;
}

err_t http_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
    struct http_state *hs = (struct http_state *)arg;
    static u16_t http_sent_mss;
    static u16_t http_sent_len;

    // Отправляем тело HTML
    while (hs->file_offset < sizeof(html_content) - 1) {
        http_sent_mss = tcp_mss(pcb);
        http_sent_len = LWIP_MIN(tcp_sndbuf(pcb), LWIP_MIN(http_sent_mss, sizeof(html_content) - hs->file_offset));
        err_t err;

        if (http_sent_len == 0) {
            // Буфер полон — выходим, будем ждать следующего вызова
            return ERR_OK;
        }

        err = tcp_write(pcb, &html_content[hs->file_offset], http_sent_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            hs->file_offset += http_sent_len;
        } else if (err == ERR_MEM) {
            // Недостаточно памяти — выходим, ждём освобождения
            return ERR_OK;
        } else {
            return err;
        }

    }


    tcp_sent(pcb, NULL);
    tcp_recv(pcb, http_close);
    http_state_free(hs);
  //  tcp_abort(pcb);
    return ERR_OK;
}


/**
 * @brief Callback for receiving HTTP requests
 */
static err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
  struct http_state *hs = (struct http_state *)arg;

    if (p == NULL) {
        // Соединение закрыто клиентом
        tcp_sent(pcb, NULL);
        http_state_free(hs);
        tcp_close(pcb);
        return ERR_OK;
    }

    // Освобождаем полученные данные (нам не нужно их читать)
    pbuf_free(p);

    // Генерируем заголовок
    int content_len = sizeof(html_content) - 1;

    // Формируем заголовок
    int offset = 0;
    offset += sprintf(&http_header[offset], "HTTP/1.1 200 OK\r\n");
    offset += sprintf(&http_header[offset], "Content-Type: text/html\r\n");
    offset += sprintf(&http_header[offset], "Content-Length: %d\r\n", content_len);
    offset += sprintf(&http_header[offset], "Connection: Close\r\n");
    offset += sprintf(&http_header[offset], "\r\n");

    // Отправляем заголовок
    err_t err1 = tcp_write(pcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);
    if (err1 == ERR_OK) {
        hs->sent_header = 1;
        tcp_sent(pcb, http_sent);     // Устанавливаем callback для отправки тела
        http_sent(arg, pcb, 0);       // Начинаем отправку
    }
    return ERR_OK;
}

/**
 * @brief Callback for handling incoming TCP connections
 */
static err_t http_accept(void *arg, struct tcp_pcb *new_pcb, err_t err) {
 static struct http_state *hs;

    if (err != ERR_OK || new_pcb == NULL) {
        return ERR_VAL;
    }

    // Выделяем состояние
    hs = (struct http_state *)mem_malloc(sizeof(struct http_state));
    if (hs == NULL) {
        tcp_abort(new_pcb);
        return ERR_MEM;
    }

    // Инициализируем
    hs->file_offset = 0;
    hs->sent_header = 0;
    hs->sent_done = 0;

    // Настраиваем соединение
    tcp_arg(new_pcb, hs);
    tcp_recv(new_pcb, http_recv);
    tcp_err(new_pcb, NULL); // можно добавить обработчик ошибок

    return ERR_OK;

}

/**
 * @brief Initialize HTTP server
 */
void http_server_init(void) {
    // Create TCP PCB
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        return; // Failed to create PCB
    }

    // Bind to port 80
    err_t err = tcp_bind(pcb, IP_ADDR_ANY, HTTP_PORT);
    if (err != ERR_OK) {
        tcp_close(pcb);
        return;
    }

    // Start listening
    pcb = tcp_listen(pcb);
    if (!pcb) {
        return;
    }

    // Set accept callback
    tcp_accept(pcb, http_accept);
}