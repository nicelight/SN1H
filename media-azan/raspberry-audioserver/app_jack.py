# (Python FastAPI + GPIOZero + ALSA)
from fastapi import FastAPI, HTTPException, Query
from pathlib import Path
import subprocess, os, signal, threading, time

from gpiozero import OutputDevice

AUDIO_DIR = Path("/opt/azan/audio")
LOCK  = Path("/opt/azan/lock/azan.lock")
PIDF  = Path("/opt/azan/lock/azan.pid")

# ALSA device for Jack playback
DEV = "hw:0,0"

# Jack mixer: card 0 has 'PCM'
MIXER_CARD = 0
MIXER_NAME = "PCM"

# Relay (speakers power) on GPIO17
RELAY_GPIO = 17
# Most relay modules are active-LOW
RELAY_ACTIVE_HIGH = False

SPEAKER_DELAY_SEC = 0.5
RAMP_TICK_SEC = 0.10  # 100 ms

app = FastAPI()

_state_lock = threading.Lock()
_current_proc: subprocess.Popen | None = None
_stop_event = threading.Event()
STALE_LOCK_SEC = 10  # auto-clear lock if older and process is dead
_relay: OutputDevice | None = None


def _pid_alive(pid: int) -> bool:
    try:
        os.kill(pid, 0)
        return True
    except ProcessLookupError:
        return False
    except PermissionError:
        # Assume alive if we cannot signal (unlikely on RPi)
        return True


def _clear_stale_lock() -> bool:
    """
    Removes lock if there is no alive process and the lock looks stale.
    Returns True when a stale lock was cleared.
    """
    if not LOCK.exists():
        return False
    try:
        age = time.time() - LOCK.stat().st_mtime
    except FileNotFoundError:
        return False

    pid = None
    if PIDF.exists():
        try:
            pid = int(PIDF.read_text().strip())
        except (ValueError, OSError):
            pid = None

    if pid and _pid_alive(pid):
        return False  # real playback in progress

    # If no PID is known, avoid killing a just-starting request; wait a short age
    if pid is None and age < 2:
        return False

    if age >= STALE_LOCK_SEC or pid is None or not _pid_alive(pid):
        unlock()
        return True

    return False


def _acquire_lock() -> bool:
    try:
        fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
        os.close(fd)
        return True
    except FileExistsError:
        # stale lock? try to clean once
        if _clear_stale_lock():
            try:
                fd = os.open(LOCK, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                os.close(fd)
                return True
            except FileExistsError:
                return False
        return False


def unlock():
    LOCK.unlink(missing_ok=True)
    PIDF.unlink(missing_ok=True)


def _clamp(p: int) -> int:
    return max(0, min(100, int(p)))


def _ensure_relay() -> OutputDevice:
    global _relay
    if _relay is None or _relay.closed:
        _relay = OutputDevice(RELAY_GPIO, active_high=RELAY_ACTIVE_HIGH, initial_value=False)
    return _relay


def _close_relay():
    global _relay
    if _relay is not None and not _relay.closed:
        _relay.close()
    _relay = None


def _amixer_set(pct: int):
    subprocess.run(
        ["amixer", "-c", str(MIXER_CARD), "sset", MIXER_NAME, f"{_clamp(pct)}%"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )


def _speakers_on():
    _ensure_relay().on()


def _speakers_off():
    if _relay:
        _relay.off()


def _ramp(duration: float, proc: subprocess.Popen, v_from: int, v_to: int):
    if duration <= 0:
        _amixer_set(v_to)
        return

    start = time.monotonic()
    end = start + duration

    while True:
        if _stop_event.is_set() or proc.poll() is not None:
            return

        now = time.monotonic()
        if now >= end:
            _amixer_set(v_to)
            return

        k = (now - start) / duration
        _amixer_set(int(v_from + (v_to - v_from) * k))
        time.sleep(RAMP_TICK_SEC)


def _end_cleanup(proc: subprocess.Popen):
    proc.wait()
    _amixer_set(0)
    time.sleep(SPEAKER_DELAY_SEC)
    _speakers_off()
    _close_relay()

    with _state_lock:
        global _current_proc
        _current_proc = None
        _stop_event.clear()
        unlock()


@app.get("/play")
def play(
    file: str = Query(..., description="audio file name from /opt/azan/audio"),
    rise: float = Query(3.0, ge=0, le=120),
    volFrom: int = Query(20, ge=0, le=100),
    volTo: int = Query(100, ge=0, le=100),
):
    audio = (AUDIO_DIR / file).resolve()
    if not audio.exists() or audio.parent != AUDIO_DIR:
        raise HTTPException(404, "file not found")

    if not _acquire_lock():
        raise HTTPException(409, "busy")

    try:
        with _state_lock:
            global _current_proc
            _stop_event.clear()

            v_from = _clamp(volFrom)
            v_to = _clamp(volTo)
            if v_to < v_from:
                v_from, v_to = v_to, v_from

            _speakers_on()
            time.sleep(SPEAKER_DELAY_SEC)

            _amixer_set(v_from)

            p = subprocess.Popen([
                "cvlc", "--intf", "dummy",
                "--aout=alsa",
                f"--alsa-audio-device={DEV}",
                "--play-and-exit",
                str(audio)
            ])
            _current_proc = p
            PIDF.write_text(str(p.pid))

            threading.Thread(target=_ramp, args=(rise, p, v_from, v_to), daemon=True).start()
            threading.Thread(target=_end_cleanup, args=(p,), daemon=True).start()

        return {
            "ok": True,
            "file": file,
            "rise": rise,
            "volFrom": v_from,
            "volTo": v_to
        }
    except Exception:
        with _state_lock:
            _current_proc = None
            _stop_event.clear()
            unlock()
        _speakers_off()
        _close_relay()
        raise


@app.get("/stop")
def stop():
    with _state_lock:
        global _current_proc
        if _current_proc is None or not PIDF.exists():
            return {"ok": False, "msg": "nothing playing"}

        _stop_event.set()
        _amixer_set(0)

        pid = int(PIDF.read_text())
        try:
            os.kill(pid, signal.SIGTERM)
        except ProcessLookupError:
            pass

    time.sleep(SPEAKER_DELAY_SEC)
    _speakers_off()
    _close_relay()

    with _state_lock:
        _current_proc = None
        _stop_event.clear()
        unlock()

    return {"ok": True, "stopped": True}
