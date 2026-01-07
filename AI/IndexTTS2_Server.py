
"""
tts_server.py
AI code

TTS TCP Service using IndexTTS2. Easily stream text to be broken into sentences and then played in an audioplayer.

* TCP: HOST:PORT (defaults: 127.0.0.1:17777)
* Encoding: UTF-8 text lines (one JSON object per line)
* Framing: newline-delimited (NDJSON)
* Send exactly one JSON object per line.
* Read exactly one JSON object per line as the response.
* The server is designed for a single producer client, but can handle multiple
  concurrent connections; ordering is preserved at the global synth queue.
* Synthesis is queued into a bounded FIFO queue (SYNTH_Q_MAX).
* If the queue is full, some commands (notably `speak` and stream enqueue) may
  reject work or drop newest stream units depending on implementation choices.
* The `speak` command returns {ok:false, error:"busy"} if no units were accepted.

All responses are JSON objects and include:
  - ok: bool (success/failure)
On failure:
  - error: str (human-readable error message)
Some responses echo:
  - cmd: str (command name)

=============================================================================
Commands
=============================================================================

ping
Health check.

    Request:
      {"cmd":"ping"}
    Response:
      {"ok":true,"cmd":"ping"}

    get_state
    ------------
    Returns server playback and queue status.

    Request:
      {"cmd":"get_state"}
    Response:
      {
        "ok": true,
        "cmd": "get_state",
        "is_playing": bool,         # GlobalAudioPlayer currently playing
        "pending_audio": int,       # number of queued audio items
        "pending_synth": int,       # number of queued synth jobs
        "voice_prompt_wav": str,    # current speaker prompt wav path
        "active_streams": int       # number of active SpeechStream sessions
      }

set_voice / set_voice_prompt
Sets the speaker prompt WAV used by IndexTTS2 for subsequent synthesis.

    Request:
        {"cmd":"set_voice_prompt","wav_path":"R:\\tts\\speaker.wav"}
        {"cmd":"set_voice","wav":"R:\\tts\\speaker.wav"}
    Response:
        {"ok":true,"cmd":"set_voice_prompt"}

    Notes:
    * The path is stored and used on future synthesis calls.
    * No existence/format validation is performed here unless you add it.


speak
One-shot TTS. Text is segmented into sentence-ish units and queued for
synthesis and playback in FIFO order.

    Request:
        {"cmd":"speak","text":"Hello there. This is queued."}

    Response (success):
        {"ok":true,"cmd":"speak","accepted":N}

    Response (busy / no units accepted):
        {"ok":false,"error":"busy"}

    Notes:
    * `accepted` is the number of segmented units accepted into the synth queue.
    * Audio plays asynchronously; `speak` does not wait for playback to finish.


begin_stream / stream_begin
Creates a streaming session for incremental text input.

    Request:
        {"cmd":"begin_stream"}
        {"cmd":"stream_begin","stream_id":"optional_id"}   # or {"id":"optional_id"}

    Response:
        {"ok":true,"cmd":"begin_stream","stream_id":"<id>"}

    Notes:
    * Use the returned stream_id for subsequent stream_* calls.
    * A stream buffers partial text and emits segmented units as text arrives.

stream_push
Pushes additional text into an existing stream. The stream will segment and
enqueue completed units into the global synth queue.

    Request:
        {"cmd":"stream_push","stream_id":"<id>","text":"More text ..."}
        # also accepts "id" instead of "stream_id"

    Response (success):
        {"ok":true}

    Response (missing / unknown id):
        {"ok":false,"error":"missing stream_id"}
        {"ok":false,"error":"unknown stream id: <id>"}

    Notes:
    * Streaming is best-effort under backpressure; newest units may be dropped if
    the synth queue is full (see SpeechStream.push()).


stream_finish
Closes the stream for input and flushes any remaining buffered text into the
synth queue (segmented and hard-wrapped). Playback continues asynchronously.

    Request:
        {"cmd":"stream_finish","stream_id":"<id>"}

    Response:
        {"ok":true}

    Notes:
    * After finish, additional stream_push calls are ignored by the stream object.
    * This implementation does not provide a "wait until done speaking" primitive.


stream_abort
Aborts a stream: clears buffered text and marks it closed. The session is
removed from the registry.

    Request:
        {"cmd":"stream_abort","stream_id":"<id>"}

    Response:
        {"ok":true}

    Notes:
    * This does NOT necessarily cancel already-queued synthesis/playback from other
      sources unless you explicitly call stop_and_clear or implement per-stream
      cancellation. It only stops further enqueues from this stream.


stream_status
Minimal status: reports whether the stream session still exists in the server’s
registry. In this simplified model, "exists" is treated as not-done.

    Request:
        {"cmd":"stream_status","stream_id":"<id>"}

    Response:
        {"ok":true,"cmd":"stream_status","stream_id":"<id>","done":true|false}

    Notes:
    * done=true means the stream id is not present in the session registry.
    * It does NOT mean audio playback for that stream has finished.


stop_all
Stops current playback immediately. Does not clear queued synthesis jobs and
may not clear queued audio items unless GlobalAudioPlayer implements stop as a
flush (typically it does not).

    Request:
        {"cmd":"stop_all"}
    Response:
        {"ok":true,"cmd":"stop_all"}


stop_and_clear
Hard reset:
* stop current playback immediately
* clear pending audio queue
* clear pending synthesis queue
* abort all active streams and clear session registry

    Request:
        {"cmd":"stop_and_clear"}
    Response:
        {"ok":true,"cmd":"stop_and_clear"}
"""

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
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Optional

