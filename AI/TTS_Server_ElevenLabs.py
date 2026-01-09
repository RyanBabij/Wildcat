from __future__ import annotations

import json
import os
import queue
import re
import socket
import threading
import time
import traceback
import uuid
import wave
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Optional

from elevenlabs import VoiceSettings
from elevenlabs.client import ElevenLabs

from Wildcat.Audio.AudioPlayer import GlobalAudioPlayer


# ----------------------------
# Config
# ----------------------------
HOST = "127.0.0.1"
PORT = 17777

# ElevenLabs auth: keep this server-side
ELEVENLABS_API_KEY = os.getenv("ELEVENLABS_API_KEY", "").strip()

# Default voice/model. You should override these via env or set_voice/set_voice_prompt.
# Example voice_id from docs is fine for testing; replace with your preferred voice.
# DEFAULT_VOICE_ID = os.getenv("ELEVENLABS_VOICE_ID", "pNInz6obpgDQGcFmaJgB").strip()
DEFAULT_VOICE_ID = os.getenv("ELEVENLABS_VOICE_ID", "Y8CIE2UXHetTCsBPUIBa").strip()
DEFAULT_MODEL_ID = os.getenv("ELEVENLABS_MODEL_ID", "eleven_turbo_v2_5").strip()

# We request PCM so we can write a WAV for your GlobalAudioPlayer.
# Supported output formats include pcm_16000/22050/24000/32000/44100/etc. :contentReference[oaicite:2]{index=2}
OUTPUT_FORMAT = os.getenv("ELEVENLABS_OUTPUT_FORMAT", "pcm_22050").strip()

# Voice settings: tune as desired (or expose via protocol later).
VOICE_SETTINGS = VoiceSettings(
    stability=float(os.getenv("ELEVENLABS_STABILITY", "0.6")),
    similarity_boost=float(os.getenv("ELEVENLABS_SIMILARITY_BOOST", "0.6")),
    style=float(os.getenv("ELEVENLABS_STYLE", "0.1")),
    use_speaker_boost=os.getenv("ELEVENLABS_SPEAKER_BOOST", "false").lower() in ("1", "true", "yes", "y"),
    speed=float(os.getenv("ELEVENLABS_SPEED", "1.1")),
)

TTS_MAX_CHARS = 320
PLAYBACK_GAP_S = 0.8

TTS_OUT_DIR = Path(r"R:\outputs_tts_server")
TTS_OUT_DIR.mkdir(parents=True, exist_ok=True)

# Backpressure: bound *synthesis* queue (more important than bounding playback queue)
SYNTH_Q_MAX = 256

# Playback queue bounds still matter, but use them as a “secondary” safety net.
MAX_PLAY_QUEUE = 256


# ----------------------------
# Text segmentation
# ----------------------------
_WS_RE = re.compile(r"\s+")
_SENT_BOUNDARY_RE = re.compile(r"(?<=[.!?])\s+")
_HARD_SPLIT_RE = re.compile(r"\n+")


def normalize_ws(s: str) -> str:
    return _WS_RE.sub(" ", s).strip()


def segment_stream(text: str) -> list[str]:
    """
    Streaming segmentation:
      - normalize CRLF
      - split on newlines first
      - then sentence-ish split on . ! ?
      - hard wrap to max chars
    """
    if not text:
        return []
    text = text.replace("\r\n", "\n").replace("\r", "\n")
    parts: list[str] = []
    for block in _HARD_SPLIT_RE.split(text):
        block = normalize_ws(block)
        if not block:
            continue
        for sent in _SENT_BOUNDARY_RE.split(block):
            sent = sent.strip()
            if not sent:
                continue
            parts.append(sent)
    return parts


def hard_wrap(units: list[str], max_chars: int) -> list[str]:
    out: list[str] = []
    for u in units:
        if len(u) <= max_chars:
            out.append(u)
        else:
            for i in range(0, len(u), max_chars):
                c = u[i : i + max_chars].strip()
                if c:
                    out.append(c)
    return out


def _parse_pcm_sample_rate(output_format: str) -> int:
    """
    output_format examples:
      - pcm_16000
      - pcm_22050
      - pcm_24000
      - pcm_44100

    Defaults to 22050 if not parseable.
    """
    m = re.match(r"^pcm_(\d+)$", (output_format or "").strip())
    if not m:
        return 22050
    try:
        return int(m.group(1))
    except Exception:
        return 22050


