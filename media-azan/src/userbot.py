from __future__ import annotations

import asyncio
import io
import sqlite3
import time
from getpass import getpass
from pathlib import Path

import qrcode
from telethon import TelegramClient
from telethon.errors import SessionPasswordNeededError

from .config import AppConfig
from .logger import log_event


def _session_base_path(session_name: str) -> Path:
    if session_name.endswith(".session"):
        return Path(session_name)
    return Path(f"{session_name}.session")


def _cleanup_session_files(logger, session_name: str, force_new: bool = False) -> None:
    session_base = _session_base_path(session_name)
    removed: list[str] = []
    for suffix in ("", "-wal", "-shm"):
        path = Path(f"{session_base}{suffix}")
        if path.exists():
            try:
                path.unlink()
                removed.append(path.name)
            except Exception as exc:  # best-effort cleanup
                log_event(logger, "warn", "userbot", "session_cleanup_failed", f"{path}: {exc}")
    rotated = None
    if force_new and session_base.exists():
        try:
            rotated = session_base.with_name(f"{session_base.stem}-{int(time.time())}.bak.session")
            session_base.rename(rotated)
        except Exception as exc:
            log_event(logger, "warn", "userbot", "session_rotate_failed", str(exc))
    if removed or rotated:
        details = ", ".join(removed) if removed else "none"
        if rotated:
            details = f"{details}; rotated={rotated.name}"
        log_event(logger, "info", "userbot", "session_cleanup", details)


async def open_user_client_session(
    logger,
    config: AppConfig,
    max_attempts: int = 3,
    delay: int = 5,
) -> TelegramClient:
    for attempt in range(1, max_attempts + 1):
        client: TelegramClient | None = None
        try:
            log_event(logger, "info", "userbot", "session_open", f"attempt={attempt}/{max_attempts}")
            client = TelegramClient(config.telegram_user_session, config.telegram_user_api_id, config.telegram_user_api_hash)
            await client.connect()
            return client
        except sqlite3.OperationalError as exc:
            if "database is locked" in str(exc).lower():
                log_event(logger, "warn", "userbot", "session_locked", str(exc))
                if client:
                    try:
                        await client.disconnect()
                    except Exception:
                        pass
                _cleanup_session_files(logger, config.telegram_user_session, force_new=(attempt == max_attempts))
                if attempt < max_attempts:
                    await asyncio.sleep(delay)
                continue
            raise
        except Exception as exc:
            log_event(logger, "error", "userbot", "session_open_failed", str(exc))
            if client:
                try:
                    await client.disconnect()
                except Exception:
                    pass
            if attempt < max_attempts:
                await asyncio.sleep(delay)
    raise RuntimeError("Unable to open Telegram user session after retries.")


async def ensure_user_authorized(client: TelegramClient, logger, phone: str | None) -> int | None:
    if await client.is_user_authorized():
        me = await client.get_me()
        return me.id if me else None

    log_event(logger, "info", "userbot", "auth_required", "interactive_login")
    me = await perform_interactive_login(client, logger, phone)
    return me.id if me else None


async def perform_qr_login(client: TelegramClient, logger):
    try:
        log_event(logger, "info", "userbot", "qr_login_start")
        qr_login = await client.qr_login()
        qr = qrcode.QRCode()
        qr.add_data(qr_login.url)
        qr.make(fit=True)
        img = qr.make_image(fill="black", back_color="white")
        buf = io.BytesIO()
        img.save(buf, format="PNG")
        buf.seek(0)
        try:
            import PIL.Image

            PIL.Image.open(buf).show()
        except Exception:
            pass
        print("Scan the QR-code with Telegram (Desktop/Mobile).")
        try:
            qr.print_ascii(invert=True)
        except Exception:
            print(f"Open this link to login: {qr_login.url}")
        await qr_login.wait()
        log_event(logger, "info", "userbot", "qr_login_done")
        return await client.get_me()
    except Exception as exc:
        log_event(logger, "error", "userbot", "qr_login_failed", str(exc))
        return None


async def perform_interactive_login(client: TelegramClient, logger, phone: str | None):
    try:
        user_phone = (phone or "").strip()
        if not user_phone:
            user_phone = input("Enter phone (+71234567890): ").strip()
        await client.send_code_request(user_phone)
        code = input("Enter Telegram code: ").strip()
        try:
            me = await client.sign_in(phone=user_phone, code=code)
        except SessionPasswordNeededError:
            pwd = getpass("Enter 2FA password: ")
            me = await client.sign_in(password=pwd)
        log_event(logger, "info", "userbot", "interactive_login_done")
        return me
    except Exception as auth_exc:
        log_event(logger, "warn", "userbot", "interactive_login_failed", str(auth_exc))
        log_event(logger, "info", "userbot", "qr_login_fallback")
        return await perform_qr_login(client, logger)
