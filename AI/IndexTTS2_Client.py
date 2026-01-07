
"""
IndexTTS2_Client.py
Wildcat/AI/IndexTTS2_Client.py
Wildcat.AI.IndexTTS2_Client
AI Code

Client to interface with my TTS Server.

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
    )

Attributes
    playback_lock: threading.RLock - External mutual exclusion for audio operations.
    is_playing: bool - Best-effort mirror of remote playback state.
    voice_prompt_wav: str - Best-effort mirror of currently selected voice prompt.
    pending_audio: int - Number of queued audio items on the server.
    pending_synth: int - Number of queued synthesis jobs.
    active_streams: int - Number of active streaming sessions.

Lifecycle
    close() - Stop background poller thread.

Health / State
    ping() - Health check. Raises on failure.
    get_state() -> RemoteState - Query authoritative playback and queue state from server.

Control
    stop_all() - Stop current playback immediately.
    stop_and_clear() - Hard reset: stop playback, clear queues, abort all streams.

Voice
    set_voice_prompt(wav_path: str) - Set speaker prompt WAV for future synthesis.
    set_voice(wav_path: str) - Alias for set_voice_prompt using {"cmd":"set_voice"}.

Speech
    speak(text: str) -> int
        Asynchronous server-side TTS.
        Returns number of segmented units accepted into the synth queue.
        Raises RuntimeError if server is busy.

Streaming
    begin_stream(stream_id: Optional[str] = None) -> TTSClient.Stream
        Start a streaming TTS session.

---------------------------------------------------------------------------
TTSClient.Stream
---------------------------------------------------------------------------

push(text: str) -> None
    Non-blocking. Enqueue text for streaming synthesis.

finish() -> None
    Close the stream for input and flush remaining buffered text.

abort() -> None
    Abort the stream session and clear buffered text on the server.

wait_gone(timeout: Optional[float] = None) -> bool
    Poll stream_status until done==True.
    NOTE: done only means the stream session no longer exists;
          it does NOT guarantee audio playback has finished.

===============================================================================
WIRE PROTOCOL (Client ⇄ Server)
===============================================================================

Newline-delimited JSON over TCP.

Request:  {"cmd": "...", ...}\\n
Response: {"ok": true, ...}\\n

Commands sent by this client
----------------------------

ping -> { ok, cmd }
get_state ->
{
    ok,
    is_playing: bool,
    pending_audio: int,
    pending_synth: int,
    voice_prompt_wav: str,
    active_streams: int
}
set_voice_prompt -> { ok }
set_voice -> { ok }
speak -> { ok, accepted: int } | { ok:false, error:"busy" }
begin_stream / stream_begin -> { ok, stream_id }
stream_push -> { ok } fire and forget
stream_finish -> { ok } fire and forget
stream_abort -> { ok }
stream_status -> { ok, done: bool }
stop_all -> { ok }
stop_and_clear -> { ok }

DESIGN NOTES
* Protocol/transport is isolated in JsonLineClient.
* speak() queues synthesis and playback asynchronously; it does not wait for audio.
* stream_push/stream_finish are fire-and-forget to avoid stalling GPT or UI threads.
* Stream.wait_gone() only tracks session lifetime, not playback completion.
* Local state mirrors (is_playing, voice_prompt_wav, etc.) are conveniences;
  the server remains the source of truth.
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
    pending_audio: int = 0
    pending_synth: int = 0
    voice_prompt_wav: str = ""
    active_streams: int = 0


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
            pass


# =============================================================================
# Client
# =============================================================================

class TTSClient:
    """
    Remote controller for the IndexTTS2 server.

    Compatibility-oriented surface:
      - playback_lock
      - is_playing
      - voice_prompt_wav
      - stop_all()
      - stop_and_clear()
      - set_voice_prompt()
      - speak()          (async server-side)
      - begin_stream()
    """

    def __init__(
        self,
        host: str = "127.0.0.1",
        port: int = 8765,
        *,
        poll_interval_s: float = 0.15,
        request_timeout_s: float = 3.0,
    ) -> None:
        self._rpc = JsonLineClient(host, port)
        self._request_timeout_s = float(request_timeout_s)

        self.playback_lock = threading.RLock()

        self._state_lock = threading.Lock()
        self.is_playing: bool = False
        self.voice_prompt_wav: str = ""
        self.pending_audio: int = 0
        self.pending_synth: int = 0
        self.active_streams: int = 0

        self._poll_interval_s = float(poll_interval_s)
        self._stop = threading.Event()
        self._poll_thread = threading.Thread(target=self._poll_loop, name="TTSClientPoller", daemon=True)
        self._poll_thread.start()

    # -------------------------
    # Lifecycle
    # -------------------------

    def close(self, *, stop_and_clear: bool = True, timeout_s: float = 1.0) -> None:
        """
        Shutdown the client cleanly.

        - Stops background poller thread.
        - Optionally asks the server to stop_and_clear() to wipe any backlog.

        Never raises. Safe to call multiple times.
        """
        # Idempotent stop signal
        self._stop.set()

        # Best-effort: wipe server queues so we don't leave speech hanging after exit.
        # Do this BEFORE join so we don't depend on the poller.
        if stop_and_clear:
            try:
                # Don't take playback_lock here; close() may be called from shutdown paths
                # where other threads might be stuck holding it.
                self._call("stop_and_clear", timeout=min(self._request_timeout_s, max(0.1, float(timeout_s))))
            except Exception:
                pass

        # Ensure the poll thread exits
        try:
            self._poll_thread.join(timeout=max(0.0, float(timeout_s)))
        except Exception:
            pass


    # -------------------------
    # Internal helpers
    # -------------------------

    def _call(self, cmd: str, payload: Optional[dict[str, Any]] = None, *, timeout: Optional[float] = None) -> dict[str, Any]:
        body: dict[str, Any] = {"cmd": cmd}
        if payload:
            body.update(payload)
        t = self._request_timeout_s if timeout is None else float(timeout)
        return self._rpc.request(body, timeout=t)

    @staticmethod
    def _require_ok(resp: dict[str, Any], context: str) -> dict[str, Any]:
        if resp.get("ok"):
            return resp
        raise RuntimeError(resp.get("error") or f"{context} failed")

    def _apply_state(self, st: RemoteState) -> None:
        with self._state_lock:
            self.is_playing = st.is_playing
            self.pending_audio = st.pending_audio
            self.pending_synth = st.pending_synth
            self.active_streams = st.active_streams
            if st.voice_prompt_wav:
                self.voice_prompt_wav = st.voice_prompt_wav

    # -------------------------
    # Polling
    # -------------------------

    def _poll_loop(self) -> None:
        while not self._stop.is_set():
            try:
                st = self.get_state()
                self._apply_state(st)
            except Exception:
                with self._state_lock:
                    self.is_playing = False
            self._stop.wait(self._poll_interval_s)

    # -------------------------
    # Health / State
    # -------------------------

    def ping(self) -> None:
        resp = self._call("ping")
        self._require_ok(resp, "ping")

    def get_state(self) -> RemoteState:
        resp = self._call("get_state")
        self._require_ok(resp, "get_state")
        return RemoteState(
            is_playing=bool(resp.get("is_playing", False)),
            pending_audio=int(resp.get("pending_audio", 0) or 0),
            pending_synth=int(resp.get("pending_synth", 0) or 0),
            voice_prompt_wav=str(resp.get("voice_prompt_wav", "")),
            active_streams=int(resp.get("active_streams", 0) or 0),
        )

    # -------------------------
    # Control
    # -------------------------

    def stop_all(self) -> None:
        resp = self._call("stop_all")
        self._require_ok(resp, "stop_all")
        # do not assume queues are cleared, but playback is stopped
        with self._state_lock:
            self.is_playing = False

    def stop_and_clear(self) -> None:
        resp = self._call("stop_and_clear")
        self._require_ok(resp, "stop_and_clear")
        with self._state_lock:
            self.is_playing = False
            self.pending_audio = 0
            self.pending_synth = 0
            self.active_streams = 0

    # -------------------------
    # Voice
    # -------------------------

    def set_voice_prompt(self, wav_path: str) -> None:
        resp = self._call("set_voice_prompt", {"wav_path": wav_path})
        self._require_ok(resp, "set_voice_prompt")
        with self._state_lock:
            self.voice_prompt_wav = str(wav_path)

    def set_voice(self, wav_path: str) -> None:
        resp = self._call("set_voice", {"wav": wav_path})
        self._require_ok(resp, "set_voice")
        with self._state_lock:
            self.voice_prompt_wav = str(wav_path)

    # -------------------------
    # Speech
    # -------------------------

    def speak(self, text: str) -> int:
        """
        Asynchronous server-side: returns quickly after queueing.
        Returns 'accepted' (number of segmented units accepted).
        Raises RuntimeError on busy / failure.
        """
        with self.playback_lock:
            resp = self._call("speak", {"text": text}, timeout=self._request_timeout_s)
            self._require_ok(resp, "speak")
            accepted = int(resp.get("accepted", 0) or 0)

            # best-effort mirror: if we accepted anything, assume playback will happen
            if accepted > 0:
                with self._state_lock:
                    self.is_playing = True
            return accepted

    # =============================================================================
    # Streaming
    # =============================================================================

    class Stream:
        """
        Streaming session wrapper.

        Implementation detail:
        - push() enqueues text; a single sender thread sends stream_push in-order.
        - finish()/abort() send the final control messages.
        - wait_gone() polls stream_status.done (means session removed; not audio done).
        """

        def __init__(self, parent: "TTSClient", stream_id: str) -> None:
            self._parent = parent
            self.stream_id = stream_id

            self._q: "queue.Queue[Optional[str]]" = queue.Queue()
            self._stop = threading.Event()
            self._closed = False

            self._sender_done = threading.Event()
            self._sender = threading.Thread(
                target=self._send_loop,
                name=f"TTSStreamSender:{stream_id}",
                daemon=True,
            )
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

                    # Fire-and-forget to keep callers (GPT thread) non-blocking.
                    # If you want error visibility, switch to _parent._call("stream_push", ...) instead.
                    self._parent._rpc.send_only(
                        {"cmd": "stream_push", "stream_id": self.stream_id, "text": item},
                        timeout=0.25,
                    )
            finally:
                self._sender_done.set()

        def push(self, text: str) -> None:
            if self._closed:
                return
            s = (text or "").strip()
            if not s:
                return
            self._q.put(s)

        def finish(self) -> None:
            if self._closed:
                return
            self._closed = True

            # stop sender after draining queued items
            self._q.put(None)
            self._stop.set()
            self._sender_done.wait(timeout=1.0)

            # fire-and-forget finish
            self._parent._rpc.send_only(
                {"cmd": "stream_finish", "stream_id": self.stream_id},
                timeout=0.25,
            )

        def abort(self) -> None:
            if self._closed:
                return
            self._closed = True

            # stop sender immediately; drop pending queued items
            self._stop.set()
            try:
                while True:
                    self._q.get_nowait()
            except queue.Empty:
                pass
            self._q.put(None)
            self._sender_done.wait(timeout=1.0)

            # abort is a real request (you likely want to know if id was unknown)
            resp = self._parent._call("stream_abort", {"stream_id": self.stream_id})
            self._parent._require_ok(resp, "stream_abort")

        def wait_gone(self, timeout: Optional[float] = None) -> bool:
            """
            Wait until stream_status.done==True, meaning the stream id no longer exists
            in the server registry.

            IMPORTANT: This does not mean audio playback has finished.
            """
            t0 = time.monotonic()
            while True:
                resp = self._parent._call("stream_status", {"stream_id": self.stream_id})
                if resp.get("ok") and bool(resp.get("done", False)):
                    return True

                if timeout is not None and (time.monotonic() - t0) >= float(timeout):
                    return False

                time.sleep(0.05)

    def begin_stream(self, stream_id: Optional[str] = None) -> "TTSClient.Stream":
        """
        If stream_id is provided, uses stream_begin with an explicit id.
        Otherwise uses begin_stream and accepts the server-generated id.
        """
        with self.playback_lock:
            if stream_id:
                # server accepts {"stream_id": "..."} or {"id": "..."}; we send stream_id
                resp = self._call("stream_begin", {"stream_id": stream_id})
                self._require_ok(resp, "stream_begin")
            else:
                resp = self._call("begin_stream")
                self._require_ok(resp, "begin_stream")

            sid = str(resp.get("stream_id") or "")
            if not sid:
                raise RuntimeError("begin_stream failed: missing stream_id")

            # best-effort mirror: starting a stream generally implies future playback
            with self._state_lock:
                self.is_playing = True

            return TTSClient.Stream(self, sid)
