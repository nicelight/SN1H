# RUNBOOK — Telegram Azan Bot

## Развертывание (systemd)
1) Создай пользователя:
```
sudo useradd -r -s /usr/sbin/nologin azan

```
2) Создай папки и выдай права:
```
sudo mkdir -p /opt/azan-bot /var/log/azan-bot
sudo mkdir -p /opt/azan-bot/src
sudo chown -R azan:azan /opt/azan-bot /var/log/azan-bot
sudo chown -R azan:azan /opt/azan-bot
sudo chown -R suhrobcozyden:suhrobcozyden /opt/azan-bot
```
3) Скопируй папку проекта в `/opt/azan-bot`.
4) Выдай права на папку проекта:
```
sudo chown -R azan:azan /opt/azan-bot
```
5) Создай venv и установи зависимости:
```
sudo python3 -m venv /opt/azan-bot/.venv
```
```
sudo apt update 
sudo apt install -y build-essential python3-dev
sudo -u azan /opt/azan-bot/.venv/bin/pip install -r /opt/azan-bot/requirements.txt

```
6) Создай файл окружения `/opt/azan-bot/.env`:
```
TELEGRAM_BOT_TOKEN=...
TELEGRAM_CHAT_ID=-1001234567890
MQTT_HOST=192.168.10.141
MQTT_PORT=1883
MQTT_USERNAME=mq
MQTT_PASSWORD=LongmqttpassworD1
TZ_OFFSET=+05:00
DAY_START_TIME=02:00
REQUEST_INTERVAL_SECONDS=300
LOG_DIR=/var/log/azan-bot
LOG_MAX_BYTES=524288
```
7) Создай systemd unit `/etc/systemd/system/azan-bot.service`:
```
[Unit]
Description=Telegram Azan Bot
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
WorkingDirectory=/opt/azan-bot
EnvironmentFile=/opt/azan-bot/.env
ExecStart=/opt/azan-bot/.venv/bin/python -m src.main
Restart=always
RestartSec=5
User=azan
Group=azan

[Install]
WantedBy=multi-user.target
```
8) Запусти сервис:
```
sudo systemctl daemon-reload
```
```
sudo systemctl enable --now azan-bot
```

## Наблюдаемость
- Логи структурированы и ротируются по размеру.
- Файл логов: `/var/log/azan-bot/azan-bot.log`.
- Проверка статуса: `systemctl status azan-bot`.
- Журнал systemd: `journalctl -u azan-bot -f`.

## Отладка
- Проверить доступность MQTT: `nc -vz 192.168.10.141 1883`.
- Проверить права на лог‑каталог: `ls -ld /var/log/azan-bot`.
- Проверить env: `systemctl show azan-bot --property=Environment`.
- Проверить работоспособность токена: `/opt/azan-bot/.venv/bin/python -m src.main`.

## Восстановление
- При обрыве MQTT выполняется reconnect с backoff.
- После восстановления публикация только при изменении данных (cache compare).