def _write_wav_s16le_mono(out_wav: Path, pcm_bytes: bytes, sample_rate: int) -> None:
    """
    Wrap raw PCM S16LE mono bytes into a WAV container.
    ElevenLabs PCM is documented as 16-bit depth (S16LE). :contentReference[oaicite:3]{index=3}
    """
    out_wav.parent.mkdir(parents=True, exist_ok=True)

    # Ensure alignment (16-bit = 2 bytes/sample)
    if len(pcm_bytes) % 2 != 0:
        pcm_bytes = pcm_bytes[:-1]

    with wave.open(str(out_wav), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)  # 16-bit
        wf.setframerate(int(sample_rate))
        wf.writeframes(pcm_bytes)


# ----------------------------
# Synthesis queue
# ----------------------------
@dataclass(frozen=True)
class SynthJob:
    job_id: str
    stream_id: Optional[str]   # None for non-stream speak()
    text: str
    created_s: float


class SynthesisQueue:
    """
    Single FIFO queue that enforces global ordering for synthesis -> playback.
    """
    def __init__(self, server: "TTSServer", maxsize: int = SYNTH_Q_MAX) -> None:
        self.server = server
        self.q: "queue.Queue[Optional[SynthJob]]" = queue.Queue(maxsize=maxsize)
        self._stop = threading.Event()
        self._t = threading.Thread(target=self._worker, daemon=True)
        self._t.start()

    def put(self, job: SynthJob, block: bool = False, timeout: float = 0.0) -> bool:
        if self._stop.is_set():
            return False
        try:
            self.q.put(job, block=block, timeout=timeout)
            return True
        except queue.Full:
            return False

    def stop(self) -> None:
        if self._stop.is_set():
            return
        self._stop.set()
        try:
            self.q.put_nowait(None)
        except Exception:
            pass
        self._t.join(timeout=2.0)

    def clear_pending(self) -> None:
        try:
            while True:
                self.q.get_nowait()
        except queue.Empty:
            pass

    def _worker(self) -> None:
        i = 0
        while not self._stop.is_set():
            job = self.q.get()
            if job is None:
                break

            text = (job.text or "").strip()
            if not text:
                continue

            out_wav = TTS_OUT_DIR / f"job_{job.job_id}_{i}.wav"
            i += 1

            try:
                self.server._synth_one(text, out_wav)
            except Exception:
                traceback.print_exc()
                continue

            # Secondary safety: if playback queue is full, drop or wait briefly.
            if self.server.player.pending_items() >= MAX_PLAY_QUEUE:
                time.sleep(0.01)

            self.server.player.enqueue(str(out_wav), delete_after=True)


# ----------------------------
# Streams
# ----------------------------
class SpeechStream:
    """
    A stream only buffers partial text and emits segmented units into the global synth queue.
    """
    def __init__(self, server: "TTSServer", stream_id: str) -> None:
        self.server = server
        self.stream_id = stream_id
        self._buf = ""
        self._closed = False
        self._aborted = False
        self._lock = threading.Lock()

    def push(self, text: str) -> None:
        if not text:
            return
        with self._lock:
            if self._closed or self._aborted:
                return

            self._buf += text

            units = hard_wrap(segment_stream(self._buf), TTS_MAX_CHARS)
            if not units:
                return

            buf_end = self._buf[-1]
            finished = buf_end.isspace() or buf_end in ".!?\n\r\t"
            if not finished and len(units) >= 2:
                emit, tail = units[:-1], units[-1]
                self._buf = tail
            else:
                emit = units
                self._buf = ""

        for u in emit:
            if not u.strip():
                continue
            job = SynthJob(job_id=uuid.uuid4().hex, stream_id=self.stream_id, text=u, created_s=time.time())
            ok = self.server.synth_q.put(job, block=False)
            if not ok:
                break

    def finish(self) -> None:
        with self._lock:
            if self._closed or self._aborted:
                return
            self._closed = True
            leftover = self._buf
            self._buf = ""

        units = hard_wrap(segment_stream(leftover), TTS_MAX_CHARS) if leftover else []
        for u in units:
            if not u.strip():
                continue
            job = SynthJob(job_id=uuid.uuid4().hex, stream_id=self.stream_id, text=u, created_s=time.time())
            self.server.synth_q.put(job, block=False)

    def abort(self) -> None:
        with self._lock:
            self._aborted = True
            self._closed = True
            self._buf = ""


