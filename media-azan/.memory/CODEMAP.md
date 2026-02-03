# CODEMAP — Telegram Azan Bot

## Структура
- `requirements.txt` — зависимости (aiomqtt, python-dotenv, telethon, qrcode).
- `src/config.py` — загрузка `.env` и AppConfig.
- `src/logger.py` — структурные логи и ротация по размеру.
- `src/parser.py` — парсинг сообщения `@vaktiNamozBot` и ISO‑timestamp.
- `src/time_utils.py` — расчет "сегодня" с границей дня 02:00.
- `src/cache.py` — чтение/запись `today-azan-time.json`.
- `src/mqtt_client.py` — публикация в MQTT (QoS1, retain, retry/backoff).
- `src/main.py` — запуск userbot (Telethon), scheduler и обработчик сообщений.
- `logs/azan-bot.log` — runtime логи (путь задается через `LOG_DIR`).
- `src/userbot.py` — Telethon userbot: сессия, авторизация, отправка от имени пользователя.
