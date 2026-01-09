from __future__ import annotations

import asyncio
import base64
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

import websockets  # pip install websockets

from elevenlabs import VoiceSettings
from elevenlabs.client import ElevenLabs

from Wildcat.Audio.AudioPlayer import GlobalAudioPlayer


# ----------------------------
# Config
# ----------------------------
HOST = "127.0.0.1"
PORT = 17777

ELEVENLABS_API_KEY = os.getenv("ELEVENLABS_API_KEY", "").strip()

DEFAULT_VOICE_ID = os.getenv("ELEVENLABS_VOICE_ID", "Y8CIE2UXHetTCsBPUIBa").strip()
DEFAULT_MODEL_ID = os.getenv("ELEVENLABS_MODEL_ID", "eleven_turbo_v2_5").strip()
OUTPUT_FORMAT = os.getenv("ELEVENLABS_OUTPUT_FORMAT", "pcm_22050").strip()

VOICE_SETTINGS = VoiceSettings(
    stability=float(os.getenv("ELEVENLABS_STABILITY", "0.6")),
    similarity_boost=float(os.getenv("ELEVENLABS_SIMILARITY_BOOST", "0.6")),
    style=float(os.getenv("ELEVENLABS_STYLE", "0.1")),
    use_speaker_boost=os.getenv("ELEVENLABS_SPEAKER_BOOST", "false").lower() in ("1", "true", "yes", "y"),
    speed=float(os.getenv("ELEVENLABS_SPEED", "1.1")),
)

# Optional: chunk schedule can reduce latency at cost of quality by generating more frequently. :contentReference[oaicite:4]{index=4}
CHUNK_LENGTH_SCHEDULE = [120, 160, 250, 290]

TTS_MAX_CHARS = 320
PLAYBACK_GAP_S = 0.8

TTS_OUT_DIR = Path(r"R:\outputs_tts_server")
TTS_OUT_DIR.mkdir(parents=True, exist_ok=True)

SYNTH_Q_MAX = 256
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
    m = re.match(r"^pcm_(\d+)$", (output_format or "").strip())
    if not m:
        return 22050
    try:
        return int(m.group(1))
    except Exception:
        return 22050


def _write_wav_s16le_mono(out_wav: Path, pcm_bytes: bytes, sample_rate: int) -> None:
    out_wav.parent.mkdir(parents=True, exist_ok=True)
    if len(pcm_bytes) % 2 != 0:
        pcm_bytes = pcm_bytes[:-1]
    with wave.open(str(out_wav), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)  # 16-bit
        wf.setframerate(int(sample_rate))
        wf.writeframes(pcm_bytes)


# ----------------------------
# Synthesis queue (non-stream speak())
# ----------------------------
@dataclass(frozen=True)
class SynthJob:
    job_id: str
    stream_id: Optional[str]
    text: str
    created_s: float


