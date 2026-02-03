from __future__ import annotations

import asyncio
from typing import Iterable

import aiomqtt

from .logger import log_event


TOPICS = {
    "fajr": "azan/today/fajr",
    "dhuhr": "azan/today/dhuhr",
    "asr": "azan/today/asr",
    "maghrib": "azan/today/maghrib",
    "isha": "azan/today/isha",
}


async def publish_times(logger, config, times_iso: dict[str, str]) -> None:
    retries = [1, 2, 5, 10, 30, 60]
    attempt = 0

    while True:
        try:
            async with aiomqtt.Client(
                hostname=config.mqtt_host,
                port=config.mqtt_port,
                username=config.mqtt_username,
                password=config.mqtt_password,
            ) as client:
                for key, topic in TOPICS.items():
                    await client.publish(topic, payload=times_iso[key], qos=1, retain=True)
            log_event(logger, "info", "mqtt", "publish_ok", "published 5 topics")
            return
        except Exception as exc:  # noqa: BLE001
            delay = retries[min(attempt, len(retries) - 1)]
            log_event(logger, "warn", "mqtt", "publish_failed", f"{exc}; retry in {delay}s")
            attempt += 1
            await asyncio.sleep(delay)
