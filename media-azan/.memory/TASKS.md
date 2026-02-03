---
id: tasks
updated: 2026-01-31
---

# Tasks (канбан)
> Политика: вместе с имплементационными пунктами обязательно веди задачи для размышлений и консультаций с тимлидом. Формулируй их с префиксом `CONSULT` или `REFLECT`.
> Формат Kanban с иерархией:
> `[ ]` — не начато, `[~]` — в работе, `[x]` — выполнено.
> Уровни: **EP → FEAT → US → T**.

## TODO
- [ ] EP-001: Memory bank и спецификации
  - [ ] FEAT-001: Декларативные спецификации
    - [ ] US-001: Подготовить декларации по workflow и политикам
      - [ ] T-001: REFLECT определить состав деклараций
      - [x] T-003: CONSULT — зафиксировать новые правила дня/ретраев и хранение секретов

- [ ] EP-002: Реализация телеграм-бота
  - [x] FEAT-002: Базовый сервис и потоки
    - [x] US-002.GOV: Governance — планирование реализации
      - [x] T-002: CONSULT — согласовать план разработки и разбиение по модулям
    - [x] US-003: Конфигурация и запуск
      - [x] T-003: IMPLEMENT — подготовить `requirements.txt`
      - [x] T-004: IMPLEMENT — конфиг через `.env` (MQTT/Telegram/TZ/interval/log)
    - [x] US-004: Логирование
      - [x] T-005: IMPLEMENT — структурные логи и ротация по размеру
    - [x] US-005: Telegram слой
      - [x] T-006: IMPLEMENT — listener для сообщений `@vaktiNamozBot`
      - [x] T-007: IMPLEMENT — отправка "Имрӯз" по таймеру
    - [x] US-006: Парсер
      - [x] T-008: IMPLEMENT — regex парсинг 5 времен + дата
      - [x] T-009: IMPLEMENT — проверка даты по TZ `+05:00` и day_start 02:00
    - [x] US-007: MQTT слой
      - [x] T-010: IMPLEMENT — publish QoS=1 retain=true
      - [x] T-011: IMPLEMENT — reconnect/backoff и идемпотентность
    - [x] US-008: Cache
      - [x] T-012: IMPLEMENT — чтение/запись `today-azan-time.json`
    - [x] US-009: Entry point
      - [x] T-013: IMPLEMENT — main loop и graceful shutdown
    - [x] US-010.GOV: Governance — идентификация группы
      - [x] T-014: CONSULT — как получать chat_id для отправки "Имрӯз"
  - [x] FEAT-003: Валидация запуска
    - [x] US-011: Локальный запуск и smoke-тест
      - [x] T-015: CONSULT — подтвердить окружение/секреты и сценарий smoke-теста
  - [x] FEAT-004: Userbot для триггера
    - [x] US-012: Отправка "Имрӯз" от имени пользователя
      - [x] T-016: CONSULT — легальность/политики и параметры MTProto (api_id/api_hash/phone/session)
      - [x] T-017: IMPLEMENT — Telethon userbot (session/авторизация/отправка)
  - [x] FEAT-005: Полный переход на userbot
    - [x] US-013: Прием сообщений и отправка только через MTProto
      - [x] T-018: CONSULT — отказаться от Bot API и aiogram, оставить только Telethon
      - [x] T-019: IMPLEMENT — чтение сообщений через Telethon (вместо aiogram)
  - [x] FEAT-006: Устойчивость к сети
    - [x] US-014: Переподключение userbot при сетевых сбоях
      - [x] T-020: CONSULT — стратегия reconnect/backoff без краша процесса
      - [x] T-021: IMPLEMENT — loop run_until_disconnected + backoff, обработка ошибок в handler
  - [x] FEAT-007: Ping/Pong
    - [x] US-015: Ответ userbot на ping в группе
      - [x] T-022: CONSULT — условия (только reply на сообщения userbot, только в целевом chat_id)
      - [x] T-023: IMPLEMENT — ping/pong (lowercase, only configured chat_id)

## IN PROGRESS

## DONE
- [x] CONSULT-008: Добавить недостающие контракты (MQTT publish, app config, logging)
- [x] CONSULT-001: MQTT конфигурация и параметры подключения
- [x] CONSULT-002: Идентификация источника (username)
- [x] CONSULT-003: Окно отправки "Имрӯз" (24/7)
- [x] CONSULT-004: Лейблы молитв без альтернатив
- [x] CONSULT-005: Формат today-azan-time.json (минимальный)
- [x] CONSULT-006: Политика republish при reconnect
- [x] CONSULT-007: Стратегия ротации логов
