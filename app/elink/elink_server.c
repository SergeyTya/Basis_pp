#include <string.h>
#include <stdbool.h>

#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/err.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "elink.h"

/* Request data from power module to PLC. Awaiting ELINK_SERVER_RX_SZ bytes from power module, sendback ELINK_SERVER_RX_SZ byte to power module */

// --- В начало файла: структура клиента ---
#define ELINK_MAX_CLIENTS 1
#define ELINK_CLIENT_TIMEOUT_MS 150
#define ELINK_SERVER_PORT 12855
#define ELINK_SERVER_RX_SZ 100
#define ELINK_SERVER_TX_SZ 500

// --- Разрешённые IP-адреса (в порядке приоритета: индекс 0, 1) ---
static const ip_addr_t allowed_ips[ELINK_MAX_CLIENTS] = {
    IPADDR4_INIT_BYTES(192, 168, 137, 34)   // IP #2
};

typedef struct {
    struct tcp_pcb *pcb;
    uint8_t rx_buffer[ELINK_SERVER_RX_SZ];
    size_t rx_received;
    bool is_active;

    // TX-состояние для каждого клиента
    struct {
        uint8_t *data;
        size_t length;
        size_t sent;
    } tx;
    TickType_t last_activity;  // время последнего пакета (в тиках FreeRTOS)
} elink_client_t;

static elink_client_t clients[ELINK_MAX_CLIENTS] = {0};

// Глобальная переменная для слушающего PCB
static struct tcp_pcb *pxPCBListen_EL = NULL;

static TimerHandle_t xSendTimer = NULL;

volatile uint16_t link_srv_rcv_cnt = 0;

// --- Прототипы функций ---
err_t elink_server_accept(__attribute__((unused)) void *pvArg, struct tcp_pcb *pxPCB, err_t xErr);
bool elink_server_send(const uint8_t *pucFrame, size_t usLength, elink_client_t *client);
void elink_server_release(struct tcp_pcb *pxPCB);
void elink_server_error(void *pvArg, __attribute__((unused)) err_t xErr);
err_t elink_server_receive(void *pvArg, struct tcp_pcb *pxPCB, struct pbuf *p, err_t xErr);
err_t elink_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
void elink_check_timeouts(void *);
static int elink_server_find_allowed_ip_index(const ip_addr_t *ip);


// --- elink_init() ---
void elink_server_start()
{
    struct tcp_pcb *pxPCBListenNew, *pxPCBListenOld;
    uint16_t usPort = ELINK_SERVER_PORT;

    if ((pxPCBListenNew = pxPCBListenOld = tcp_new()) == NULL) {
        /* Can't create TCP socket. */
    }
    else if (tcp_bind(pxPCBListenNew, IP_ADDR_ANY, (u16_t)usPort) != ERR_OK) {
        /* Bind failed - Maybe illegal port value or in use. */
        (void)tcp_close(pxPCBListenOld);
    }
    else if ((pxPCBListenNew = tcp_listen(pxPCBListenNew)) == NULL) {
        // lwip 2.0 capability 
        (void)tcp_close(pxPCBListenOld);
    }
    else {
        /* Register callback function for new clients. */
        tcp_accept(pxPCBListenNew, elink_server_accept);

        /* Everything okay. Set global variable. */
        pxPCBListen_EL = pxPCBListenNew;  

            // 🔥 Создаём и запускаем таймер при первом клиенте
        if (xSendTimer == NULL) {
            xSendTimer = xTimerCreate("elink_timeout", pdMS_TO_TICKS(ELINK_CLIENT_TIMEOUT_MS/5), pdTRUE, NULL, elink_check_timeouts);
            if (xSendTimer != NULL) {
                xTimerStart(xSendTimer, 0);
            }
        }
    }
}

