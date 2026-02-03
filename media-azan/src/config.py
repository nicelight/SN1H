from __future__ import annotations

import os
from dataclasses import dataclass
from datetime import time, timedelta, timezone

from dotenv import load_dotenv


@dataclass(frozen=True)
class AppConfig:
    telegram_chat_id: int
    source_username: str
    telegram_user_api_id: int
    telegram_user_api_hash: str
    telegram_user_session: str
    telegram_user_phone: str | None
    mqtt_host: str
    mqtt_port: int
    mqtt_username: str | None
    mqtt_password: str | None
    tz: timezone
    day_start: time
    request_interval_seconds: int
    log_dir: str
    log_max_bytes: int


def _parse_day_start(value: str) -> time:
    parts = value.split(":")
    if len(parts) != 2:
        raise ValueError("DAY_START_TIME must be HH:MM")
    hour = int(parts[0])
    minute = int(parts[1])
    return time(hour=hour, minute=minute)


def load_config() -> AppConfig:
    load_dotenv()

    chat_id_raw = os.getenv("TELEGRAM_CHAT_ID", "").strip()
    if not chat_id_raw:
        raise ValueError("TELEGRAM_CHAT_ID is required")
    telegram_chat_id = int(chat_id_raw)

    source_username = os.getenv("SOURCE_USERNAME", "vaktiNamozBot").strip()

    api_id_raw = os.getenv("TELEGRAM_USER_API_ID", "").strip()
    if not api_id_raw:
        raise ValueError("TELEGRAM_USER_API_ID is required")
    telegram_user_api_id = int(api_id_raw)

    telegram_user_api_hash = os.getenv("TELEGRAM_USER_API_HASH", "").strip()
    if not telegram_user_api_hash:
        raise ValueError("TELEGRAM_USER_API_HASH is required")

    telegram_user_session = os.getenv("TELEGRAM_USER_SESSION", "azan_user").strip()
    telegram_user_phone = os.getenv("TELEGRAM_USER_PHONE")

    mqtt_host = os.getenv("MQTT_HOST", "192.168.10.141").strip()
    mqtt_port = int(os.getenv("MQTT_PORT", "1883"))
    mqtt_username = os.getenv("MQTT_USERNAME")
    mqtt_password = os.getenv("MQTT_PASSWORD")

    tz_offset = os.getenv("TZ_OFFSET", "+05:00").strip()
    if not tz_offset.startswith(('+', '-')) or len(tz_offset) != 6:
        raise ValueError("TZ_OFFSET must be in format +HH:MM")
    sign = 1 if tz_offset[0] == '+' else -1
    hours = int(tz_offset[1:3])
    minutes = int(tz_offset[4:6])
    tz = timezone(sign * timedelta(hours=hours, minutes=minutes))

    day_start_raw = os.getenv("DAY_START_TIME", "02:00").strip()
    day_start = _parse_day_start(day_start_raw)

    request_interval_seconds = int(os.getenv("REQUEST_INTERVAL_SECONDS", "300"))
    log_dir = os.getenv("LOG_DIR", "/logs").strip()
    log_max_bytes = int(os.getenv("LOG_MAX_BYTES", "524288"))

    return AppConfig(
        telegram_chat_id=telegram_chat_id,
        source_username=source_username,
        telegram_user_api_id=telegram_user_api_id,
        telegram_user_api_hash=telegram_user_api_hash,
        telegram_user_session=telegram_user_session,
        telegram_user_phone=telegram_user_phone,
        mqtt_host=mqtt_host,
        mqtt_port=mqtt_port,
        mqtt_username=mqtt_username,
        mqtt_password=mqtt_password,
        tz=tz,
        day_start=day_start,
        request_interval_seconds=request_interval_seconds,
        log_dir=log_dir,
        log_max_bytes=log_max_bytes,
    )
