
"""
IndexTTS2_Client.py
Wildcat/AI/IndexTTS2_Client.py
Wildcat.AI.IndexTTS2_Client
AI Code

===============================================================================
PUBLIC API (Client-side)
===============================================================================

TTSClient
---------
Constructor
    TTSClient(
        host: str = "127.0.0.1",
        port: int = 8765,
        *,
        poll_interval_s: float = 0.15,
        request_timeout_s: float = 3.0,
        speak_timeout_s: float = 10.0,
    )

playback_lock: threading.RLock - External mutual exclusion for audio operations.
is_playing: bool - Best-effort mirror of remote playback state.
voice_prompt_wav: str - Best-effort mirror of currently selected voice prompt.

close() - Stop background poller thread.
get_state() -> RemoteState- Query authoritative playback state from server.
stop_all() - Stop playback immediately.
stop_and_clear() - Stop playback and clear queued audio.
set_voice_prompt(wav_path: str) - Change active voice prompt on the server.
speak(text: str) - Blocking call. Sends text to server and waits for completion.
begin_stream() -> TTSClient.Stream - Start a streaming TTS session.

---------------------------------------------------------------------------
TTSClient.Stream
---------------------------------------------------------------------------

push(sentence: str) - Non-blocking. Enqueue a sentence for streaming playback.
finish() - Signal end of stream. Best-effort, non-blocking.
wait(timeout: Optional[float] = None) -> bool - Poll server until stream is complete or timeout expires.

===============================================================================
WIRE PROTOCOL (Client ⇄ Server)
===============================================================================

Transport
    Newline-delimited JSON over TCP.

General form
    Request:  {"cmd": "...", ...}\\n
    Response: {"ok": true, ...}\\n

Commands sent by this client
----------------------------
get_state -> { ok, is_playing: bool, voice_prompt_wav: str }
stop_all -> { ok }
stop_and_clear -> { ok }
set_voice_prompt -> { ok }
speak -> { ok }
begin_stream -> { ok, stream_id }
stream_push (fire-and-forget) -> no response expected
stream_finish (fire-and-forget) -> no response expected
stream_status -> { ok, done: bool }

===============================================================================
DESIGN NOTES
===============================================================================
- Protocol/transport is isolated in JsonLineClient.
- speak() is intentionally blocking; streaming calls are non-blocking.
- stream_push/stream_finish use fire-and-forget semantics to avoid stalling
  GPT token streaming or UI threads.
- Local is_playing/voice_prompt_wav are convenience mirrors; the server is the
  source of truth.
===============================================================================
"""

from __future__ import annotations

import json
import queue
import socket
import threading
import time
from dataclasses import dataclass
from typing import Any, Optional


# =============================================================================
# Data model
# =============================================================================

@dataclass(frozen=True)
class RemoteState:
    is_playing: bool = False
    voice_prompt_wav: str = ""


# =============================================================================
# Transport
# =============================================================================

class JsonLineClient:
    """
    Minimal newline-delimited JSON client.
    - request(): send payload, wait for one JSON line response
    - send_only(): best-effort send payload, do not wait for a response
    """

    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port

    def request(self, payload: dict[str, Any], *, timeout: float = 3.0) -> dict[str, Any]:
        data = (json.dumps(payload) + "\n").encode("utf-8")

        with socket.create_connection((self.host, self.port), timeout=timeout) as s:
            s.sendall(data)

            buf = b""
            # read one line (newline-delimited protocol)
            while b"\n" not in buf:
                chunk = s.recv(65536)
                if not chunk:
                    break
                buf += chunk

        line = buf.split(b"\n", 1)[0].decode("utf-8", errors="replace").strip()
        if not line:
            return {"ok": False, "error": "empty response"}

        try:
            obj = json.loads(line)
        except Exception:
            return {"ok": False, "error": f"bad json: {line[:200]}"}

        if not isinstance(obj, dict):
            return {"ok": False, "error": "response was not a JSON object"}
        return obj

    def send_only(self, payload: dict[str, Any], *, timeout: float = 0.25) -> None:
        """
        Fire-and-forget. Best-effort.
        Intentionally does not recv() to avoid stalling the caller thread.
        """
        data = (json.dumps(payload) + "\n").encode("utf-8")
        try:
            with socket.create_connection((self.host, self.port), timeout=timeout) as s:
                s.sendall(data)
                try:
                    s.shutdown(socket.SHUT_WR)
                except Exception:
                    pass
        except Exception:
            # best-effort; caller must remain non-blocking
            pass


# =============================================================================
# Client
# =============================================================================

