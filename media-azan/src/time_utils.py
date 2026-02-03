from __future__ import annotations

from datetime import datetime, date, time, timedelta


def resolve_target_date(now: datetime, day_start: time) -> date:
    local_time = now.timetz()
    if (local_time.hour, local_time.minute) < (day_start.hour, day_start.minute):
        return (now.date() - timedelta(days=1))
    return now.date()