class SynthesisQueue:
    """
    FIFO queue for non-stream synthesis (speak()).
    Streams use WebSockets and bypass this queue.
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
                self.server._synth_one_http(text, out_wav)
            except Exception:
                traceback.print_exc()
                continue

            if self.server.player.pending_items() >= MAX_PLAY_QUEUE:
                time.sleep(0.01)

            self.server.player.enqueue(str(out_wav), delete_after=True)


# ----------------------------
# ElevenLabs WebSocket stream session
# ----------------------------
class ElevenLabsWSStream:
    """
    One stream-input WebSocket session per protocol stream_id.

    Current sink behavior:
      - buffer all PCM bytes in-memory (bytearray)
      - on final, write a WAV and enqueue it

    Later:
      - replace _on_audio_bytes(...) with a true PCM streaming sink to GlobalAudioPlayer.
    """
    def __init__(self, server: "TTSServer", stream_id: str) -> None:
        self.server = server
        self.stream_id = stream_id

        self._out_q: "asyncio.Queue[Optional[dict[str, Any]]]" = asyncio.Queue()
        self._done = threading.Event()
        self._aborted = threading.Event()
        self._error_lock = threading.Lock()
        self._error: Optional[str] = None

        self._pcm = bytearray()
        self._pcm_lock = threading.Lock()

        self._loop_ready = threading.Event()
        self._loop: Optional[asyncio.AbstractEventLoop] = None

        self._thread = threading.Thread(target=self._thread_main, name=f"ELWS:{stream_id}", daemon=True)
        self._thread.start()

        # Wait briefly for loop init so push() can schedule safely.
        self._loop_ready.wait(timeout=2.0)

    def is_done(self) -> bool:
        return self._done.is_set()

    def get_error(self) -> Optional[str]:
        with self._error_lock:
            return self._error

    def _set_error(self, msg: str) -> None:
        with self._error_lock:
            self._error = msg

    def abort(self) -> None:
        self._aborted.set()
        self._enqueue_ws_message_threadsafe(None)  # signal sender to close

        # Also try to stop the loop promptly
        if self._loop is not None:
            try:
                self._loop.call_soon_threadsafe(lambda: None)
            except Exception:
                pass

    def push_units(self, units: list[str]) -> None:
        """
        Enqueue one or more text units to the websocket sender.
        We include try_trigger_generation to reduce latency. :contentReference[oaicite:5]{index=5}
        """
        for u in units:
            s = (u or "").strip()
            if not s or self._aborted.is_set():
                continue
            self._enqueue_ws_message_threadsafe({"text": s, "try_trigger_generation": True})

    def finish(self) -> None:
        """
        Signal end-of-text to server by sending {"text": ""}. :contentReference[oaicite:6]{index=6}
        """
        if self._aborted.is_set():
            return
        self._enqueue_ws_message_threadsafe({"text": ""})

    # --- internals ---

    def _enqueue_ws_message_threadsafe(self, msg: Optional[dict[str, Any]]) -> None:
        if self._loop is None:
            # loop not ready yet; best-effort drop
            return

        def _put() -> None:
            self._out_q.put_nowait(msg)

        try:
            self._loop.call_soon_threadsafe(_put)
        except Exception:
            pass

    def _thread_main(self) -> None:
        try:
            asyncio.run(self._run())
        except Exception as e:
            self._set_error(f"stream thread crashed: {e}")
        finally:
            self._done.set()

    async def _run(self) -> None:
        self._loop = asyncio.get_running_loop()
        self._loop_ready.set()

        voice_id = (self.server.voice_prompt_wav or "").strip() or DEFAULT_VOICE_ID
        model_id = (self.server.model_id or "").strip() or DEFAULT_MODEL_ID
        output_format = (self.server.output_format or "").strip() or OUTPUT_FORMAT

        # Stream-input endpoint and message schema from ElevenLabs docs. :contentReference[oaicite:7]{index=7}
        uri = (
            f"wss://api.elevenlabs.io/v1/text-to-speech/{voice_id}/stream-input"
            f"?model_id={model_id}&output_format={output_format}"
        )

        # The docs show xi-api-key in headers and/or in init payload.
        # We'll do both for robustness; server-side only. :contentReference[oaicite:8]{index=8}
        headers = {"xi-api-key": ELEVENLABS_API_KEY}

        sample_rate = _parse_pcm_sample_rate(output_format)

        try:
            async with websockets.connect(uri, extra_headers=headers, ping_interval=20, ping_timeout=20) as ws:
                # Init message: must set voice settings first; docs use text=" " and xi_api_key in payload. :contentReference[oaicite:9]{index=9}
                init = {
                    "text": " ",
                    "xi_api_key": ELEVENLABS_API_KEY,
                    "voice_settings": {
                        "stability": VOICE_SETTINGS.stability,
                        "similarity_boost": VOICE_SETTINGS.similarity_boost,
                        "style": VOICE_SETTINGS.style,
                        "use_speaker_boost": VOICE_SETTINGS.use_speaker_boost,
                        "speed": VOICE_SETTINGS.speed,
                    },
                    "generation_config": {"chunk_length_schedule": CHUNK_LENGTH_SCHEDULE},
                }
                await ws.send(json.dumps(init))

                sender = asyncio.create_task(self._sender_loop(ws))
                receiver = asyncio.create_task(self._receiver_loop(ws))

                done, pending = await asyncio.wait(
                    {sender, receiver},
                    return_when=asyncio.FIRST_EXCEPTION,
                )

                for t in pending:
                    t.cancel()

                # Ensure connection closed promptly if we aborted
                if self._aborted.is_set():
                    try:
                        await ws.close()
                    except Exception:
                        pass

        except Exception as e:
            self._set_error(f"websocket error: {e}")
            return

        # If we got audio and we weren't aborted, emit a WAV file now
        if not self._aborted.is_set():
            with self._pcm_lock:
                pcm = bytes(self._pcm)

            if pcm:
                out_wav = TTS_OUT_DIR / f"stream_{self.stream_id}.wav"
                try:
                    _write_wav_s16le_mono(out_wav, pcm, sample_rate=sample_rate)
                    if self.server.player.pending_items() >= MAX_PLAY_QUEUE:
                        time.sleep(0.01)
                    self.server.player.enqueue(str(out_wav), delete_after=True)
                except Exception as e:
                    self._set_error(f"finalize wav failed: {e}")

        # Finally, tell server to remove the session (stream_status becomes done=True)
        self.server._end_stream_session(self.stream_id)

    async def _sender_loop(self, ws) -> None:
        while True:
            if self._aborted.is_set():
                # best-effort close sequence
                try:
                    await ws.send(json.dumps({"text": ""}))
                except Exception:
                    pass
                return

            msg = await self._out_q.get()
            if msg is None:
                # Abort signal from host
                try:
                    await ws.send(json.dumps({"text": ""}))
                except Exception:
                    pass
                return

            try:
                await ws.send(json.dumps(msg))
            except Exception as e:
                self._set_error(f"send failed: {e}")
                return

            # If this was the explicit end marker, sender can stop;
            # receiver will continue until isFinal. :contentReference[oaicite:10]{index=10}
            if msg.get("text", None) == "":
                return

    async def _receiver_loop(self, ws) -> None:
        while True:
            if self._aborted.is_set():
                return
            try:
                raw = await ws.recv()
            except Exception as e:
                self._set_error(f"recv failed: {e}")
                return

            try:
                data = json.loads(raw)
            except Exception:
                continue

            # WebSocket API returns base64 audio chunks and isFinal flag. :contentReference[oaicite:11]{index=11}
            a64 = data.get("audio")
            if a64:
                try:
                    chunk = base64.b64decode(a64)
                    if chunk:
                        with self._pcm_lock:
                            self._pcm += chunk
                        # Future hook: server/player streaming sink here
                except Exception:
                    pass

            if bool(data.get("isFinal", False)):
                return


# ----------------------------
# Streams (protocol wrapper)
# ----------------------------
class SpeechStream:
    """
    Protocol-facing stream object.
    Buffers partial text, segments it, and feeds units into ElevenLabsWSStream.
    """
    def __init__(self, server: "TTSServer", stream_id: str, ws: ElevenLabsWSStream) -> None:
        self.server = server
        self.stream_id = stream_id
        self._ws = ws

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

        self._ws.push_units(emit)

    def finish(self) -> None:
        with self._lock:
            if self._closed or self._aborted:
                return
            self._closed = True
            leftover = self._buf
            self._buf = ""

        units = hard_wrap(segment_stream(leftover), TTS_MAX_CHARS) if leftover else []
        if units:
            self._ws.push_units(units)

        self._ws.finish()

    def abort(self) -> None:
        with self._lock:
            self._aborted = True
            self._closed = True
            self._buf = ""
        self._ws.abort()


@dataclass
class Session:
    stream: SpeechStream
    stream_id: str
    ws: ElevenLabsWSStream


# ----------------------------
# Server
# ----------------------------
class TTSServer:
    def __init__(self) -> None:
        if not ELEVENLABS_API_KEY:
            raise RuntimeError("Missing ELEVENLABS_API_KEY env var (server-side).")

        print("[tts_server] Initializing ElevenLabs client (HTTP fallback for speak)...")
        self.eleven = ElevenLabs(api_key=ELEVENLABS_API_KEY)
        self._synth_lock = threading.Lock()

        self.player = GlobalAudioPlayer(playback_gap_s=PLAYBACK_GAP_S, max_queue=MAX_PLAY_QUEUE)
        self.synth_q = SynthesisQueue(self, maxsize=SYNTH_Q_MAX)

        self._sessions_lock = threading.Lock()
        self._sessions: dict[str, Session] = {}

        # Compatibility field name; stores voice_id now
        self.voice_prompt_wav = DEFAULT_VOICE_ID

        self.model_id = DEFAULT_MODEL_ID
        self.output_format = OUTPUT_FORMAT

    # --- HTTP synthesis for non-stream speak() ---
    def _synth_one_http(self, text: str, out_wav: Path) -> None:
        voice_id = (self.voice_prompt_wav or "").strip() or DEFAULT_VOICE_ID
        model_id = (self.model_id or "").strip() or DEFAULT_MODEL_ID
        output_format = (self.output_format or "").strip() or "pcm_22050"
        sample_rate = _parse_pcm_sample_rate(output_format)

        with self._synth_lock:
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

    def _end_stream_session(self, sid: str) -> None:
        with self._sessions_lock:
            self._sessions.pop(sid, None)

    def handle(self, req: dict[str, Any]) -> dict[str, Any]:
        cmd = (req.get("cmd") or "").strip()

        if cmd == "ping":
            return {"ok": True, "cmd": "ping"}

        if cmd == "get_state":
            with self._sessions_lock:
                active_streams = len(self._sessions)
            return {
                "ok": True,
                "cmd": "get_state",
                "is_playing": bool(self.player.is_playing()),
                "pending_audio": int(self.player.pending_items()),
                "pending_synth": int(self.synth_q.q.qsize()),
                "voice_prompt_wav": str(self.voice_prompt_wav),
                "active_streams": active_streams,
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

        # Non-stream speak(): still uses synth queue + HTTP convert()
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

        # Stream commands now create a WebSocket stream-input session
        if cmd in ("stream_begin", "begin_stream"):
            sid = ((req.get("id") or req.get("stream_id") or "").strip()) or uuid.uuid4().hex

            ws = ElevenLabsWSStream(self, stream_id=sid)
            stream = SpeechStream(self, stream_id=sid, ws=ws)

            with self._sessions_lock:
                self._sessions[sid] = Session(stream=stream, stream_id=sid, ws=ws)

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
            self._end_stream_session(sid)
            return {"ok": True}

        if cmd == "stream_status":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": True, "cmd": "stream_status", "stream_id": sid, "done": True}

            # "done" means session removed; report extra diagnostics non-breaking
            err = s.ws.get_error()
            return {"ok": True, "cmd": "stream_status", "stream_id": sid, "done": False, "error": err or ""}

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

    print(f"[tts_server] listening on {HOST}:{PORT} (ElevenLabs backend + WS stream-input)")
    print(f"[tts_server] default voice_id={srv.voice_prompt_wav} model_id={srv.model_id} output_format={srv.output_format}")

    while True:
        conn, addr = sock.accept()
        t = threading.Thread(target=_serve_client, args=(conn, addr, srv), daemon=True)
        t.start()


if __name__ == "__main__":
    main()
