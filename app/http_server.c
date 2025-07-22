#include <string.h>
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/err.h"

// HTTP server port (HTTP = 80)
#define HTTP_PORT 80

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

// Simple HTTP response
// static const char *http_response =
//     "HTTP/1.1 200 OK\r\n"
//     "Content-Type: text/html\r\n"
//     "Content-Length: 57\r\n"
//     "\r\n"
//     "Content-Length: 64\r\n"
//     "\r\n"
//     "Welcome to <a href=\"https://bazisaero.ru\">Bazis Aero</a> power source!";

extern const char *http_response;

/**
 * @brief Callback for handling incoming TCP connections
 */
static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    // Set callback for receiving data
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

/**
 * @brief Callback for receiving HTTP requests
 */
static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    LWIP_UNUSED_ARG(arg);

    if (err != ERR_OK) {
        return err;
    }


    if (p != NULL) {
        // Send HTTP response

        // Allocate response buffer
        struct pbuf *resp = pbuf_alloc(PBUF_TRANSPORT, strlen(http_response), PBUF_RAM);
        
        if (resp) {
            memcpy(resp->payload, http_response, strlen(http_response));
            tcp_write(tpcb, resp->payload, resp->len, TCP_WRITE_FLAG_COPY);
            pbuf_free(resp);
        }else{
            // Buffer allocate
            pbuf_free(p);
            tcp_abort(tpcb);
            return ERR_MEM;
        }

        // Close connection after response
     //   tcp_close(tpcb);
    }else{
        // End of stream
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Free received buffer
    if (p != NULL) {
        pbuf_free(p);
    }
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