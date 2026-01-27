
# запуск 
# python fromWin10-voice-to-DockerWhisper_3sec.py --host 192.168.10.140 --port 10300 --language ru

# зависимости
# pip install sounddevice numpy wyoming

import argparse
import json
import socket
import sys
import time

import numpy as np
import sounddevice as sd

RATE = 16000
WIDTH_BYTES = 2      # int16
CHANNELS = 1
DURATION_SEC = 3.0   # длительность записи по /voice


def send_event(sock: socket.socket, event_type: str, data: dict | None = None, payload: bytes | None = None):
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


def recv_exact(sock: socket.socket, n: int) -> bytes:
    buf = b""
    while len(buf) < n:
        chunk = sock.recv(n - len(buf))
        if not chunk:
            raise ConnectionError("Соединение с Wyoming-сервером разорвано")
        buf += chunk
    return buf


def recv_event(sock: socket.socket) -> dict:
    # читаем строку заголовка до '\n'
    header_bytes = b""
    while b"\n" not in header_bytes:
        chunk = sock.recv(4096)
        if not chunk:
            raise ConnectionError("Соединение с Wyoming-сервером разорвано при чтении заголовка")
        header_bytes += chunk

    line, rest = header_bytes.split(b"\n", 1)
    header = json.loads(line.decode("utf-8"))

    data_len = header.get("data_length", 0)
    payload_len = header.get("payload_length", 0)

    # уже прочитанный "rest" нужно учесть как начало data/payload
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
                    raise ConnectionError("Соединение разорвано при чтении data/payload")
                buf += chunk2
        return out

    data_bytes = take_from_buf(data_len)
    payload_bytes = take_from_buf(payload_len)

    # если есть доп. data — мержим в header["data"]
    if data_len:
        extra_data = json.loads(data_bytes.decode("utf-8"))
        base = header.get("data") or {}
        base.update(extra_data)
        header["data"] = base

    header["payload"] = payload_bytes
    return header


def record_audio(seconds: float) -> bytes:
    print(f"Запись {seconds:.1f} сек. Говори...")
    sd.default.samplerate = RATE
    sd.default.channels = CHANNELS

    audio = sd.rec(int(seconds * RATE), samplerate=RATE, channels=CHANNELS, dtype="int16")
    sd.wait()
    print("Запись остановлена, отправляю на сервер...")
    return audio.tobytes()


def transcribe_once(host: str, port: int, language: str | None = None) -> str:
    audio_bytes = record_audio(DURATION_SEC)

    with socket.create_connection((host, port), timeout=10) as sock:
        # 1. запрос на распознавание
        data = {}
        if language:
            data["language"] = language

        send_event(sock, "transcribe", data)

        # 2. начало аудиопотока
        audio_format = {
            "rate": RATE,
            "width": WIDTH_BYTES,
            "channels": CHANNELS,
        }
        send_event(sock, "audio-start", audio_format)

        # 3. один audio-chunk (можно разбивать, но для 3 сек можно и одним куском)
        send_event(sock, "audio-chunk", audio_format, payload=audio_bytes)

        # 4. окончание аудиопотока
        send_event(sock, "audio-stop", {})

        # 5. ждём transcript
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

            # на всякий случай логируем прочие события
            # (например, transcript-start / transcript-chunk)
            if etype in ("transcript-start", "transcript-chunk", "transcript-stop"):
                # можно раскомментировать для дебага
                # print("DEBUG event:", etype, data)
                continue


def main():
    parser = argparse.ArgumentParser(description="Простой Wyoming Whisper клиент с командой /voice")
    parser.add_argument("--host", default="192.168.110.10", help="IP сервера с wyoming-whisper")
    parser.add_argument("--port", type=int, default=10300, help="Порт wyoming-whisper")
    parser.add_argument("--language", default=None, help="Код языка (например, ru, en). Можно не указывать.")
    args = parser.parse_args()

    host = args.host
    port = args.port
    language = args.language

    print(f"Подключение к Wyoming STT на {host}:{port}")
    print("Команды:")
    print("  /voice  - записать голос 3 сек и распознать")
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
