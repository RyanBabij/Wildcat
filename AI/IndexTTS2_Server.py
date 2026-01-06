"""
IndexTTS2-Server.py
AI code

Networked text-to-speech service built on IndexTTS2 with guaranteed
sequential audio playback and low-latency streaming support.

Purpose
-------
Provides a TCP-based TTS server that:
- Accepts JSON commands over a socket interface.
- Performs speech synthesis using IndexTTS2.
- Streams or batches synthesized speech.
- Routes all audio through a single global playback queue to prevent overlap.

Key Capabilities
----------------
- Two speech modes:
    * speak: non-streaming, batch synthesis.
    * streaming: incremental synthesis while text is still arriving.
- Global audio serialization via GlobalAudioPlayer:
    * Ensures zero overlap between clips.
    * Supports immediate interruption and queue clearing.
- Backpressure control:
    * Prevents unbounded playback latency when synthesis outruns audio output.
- Session-based stream handling:
    * Each stream has its own ingestion buffer and worker thread.
- Hard and soft stop semantics:
    * stop_all: stops current playback only.
    * stop_and_clear: stops playback, clears queues, aborts all streams.

Protocol Overview
-----------------
Communication is newline-delimited JSON over TCP.

Common commands:
- ping
- speak
- begin_stream / stream_begin
- stream_push
- stream_finish
- stream_abort
- stop_all
- stop_and_clear
- get_state
- set_voice / set_voice_prompt

Example request:
    {"cmd": "speak", "text": "Hello world"}

Threading Model
---------------
- One server thread accepts clients.
- Each client handled in its own thread.
- Each active stream owns a worker thread for synthesis.
- IndexTTS2 inference is serialized via a global lock to ensure GPU stability.
- Audio playback runs in a dedicated background thread.

Design Constraints
------------------
- Only one audio clip may play at a time.
- All audio output is WAV, float32 PCM.
- Intended for assistant systems, voice agents, and TTS pipelines where:
    * Speech order matters.
    * Interruptibility is required.
    * Streaming latency must be bounded.
"""

from __future__ import annotations

import json
import os
import queue
import re
import socket
import threading
import traceback
import uuid
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Optional

from indextts.infer_v2 import IndexTTS2

from wildcat import GlobalAudioPlayer



# ----------------------------
# Server config
# ----------------------------
HOST = "127.0.0.1"
PORT = 17777

# ----------------------------
# IndexTTS2 config
# ----------------------------
VOICE_PROMPT_WAV = r"R:\voice.wav"
INDEX_CFG_PATH = "checkpoints/config.yaml"
INDEX_MODEL_DIR = "checkpoints"

# ----------------------------
# TTS tuning
# ----------------------------
TTS_MAX_CHARS = 320
END_OF_TRANSMISSION = ""  # optional marker; keep "" to disable
PLAYBACK_GAP_S = 0.8  # pause between wavs (seconds)
TTS_OUT_DIR = Path(r"R:\outputs_tts_server")
TTS_OUT_DIR.mkdir(parents=True, exist_ok=True)

# Simple “don’t let playback latency grow unbounded” queue limits
MAX_PLAY_QUEUE = 256
PLAY_Q_HIGH_WATER = 192  # backpressure threshold for synth threads


_WS_RE = re.compile(r"\s+")
_SENT_SPLIT_RE = re.compile(r"(?<=[.!?:-—])(?:[ \t]+|\n+)")
_STREAM_SENT_RE = re.compile(r"(?<=[.!?])\s+")


def split_text(text: str, max_chars: int = TTS_MAX_CHARS) -> list[str]:
    """Used by non-streaming speak()."""
    text = _WS_RE.sub(" ", text).strip()
    if not text:
        return []

    parts = _SENT_SPLIT_RE.split(text)
    chunks: list[str] = []
    buf = ""

    def flush() -> None:
        nonlocal buf
        b = buf.strip()
        if b:
            chunks.append(b)
        buf = ""

    for p in parts:
        if not p:
            continue
        if len(buf) + 1 + len(p) <= max_chars:
            buf = f"{buf} {p}".strip()
        else:
            flush()
            if len(p) <= max_chars:
                buf = p
            else:
                for i in range(0, len(p), max_chars):
                    c = p[i : i + max_chars].strip()
                    if c:
                        chunks.append(c)
                buf = ""
    flush()
    return chunks


def split_stream_units(text: str, max_chars: int = TTS_MAX_CHARS) -> list[str]:
    """Streaming splitter: newlines -> sentence-ish units -> hard wrap."""
    if not text:
        return []

    raw_lines = text.replace("\r\n", "\n").replace("\r", "\n").split("\n")
    units: list[str] = []

    for line in raw_lines:
        line = line.strip()
        if not line:
            continue

        parts = _STREAM_SENT_RE.split(line)
        for p in parts:
            p = p.strip()
            if not p:
                continue

            if len(p) <= max_chars:
                units.append(p)
            else:
                for i in range(0, len(p), max_chars):
                    chunk = p[i : i + max_chars].strip()
                    if chunk:
                        units.append(chunk)

    return units


