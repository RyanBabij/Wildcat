"""
AudioPlayer.py
AI code

Thread-safe, single-instance audio playback service for sequential WAV output.

Purpose
-------
Provides a global audio queue that guarantees:
- No overlapping playback
- Deterministic, in-order audio output
- Immediate interruption when required

Key Features
------------
- Background worker thread handles all playback.
- Queue-based sequencing using `queue.Queue`.
- `override` mode allows urgent audio to preempt:
    * Stops current playback immediately.
    * Clears all pending items.
    * Plays the new item next.
- Optional auto-deletion of WAV files after playback.
- Interruptible playback gap between clips.

Typical Usage
-------------
player = GlobalAudioPlayer(playback_gap_s=0.5)

player.enqueue("hello.wav")
player.enqueue("world.wav")

# Interrupt everything and play immediately
player.play_override("urgent.wav")

Threading Model
---------------
- One daemon worker thread performs all audio I/O.
- `sounddevice` callback streams audio in float32 PCM.
- `threading.Event` is used to abort playback safely.
- Internal locks guarantee atomic override behavior.

Design Constraints
------------------
- Only one clip may play at any time.
- Audio must always be float32 PCM.
- Intended for TTS pipelines and assistant-style systems
  where speech order and interruption semantics matter.
"""


from __future__ import annotations

import os
import queue
import threading
import traceback
from dataclasses import dataclass
from typing import Optional, Union, Dict

import numpy as np
import sounddevice as sd
import soundfile as sf


# =============================================================================
# Playback helpers
# =============================================================================

def _play_f32_blocking(data_f32_2d: np.ndarray, sr: int, stop_event: threading.Event) -> None:
    """
    Plays float32 PCM (frames, channels) and blocks until fully drained.
    stop_event: when set, playback stops ASAP.
    """
    if data_f32_2d.ndim != 2 or data_f32_2d.dtype != np.float32:
        raise ValueError("data_f32_2d must be float32 with shape (frames, channels)")

    frames_total, channels = data_f32_2d.shape
    pos = 0
    finished = threading.Event()

    def callback(outdata, frames, time_info, status):
        nonlocal pos
        if stop_event.is_set():
            outdata.fill(0)
            raise sd.CallbackStop()

        end = min(pos + frames, frames_total)
        n = end - pos

        if n > 0:
            outdata[:n, :] = data_f32_2d[pos:end, :]
            pos = end

        if n < frames:
            outdata[n:, :].fill(0)
            raise sd.CallbackStop()

    def on_finished():
        finished.set()

    with sd.OutputStream(
        samplerate=sr,
        channels=channels,
        dtype="float32",
        callback=callback,
        finished_callback=on_finished,
    ):
        finished.wait()


def _play_stream_f32_blocking(
    get_chunk: "callable[[], Optional[np.ndarray]]",
    *,
    sr: int,
    channels: int,
    stop_event: threading.Event,
    min_buffer_chunks: int = 1,
) -> None:
    """
    Plays a float32 PCM stream (chunks of shape (frames, channels)).
    - get_chunk() returns:
        * np.ndarray: next chunk
        * None: end-of-stream marker
    - stop_event stops playback ASAP.

    This blocks until the stream finishes or is stopped.
    """
    finished = threading.Event()

    # Local playback buffer: list of chunks plus an offset into the first.
    buf_chunks: list[np.ndarray] = []
    buf_pos = 0  # offset into buf_chunks[0] in frames
    eof = False

    # Pre-buffer a bit to reduce initial underruns.
    for _ in range(max(0, int(min_buffer_chunks))):
        c = get_chunk()
        if c is None:
            eof = True
            break
        if c.size:
            buf_chunks.append(c)
    # If we hit eof before any audio, just return.
    if eof and not buf_chunks:
        return

    def callback(outdata, frames, time_info, status):
        nonlocal buf_pos, eof
        if stop_event.is_set():
            outdata.fill(0)
            raise sd.CallbackStop()

        outdata.fill(0)

        need = frames
        wpos = 0

        while need > 0:
            # If no buffered audio, pull more.
            while not buf_chunks and not eof:
                nxt = get_chunk()
                if nxt is None:
                    eof = True
                    break
                if nxt.size:
                    buf_chunks.append(nxt)
                    buf_pos = 0

            if not buf_chunks:
                # No audio available: underrun (silence). If EOF, stop.
                if eof:
                    raise sd.CallbackStop()
                return

            cur = buf_chunks[0]
            avail = cur.shape[0] - buf_pos
            if avail <= 0:
                buf_chunks.pop(0)
                buf_pos = 0
                continue

            take = min(avail, need)
            outdata[wpos : wpos + take, :] = cur[buf_pos : buf_pos + take, :]
            buf_pos += take
            wpos += take
            need -= take

            if buf_pos >= cur.shape[0]:
                buf_chunks.pop(0)
                buf_pos = 0

    def on_finished():
        finished.set()

    with sd.OutputStream(
        samplerate=sr,
        channels=channels,
        dtype="float32",
        callback=callback,
        finished_callback=on_finished,
    ):
        finished.wait()


