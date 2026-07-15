#!/usr/bin/env python3
import socket
import time
import sys

# ================= CONFIGURATION =================
HOST = '0.0.0.0'
PORT = 12850

DATA_TO_RECEIVE = 64   # сколько байт ждём от клиента
DATA_TO_SEND = 346      # сколько байт отправляем в ответ
ACK_MESSAGE = b"OK\n"    # fallback — если DATA_TO_SEND < len(ACK_MESSAGE)

RECEIVE_TIMEOUT = 10     # таймаут ожидания следующей порции данных

# ================= HELPER FUNCTIONS =================
def send_all(conn, data: bytes):
    """Отправляет ровно `len(data)` байт, вызывает исключение при ошибке"""
    total_sent = 0
    while total_sent < len(data):
        sent = conn.send(data[total_sent:])
        if sent == 0:
            raise RuntimeError("connection broken during send")
        total_sent += sent

def get_response_data(n):
    if n <= len(ACK_MESSAGE):
        return ACK_MESSAGE[:n]
    # генерируем фиктивные данные длиной ровно n байт
    base = bytes(range(256))  # 0..255
    full_repeats = n // 256
    remainder = n % 256
    return base * full_repeats + base[:remainder]

DATA_RESPONSE = get_response_data(DATA_TO_SEND)


# ================= SERVER LOGIC =================
# ... existing imports and config ...

# ================= SERVER LOGIC =================
def handle_client(conn, addr):
    print(f"[SERVER] Подключение от {addr}")

    cycle_count = 0
    last_send_time = None

    while True:
        try:
            conn.settimeout(RECEIVE_TIMEOUT)
            recv_time = time.time()  # начало ожидания данных
            data = conn.recv(DATA_TO_RECEIVE)
            
            if not data:
                print("[SERVER] Клиент закрыл соединение.")
                break

            #print(f"[SERVER] Принято {len(data)} байт (ожидалось ~{DATA_TO_RECEIVE})")

            if len(data) != DATA_TO_RECEIVE:
                print(f"[SERVER] ⚠️ Принято меньше данных, чем ожидалось: {len(data)} < {DATA_TO_RECEIVE}")

            # Отправляем ответ
            send_all(conn, DATA_RESPONSE)
            send_time = time.time()
            
            # --- Диагностика времени ---
            cycle_count += 1
            # if last_send_time is not None:
            #     time_gap = recv_time - last_send_time
            #     print(f"[SERVER] 🕒 cycle #{cycle_count} — время между пакетами: {time_gap:.3f} сек")
            
            receive_to_send = send_time - recv_time
            print(f"[SERVER] cycle #{cycle_count}: принятo ✅ {len(data)} байт → отправлено {len(DATA_RESPONSE)} байт.{receive_to_send:.3f} сек")
            last_send_time = send_time  # сохраним время окончания отправки

        except socket.timeout:
            print("[SERVER] Таймаут ожидания данных → закрываем соединение.")
            break
        except Exception as e:
            print(f"[SERVER] Ошибка: {e}")
            break

    conn.close()
    print(f"[SERVER] Всего циклов: {cycle_count}")

# ... rest of main() unchanged ...


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen(5)
        print(f"[SERVER] ✅ Запущен на {HOST}:{PORT}")
        print(f"[SERVER] Ожидание: {DATA_TO_RECEIVE} байт → ответ: {DATA_TO_SEND} байт → повтор без закрытия")
        while True:
            conn, addr = server_socket.accept()
            handle_client(conn, addr)
            

if __name__ == "__main__":
    main()