@dataclass
class Session:
    stream: Any  # SpeechStream
    stream_id: str


class SpeechStream:
    """
    Per-stream text ingestion + synth worker.
    Synth worker writes wavs and enqueues them to the GLOBAL audio player.
    """

    _TEXT_END: Optional[str] = None

    def __init__(self, server: "TTSServer", stream_id: str) -> None:
        self.server = server
        self.stream_id = stream_id

        self._buf = ""
        self.text_q: "queue.Queue[Optional[str]]" = queue.Queue()

        self._closed_in = threading.Event()
        self._aborted = threading.Event()

        self._t = threading.Thread(target=self._worker, daemon=True)
        self._t.start()

    def push(self, text: str) -> None:
        if self._aborted.is_set() or self._closed_in.is_set():
            return
        s = text or ""
        if not s:
            return

        self._buf += s

        units = split_stream_units(self._buf, TTS_MAX_CHARS)
        if not units:
            return

        flush_all = bool(self._buf and (self._buf[-1].isspace() or self._buf[-1] in ".!?"))
        if not flush_all and len(units) >= 2:
            to_send, tail = units[:-1], units[-1]
            self._buf = tail
        else:
            to_send = units
            self._buf = ""

        for u in to_send:
            if self._aborted.is_set() or self._closed_in.is_set():
                return
            if u.strip():
                self.text_q.put(u)

    def finish(self) -> None:
        if self._closed_in.is_set():
            return
        self._closed_in.set()

        # Flush any remaining buffered text
        if self._buf:
            units = split_stream_units(self._buf, TTS_MAX_CHARS)
            for u in units:
                if self._aborted.is_set():
                    return
                if u and u.strip():
                    self.text_q.put(u)
            self._buf = ""

        self.text_q.put(self._TEXT_END)

    def abort(self) -> None:
        # Your previous behavior: abort stops current playback too.
        self.server.player.stop_now()

        if self._aborted.is_set():
            return
        self._aborted.set()
        self._closed_in.set()

        try:
            while True:
                self.text_q.get_nowait()
        except queue.Empty:
            pass

        self.text_q.put(self._TEXT_END)

    def wait(self, timeout: Optional[float] = None) -> bool:
        self._t.join(timeout=timeout)
        return not self._t.is_alive()

    def _worker(self) -> None:
        i = 0
        while not self._aborted.is_set():
            # Backpressure so we don't build minutes of latency
            if self.server.player.pending_items() >= PLAY_Q_HIGH_WATER:
                self._aborted.wait(timeout=0.02)
                continue

            text = self.text_q.get()
            if text is self._TEXT_END:
                break

            if not text or not text.strip():
                continue

            base = uuid.uuid4().hex
            out_wav = TTS_OUT_DIR / f"stream_{self.stream_id}_{base}_{i}.wav"
            i += 1

            try:
                self.server._synth_one(text, out_wav)
            except Exception:
                traceback.print_exc()
                continue

            if self._aborted.is_set():
                break

            # enqueue and delete after load to save space
            self.server.player.enqueue(str(out_wav), delete_after=True)


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

        # Serialize IndexTTS2 inference (thread safety / GPU stability)
        self._synth_lock = threading.Lock()

        # Global playback-only player
        self.player = GlobalAudioPlayer(playback_gap_s=PLAYBACK_GAP_S, max_queue=MAX_PLAY_QUEUE)

        # Sessions
        self._sessions_lock = threading.Lock()
        self._sessions: dict[str, Session] = {}

        self.voice_prompt_wav = VOICE_PROMPT_WAV

    def _get_session(self, sid: str) -> Optional[Session]:
        with self._sessions_lock:
            return self._sessions.get(sid)

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
        """
        Hard reset:
          - stop current playback immediately
          - clear pending global audio
          - abort every active stream (clears their text queues)
          - clear the session registry
        """
        self.player.stop_now()
        self.player.clear_pending()

        with self._sessions_lock:
            sessions = list(self._sessions.values())
            self._sessions.clear()

        for s in sessions:
            try:
                s.stream.abort()
            except Exception:
                pass

        # Optional: best-effort cleanup of stray wavs
        # (Only safe if TTS_OUT_DIR is dedicated to this service.)
        try:
            for p in TTS_OUT_DIR.glob("stream_*.wav"):
                try:
                    os.remove(p)
                except OSError:
                    pass
            for p in TTS_OUT_DIR.glob("tts_*.wav"):
                try:
                    os.remove(p)
                except OSError:
                    pass
        except Exception:
            pass

    def handle(self, req: dict[str, Any]) -> dict[str, Any]:
        cmd = (req.get("cmd") or "").strip()

        if cmd == "ping":
            return {"ok": True, "cmd": "ping"}

        # Soft stop: stops current playback only
        if cmd == "stop_all":
            self.player.stop_now()
            return {"ok": True, "cmd": "stop_all"}

        # Hard stop: stops playback and clears global queue + stream queues + sessions
        if cmd == "stop_and_clear":
            self.stop_and_clear()
            return {"ok": True, "cmd": "stop_and_clear"}

        if cmd == "get_state":
            try:
                return {
                    "ok": True,
                    "cmd": "get_state",
                    "is_playing": bool(self.player.is_playing()),
                    "pending_audio": int(self.player.pending_items()),
                    "voice_prompt_wav": str(self.voice_prompt_wav),
                    "active_streams": len(self._sessions),
                }
            except Exception as e:
                return {"ok": False, "error": f"get_state failed: {e}"}

        if cmd in ("set_voice", "set_voice_prompt"):
            wav = (req.get("wav") or req.get("wav_path") or "").strip()
            if not wav:
                return {"ok": False, "error": "missing wav path"}
            self.set_voice_prompt(wav)
            return {"ok": True, "cmd": cmd}

        if cmd == "speak":
            text = (req.get("text") or "").strip()
            if not text:
                return {"ok": False, "error": "missing text"}

            def _run() -> None:
                try:
                    t = text
                    if END_OF_TRANSMISSION:
                        t = f"{t.strip()} {END_OF_TRANSMISSION}".strip()

                    chunks = split_text(t, TTS_MAX_CHARS)
                    for idx, chunk in enumerate(chunks):
                        base = uuid.uuid4().hex
                        out_wav = TTS_OUT_DIR / f"tts_{base}_{idx}.wav"
                        try:
                            self._synth_one(chunk, out_wav)
                        except Exception:
                            traceback.print_exc()
                            continue
                        self.player.enqueue(str(out_wav), delete_after=True)
                except Exception:
                    traceback.print_exc()

            threading.Thread(target=_run, daemon=True).start()
            return {"ok": True, "cmd": "speak"}

        if cmd in ("stream_begin", "begin_stream"):
            sid = (
                (req.get("id") or "").strip()
                or (req.get("stream_id") or "").strip()
                or str(uuid.uuid4().hex)
            )
            stream = SpeechStream(self, stream_id=sid)
            with self._sessions_lock:
                self._sessions[sid] = Session(stream=stream, stream_id=sid)
            return {"ok": True, "cmd": cmd, "id": sid, "stream_id": sid}

        if cmd == "stream_push":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            text = (req.get("text") or "")
            if not sid:
                return {"ok": False, "error": "missing id/stream_id"}

            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}

            s.stream.push(text)
            return {"ok": True}

        if cmd == "stream_finish":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing id/stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}
            s.stream.finish()
            return {"ok": True}

        if cmd == "stream_abort":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing id/stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}
            try:
                s.stream.abort()
            finally:
                with self._sessions_lock:
                    self._sessions.pop(sid, None)
            return {"ok": True}

        if cmd == "stream_wait":
            sid = (req.get("id") or req.get("stream_id") or "").strip()
            timeout = req.get("timeout", None)
            if not sid:
                return {"ok": False, "error": "missing id/stream_id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": False, "error": f"unknown stream id: {sid}"}
            try:
                done = s.stream.wait(timeout=timeout)
            except Exception as e:
                return {"ok": False, "error": f"wait failed: {e}"}
            if done:
                with self._sessions_lock:
                    self._sessions.pop(sid, None)
            return {"ok": True, "cmd": "stream_wait", "id": sid, "stream_id": sid, "done": bool(done)}

        if cmd == "stream_status":
            sid = (req.get("stream_id") or req.get("id") or "").strip()
            if not sid:
                return {"ok": False, "error": "missing stream_id/id"}
            s = self._get_session(sid)
            if s is None:
                return {"ok": True, "cmd": "stream_status", "stream_id": sid, "done": True}
            done = bool(s.stream.wait(timeout=0.0))
            if done:
                with self._sessions_lock:
                    self._sessions.pop(sid, None)
            return {"ok": True, "cmd": "stream_status", "stream_id": sid, "done": done}

        return {"ok": False, "error": f"unknown cmd: {cmd}"}


def _serve_client(conn: socket.socket, addr, server: TTSServer) -> None:
    f_r = conn.makefile("r", encoding="utf-8", newline="\n")
    f_w = conn.makefile("w", encoding="utf-8", newline="\n")
    try:
        while True:
            try:
                line = f_r.readline()
            except (ConnectionResetError, ConnectionAbortedError, OSError):
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

    print(f"[tts_server] listening on {HOST}:{PORT}")

    while True:
        conn, addr = sock.accept()
        t = threading.Thread(target=_serve_client, args=(conn, addr, srv), daemon=True)
        t.start()


if __name__ == "__main__":
    main()
    