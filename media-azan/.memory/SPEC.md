# SPEC — Telegram Azan Bot

## Цель и дата
- Цель: ежедневная публикация 5 времен азана из Telegram в MQTT.
- Таймзона: фиксированная `+05:00`.
- Новый день начинается в 02:00 (TZ `+05:00`).

## Входы/выходы
- Вход: Telegram сообщения от источника `@vaktiNamozBot` (по username), получаемые через MTProto (Telethon userbot).
- Выход: 5 MQTT топиков `azan/today/*` с ISO‑timestamp payload.
- Запрос "Имрӯз" отправляется от имени пользователя (MTProto/Telethon).

## Контракты
- Telegram message schema: $ref `decl/schemas/telegram-message.schema.json`.
- Cache schema: $ref `decl/schemas/cache-today.schema.json`.
- MQTT payload schema: $ref `decl/schemas/azan-payload.schema.json`.
- MQTT config schema: $ref `decl/schemas/mqtt-config.schema.json`.
- MQTT publish schema: $ref `decl/schemas/mqtt-publish.schema.json`.
- App config schema: $ref `decl/schemas/app-config.schema.json`.
- Log event schema: $ref `decl/schemas/log-event.schema.json`.

## События
- `TelegramMessageReceived`: $ref `decl/events/telegram.message.received.yaml`.
- `AzanParsed`: $ref `decl/events/azan.parsed.yaml`.
- `MqttPublished`: $ref `decl/events/mqtt.published.yaml`.

## Workflow
- DailyAzanSync: $ref `decl/workflows/daily-azan-sync.yaml`.

## Политики
- MQTT публикация: $ref `decl/policies/mqtt-publication.yaml`.
- Сопоставление даты: $ref `decl/policies/date-matching.yaml`.
- Лейблы парсинга: $ref `decl/policies/parsing-labels.yaml`.

## Идемпотентность
- Публикация только при изменении данных относительно cache (включая reconnect).

## Ретраи и день
- Ретраи запросов каждые 5 минут до получения валидного ответа.
- Смена дня не останавливает ретраи.
- Валидность ответа определяется только по дате в тексте (в TZ `+05:00`), время получения сообщения не учитывается.

## Конфигурация и секреты
- MQTT endpoint: `192.168.10.141:1883` (Mosquitto).
- Креды/секреты храним только в `.env`, в спецификациях их нет.
- `TELEGRAM_CHAT_ID` задается явно в `.env`.
- Userbot использует `TELEGRAM_USER_API_ID`, `TELEGRAM_USER_API_HASH`, `TELEGRAM_USER_SESSION` (и при необходимости `TELEGRAM_USER_PHONE`).
- Bot API не используется.
- Зависимости фиксируем в `requirements.txt`.

## Логирование
- События логируются с полями: level, time, component, event, details.
- Ротация по размеру < 512KB, путь `/logs`.