@dataclass
class Session:
    stream: SpeechStream
    stream_id: str


# ----------------------------
# Server
# ----------------------------
class TTSServer:
    def __init__(self) -> None:
        if not ELEVENLABS_API_KEY:
            raise RuntimeError("Missing ELEVENLABS_API_KEY env var (server-side).")

        print("[tts_server] Initializing ElevenLabs client...")
        self.eleven = ElevenLabs(api_key=ELEVENLABS_API_KEY)

        # Synthesis lock: preserves ordering and avoids concurrent SDK calls if desired.
        self._synth_lock = threading.Lock()

        self.player = GlobalAudioPlayer(playback_gap_s=PLAYBACK_GAP_S, max_queue=MAX_PLAY_QUEUE)
        self.synth_q = SynthesisQueue(self, maxsize=SYNTH_Q_MAX)

        self._sessions_lock = threading.Lock()
        self._sessions: dict[str, Session] = {}

        # Compatibility field name; now holds ElevenLabs voice_id (not a wav path).
        self.voice_prompt_wav = DEFAULT_VOICE_ID

        # Stored for transparency / diagnostics
        self.model_id = DEFAULT_MODEL_ID
        self.output_format = OUTPUT_FORMAT

    def _synth_one(self, text: str, out_wav: Path) -> None:
        """
        Generate PCM audio via ElevenLabs, then wrap into WAV for GlobalAudioPlayer.

        Docs: output_format includes PCM variants; SDK supports convert() streaming bytes. :contentReference[oaicite:4]{index=4}
        """
        voice_id = (self.voice_prompt_wav or "").strip() or DEFAULT_VOICE_ID
        model_id = (self.model_id or "").strip() or DEFAULT_MODEL_ID
        output_format = (self.output_format or "").strip() or "pcm_22050"

        sample_rate = _parse_pcm_sample_rate(output_format)

        with self._synth_lock:
            # convert() yields chunks of audio bytes
            response = self.eleven.text_to_speech.convert(
                voice_id=voice_id,
                output_format=output_format,
                text=text,
                model_id=model_id,
                voice_settings=VOICE_SETTINGS,
            )

            pcm = bytearray()
            for chunk in response:
                if chunk:
                    pcm += chunk

        _write_wav_s16le_mono(out_wav, bytes(pcm), sample_rate=sample_rate)

    def set_voice_prompt(self, maybe_voice_id: str) -> None:
        """
        Compatibility method:
        - If maybe_voice_id is an existing local file path, reject (ElevenLabs doesn't accept this here).
        - Otherwise treat it as an ElevenLabs voice_id string.
        """
        s = (maybe_voice_id or "").strip()
        if not s:
            raise ValueError("empty voice id")
        if Path(s).exists():
            raise ValueError(
                "ElevenLabs server expects a voice_id (string), not a local wav prompt path. "
                "Provide an ElevenLabs voice_id instead."
            )
        self.voice_prompt_wav = s

    def stop_and_clear(self) -> None:
        self.player.stop_now()
        self.player.clear_pending()
        self.synth_q.clear_pending()

        with self._sessions_lock:
            sessions = list(self._sessions.values())
            self._sessions.clear()
        for s in sessions:
            try:
                s.stream.abort()
            except Exception:
                pass

    def _get_session(self, sid: str) -> Optional[Session]:
        with self._sessions_lock:
            return self._sessions.get(sid)

    def handle(self, req: dict[str, Any]) -> dict[str, Any]:
        cmd = (req.get("cmd") or "").strip()

        if cmd == "ping":
            return {"ok": True, "cmd": "ping"}

        if cmd == "get_state":
            return {
                "ok": True,
                "cmd": "get_state",
                "is_playing": bool(self.player.is_playing()),
                "pending_audio": int(self.player.pending_items()),
                "pending_synth": int(self.synth_q.q.qsize()),
                "voice_prompt_wav": str(self.voice_prompt_wav),  # now a voice_id
                "active_streams": len(self._sessions),
                # extra (non-breaking): useful diagnostics
                "model_id": str(self.model_id),
                "output_format": str(self.output_format),
            }

        if cmd in ("set_voice", "set_voice_prompt"):
            v = (req.get("wav") or req.get("wav_path") or "").strip()
            if not v:
                return {"ok": False, "error": "missing voice id (use wav/wav_path field for compatibility)"}
            try:
                self.set_voice_prompt(v)
            except Exception as e:
                return {"ok": False, "error": str(e)}
            return {"ok": True, "cmd": cmd}

        if cmd == "stop_all":
            self.player.stop_now()
            return {"ok": True, "cmd": "stop_all"}

        if cmd == "stop_and_clear":
            self.stop_and_clear()
            return {"ok": True, "cmd": "stop_and_clear"}

        if cmd == "speak":
            text = normalize_ws(req.get("text") or "")
            if not text:
                return {"ok": False, "error": "missing text"}

            units = hard_wrap(segment_stream(text), TTS_MAX_CHARS)
            accepted = 0
            for u in units:
                job = SynthJob(job_id=uuid.uuid4().hex, stream_id=None, text=u, created_s=time.time())
                if not self.synth_q.put(job, block=False):
                    break
                accepted += 1

            if accepted == 0:
                return {"ok": False, "error": "busy"}
            return {"ok": True, "cmd": "speak", "accepted": accepted}

        if cmd in ("stream_begin", "begin_stream"):
            sid = ((req.get("id") or req.get("stream_id") or "").strip()) or uuid.uuid4().hex
            stream = SpeechStream(self, stream_id=sid)
            with self._sessions_lock:
                self._sessions[sid] = Session(stream=stream, stream_id=sid)
            return {"ok": True, "cmd": cmd, "stream_id": sid}

        if cmd == "stream_push":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}
            s.stream.push(req.get("text") or "")
            return {"ok": True}

        if cmd == "stream_finish":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}
            s.stream.finish()
            return {"ok": True}

        if cmd == "stream_abort":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}
            s.stream.abort()
            with self._sessions_lock:
                self._sessions.pop(sid, None)
            return {"ok": True}

        if cmd == "stream_status":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing stream_id"}
            s = self._get_session(sid)
            done = s is None
            return {"ok": True, "cmd": "stream_status", "stream_id": sid, "done": done}

        return {"ok": False, "error": f"unknown cmd: {cmd}"}