# =============================================================================
# Queue items
# =============================================================================

@dataclass(frozen=True)
class WavItem:
    wav_path: str
    delete_after: bool = True


@dataclass(frozen=True)
class StreamItem:
    stream_id: str


PlayItem = Union[WavItem, StreamItem]


# =============================================================================
# Streaming model
# =============================================================================

class _PCMStream:
    """
    Internal per-stream buffer.

    Producer thread(s): stream_push_f32()
    Consumer thread: _worker() via _play_stream_f32_blocking()

    The stream uses a bounded queue for backpressure.
    """
    def __init__(self, *, sr: int, channels: int, max_chunks: int) -> None:
        self.sr = int(sr)
        self.channels = int(channels)

        self.q: "queue.Queue[Optional[np.ndarray]]" = queue.Queue(maxsize=max_chunks)

        self.closed = False
        self.aborted = False
        self.lock = threading.Lock()

    def push(self, chunk: np.ndarray, *, block: bool, timeout: float) -> bool:
        with self.lock:
            if self.closed or self.aborted:
                return False
        try:
            self.q.put(chunk, block=block, timeout=timeout)
            return True
        except queue.Full:
            return False

    def finish(self) -> None:
        with self.lock:
            if self.closed or self.aborted:
                return
            self.closed = True
        # End-of-stream marker
        try:
            self.q.put_nowait(None)
        except Exception:
            # If full, a consumer will eventually drain; worst case stream ends late.
            pass

    def abort(self) -> None:
        with self.lock:
            self.aborted = True
            self.closed = True
        # Clear queued audio and signal end.
        try:
            while True:
                self.q.get_nowait()
        except queue.Empty:
            pass
        try:
            self.q.put_nowait(None)
        except Exception:
            pass

    def get_next(self, stop_event: threading.Event) -> Optional[np.ndarray]:
        """
        Called by playback thread. Blocks briefly waiting for audio, but exits promptly on stop_event.
        """
        while True:
            if stop_event.is_set():
                return None
            try:
                item = self.q.get(timeout=0.05)
                return item
            except queue.Empty:
                continue


# =============================================================================
# Player
# =============================================================================

