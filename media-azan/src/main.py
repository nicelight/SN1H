from __future__ import annotations

import asyncio
import contextlib
import sys
from datetime import datetime
from pathlib import Path

from telethon import events

from .cache import read_cache, write_cache
from .config import load_config
from .logger import log_event, setup_logger
from .mqtt_client import publish_times
from .parser import parse_message, to_iso
from .time_utils import resolve_target_date
from .userbot import ensure_user_authorized, open_user_client_session


async def run() -> None:
    config = load_config()
    logger = setup_logger(config.log_dir, config.log_max_bytes)
    cache_path = Path("today-azan-time.json")
    state = {"cache": read_cache(cache_path)}
    lock = asyncio.Lock()

    user_client = await open_user_client_session(logger, config)
    user_id = await ensure_user_authorized(user_client, logger, config.telegram_user_phone)
    if not user_id:
        await user_client.disconnect()
        raise RuntimeError("Userbot authorization failed")

    async def handle_message(event: events.NewMessage.Event) -> None:
        try:
            if event.chat_id != config.telegram_chat_id:
                return
            if event.raw_text and event.raw_text.strip().lower() == "ping" and event.is_reply:
                replied = await event.get_reply_message()
                if replied and replied.sender_id == user_id:
                    await event.respond("pong")
                    log_event(logger, "info", "userbot", "ping_pong", f"id={event.id}")
                    return

            sender = await event.get_sender()
            sender_username = getattr(sender, "username", None) if sender else None
            if sender_username != config.source_username:
                return
            if not event.raw_text:
                return

            log_event(logger, "info", "telegram", "message_received", f"id={event.id}")
            parsed = parse_message(event.raw_text)
            if not parsed:
                log_event(logger, "warn", "parser", "parse_failed", "missing date or labels")
                return

            message_date, times = parsed
            target_date = resolve_target_date(datetime.now(config.tz), config.day_start)
            if message_date != target_date:
                log_event(logger, "info", "parser", "date_mismatch", f"msg={message_date} target={target_date}")
                return

            times_iso = {key: to_iso(message_date, value, config.tz) for key, value in times.items()}
            date_str = message_date.isoformat()

            async with lock:
                cache = state.get("cache")
                if cache and cache.get("date") == date_str and cache.get("times") == times_iso:
                    log_event(logger, "info", "cache", "no_change", date_str)
                    return

            await publish_times(logger, config, times_iso)

            async with lock:
                state["cache"] = {"date": date_str, "times": times_iso}
                write_cache(cache_path, date_str, times_iso)
        except Exception as exc:  # noqa: BLE001
            log_event(logger, "error", "telegram", "handler_failed", str(exc))

    user_client.add_event_handler(handle_message, events.NewMessage())

    async def scheduler() -> None:
        while True:
            target_date = resolve_target_date(datetime.now(config.tz), config.day_start)
            async with lock:
                cache = state.get("cache")
                already_done = cache and cache.get("date") == target_date.isoformat()

            if not already_done:
                try:
                    if not user_client.is_connected():
                        await user_client.connect()
                    await user_client.send_message(config.telegram_chat_id, "Имрӯз")
                    log_event(logger, "info", "userbot", "send_request", f"chat_id={config.telegram_chat_id}")
                except Exception as exc:  # noqa: BLE001
                    log_event(logger, "warn", "userbot", "send_failed", str(exc))

            await asyncio.sleep(config.request_interval_seconds)

    scheduler_task = asyncio.create_task(scheduler())
    reconnect_delays = [1, 2, 5, 10, 30, 60]
    attempt = 0

    try:
        while True:
            try:
                if not user_client.is_connected():
                    await user_client.connect()
                attempt = 0
                log_event(logger, "info", "userbot", "connected")
                await user_client.run_until_disconnected()
                log_event(logger, "warn", "userbot", "disconnected")
            except Exception as exc:  # noqa: BLE001
                log_event(logger, "warn", "userbot", "connection_failed", str(exc))
            delay = reconnect_delays[min(attempt, len(reconnect_delays) - 1)]
            attempt += 1
            log_event(logger, "info", "userbot", "reconnect_sleep", f"{delay}s")
            await asyncio.sleep(delay)
    finally:
        scheduler_task.cancel()
        with contextlib.suppress(asyncio.CancelledError):
            await scheduler_task
        await user_client.disconnect()


def main() -> None:
    if sys.platform.startswith("win"):
        asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
    asyncio.run(run())


if __name__ == "__main__":
    main()
