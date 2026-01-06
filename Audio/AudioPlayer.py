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

import numpy as np
import sounddevice as sd
import soundfile as sf


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


@dataclass(frozen=True)
class PlayItem:
    wav_path: str
    delete_after: bool = True


class GlobalAudioPlayer:
    """
    A single global sequential WAV player.

    New:
      - enqueue(..., override=True) will:
          1) stop current playback ASAP
          2) clear any queued items
          3) enqueue this item to play next
    """

    def __init__(self, *, playback_gap_s: float = 0.0, max_queue: int = 256) -> None:
        self.playback_gap_s = float(playback_gap_s)
        self._q: "queue.Queue[PlayItem]" = queue.Queue(maxsize=max_queue)

        # Set to request immediate stop of the currently playing clip.
        self._stop_now = threading.Event()

        # Protects _playing/_pending counters.
        self._state_lock = threading.Lock()
        self._playing = False
        self._pending = 0

        # Ensures override operations (stop+clear+enqueue) behave atomically relative
        # to other enqueues/clears.
        self._enqueue_lock = threading.Lock()

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
        """Stops the currently-playing clip ASAP. Does not clear the queue."""
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

    # ---- enqueue ----

    def enqueue(self, wav_path: str, *, delete_after: bool = False, override: bool = False) -> None:
        """
        Enqueue a wav for sequential playback. Blocks if queue is full.

        override=True:
          - stop current playback ASAP
          - wipe pending queue
          - enqueue this item so it plays next
        """
        item = PlayItem(wav_path=wav_path, delete_after=delete_after)

        if not override:
            with self._state_lock:
                self._pending += 1
            self._q.put(item)
            return

        # Override path: make stop+clear+enqueue behave as one operation relative to
        # other concurrent enqueue/clear calls.
        with self._enqueue_lock:
            self.stop_now()
            self.clear_pending()

            with self._state_lock:
                self._pending += 1

            # Put the override item in the queue. If the queue is full, this blocks,
            # but since we just cleared it, this should normally return immediately.
            self._q.put(item)
            
    def play_override(self, wav_path: str, *, delete_after: bool = False) -> None:
        self.enqueue(wav_path, delete_after=delete_after, override=True)


    # ---- internal ----

    def _worker(self) -> None:
        while True:
            item = self._q.get()

            # Clear stale stop before starting a new clip.
            # (If override asked to stop, it will be set again while playing.)
            if self._stop_now.is_set():
                self._stop_now.clear()

            try:
                data, sr = sf.read(item.wav_path, dtype="float32", always_2d=True)

                if item.delete_after:
                    try:
                        os.remove(item.wav_path)
                    except OSError:
                        pass

                with self._state_lock:
                    self._playing = True

                _play_f32_blocking(data, sr, self._stop_now)

            except Exception:
                traceback.print_exc()

            finally:
                with self._state_lock:
                    self._playing = False
                    self._pending = max(0, self._pending - 1)

            if self.playback_gap_s > 0:
                # Use the stop flag as an interruptible sleep.
                self._stop_now.wait(timeout=self.playback_gap_s)
                if self._stop_now.is_set():
                    self._stop_now.clear()