// --- elink_server_accept() ---
err_t elink_server_accept(__attribute__((unused)) void *pvArg, struct tcp_pcb *pxPCB, err_t xErr)
{
    if (xErr != ERR_OK) return xErr;

     // 🔒 Проверка: разрешён ли IP?
    int ip_index = elink_server_find_allowed_ip_index(&pxPCB->remote_ip);
    if (ip_index < 0) {
        // 🔴 Неразрешённый IP — отклоняем
        tcp_abort(pxPCB);
        return ERR_ABRT;
    }

    // 🔍 Ищем свободный слот
    int slot = -1;
    // if(ip_index < ELINK_MAX_CLIENTS){
    //     if(!clients[ip_index].is_active){
    //         slot = ip_index;
    //     }else{
    //         // 🔴 Обращение в незакрытую сессию
    //         tcp_abort(pxPCB);
    //         return ERR_ABRT;
    //     }
    // }

    if(ip_index < ELINK_MAX_CLIENTS){
         slot = ip_index;
    }
    
    if (slot < 0) {
        // Нет свободных слотов — отклоняем клиента
        tcp_abort(pxPCB);
        return ERR_ABRT;
    }

    // 📌 Регистрируем клиента
    clients[slot].pcb = pxPCB;
    clients[slot].rx_received = 0;
    clients[slot].is_active = true;
    clients[slot].tx.data = NULL;
    clients[slot].tx.length = 0;
    clients[slot].tx.sent = 0;

    // 🔥 Обновление: сохраняем время активности
    clients[slot].last_activity = xTaskGetTickCount();

    // Устанавливаем колбэки
    tcp_recv(pxPCB, elink_server_receive);
    tcp_err(pxPCB, elink_server_error);
    tcp_arg(pxPCB, &clients[slot]);  // ← передаём указатель на клиент


    return ERR_OK;
}

// --- elink_server_release() ---
void elink_server_release(struct tcp_pcb *pxPCB)
{
    if (pxPCB == NULL) return;

    // Находим клиента по PCB
    for (int i = 0; i < ELINK_MAX_CLIENTS; i++) {
        if (clients[i].pcb == pxPCB) {
            // Очищаем TX-буфер
            if (clients[i].tx.data) {
                mem_free(clients[i].tx.data);
                clients[i].tx.data = NULL;
            }

            clients[i].is_active = false;
            clients[i].pcb = NULL;

            if (tcp_close(pxPCB) != ERR_OK) {
                tcp_abort(pxPCB);
            }
            return;
        }
    }
}

// --- elink_server_error() ---
void elink_server_error(void *pvArg, __attribute__((unused)) err_t xErr)
{
    elink_client_t *client = (elink_client_t *)pvArg;
    if (client == NULL || !client->is_active) return;

    client->is_active = false;
    client->pcb = NULL;

    // Очищаем TX-буфер
    if (client->tx.data) {
        mem_free(client->tx.data);
        client->tx.data = NULL;
        client->tx.length = 0;
        client->tx.sent = 0;
    }

    // Снимаем колбэки (безопаснее)
    if (client->pcb != NULL) {
        tcp_arg(client->pcb, NULL);
        tcp_recv(client->pcb, NULL);
        tcp_sent(client->pcb, NULL);
        tcp_err(client->pcb, NULL);
        tcp_abort(client->pcb);
    }
}

// --- elink_server_receive() ---
err_t elink_server_receive(void *pvArg, struct tcp_pcb *pxPCB, struct pbuf *p, err_t xErr)
{
    elink_client_t *client = (elink_client_t *)pvArg;
    if (client == NULL || !client->is_active) {
        return ERR_ABRT;
    }

    if (xErr != ERR_OK) return xErr;
    if (p == NULL) {
        elink_server_release(pxPCB);
        return ERR_OK;
    }

    // 📥 Читаем в клиентский буфер
    u16_t i = 0;
    while (i < p->len && client->rx_received < ELINK_SERVER_RX_SZ) {
        client->rx_buffer[client->rx_received] = ((uint8_t *)p->payload)[i];
        client->rx_received++;
        i++;
    }

    // 🔥 Обновление: сброс таймаута при получении данных
    client->last_activity = xTaskGetTickCount();

    tcp_recved(pxPCB, p->len);
    pbuf_free(p);

    // 📡 Если получили ELINK_SERVER_RX_SZ байта — обрабатываем
    if (client->rx_received == ELINK_SERVER_RX_SZ) {
        uint8_t aucResponse[ELINK_SERVER_TX_SZ];  // ⚠️ Используем стек — безопаснее
        memset(aucResponse, 0, sizeof(aucResponse));

        elink_server_get_pld(client->rx_buffer, client->rx_received, aucResponse, sizeof(aucResponse));
        
        client->rx_received = 0;  // сброс
        link_srv_rcv_cnt++;

       // vTaskDelay(100);
        // Отправка ответа (ELINK_SERVER_RX_SZ байт)
        if (!elink_server_send(aucResponse, ELINK_SERVER_TX_SZ, client)) {
            elink_server_release(pxPCB);
            return ERR_OK;
        }
    }

    return ERR_OK;
}