# ----------------------------
# TCP server
# ----------------------------
def _serve_client(conn: socket.socket, addr, server: TTSServer) -> None:
    conn.settimeout(60.0)
    f_r = conn.makefile("r", encoding="utf-8", newline="\n")
    f_w = conn.makefile("w", encoding="utf-8", newline="\n")
    try:
        while True:
            try:
                line = f_r.readline()
            except (TimeoutError, ConnectionResetError, ConnectionAbortedError, OSError):
                break
            if not line:
                break

            line = line.strip()
            if not line:
                continue

            try:
                req = json.loads(line)
                if not isinstance(req, dict):
                    raise ValueError("request must be a JSON object")
                resp = server.handle(req)
            except Exception as e:
                resp = {"ok": False, "error": str(e)}

            try:
                f_w.write(json.dumps(resp, ensure_ascii=False) + "\n")
                f_w.flush()
            except (BrokenPipeError, ConnectionResetError, ConnectionAbortedError, OSError):
                break
    finally:
        try:
            f_r.close()
        except Exception:
            pass
        try:
            f_w.close()
        except Exception:
            pass
        try:
            conn.close()
        except Exception:
            pass


def main() -> None:
    srv = TTSServer()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((HOST, PORT))
    sock.listen(8)

    print(f"[tts_server] listening on {HOST}:{PORT} (ElevenLabs backend)")
    print(f"[tts_server] default voice_id={srv.voice_prompt_wav} model_id={srv.model_id} output_format={srv.output_format}")

    while True:
        conn, addr = sock.accept()
        t = threading.Thread(target=_serve_client, args=(conn, addr, srv), daemon=True)
        t.start()


if __name__ == "__main__":
    main()
