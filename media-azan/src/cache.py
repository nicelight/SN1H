from __future__ import annotations

import json
from pathlib import Path


def read_cache(path: Path) -> dict | None:
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError):
        return None


def write_cache(path: Path, date_str: str, times: dict[str, str]) -> None:
    payload = {"date": date_str, "times": times}
    path.write_text(json.dumps(payload, ensure_ascii=False), encoding="utf-8")