// --- elink_send() ---
bool elink_server_send(const uint8_t *pucFrame, size_t usLength, elink_client_t *client)
{
    if (client == NULL || client->pcb == NULL || usLength == 0) {
        return false;
    }

    // Проверяем, не занято ли уже отправление у этого клиента
    if (client->tx.data != NULL) {
        return false;  // уже идёт отправка
    }

    // Выделяем память под буфер данных
    client->tx.data = mem_malloc(usLength);
    if (!client->tx.data) {
        return false;
    }

    memcpy(client->tx.data, pucFrame, usLength);
    client->tx.length = usLength;
    client->tx.sent = 0;

    tcp_sent(client->pcb, elink_server_sent);  // ← ставим на клиентский PCB
   // xTimerReset(xSendTimer, 0);

    // Попытка отправить сразу (если буфер свободен)
    return elink_server_sent(NULL, client->pcb, 0) == ERR_OK;
}

// --- elink_server_sent() ---
err_t elink_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    // Ищем клиента по pcb
    elink_client_t *client = NULL;
    for (int i = 0; i < ELINK_MAX_CLIENTS; i++) {
        if (clients[i].pcb == pcb && clients[i].is_active) {
            client = &clients[i];
            break;
        }
    }

    if (client == NULL || client->tx.data == NULL) {
        return ERR_OK;
    }

    client->tx.sent += len;
      // 🔥 Сброс таймаута при отправке (опционально)
    client->last_activity = xTaskGetTickCount();

    // Если ещё есть данные для отправки
    if (client->tx.sent < client->tx.length) {
        size_t remaining = client->tx.length - client->tx.sent;
        u16_t mss = tcp_mss(pcb);
        u16_t chunk = LWIP_MIN(tcp_sndbuf(pcb), LWIP_MIN(mss, remaining));

        if (chunk > 0) {
            err_t err = tcp_write(pcb, &client->tx.data[client->tx.sent], chunk, TCP_WRITE_FLAG_COPY);
            //tcp_output(pcb);
            if (err == ERR_OK) {   
                err = tcp_output(pcb);
            }

            if (err == ERR_MEM) {
            // Слишком много неподтверждённых данных — подождём tcp_sent или повторим позже
                return ERR_OK; // не разрывать соединение!
            }

        }
        return ERR_OK;
    }

    // Отправка завершена — очищаем
    if (client->tx.data) {
        mem_free(client->tx.data);
        client->tx.data = NULL;
        client->tx.length = 0;
        client->tx.sent = 0;
    }

    return ERR_OK;
}


// --- elink_check_timeouts() ---
void elink_check_timeouts(void * arg)
{
    const TickType_t TIMEOUT_MS = ELINK_CLIENT_TIMEOUT_MS;  // 3 секунды таймаута
    const TickType_t TIMEOUT_TICKS = pdMS_TO_TICKS(TIMEOUT_MS);

    bool online = true;
    for (int i = 0; i < ELINK_MAX_CLIENTS; i++) {
            TickType_t elapsed = xTaskGetTickCount() - clients[i].last_activity;
            if (elapsed > TIMEOUT_TICKS) {
                // 🔴 Таймаут — закрываем клиента
                online = false;
        }
    }


}


// --- Поиск индекса IP в разрешённом списке (-1, если не найден) ---
static int elink_server_find_allowed_ip_index(const ip_addr_t *ip)
{
    for (int i = 0; i < ELINK_MAX_CLIENTS; i++) {
        if (ip_addr_cmp(ip, &allowed_ips[i])) {
            return i;
        }
    }
    return -1; // не найден
}