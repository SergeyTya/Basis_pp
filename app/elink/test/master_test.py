#!/usr/bin/env python3
"""
Циклический тест TCP-мастера (elink_master.c):
✅ Проверка, что соединение не разрывается после множества обменов данными
"""

import socket
import struct
import time
import sys

# === Настройки ===
HOST = "192.168.137.35"   # IP-адрес мастера (указать реальный)
PORT = 12855             # стандартный порт
ELINK_REQUEST_SIZE = 100
ELINK_RESPONSE_SIZE = 500

# Тест-параметры
TEST_CYCLES = 100000           # число повторов (или 0 — бесконечно)
PAUSE_BETWEEN_REQUESTS = 0.500  # секунд между запросами
VERBOSE = True               # выводить детали

# === Вспомогательные функции ===

def make_request(
    slave_id: int = 1,
    is_write_request: bool = False,
    address: int = 0x0000,
    data: int = 0x0000,
    log_index: int = 1
) -> bytes:
    """Генерирует 100-байтный запрос в формате Typedef_SlaveRequest"""
    req = bytearray(ELINK_REQUEST_SIZE)

    # 0: slave_id (uint8)
    req[0] = slave_id & 0xFF

    # 1: is_write_request (uint8)
    req[1] = 1 if is_write_request else 0

    # 2–3: register_address (LE)
    req[2:4] = struct.pack('<H', address & 0xFFFF)

    # 4–5: register_value (LE)
    req[4:6] = struct.pack('<H', data & 0xFFFF)

    # 6: log_index (uint8)
    # Допустимый диапазон: 1..20 → оставим модуль, но лучше:
    # assert 1 <= log_index <= 20, "log_index must be in [1, 20]"
    req[6] = log_index if 1 <= log_index <= 20 else 1

    # req[7..99] остаются 0 — это `reserved`

    return bytes(req)

def run_cyclic_test(host: str = HOST, port: int = PORT):
    cycle = 0
    success_count = 0
    failures = []
    latencies = []  # ✅ новое: список задержек

    print(f"🚀 Starting cyclic test: {HOST}:{PORT}")
    print(f"   Requests: {TEST_CYCLES} (0 = infinite), pause: {PAUSE_BETWEEN_REQUESTS:.2f}s")
    print("-" * 60)

    while True:
        cycle += 1
        if TEST_CYCLES and cycle > TEST_CYCLES:
            break

        try:
            with socket.create_connection((host, port), timeout=5) as sock:
                sock.settimeout(5)

                # Отправляем запрос
                request = make_request(
                    slave_id = 1,
                    is_write_request=False,
                    address=102,
                    data=50,
                    log_index=cycle % 20 + 1
                )

                if VERBOSE:
                    print(f"[{cycle:4d}] Sending {len(request)}B request...", end=" ")

                send_time = time.time()  # 🔹 измерение времени отправки
                sock.sendall(request)

                # Читаем ответ
                response = b""
                start = time.time()
                while len(response) < ELINK_RESPONSE_SIZE:
                    if time.time() - start > 3.0:
                        raise TimeoutError("Timeout on receive")

                    chunk = sock.recv(ELINK_RESPONSE_SIZE - len(response))
                    if not chunk:
                        raise ConnectionError("Server closed connection")

                    response += chunk
                
                if len(response) != ELINK_RESPONSE_SIZE:
                    raise ValueError(f"Got {len(response)}B instead of {ELINK_RESPONSE_SIZE}B")

                # 🔹 Вычисляем RTT
                roundtrip_ms = (time.time() - send_time) * 1000
                latencies.append(roundtrip_ms)

                # Успех
                success_count += 1
                if VERBOSE:
                    print(f"✅ {len(response)}B received ({roundtrip_ms:.1f} ms)")

                    # 🆕 Вывод первых 256 байт в hex (с группировкой по 16 байт)
                    dump_len = min(256, len(response))
                    print("   First 256 bytes (hex dump):")
                    for i in range(0, dump_len, 16):
                        chunk = response[i:i+16]
                        hex_part = " ".join(f"{b:02x}" for b in chunk)
                        ascii_part = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
                        print(f"     {i:04x}: {hex_part:<48} |{ascii_part}|")


                # Пауза перед следующим запросом (в рамках того же соединения)
                time.sleep(PAUSE_BETWEEN_REQUESTS)


        except (ConnectionRefusedError, OSError) as e:
            if VERBOSE:
                print(f"\n❌ [CRITICAL] Connection refused or error: {e}")
            failures.append((cycle, f"Connection error: {e}"))
            break  # не стоит продолжать без сервера

        except (ConnectionError, TimeoutError, ValueError) as e:
            if VERBOSE:
                print(f"\n❌ [FAIL] Cycle {cycle}: {e}")
            failures.append((cycle, str(e)))
            # Продолжаем или останавливаем? Обычно — продолжаем, чтобы накопить статистику
            # Но можно и break — зависит от ситуации

        except KeyboardInterrupt:
            print("\n⏹️  Test interrupted by user.")
            break

        except Exception as e:
            failures.append((cycle, f"Unexpected: {e}"))
            if VERBOSE:
                print(f"\n⚠️  Cycle {cycle} failed with unknown error: {e}")

    # === Результаты ===
    print("\n" + "=" * 60)
    print(f"📊 Final stats:")
    print(f"   Total cycles: {cycle - 1 if TEST_CYCLES else cycle}")
    print(f"   Successes: {success_count}")
    print(f"   Failures: {len(failures)}")
    if failures:
        print(f"\n📋 Failures:")
        for c, err in failures[:10]:  # только первые 10
            print(f"   #{c}: {err}")
        if len(failures) > 10:
            print(f"   ... and {len(failures) - 10} more")

    # ✅ RTT статистика
    if latencies:
        avg_ms = sum(latencies) / len(latencies)
        min_ms = min(latencies)
        max_ms = max(latencies)
        print(f"\n⏱️  Response time (RTT): avg / min / max = {avg_ms:.1f} / {min_ms:.1f} / {max_ms:.1f} ms")

    if not failures:
        print("✅ TEST PASSED: Connection remained stable during all cycles.")
    else:
        print("❌ TEST FAILED: Connection was broken at some point.")
        sys.exit(1)

if __name__ == "__main__":
    run_cyclic_test()