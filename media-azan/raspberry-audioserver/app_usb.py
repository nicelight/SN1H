
# (Python FastAPI + GPIOZero + ALSA)
from fastapi import FastAPI, HTTPException, Query
from pathlib import Path
import subprocess, os, signal, threading, time

from gpiozero import OutputDevice
from gpiozero.exc import GPIOPinInUse

AUDIO_DIR = Path("/opt/azan/audio")
LOCK  = Path("/opt/azan/lock/azan.lock")
PIDF  = Path("/opt/azan/lock/azan.pid")

# ALSA device for USB playback (as in your setup with /etc/asound.conf pcm.usb)
DEV = "usb"

# USB mixer:
# You previously had: card 1 = USB Audio Device. Mixer controls may be absent.
# If your USB has a mixer control, set these accordingly (e.g. MIXER_NAME="PCM" or "Master").
# If there is NO mixer, we fall back to VLC gain ramp (software).
USB_CARD = 1
MIXER_NAME = "PCM"   # change if needed
USE_AMIXER = True    # set False if `amixer -c 1 scontrols` is empty or PCM/Master not found

# Relay (speakers power) on GPIO17
RELAY_GPIO = 17
RELAY_ACTIVE_HIGH = False  # most relay modules are active-LOW

SPEAKER_DELAY_SEC = 0.5
RAMP_TICK_SEC = 0.10  # 100 ms

app = FastAPI()

_state_lock = threading.Lock()
_current_proc: subprocess.Popen | None = None
_stop_event = threading.Event()
STALE_LOCK_SEC = 10  # auto-clear lock if older and process is dead
_relay: OutputDevice | None = None


class _DummyRelay:
    """Fallback when GPIO17 уже занят другим процессом: no-op on/off/close."""

    closed = False

    def on(self):  # noqa: D401
        return None

    def off(self):
        return None

    def close(self):
        self.closed = True


def _pid_alive(pid: int) -> bool:
    try:
        os.kill(pid, 0)
        return True
    except ProcessLookupError:
        return False
    except PermissionError:
        return True


def _clear_stale_lock() -> bool:
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
        return False

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
        try:
            _relay = OutputDevice(RELAY_GPIO, active_high=RELAY_ACTIVE_HIGH, initial_value=False)
        except GPIOPinInUse:
            # Другой сервис держит пин (например, azan-jack) — работаем без управления реле.
            _relay = _DummyRelay()
    return _relay


def _close_relay():
    global _relay
    if _relay is not None and not _relay.closed:
        _relay.close()
    _relay = None


def _speakers_on():
    _ensure_relay().on()


def _speakers_off():
    if _relay:
        _relay.off()


def _amixer_set(pct: int):
    pct = _clamp(pct)
    subprocess.run(
        ["amixer", "-c", str(USB_CARD), "sset", MIXER_NAME, f"{pct}%"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )


def _ramp_amixer(duration: float, proc: subprocess.Popen, v_from: int, v_to: int):
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


def _ramp_vlc_gain(duration: float, proc: subprocess.Popen, v_from: int, v_to: int):
    """
    Fallback ramp when USB mixer has no controls.
    We can't reliably adjust gain of already running cvlc via CLI, so we approximate:
    - start cvlc at low gain via --gain
    - then step-up by sending RC commands is not available in dummy mode
    => so here we do a simple approach: keep amixer path preferred.
    If USE_AMIXER=False, we just start at volFrom and jump to volTo after 'rise'.
    """
    if duration <= 0:
        return
    start = time.monotonic()
    while True:
        if _stop_event.is_set() or proc.poll() is not None:
            return
        if time.monotonic() - start >= duration:
            return
        time.sleep(RAMP_TICK_SEC)


def _end_cleanup(proc: subprocess.Popen, use_amixer: bool):
    proc.wait()
    if use_amixer:
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

            use_amixer = USE_AMIXER
            if use_amixer:
                # set start volume before playback
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

            if use_amixer:
                threading.Thread(target=_ramp_amixer, args=(rise, p, v_from, v_to), daemon=True).start()
            else:
                # no mixer: best-effort behavior (no smooth ramp)
                threading.Thread(target=_ramp_vlc_gain, args=(rise, p, v_from, v_to), daemon=True).start()

            threading.Thread(target=_end_cleanup, args=(p, use_amixer), daemon=True).start()

        return {"ok": True, "file": file, "rise": rise, "volFrom": v_from, "volTo": v_to, "amixer": use_amixer}
    except Exception:
        # make sure we do not leave lock or GPIO held on errors
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
        if USE_AMIXER:
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