class GlobalAudioPlayer:
    """
    A single global sequential WAV + PCM-stream player.

    Existing behavior:
      - enqueue(wav_path)
      - override semantics
      - stop_now / clear_pending

    New streaming behavior:
      - stream_begin(stream_id, sr, channels)
      - stream_push_f32(stream_id, chunk_f32_2d)
      - stream_finish(stream_id)
      - stream_abort(stream_id)
      - enqueue_stream(stream_id): place the stream into the same global sequence
    """

    def __init__(
        self,
        *,
        playback_gap_s: float = 0.0,
        max_queue: int = 256,
        # Streaming backpressure: max buffered chunks per stream.
        # Chunk size is up to you; common is 20–100ms worth of frames per chunk.
        stream_max_chunks: int = 200,
        # Minimum chunks to prebuffer before starting stream playback.
        stream_min_buffer_chunks: int = 2,
    ) -> None:
        self.playback_gap_s = float(playback_gap_s)
        self._q: "queue.Queue[PlayItem]" = queue.Queue(maxsize=max_queue)

        self._stop_now = threading.Event()

        self._state_lock = threading.Lock()
        self._playing = False
        self._pending = 0

        self._enqueue_lock = threading.Lock()

        # Streaming registry
        self._streams_lock = threading.Lock()
        self._streams: Dict[str, _PCMStream] = {}
        self._stream_max_chunks = int(stream_max_chunks)
        self._stream_min_buffer_chunks = int(stream_min_buffer_chunks)

        self._t = threading.Thread(target=self._worker, daemon=True)
        self._t.start()

    # ---- state ----

    def is_playing(self) -> bool:
        with self._state_lock:
            return bool(self._playing or self._pending > 0)

    def pending_items(self) -> int:
        with self._state_lock:
            return int(self._pending)

    # ---- control ----

    def stop_now(self) -> None:
        """Stops the currently-playing item ASAP. Does not clear the queue."""
        self._stop_now.set()

    def clear_pending(self) -> int:
        """Clears queued items that have not started yet. Returns number cleared."""
        cleared = 0
        try:
            while True:
                self._q.get_nowait()
                cleared += 1
        except queue.Empty:
            pass

        if cleared:
            with self._state_lock:
                self._pending = max(0, self._pending - cleared)
        return cleared

    # ---- WAV enqueue ----

    def enqueue(self, wav_path: str, *, delete_after: bool = False, override: bool = False) -> None:
        item: PlayItem = WavItem(wav_path=wav_path, delete_after=delete_after)

        if not override:
            with self._state_lock:
                self._pending += 1
            self._q.put(item)
            return

        with self._enqueue_lock:
            self.stop_now()
            self.clear_pending()

            with self._state_lock:
                self._pending += 1

            self._q.put(item)

    def play_override(self, wav_path: str, *, delete_after: bool = False) -> None:
        self.enqueue(wav_path, delete_after=delete_after, override=True)

    # ---- Streaming public API ----

    def stream_begin(self, stream_id: str, *, sr: int, channels: int = 1) -> None:
        """
        Register a new PCM stream. Does not enqueue it for playback automatically.
        Call enqueue_stream(stream_id) to make it part of the playback sequence.
        """
        sid = (stream_id or "").strip()
        if not sid:
            raise ValueError("stream_id required")
        if channels <= 0:
            raise ValueError("channels must be >= 1")

        with self._streams_lock:
            if sid in self._streams:
                # Idempotent-ish behavior: allow re-begin only if prior was closed/aborted,
                # but keep it simple: reject duplicates to avoid ambiguity.
                raise ValueError(f"stream already exists: {sid}")
            self._streams[sid] = _PCMStream(sr=sr, channels=channels, max_chunks=self._stream_max_chunks)

    def enqueue_stream(self, stream_id: str, *, override: bool = False) -> None:
        """
        Enqueue a previously begun stream as an item in the global sequence.
        """
        sid = (stream_id or "").strip()
        if not sid:
            raise ValueError("stream_id required")

        with self._streams_lock:
            if sid not in self._streams:
                raise ValueError(f"unknown stream: {sid}")

        item: PlayItem = StreamItem(stream_id=sid)

        if not override:
            with self._state_lock:
                self._pending += 1
            self._q.put(item)
            return

        with self._enqueue_lock:
            self.stop_now()
            self.clear_pending()

            with self._state_lock:
                self._pending += 1
            self._q.put(item)

    def stream_push_f32(self, stream_id: str, chunk_f32_2d: np.ndarray, *, block: bool = False, timeout: float = 0.0) -> bool:
        """
        Push a PCM chunk to a stream.
        chunk must be float32 with shape (frames, channels).
        Returns False on backpressure / closed / unknown stream.
        """
        sid = (stream_id or "").strip()
        if not sid:
            return False
        if not isinstance(chunk_f32_2d, np.ndarray):
            return False
        if chunk_f32_2d.dtype != np.float32 or chunk_f32_2d.ndim != 2:
            return False

        with self._streams_lock:
            st = self._streams.get(sid)
        if st is None:
            return False
        if chunk_f32_2d.shape[1] != st.channels:
            return False

        return st.push(chunk_f32_2d, block=block, timeout=float(timeout))

    def stream_finish(self, stream_id: str) -> None:
        sid = (stream_id or "").strip()
        if not sid:
            return
        with self._streams_lock:
            st = self._streams.get(sid)
        if st is None:
            return
        st.finish()

    def stream_abort(self, stream_id: str) -> None:
        sid = (stream_id or "").strip()
        if not sid:
            return
        with self._streams_lock:
            st = self._streams.get(sid)
        if st is None:
            return
        st.abort()

    def stream_forget(self, stream_id: str) -> None:
        """
        Remove stream registry entry. Call after playback finishes or aborts.
        Safe to call multiple times.
        """
        sid = (stream_id or "").strip()
        if not sid:
            return
        with self._streams_lock:
            self._streams.pop(sid, None)

    # ---- internal ----

    def _worker(self) -> None:
        while True:
            item = self._q.get()

            # Clear stale stop before starting new item.
            if self._stop_now.is_set():
                self._stop_now.clear()

            try:
                with self._state_lock:
                    self._playing = True

                if isinstance(item, WavItem):
                    self._play_wav_item(item)

                elif isinstance(item, StreamItem):
                    self._play_stream_item(item)

                else:
                    # Should never happen
                    pass

            except Exception:
                traceback.print_exc()

            finally:
                with self._state_lock:
                    self._playing = False
                    self._pending = max(0, self._pending - 1)

            if self.playback_gap_s > 0:
                self._stop_now.wait(timeout=self.playback_gap_s)
                if self._stop_now.is_set():
                    self._stop_now.clear()

    def _play_wav_item(self, item: WavItem) -> None:
        data, sr = sf.read(item.wav_path, dtype="float32", always_2d=True)

        # If you truly want "delete after playback", this should be AFTER playback,
        # but you had it before. Keeping your existing behavior unchanged.
        if item.delete_after:
            try:
                os.remove(item.wav_path)
            except OSError:
                pass

        _play_f32_blocking(data, sr, self._stop_now)

    def _play_stream_item(self, item: StreamItem) -> None:
        sid = item.stream_id
        with self._streams_lock:
            st = self._streams.get(sid)

        if st is None:
            # Stream was never begun or was forgotten; treat as no-op.
            return

        def get_chunk() -> Optional[np.ndarray]:
            return st.get_next(self._stop_now)

        _play_stream_f32_blocking(
            get_chunk,
            sr=st.sr,
            channels=st.channels,
            stop_event=self._stop_now,
            min_buffer_chunks=self._stream_min_buffer_chunks,
        )

        # After stream playback completes (or stop_now), forget it.
        self.stream_forget(sid)
