# Raspberry Pi Azan Player — гайд по установке и настройке

Цель: Raspberry Pi принимает HTTP‑команду и проигрывает аудиофайл (Azan) через **USB‑аудиокарту** или **3.5 мм Jack** с помощью `cvlc`.

---

## 1. ОС

* **Raspberry Pi OS Lite**
* Включить SSH

Обновление системы:

```bash
sudo apt update && sudo apt upgrade -y
```

---

## 2. Установка пакетов

```bash
sudo apt install -y  vlc alsa-utils  python3-pip python3-venv python3-full git samba
```

---

## 3. Структура проекта

```bash
sudo mkdir -p /opt/azan/{audio,service,lock}
sudo chown -R pi:pi /opt/azan
```

---

## 4. Python virtualenv (обязательно)

Debian запрещает system‑wide pip (PEP 668).

```bash
python3 -m venv /opt/azan/venv
/opt/azan/venv/bin/pip install --upgrade pip
/opt/azan/venv/bin/pip install fastapi uvicorn
```

Проверка:

```bash
/opt/azan/venv/bin/python -c "import fastapi, uvicorn; print('ok')"
```

---

## 5. Samba (добавление аудиофайлов по сети)

Файл `/etc/samba/smb.conf` — добавить в конец:

```ini
[azan-audio]
 path = /opt/azan/audio
 writable = yes
 browseable = yes
 read only = no
```

Перезапуск:

```bash
sudo systemctl restart smbd
```

Доступ с ПК:

```
\\192.168.10.15\azan-audio
```

Скопировать файл, например:

```
start.mp3
```

---

## 6. Аудиоустройства (ALSA)

Подключить USB‑аудиокарту и проверить:

```bash
aplay -l
```

Типичный результат:

* `card 0` — bcm2835 Headphones → **3.5 мм Jack**
* `card 2` — USB Audio Device → **USB‑аудиокарта**

---

## 7. ALSA конфигурация (только USB)

⚠️ Jack **не** объявляем через `asound.conf`, используем напрямую `hw:0,0`.

Файл `/etc/asound.conf`:

```ini
pcm.usb {
  type plug
  slave {
    pcm "hw:2,0"
  }
}
```

---

## 8. Проверка звука (MP3)

`aplay` для mp3 **не использовать**.
 `interface error` Сообщения **нормальны** для `--intf dummy`.

### USB

```bash
cvlc --intf dummy --aout=alsa --alsa-audio-device=usb --play-and-exit /opt/azan/audio/start.mp3
```

### 3.5 мм Jack

```bash
cvlc --intf dummy --aout=alsa --alsa-audio-device=hw:0,0 --play-and-exit /opt/azan/audio/start.mp3
```


---

## 9. HTTP mini‑service (концепция)

* FastAPI + Uvicorn
* `GET /play?file=name.mp3`
* Lock через файл:

```
/opt/azan/lock/azan.lock
```

* Повторные запросы во время воспроизведения → `409 Busy`
* Два сервиса:

  * USB — порт `8081`
  * Jack — порт `8082`

---

## 10. systemd (запуск как сервис)

Каждый плеер запускается как отдельный systemd‑сервис:

* `azan-usb.service`
* `azan-jack.service`

Использовать **python и uvicorn из venv**.

---

## 11. Проверка HTTP

```bash
curl "http://192.168.10.15:8081/play?file=start.mp3"   # USB
curl "http://192.168.10.15:8082/play?file=start.mp3"   # Jack
```

---

## 12. Логи

```bash
journalctl -u azan-usb -f
journalctl -u azan-jack -f
```

---

## Ключевые выводы

* `cvlc` — основной плеер
* USB‑карта требует `plug`
* Jack (`bcm2835`) стабилен только через `hw:0,0`
* HTTP + lock надёжнее MQTT для этой задачи
* Файлы добавляются вручную по Samba

---

 
## 13. ТЕСТИРОВАНИЕ. HTTP-сервисы воспроизведения (USB и Jack)

Назначение: два автозапускаемых HTTP-сервиса на Raspberry Pi, каждый со своим endpoint, которые **мгновенно отвечают на запрос** и запускают воспроизведение `start.mp3` в фоне.

* USB-аудио: порт **8081**
* 3.5 мм Jack: порт **8082**
* Защита от повторных вызовов: **lock-файл**
* Поддержка `/stop`

---

### 13.1 Файлы и каталоги

```bash
/opt/azan/audio/start.mp3
/opt/azan/lock/azan.lock
/opt/azan/lock/azan.pid
/opt/azan/service/app_usb.py
/opt/azan/service/app_jack.py
```

