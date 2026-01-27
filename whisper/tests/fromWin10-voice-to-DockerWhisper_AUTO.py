# запуск
# python fromWin10-voice-to-DockerWhisper_AUTO.py --host 192.168.10.140 --port 10300 --language ru

# зависимости
# pip install sounddevice numpy wyoming


# /voice
# →
# минимум `min_duration` секунд слушаем,
# считаем RMS (громкость) каждого блока,

# если громкость ниже порога silence_threshold дольше,
# чем silence_duration
# →
# считаем, что ты закончил говорить и останавливаемся,
# отправляем всё аудио одним запросом на Wyoming.

# Порог можно будет потом подкрутить.

# настройки "авто-остановки"
BLOCK_DURATION = 0.2  # длина блока в секундах (0.2 = 200 мс)
MIN_DURATION = 1.5  # минимум, сколько записывать (сек)
SILENCE_DURATION = 1.5  # сколько тишины подряд считать "концом" (сек)
SILENCE_THRESHOLD = 300.0  # порог громкости (RMS), ниже — считаем тишиной
MAX_DURATION = 20.0  # защита от зависания: максимум записи (сек)

import argparse
import json
import socket
import sys
import time

import numpy as np
import sounddevice as sd

RATE = 16000
WIDTH_BYTES = 2  # int16
CHANNELS = 1


def send_event(
    sock: socket.socket,
    event_type: str,
    data: dict | None = None,
    payload: bytes | None = None,
):
    if data is None:
        data = {}

    header = {
        "type": event_type,
        "data": data,
    }

    if payload:
        header["payload_length"] = len(payload)

    header_line = json.dumps(header, ensure_ascii=False).encode("utf-8") + b"\n"
    sock.sendall(header_line)

    if payload:
        sock.sendall(payload)


def recv_event(sock: socket.socket) -> dict:
    # читаем строку заголовка до '\n'
    header_bytes = b""
    while b"\n" not in header_bytes:
        chunk = sock.recv(4096)
        if not chunk:
            raise ConnectionError(
                "Соединение с Wyoming-сервером разорвано при чтении заголовка"
            )
        header_bytes += chunk

    line, rest = header_bytes.split(b"\n", 1)
    header = json.loads(line.decode("utf-8"))

    data_len = header.get("data_length", 0)
    payload_len = header.get("payload_length", 0)

    buf = rest

    def take_from_buf(length: int) -> bytes:
        nonlocal buf
        if length == 0:
            return b""
        out = b""
        while len(out) < length:
            if buf:
                needed = length - len(out)
                out += buf[:needed]
                buf = buf[needed:]
                if len(out) == length:
                    break
            else:
                chunk2 = sock.recv(4096)
                if not chunk2:
                    raise ConnectionError(
                        "Соединение разорвано при чтении data/payload"
                    )
                buf += chunk2
        return out

    data_bytes = take_from_buf(data_len)
    payload_bytes = take_from_buf(payload_len)

    if data_len:
        extra_data = json.loads(data_bytes.decode("utf-8"))
        base = header.get("data") or {}
        base.update(extra_data)
        header["data"] = base

    header["payload"] = payload_bytes
    return header


def record_until_silence() -> bytes:
    print("Начинаю запись. Говори, остановка будет по тишине...")
    sd.default.samplerate = RATE
    sd.default.channels = CHANNELS

    block_size = int(BLOCK_DURATION * RATE)
    frames = []

    start_time = time.time()
    last_voice_time = start_time

    def rms_level(block: np.ndarray) -> float:
        # block: shape (N, 1), dtype=int16
        if block.size == 0:
            return 0.0
        x = block.astype(np.float32)
        return float(np.sqrt(np.mean(x * x)))

    with sd.InputStream(samplerate=RATE, channels=CHANNELS, dtype="int16") as stream:
        while True:
            block, _ = stream.read(block_size)
            frames.append(block.copy())

            level = rms_level(block)
            now = time.time()

            # лог можно включить для настройки порога
            # print(f"RMS: {level:.1f}")

            if level > SILENCE_THRESHOLD:
                last_voice_time = now

            elapsed = now - start_time
            silence_time = now - last_voice_time

            if elapsed > MIN_DURATION and silence_time >= SILENCE_DURATION:
                print(f"Обнаружена тишина {silence_time:.1f} с → останавливаю запись.")
                break

            if elapsed >= MAX_DURATION:
                print("Достигнут MAX_DURATION, останавливаю запись.")
                break

    audio = np.concatenate(frames, axis=0)
    print(f"Запись завершена. Длительность: {audio.shape[0] / RATE:.2f} с")
    return audio.tobytes()


def transcribe_once(host: str, port: int, language: str | None = None) -> str:
    audio_bytes = record_until_silence()

    with socket.create_connection((host, port), timeout=10) as sock:
        data = {}
        if language:
            data["language"] = language

        # запрос на распознавание
        send_event(sock, "transcribe", data)

        # формат аудио
        audio_format = {
            "rate": RATE,
            "width": WIDTH_BYTES,
            "channels": CHANNELS,
        }
        send_event(sock, "audio-start", audio_format)

        # один большой audio-chunk
        send_event(sock, "audio-chunk", audio_format, payload=audio_bytes)

        send_event(sock, "audio-stop", {})

        # ждём transcript
        while True:
            event = recv_event(sock)
            etype = event.get("type")
            data = event.get("data") or {}

            if etype == "transcript":
                text = data.get("text", "")
                lang = data.get("language")
                if lang:
                    print(f"[язык: {lang}]")
                return text

            if etype in ("transcript-start", "transcript-chunk", "transcript-stop"):
                continue


def main():
    parser = argparse.ArgumentParser(
        description="Wyoming Whisper клиент: /voice до тишины"
    )
    parser.add_argument(
        "--host", default="192.168.110.10", help="IP сервера с wyoming-whisper"
    )
    parser.add_argument("--port", type=int, default=10300, help="Порт wyoming-whisper")
    parser.add_argument(
        "--language",
        default=None,
        help="Код языка (например, ru, en). Можно не указывать.",
    )
    args = parser.parse_args()

    host = args.host
    port = args.port
    language = args.language

    print(f"Подключение к Wyoming STT на {host}:{port}")
    print("Команды:")
    print("  /voice  - записать голос до тишины и распознать")
    print("  /exit   - выйти")
    print()

    while True:
        try:
            cmd = input("> ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nВыход.")
            break

        if cmd == "/exit":
            print("Выход.")
            break
        elif cmd == "/voice":
            try:
                text = transcribe_once(host, port, language)
                print("Распознанный текст:")
                print(text or "(пусто)")
            except Exception as e:
                print(f"Ошибка: {e}")
        elif cmd == "":
            continue
        else:
            print("Неизвестная команда. Используй /voice или /exit.")


if __name__ == "__main__":
    main()
