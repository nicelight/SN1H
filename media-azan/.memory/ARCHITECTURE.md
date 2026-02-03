# ARCHITECTURE — Telegram Azan Bot

## Компоненты
- Telegram Listener (aiogram v3)
- Parser (regex, устойчивый к лишним строкам)
- Scheduler (tick 300s, 24/7 до успеха)
- MQTT Publisher (aiomqtt, broker 192.168.10.141:1883 — Mosquitto)
- Cache (today-azan-time.json)
- Logger (rotating, size-based)

## Поток
1. Scheduler отправляет "Имрӯз" в заданный `chat_id`.
2. Listener принимает сообщения от источника (фильтр по username).
3. Parser проверяет дату и обязательные поля (новый день с 02:00 по TZ `+05:00`).
4. При успехе формирует 5 ISO timestamps.
5. Publisher отправляет в MQTT с retain.
6. Cache обновляется.

## Обработка ошибок
- MQTT reconnect с backoff.
- Парсер пропускает нерелевантные сообщения.
- Смена дня не прерывает ретраи до получения валидного ответа.