class TTSClient:
    """
    Remote IndexTTS2 controller.

    Exposed, compatibility-oriented surface:
      - playback_lock (RLock)
      - is_playing (best-effort shadow state; also polled from server)
      - voice_prompt_wav (best-effort shadow state; also polled from server)
      - close()
      - get_state()
      - stop_all()
      - stop_and_clear()
      - set_voice_prompt()
      - speak()
      - begin_stream()

    Notes
    - The local is_playing/voice_prompt_wav are NOT authoritative. They are convenience mirrors.
    - speak() is a blocking request (server may block until audio drains); this is intentional.
    - stream_push/stream_finish are sent via send_only() to keep GPT streaming responsive.
    """

    def __init__(
        self,
        host: str = "127.0.0.1",
        port: int = 8765,
        *,
        poll_interval_s: float = 0.15,
        request_timeout_s: float = 3.0,
        speak_timeout_s: float = 10.0,
    ) -> None:
        self._rpc = JsonLineClient(host, port)
        self._request_timeout_s = float(request_timeout_s)
        self._speak_timeout_s = float(speak_timeout_s)

        self.playback_lock = threading.RLock()

        self._state_lock = threading.Lock()
        self.is_playing: bool = False
        self.voice_prompt_wav: str = ""

        self._poll_interval_s = float(poll_interval_s)
        self._stop = threading.Event()
        self._poll_thread = threading.Thread(target=self._poll_loop, name="TTSClientPoller", daemon=True)
        self._poll_thread.start()

    # -------------------------
    # Lifecycle
    # -------------------------

    def close(self) -> None:
        self._stop.set()

    # -------------------------
    # State polling
    # -------------------------

    def _poll_loop(self) -> None:
        while not self._stop.is_set():
            try:
                st = self.get_state()
                with self._state_lock:
                    self.is_playing = st.is_playing
                    if st.voice_prompt_wav:
                        self.voice_prompt_wav = st.voice_prompt_wav
            except Exception:
                with self._state_lock:
                    self.is_playing = False

            self._stop.wait(self._poll_interval_s)

    # -------------------------
    # RPC helpers
    # -------------------------

    def _call(self, cmd: str, payload: Optional[dict[str, Any]] = None, *, timeout: Optional[float] = None) -> dict[str, Any]:
        body: dict[str, Any] = {"cmd": cmd}
        if payload:
            body.update(payload)

        t = self._request_timeout_s if timeout is None else float(timeout)
        resp = self._rpc.request(body, timeout=t)
        return resp

    @staticmethod
    def _require_ok(resp: dict[str, Any], context: str) -> dict[str, Any]:
        if resp.get("ok"):
            return resp
        raise RuntimeError(resp.get("error") or f"{context} failed")

    def _set_local_playing(self, value: bool) -> None:
        with self._state_lock:
            self.is_playing = bool(value)

    # -------------------------
    # Public API
    # -------------------------

    def get_state(self) -> RemoteState:
        resp = self._call("get_state")
        self._require_ok(resp, "get_state")
        return RemoteState(
            is_playing=bool(resp.get("is_playing", False)),
            voice_prompt_wav=str(resp.get("voice_prompt_wav", "")),
        )

    def stop_all(self) -> None:
        resp = self._call("stop_all")
        self._require_ok(resp, "stop_all")
        self._set_local_playing(False)

    def stop_and_clear(self) -> None:
        resp = self._call("stop_and_clear")
        self._require_ok(resp, "stop_and_clear")
        self._set_local_playing(False)

    def set_voice_prompt(self, wav_path: str) -> None:
        resp = self._call("set_voice_prompt", {"wav_path": wav_path})
        self._require_ok(resp, "set_voice_prompt")
        with self._state_lock:
            self.voice_prompt_wav = str(wav_path)

    def speak(self, text: str) -> None:
        # speak is treated as an exclusive operation from the caller perspective
        with self.playback_lock:
            self._set_local_playing(True)
            try:
                resp = self._call("speak", {"text": text}, timeout=self._speak_timeout_s)
                self._require_ok(resp, "speak")
            except Exception:
                self._set_local_playing(False)
                raise

    # =============================================================================
    # Streaming
    # =============================================================================

    class Stream:
        """
        Sentence streaming helper.

        Design goals:
        - push() is non-blocking
        - network sends happen in a single worker to preserve order
        - finish() is best-effort and does not block long
        - wait() optionally polls the server for done=true
        """

        def __init__(self, parent: "TTSClient", stream_id: str) -> None:
            self._parent = parent
            self.stream_id = stream_id

            self._q: "queue.Queue[Optional[str]]" = queue.Queue()
            self._stop = threading.Event()
            self._sentinel_enqueued = False

            self._sender_done = threading.Event()
            self._sender = threading.Thread(target=self._send_loop, name=f"TTSStreamSender:{stream_id}", daemon=True)
            self._sender.start()

        def _send_loop(self) -> None:
            try:
                while not self._stop.is_set():
                    try:
                        item = self._q.get(timeout=0.1)
                    except queue.Empty:
                        continue

                    if item is None:
                        break

                    # preserve order via single sender thread
                    self._parent._rpc.send_only(
                        {"cmd": "stream_push", "stream_id": self.stream_id, "text": item},
                        timeout=0.25,
                    )
            finally:
                self._sender_done.set()

        def push(self, sentence: str) -> None:
            s = (sentence or "").strip()
            if not s:
                return
            if self._sentinel_enqueued:
                # ignore pushes after finish() was requested
                return
            self._q.put(s)

        def finish(self) -> None:
            if not self._sentinel_enqueued:
                self._sentinel_enqueued = True
                self._q.put(None)

            self._stop.set()

            # bounded wait to let sender flush queued pushes
            self._sender_done.wait(timeout=1.0)

            # fire-and-forget finish (do not block GPT path)
            self._parent._rpc.send_only(
                {"cmd": "stream_finish", "stream_id": self.stream_id},
                timeout=0.25,
            )

        def wait(self, timeout: Optional[float] = None) -> bool:
            t0 = time.monotonic()
            while True:
                resp = self._parent._call("stream_status", {"stream_id": self.stream_id})
                if resp.get("ok") and resp.get("done"):
                    return True

                if timeout is not None and (time.monotonic() - t0) >= float(timeout):
                    return False

                time.sleep(0.05)

    def begin_stream(self) -> "TTSClient.Stream":
        with self.playback_lock:
            self._set_local_playing(True)
            try:
                resp = self._call("begin_stream")
                self._require_ok(resp, "begin_stream")
                stream_id = str(resp.get("stream_id") or "")
                if not stream_id:
                    raise RuntimeError("begin_stream failed: missing stream_id")
                return TTSClient.Stream(self, stream_id)
            except Exception:
                self._set_local_playing(False)
                raise
