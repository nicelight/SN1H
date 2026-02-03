from __future__ import annotations

import json
import logging
import os
from datetime import datetime, timezone
from logging.handlers import RotatingFileHandler


def setup_logger(log_dir: str, max_bytes: int) -> logging.Logger:
    os.makedirs(log_dir, exist_ok=True)
    logger = logging.getLogger("azan")
    logger.setLevel(logging.INFO)

    log_path = os.path.join(log_dir, "azan-bot.log")
    file_handler = RotatingFileHandler(log_path, maxBytes=max_bytes, backupCount=5)
    file_handler.setFormatter(logging.Formatter("%(message)s"))

    stream_handler = logging.StreamHandler()
    stream_handler.setFormatter(logging.Formatter("%(message)s"))

    if not logger.handlers:
        logger.addHandler(file_handler)
        logger.addHandler(stream_handler)

    return logger


def log_event(logger: logging.Logger, level: str, component: str, event: str, details: str | None = None) -> None:
    payload = {
        "level": level,
        "time": datetime.now(timezone.utc).isoformat(),
        "component": component,
        "event": event,
    }
    if details:
        payload["details"] = details

    message = json.dumps(payload, ensure_ascii=False)
    if level == "debug":
        logger.debug(message)
    elif level == "warn":
        logger.warning(message)
    elif level == "error":
        logger.error(message)
    else:
        logger.info(message)
