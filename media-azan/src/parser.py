from __future__ import annotations

import re
from datetime import datetime, date, time, timezone

DATE_REGEX = re.compile(r"^(?P<date>\d{2}\.\d{2}\.\d{4})", re.MULTILINE)

LABELS = {
    "fajr": "Субҳ",
    "dhuhr": "Қиём",
    "asr": "Аср",
    "maghrib": "Шом",
    "isha": "Хуфтан",
}


def parse_message(text: str) -> tuple[date, dict[str, str]] | None:
    date_match = DATE_REGEX.search(text)
    if not date_match:
        return None

    date_str = date_match.group("date")
    try:
        message_date = datetime.strptime(date_str, "%d.%m.%Y").date()
    except ValueError:
        return None

    times: dict[str, str] = {}
    for key, label in LABELS.items():
        pattern = re.compile(rf"^\s*{re.escape(label)}\s*-\s*(\d{{2}}:\d{{2}})\s*$", re.MULTILINE)
        match = pattern.search(text)
        if not match:
            return None
        times[key] = match.group(1)

    return message_date, times


def to_iso(date_value: date, time_value: str, tz: timezone) -> str:
    hour, minute = [int(part) for part in time_value.split(":")]
    dt = datetime.combine(date_value, time(hour=hour, minute=minute), tzinfo=tz)
    return dt.isoformat()