from indextts.infer_v2 import IndexTTS2
from wildcat_py.global_audio_player import GlobalAudioPlayer


# ----------------------------
# Config
# ----------------------------
HOST = "127.0.0.1"
PORT = 17777

VOICE_PROMPT_WAV = r"R:\tts\yuki2-short.wav"
INDEX_CFG_PATH = "checkpoints/config.yaml"
INDEX_MODEL_DIR = "checkpoints"

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
            # If you prefer strict behavior, block until room is available.
            if self.server.player.pending_items() >= MAX_PLAY_QUEUE:
                # best-effort small wait to avoid runaway latency
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

            # We segment the whole buffer, then keep a tail if it looks incomplete.
            units = hard_wrap(segment_stream(self._buf), TTS_MAX_CHARS)
            if not units:
                return

            # Tail-hold heuristic: keep last unit if buffer doesn't look “finished”.
            # (Breaking-change behavior: keep it simple and deterministic.)
            buf_end = self._buf[-1]
            finished = buf_end.isspace() or buf_end in ".!?\n\r\t"
            if not finished and len(units) >= 2:
                emit, tail = units[:-1], units[-1]
                self._buf = tail
            else:
                emit = units
                self._buf = ""

        # Enqueue outside lock
        for u in emit:
            if not u.strip():
                continue
            job = SynthJob(job_id=uuid.uuid4().hex, stream_id=self.stream_id, text=u, created_s=time.time())
            ok = self.server.synth_q.put(job, block=False)
            if not ok:
                # If busy, you can either drop, block briefly, or mark stream errored.
                # Here: drop newest to prevent runaway latency.
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
        print("[tts_server] Loading IndexTTS2...")
        self.tts = IndexTTS2(
            cfg_path=INDEX_CFG_PATH,
            model_dir=INDEX_MODEL_DIR,
            use_fp16=True,
            use_cuda_kernel=True,
            use_deepspeed=False,
        )
        self._synth_lock = threading.Lock()
        self.player = GlobalAudioPlayer(playback_gap_s=PLAYBACK_GAP_S, max_queue=MAX_PLAY_QUEUE)

        self.synth_q = SynthesisQueue(self, maxsize=SYNTH_Q_MAX)

        self._sessions_lock = threading.Lock()
        self._sessions: dict[str, Session] = {}

        self.voice_prompt_wav = VOICE_PROMPT_WAV

    def _synth_one(self, text: str, out_wav: Path) -> None:
        with self._synth_lock:
            self.tts.infer(
                text=text,
                spk_audio_prompt=self.voice_prompt_wav,
                output_path=str(out_wav),
            )

    def set_voice_prompt(self, wav_path: str) -> None:
        self.voice_prompt_wav = wav_path

    def stop_and_clear(self) -> None:
        # Stop playback
        self.player.stop_now()
        self.player.clear_pending()

        # Clear synthesis jobs
        self.synth_q.clear_pending()

        # Abort streams and clear registry
        with self._sessions_lock:
            sessions = list(self._sessions.values())
            self._sessions.clear()
        for s in sessions:
            try:
                s.stream.abort()
            except Exception:
                pass

        # If you want safe cleanup, track created files instead of globbing.
        # Leaving filesystem alone here is usually safer.

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
                "voice_prompt_wav": str(self.voice_prompt_wav),
                "active_streams": len(self._sessions),
            }

        if cmd in ("set_voice", "set_voice_prompt"):
            wav = (req.get("wav") or req.get("wav_path") or "").strip()
            if not wav:
                return {"ok": False, "error": "missing wav path"}
            self.set_voice_prompt(wav)
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

            # speak uses same pipeline as streams
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
            done = s is None  # in this simplified model, status is “exists or not”
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
        try: f_r.close()
        except Exception: pass
        try: f_w.close()
        except Exception: pass
        try: conn.close()
        except Exception: pass


def main() -> None:
    srv = TTSServer()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((HOST, PORT))
    sock.listen(8)

    print(f"[tts_server] listening on {HOST}:{PORT}")

    while True:
        conn, addr = sock.accept()
        t = threading.Thread(target=_serve_client, args=(conn, addr, srv), daemon=True)
        t.start()


if __name__ == "__main__":
    main()