---

### 13.2 HTTP-сервис USB (8081)

Файл `/opt/azan/service/app_usb.py`

```python
from fastapi import FastAPI, HTTPException
from pathlib import Path
import subprocess, os, signal, threading

AUDIO = Path("/opt/azan/audio/start.mp3")
LOCK  = Path("/opt/azan/lock/azan.lock")
PIDF  = Path("/opt/azan/lock/azan.pid")
DEV   = "usb"

app = FastAPI()

def try_lock():
    try:
        fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
        os.close(fd)
        return True
    except FileExistsError:
        return False

def unlock():
    LOCK.unlink(missing_ok=True)
    PIDF.unlink(missing_ok=True)

def wait_and_unlock(p):
    p.wait()
    unlock()

@app.get("/play")
def play():
    if not AUDIO.exists():
        raise HTTPException(404, "file not found")
    if not try_lock():
        raise HTTPException(409, "busy")

    p = subprocess.Popen([
        "cvlc", "--intf", "dummy",
        "--aout=alsa",
        f"--alsa-audio-device={DEV}",
        "--play-and-exit",
        str(AUDIO)
    ])
    PIDF.write_text(str(p.pid))
    threading.Thread(target=wait_and_unlock, args=(p,), daemon=True).start()
    return {"ok": True, "started": True}

@app.get("/stop")
def stop():
    if not PIDF.exists():
        return {"ok": False, "msg": "nothing playing"}
    pid = int(PIDF.read_text())
    try:
        os.kill(pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    unlock()
    return {"ok": True, "stopped": True}
```

---

### 13.3 HTTP-сервис Jack (8082)

Файл `/opt/azan/service/app_jack.py`

```python
from fastapi import FastAPI, HTTPException
from pathlib import Path
import subprocess, os, signal, threading

AUDIO = Path("/opt/azan/audio/start.mp3")
LOCK  = Path("/opt/azan/lock/azan.lock")
PIDF  = Path("/opt/azan/lock/azan.pid")
DEV   = "hw:0,0"

app = FastAPI()

def try_lock():
    try:
        fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
        os.close(fd)
        return True
    except FileExistsError:
        return False

def unlock():
    LOCK.unlink(missing_ok=True)
    PIDF.unlink(missing_ok=True)

def wait_and_unlock(p):
    p.wait()
    unlock()

@app.get("/play")
def play():
    if not AUDIO.exists():
        raise HTTPException(404, "file not found")
    if not try_lock():
        raise HTTPException(409, "busy")

    p = subprocess.Popen([
        "cvlc", "--intf", "dummy",
        "--aout=alsa",
        f"--alsa-audio-device={DEV}",
        "--play-and-exit",
        str(AUDIO)
    ])
    PIDF.write_text(str(p.pid))
    threading.Thread(target=wait_and_unlock, args=(p,), daemon=True).start()
    return {"ok": True, "started": True}

@app.get("/stop")
def stop():
    if not PIDF.exists():
        return {"ok": False, "msg": "nothing playing"}
    pid = int(PIDF.read_text())
    try:
        os.kill(pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    unlock()
    return {"ok": True, "stopped": True}
```

---

### 13.4 systemd units

**USB** `/etc/systemd/system/azan-usb.service`

```ini
[Unit]
Description=Azan USB HTTP Player
After=network.target

[Service]
User=pi
WorkingDirectory=/opt/azan/service
ExecStart=/opt/azan/venv/bin/uvicorn app_usb:app --host 0.0.0.0 --port 8081
Restart=always

[Install]
WantedBy=multi-user.target
```

**Jack** `/etc/systemd/system/azan-jack.service`

```ini
[Unit]
Description=Azan Jack HTTP Player
After=network.target

[Service]
User=pi
WorkingDirectory=/opt/azan/service
ExecStart=/opt/azan/venv/bin/uvicorn app_jack:app --host 0.0.0.0 --port 8082
Restart=always

[Install]
WantedBy=multi-user.target
```

Активация:

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now azan-usb azan-jack
```


### 13.5 Проверка
USB:
http://192.168.10.15:8081/play
http://192.168.10.15:8081/stop

JACK:
http://192.168.10.15:8082/play
http://192.168.10.15:8082/stop
```

Ожидаемо:

* `/play` → мгновенный `200 OK`
* повторный `/play` → `409 Busy`
* `/stop` → немедленная остановка и снятие lock


##  14. Активация реле 

sudo apt install -y python3-gpiozero
