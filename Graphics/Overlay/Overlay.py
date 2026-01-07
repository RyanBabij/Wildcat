
"""
Overlay.py
Wildcat/Graphics/Overlay/Overlay.py
Wildcat.Graphics.Overlay.Overlay
AI Code

TextOverlay implements a lightweight Win32 topmost text overlay with a typewriter-style renderer.

Public functions:

    TextOverlay.feed(text, rgb) - Queue text for typewriter-style output at the current CPM rate.
    TextOverlay.instant(text, rgb) - Flush any pending output and render text immediately (still wrapped).
    TextOverlay.set_cpm(cpm) - Change characters-per-minute pacing for the feeder thread.
    TextOverlay.clear() - Clear all buffered text, history, and pending output.
    TextOverlay.show() / hide() / toggle() - Control overlay visibility.
    TextOverlay.close() - Stop background threads and signal shutdown.

Key features

    * Always on top click-through overlay using a colorkey background (magenta) for transparency.
    * Drag and resize hotspots.
    * Adjustable typewriter output with RGB support.
    * Live wrapping.
    * Threaded

Notes
    * Requires pywin32 (win32api/win32con/win32gui/win32ui).
    * Rendering is double-buffered in WM_PAINT and invalidation is throttled to reduce flicker.
"""

from __future__ import annotations

import threading
import time
from collections import deque
from dataclasses import dataclass
from typing import Deque, List, Optional, Tuple

import win32api
import win32con
import win32gui
import win32ui

WM_APP_SET_TEXT = win32con.WM_APP + 1

def _lparam_to_xy(lparam: int) -> Tuple[int, int]:
    # WM_NCHITTEST gives screen coords packed into lParam as signed 16-bit values.
    x = lparam & 0xFFFF
    y = (lparam >> 16) & 0xFFFF
    if x & 0x8000:
        x -= 0x10000
    if y & 0x8000:
        y -= 0x10000
    return x, y


# ----------------------------
# Data model
# ----------------------------

@dataclass
class Run:
    text: str
    color: int  # Win32 COLORREF (from win32api.RGB)


def _clamp8(x: int) -> int:
    return 0 if x < 0 else 255 if x > 255 else x


def _rgb_to_colorref(rgb: Tuple[int, int, int]) -> int:
    r, g, b = rgb
    return win32api.RGB(_clamp8(int(r)), _clamp8(int(g)), _clamp8(int(b)))

# ============================================================
# Overlay
# ============================================================

class TextOverlay:
    """
    Topmost, click-through, no-activate text overlay.

    Public API:
      - feed(text, rgb): enqueue text chunks and render at characters-per-minute (CPM)
      - instant(text, rgb): flush pending immediately + render text immediately

    Typewriter behavior:
      - characters are appended to the buffer as they are emitted (true typewriter)
      - word-aware wrapping at wrap_cols characters:
          * whitespace does not immediately render; it marks a pending boundary
          * when a word starts, a single space is inserted if boundary pending and not at line start
          * if a word hits the boundary, the word continues on the next line (as it types)

    Colors:
      - each chunk is assigned a Win32 RGB colour (COLORREF)
      - characters emitted from that chunk use that colour
      - colours do not change inside a chunk
    """

    # Colorkey background (transparent)
    COLORKEY = win32api.RGB(255, 0, 255)

    # Background rectangle behind text
    BG_COLOR = win32api.RGB(10, 10, 10)

    def __init__(
        self,
        x: Optional[int] = None,
        y: int = 20,
        w: int = 520,
        h: int = 240,
        max_lines: int = 200,
        cpm: int = 900,
        font_name: str = "OCR A Extended",
        font_height: int = 16,
    ):
        self.x, self.y, self.w, self.h = x, y, w, h
        self.max_lines = max(2, int(max_lines))

        self._cpm = max(1, int(cpm))
        self._font_name = font_name
        self._font_height = int(font_height)

        self.hwnd: Optional[int] = None
        self._visible: bool = True
        
        self._paint_font = None      # will be created on window thread
        self._paint_font_lock = threading.Lock()

        # Rolling buffer of lines; last line is the current editable line.
        self._lines: Deque[List[Run]] = deque(maxlen=self.max_lines)
        self._lines.append([])  # will be built from history via _reflow_from_history()
        self._lines.append([])  # current editable line placeholder

        # Canonical token history for reflow (list of Run-like chunks).
        # This is the "source of truth" for what is currently displayed.
        self._history: List[Run] = []
        self._history_lock = threading.Lock()

        # Seed history with initial message and newline to match _lines
        self._history.append(Run("Overlay ready.\n", win32api.RGB(230, 230, 255)))

        # Typewriter emission queue (text, colorref)
        self._pending: Deque[Tuple[str, int]] = deque()
        self._pending_lock = threading.Lock()
        self._pending_event = threading.Event()
        self._stop_event = threading.Event()

        # Cursor into currently active chunk (avoid rewriting tuples)
        self._pend_text: str = ""
        self._pend_i: int = 0
        self._pend_color: int = win32api.RGB(255, 255, 255)

        # Word boundary state (typewriter-visible)
        self._in_word: bool = False
        self._active_word_color: int = win32api.RGB(255, 255, 255)
        self._space_pending: bool = False  # whitespace boundary seen

        # Paint throttling
        self._last_paint: float = 0.0
        self._pending_paint: bool = False

        # -------------------------
        # Interaction hotspots (must exist before any WM_PAINT)
        # -------------------------
        self._drag_handle_px = 18   # square size (top-left)
        self._grip_px = 18          # square size (bottom-right)
        self._min_w = 220
        self._min_h = 120

        # Window thread
        self._thread = threading.Thread(target=self._thread_main, daemon=True)
        self._thread.start()


        # Wait until window is created
        while self.hwnd is None:
            time.sleep(0.01)

        self._measure_lock = threading.Lock()
        self._measure_hdc = win32gui.GetDC(self.hwnd)
        self._measure_dc = win32ui.CreateDCFromHandle(self._measure_hdc)
        self._measure_font = win32ui.CreateFont({
            "name": self._font_name,
            "height": self._font_height,
            "weight": 400,
            "quality": win32con.ANTIALIASED_QUALITY,
        })
        self._measure_dc.SelectObject(self._measure_font)
        
        self._char_px: dict[str, int] = {}
        # Optional prewarm ASCII for speed + predictability
        for code in range(32, 127):
            c = chr(code)
            self._char_px[c] = self._text_px(c)


        # padding must match paint()
        self._pad_left = 5
        self._pad_right = 5

        # current line pixel width cache
        self._cur_line_px = 0

        # word tracking for rollback
        self._word_active = False
        self._word_chars_since_start = 0
        self._word_px_since_start = 0
        self._word_start_line_px = 0
        self._word_start_line_charpos = 0  # char count position in line when word began (incl. leading space if inserted)
        # Feeder thread (typewriter)
        self._feeder_thread = threading.Thread(target=self._feeder_loop, daemon=True)
        self._feeder_thread.start()
        
        self._reflow_from_history()
        self._invalidate()


    # -------------------------
    # Visibility
    # -------------------------

    def _set_visible(self, visible: bool) -> bool:
        if not self.hwnd:
            self._visible = False
            return False

        win32gui.ShowWindow(
            self.hwnd,
            win32con.SW_SHOWNOACTIVATE if visible else win32con.SW_HIDE,
        )
        if visible:
            win32gui.UpdateWindow(self.hwnd)

        self._visible = visible
        return visible

    def show(self) -> None:
        self._set_visible(True)

    def hide(self) -> None:
        self._set_visible(False)

    def toggle(self) -> bool:
        """Returns True if overlay is now visible; False if hidden."""
        return self._set_visible(not self._visible)

    # -------------------------
    # Public API
    # -------------------------

    def set_cpm(self, cpm: int) -> None:
        self._cpm = max(1, int(cpm))
        self._pending_event.set()

    def clear(self) -> None:
        self._word_active = False
        self._space_pending = False
        self._cur_line_px = 0
        self._word_chars_since_start = 0
        self._word_px_since_start = 0
        self._word_start_line_px = 0
        self._word_start_line_charpos = 0

        with self._pending_lock:
            self._pending.clear()
            self._pend_text = ""
            self._pend_i = 0
            
        with self._history_lock:
            self._history.clear()

        self._lines.clear()
        self._lines.append([])
        self._invalidate()



    def feed(self, text: str, rgb: Tuple[int, int, int]) -> None:
        s = "" if text is None else str(text)
        if not s:
            return
        color = _rgb_to_colorref(rgb)
        with self._pending_lock:
            self._pending.append((s, color))
        self._pending_event.set()

    def instant(self, text: str, rgb: Tuple[int, int, int]) -> None:
        """
        Immediately flush any pending buffered output and then immediately emit `text`.
        This bypasses CPM pacing but preserves wrapping.
        """
        s = "" if text is None else str(text)
        color = _rgb_to_colorref(rgb)

        drained: List[Tuple[str, int]] = []
        with self._pending_lock:
            # flush partially-consumed active chunk
            if self._pend_text and self._pend_i < len(self._pend_text):
                drained.append((self._pend_text[self._pend_i :], self._pend_color))
                self._pend_text = ""
                self._pend_i = 0

            while self._pending:
                drained.append(self._pending.popleft())

        for chunk, col in drained:
            self._emit_text_immediately(chunk, col)

        if s:
            self._emit_text_immediately(s, color)

        self._invalidate()

    def close(self) -> None:
        self._stop_event.set()
        self._pending_event.set()

    # -------------------------
    # Internal: feeder loop
    # -------------------------

    def _feeder_loop(self) -> None:
        # Fixed paint cadence
        frame_dt = 1.0 / 60.0  # 60 FPS; try 30 FPS if you prefer
        last = time.monotonic()
        carry = 0.0  # fractional char accumulator

        while not self._stop_event.is_set():
            if not self._has_pending():
                self._pending_event.wait(timeout=0.25)
                self._pending_event.clear()
                last = time.monotonic()
                carry = 0.0
                continue

            now = time.monotonic()
            dt = now - last
            last = now

            cps = max(1.0, self._cpm / 60.0)  # chars per second
            carry += dt * cps

            n = int(carry)
            if n <= 0:
                time.sleep(min(frame_dt, 0.01))
                continue
            carry -= n

            emitted_any = False
            for _ in range(n):
                item = self._pop_next_char()
                if item is None:
                    break
                ch, col = item
                self._emit_char(ch, col)
                emitted_any = True

            if emitted_any:
                self._invalidate()

            # Limit loop rate (doesn't limit chars, just CPU churn)
            time.sleep(min(frame_dt, 0.005))


    def _has_pending(self) -> bool:
        with self._pending_lock:
            if self._pend_text and self._pend_i < len(self._pend_text):
                return True
            return bool(self._pending)

    def _pop_next_char(self) -> Optional[Tuple[str, int]]:
        with self._pending_lock:
            while True:
                if self._pend_text and self._pend_i < len(self._pend_text):
                    ch = self._pend_text[self._pend_i]
                    self._pend_i += 1
                    return ch, self._pend_color

                if not self._pending:
                    self._pend_text = ""
                    self._pend_i = 0
                    return None

                t, c = self._pending.popleft()
                if not t:
                    continue

                t = t.replace("\r\n", "\n").replace("\r", "")
                self._pend_text = t
                self._pend_i = 0
                self._pend_color = c

    def _emit_text_immediately(self, text: str, color: int) -> None:
        s = (text or "").replace("\r\n", "\n").replace("\r", "")
        for ch in s:
            self._emit_char(ch, color)

    # -------------------------
    # Internal: buffer + typewriter wrapping
    # -------------------------

    def _ensure_current_line(self) -> None:
        if not self._lines:
            self._lines.append([])
        if self._lines[-1] is None:
            self._lines[-1] = []

    def _append_run_to_current_line(self, text: str, color: int) -> None:
        if not text:
            return
        self._ensure_current_line()
        line = self._lines[-1]
        if line and line[-1].color == color:
            line[-1].text += text
        else:
            line.append(Run(text=text, color=color))

    def _current_line_len(self) -> int:
        self._ensure_current_line()
        return sum(len(r.text) for r in self._lines[-1])

    def _newline(self) -> None:
        self._ensure_current_line()
        self._lines.append([])

    def _begin_word_if_needed(self, color: int) -> None:
        if self._in_word:
            return

        self._in_word = True
        self._active_word_color = color

        if self._space_pending:
            cur_len = self._current_line_len()

            # If we're at/near the boundary, don't end the line with a trailing space.
            # Wrap first, then start the word on the next line with no leading space.
            if cur_len > 0 and (cur_len + 1 >= width):
                self._newline()
            elif cur_len > 0:
                self._append_run_to_current_line(" ", self._active_word_color)

        self._space_pending = False

    def _history_append(self, s: str, color: int) -> None:
        if not s:
            return
        with self._history_lock:
            if self._history and self._history[-1].color == color:
                self._history[-1].text += s
            else:
                self._history.append(Run(text=s, color=color))

    def _reflow_from_history(self) -> None:
        """
        Rebuild _lines from canonical _history using current width metrics.
        Preserves newlines and color runs. This is called on resize.
        """
        if not self.hwnd:
            return

        # Snapshot history (avoid holding lock while measuring)
        with self._history_lock:
            hist = [Run(r.text, r.color) for r in self._history]

        max_px = self._max_text_px()

        new_lines: Deque[List[Run]] = deque(maxlen=self.max_lines)
        cur_line: List[Run] = []
        cur_px = 0

        def flush_line() -> None:
            nonlocal cur_line, cur_px
            new_lines.append(cur_line)
            cur_line = []
            cur_px = 0

        def append_run_text(text: str, col: int) -> None:
            nonlocal cur_px
            if not text:
                return
            if cur_line and cur_line[-1].color == col:
                cur_line[-1].text += text
            else:
                cur_line.append(Run(text=text, color=col))
            cur_px += self._text_px(text)

        for run in hist:
            if not run.text:
                continue

            # Normalize newlines
            s = run.text.replace("\r\n", "\n").replace("\r", "")
            col = run.color

            i = 0
            while i < len(s):
                ch = s[i]

                # Explicit newline -> flush
                if ch == "\n":
                    flush_line()
                    i += 1
                    continue

                # Whitespace: keep it, but avoid starting a line with spaces
                if ch.isspace():
                    # collapse consecutive whitespace to a single space (optional; comment out if undesired)
                    # while i < len(s) and s[i].isspace() and s[i] != "\n":
                    #     i += 1
                    # ch = " "

                    if not cur_line:
                        i += 1
                        continue  # skip leading spaces
                    space_px = self._text_px(" ")
                    if cur_px + space_px > max_px and cur_line:
                        flush_line()
                    else:
                        append_run_text(" ", col)
                    i += 1
                    continue

                # Non-space: consume a "word" (until whitespace or newline)
                j = i
                while j < len(s) and (not s[j].isspace()) and s[j] != "\n":
                    j += 1
                word = s[i:j]

                # If the word fits on this line, place it
                word_px = self._text_px(word)
                if cur_px + word_px <= max_px or not cur_line:
                    # If it doesn't fit but line is empty, we may need to split long word
                    if word_px <= max_px or cur_line:
                        append_run_text(word, col)
                    else:
                        # Long word splitting at character granularity
                        for c in word:
                            cpx = self._text_px(c)
                            if cur_px + cpx > max_px and cur_line:
                                flush_line()
                            append_run_text(c, col)
                    i = j
                    continue

                # Otherwise wrap then place it
                flush_line()
                if word_px <= max_px:
                    append_run_text(word, col)
                else:
                    for c in word:
                        cpx = self._text_px(c)
                        if cur_px + cpx > max_px and cur_line:
                            flush_line()
                        append_run_text(c, col)
                i = j

        # Append last line
        new_lines.append(cur_line)

        # Ensure there is always a trailing editable line (your code expects it)
        if not new_lines:
            new_lines.append([])
        # Replace buffer and recalc current line px
        self._lines = new_lines
        self._recalc_cur_line_px()


    def _emit_char(self, ch: str, color: int) -> None:
        # Newline: end word + force new line
        if ch == "\n":
            # record canonical stream
            self._history_append("\n", color)

            self._word_active = False
            self._space_pending = False
            self._cur_line_px = 0
            self._newline()
            return

        # Whitespace boundary: mark pending; do not render immediately
        if ch.isspace():
            self._word_active = False
            self._space_pending = True
            self._word_chars_since_start = 0
            self._word_px_since_start = 0
            return

        # Starting a new word?
        if not self._word_active:
            self._word_active = True
            self._active_word_color = color

            # Record word start position for rollback
            self._word_start_line_px = self._cur_line_px
            self._word_start_line_charpos = self._current_line_len()
            self._word_chars_since_start = 0
            self._word_px_since_start = 0

            # Emit deferred single space if appropriate
            if self._space_pending and self._current_line_len() > 0:
                space_px = self._text_px(" ")

                # If space alone would hit boundary, wrap first (avoid trailing-space line)
                if self._cur_line_px + space_px >= self._max_text_px():
                    self._newline()
                    self._cur_line_px = 0
                    self._word_start_line_px = 0
                    self._word_start_line_charpos = 0
                else:
                    self._append_run_to_current_line(" ", self._active_word_color)
                    self._history_append(" ", self._active_word_color)  # record visible space
                    self._cur_line_px += space_px

                    # Update word start after inserted space (space becomes part of rollback scope)
                    self._word_start_line_px = self._cur_line_px
                    self._word_start_line_charpos = self._current_line_len()

            self._space_pending = False

        # Measure this char
        ch_px = self._char_px.get(ch)
        if ch_px is None:
            ch_px = self._text_px(ch)
            self._char_px[ch] = ch_px

        max_px = self._max_text_px()

        # Would this char overflow the line?
        if self._cur_line_px + ch_px > max_px and self._current_line_len() > 0:
            # If the word started mid-line, move the already-typed part of the word down.
            # If the word started at column 0, we are splitting a long word: just newline.
            if self._word_start_line_charpos > 0 and self._word_chars_since_start > 0:
                moved = self._pop_last_chars_from_current_line(self._word_chars_since_start)
                self._newline()
                self._cur_line_px = 0

                # Re-append moved chars onto new line
                for r in moved:
                    self._append_run_to_current_line(r.text, r.color)

                self._recalc_cur_line_px()

                # Reset word tracking to reflect that the word is now at start of line
                self._word_start_line_charpos = 0
                self._word_start_line_px = 0
                self._word_px_since_start = self._cur_line_px
                # _word_chars_since_start stays the same (we re-appended them)
            else:
                self._newline()
                self._cur_line_px = 0
                # word continues (splitting long word) at start of new line

        # Append character (typewriter)
        self._append_run_to_current_line(ch, self._active_word_color)
        self._history_append(ch, self._active_word_color)  # record visible char
        self._cur_line_px += ch_px
        self._word_chars_since_start += 1
        self._word_px_since_start += ch_px

    # -------------------------
    # Paint invalidation (throttled)
    # -------------------------

    def _invalidate(self) -> None:
        if not self.hwnd:
            return

        now = time.monotonic()
        min_interval = 1.0 / 60.0  # match your frame cadence; 60 FPS

        if now - self._last_paint < min_interval:
            return

        self._last_paint = now
        try:
            win32gui.PostMessage(self.hwnd, WM_APP_SET_TEXT, 0, 0)
        except Exception:
            pass


    # -------------------------
    # Window thread + WndProc
    # -------------------------

    def _thread_main(self) -> None:
        wc = win32gui.WNDCLASS()
        wc.lpfnWndProc = self._wnd_proc
        wc.lpszClassName = "TextOverlayClassV3"
        wc.hCursor = win32gui.LoadCursor(None, win32con.IDC_ARROW)
        hinst = win32api.GetModuleHandle(None)

        try:
            class_atom = win32gui.RegisterClass(wc)
        except win32gui.error:
            class_atom = wc.lpszClassName

        ex_style = (
            win32con.WS_EX_TOPMOST
            | win32con.WS_EX_TOOLWINDOW
            | win32con.WS_EX_LAYERED
            | 0x08000000  # WS_EX_NOACTIVATE
        )
        style = win32con.WS_POPUP

        x = self._default_x() if self.x is None else self.x

        self.hwnd = win32gui.CreateWindowEx(
            ex_style,
            class_atom,
            "",
            style,
            x,
            self.y,
            self.w,
            self.h,
            0,
            0,
            hinst,
            None,
        )

        win32gui.SetLayeredWindowAttributes(
            self.hwnd,
            self.COLORKEY,
            220,  # 0–255
            win32con.LWA_COLORKEY | win32con.LWA_ALPHA,
        )
                
        # Create paint font on the window thread before any WM_PAINT can occur
        try:
            with self._paint_font_lock:
                if self._paint_font is None:
                    self._paint_font = win32ui.CreateFont({
                        "name": self._font_name,
                        "height": self._font_height,
                        "weight": 400,
                        "quality": win32con.ANTIALIASED_QUALITY,
                    })
        except Exception:
            # If this fails, WM_PAINT should fall back gracefully (see fix B)
            self._paint_font = None


        win32gui.ShowWindow(self.hwnd, win32con.SW_SHOW)
        win32gui.UpdateWindow(self.hwnd)
        self._visible = True

        win32gui.PumpMessages()

    def _default_x(self) -> int:
        screen_w = win32api.GetSystemMetrics(win32con.SM_CXSCREEN)
        margin = 20
        return max(0, screen_w - self.w - margin)

    def _wnd_proc(self, hwnd: int, msg: int, wparam: int, lparam: int) -> int:
        if msg == WM_APP_SET_TEXT:
            win32gui.InvalidateRect(hwnd, None, False)
            return 0

        if msg == win32con.WM_ERASEBKGND:
            return 1

        if msg == win32con.WM_NCHITTEST:
            sx, sy = _lparam_to_xy(lparam)
            cx, cy = win32gui.ScreenToClient(hwnd, (sx, sy))
            return self._hit_test_regions(int(cx), int(cy))

        if msg == win32con.WM_GETMINMAXINFO:
            # Enforce minimum resize dimensions
            try:
                mmi = win32gui.PyGetMINMAXINFO(lparam)
                mmi["ptMinTrackSize"] = (self._min_w, self._min_h)
                win32gui.PySetMINMAXINFO(lparam, mmi)
                return 0
            except Exception:
                # If PyGetMINMAXINFO isn't available in your build, ignore.
                pass
                
        if msg == win32con.WM_MOVE:
            try:
                r = win32gui.GetWindowRect(hwnd)
                self.x, self.y = r[0], r[1]
            except Exception:
                pass
            return 0

        if msg == win32con.WM_SIZE:
            try:
                w = win32api.LOWORD(lparam)
                h = win32api.HIWORD(lparam)
                self.w, self.h = int(w), int(h)
            except Exception:
                pass

            self._reflow_from_history()
            self._invalidate()
            return 0


        if msg == win32con.WM_PAINT:
            hdc, ps = win32gui.BeginPaint(hwnd)
            try:
                rc = win32gui.GetClientRect(hwnd)
                w = max(1, rc[2] - rc[0])
                h = max(1, rc[3] - rc[1])

                # --- double buffer ---
                hdc_mem = win32gui.CreateCompatibleDC(hdc)
                bmp = win32gui.CreateCompatibleBitmap(hdc, w, h)
                old_bmp = win32gui.SelectObject(hdc_mem, bmp)

                try:
                    # Fill backbuffer with colorkey
                    brush = win32gui.CreateSolidBrush(self.COLORKEY)
                    win32gui.FillRect(hdc_mem, (0, 0, w, h), brush)
                    win32gui.DeleteObject(brush)

                    dc = win32ui.CreateDCFromHandle(hdc_mem)
                    dc.SetBkMode(win32con.TRANSPARENT)

                    font_obj = getattr(self, "_paint_font", None)
                    if font_obj is not None:
                        dc.SelectObject(font_obj)
                    else:
                        # Last-resort fallback: use a stock font (avoid creating/leaking fonts here)
                        try:
                            stock = win32gui.GetStockObject(win32con.DEFAULT_GUI_FONT)
                            if stock:
                                win32gui.SelectObject(hdc_mem, stock)
                        except Exception:
                            pass


                    tm = dc.GetTextMetrics()
                    line_h = tm["tmHeight"] + tm["tmExternalLeading"] + 2

                    left_pad, top_pad, right_pad, bottom_pad = 5, 5, 5, 5
                    usable_h = max(1, h - top_pad - bottom_pad)
                    max_visible = max(1, usable_h // line_h)

                    # Snapshot lines
                    lines_snapshot: List[List[Run]] = list(self._lines)
                    lines_snapshot = lines_snapshot[-max_visible:]

                    def _line_text_extent(runs: List[Run]) -> Tuple[int, int]:
                        if not runs:
                            return 0, tm["tmHeight"]
                        total_w = 0
                        max_h = 0
                        for run in runs:
                            if not run.text:
                                continue
                            tw, th = dc.GetTextExtent(run.text)
                            total_w += tw
                            max_h = max(max_h, th)
                        return total_w, max(max_h, tm["tmHeight"])

                    def _textout_shadow(x_: int, y_: int, s_: str, col_: int) -> None:
                        dc.SetTextColor(win32api.RGB(0, 0, 0))
                        dc.TextOut(x_ + 2, y_ + 2, s_)
                        dc.SetTextColor(col_)
                        dc.TextOut(x_, y_, s_)

                    y = top_pad
                    for runs in lines_snapshot:
                        tw, th = _line_text_extent(runs)

                        # background behind entire line
                        pad_x, pad_y = 6, 2
                        bg_brush = win32gui.CreateSolidBrush(self.BG_COLOR)
                        bg_rect = (left_pad - pad_x, y - pad_y, left_pad + tw + pad_x, y + th + pad_y)
                        win32gui.FillRect(hdc_mem, bg_rect, bg_brush)
                        win32gui.DeleteObject(bg_brush)

                        x = left_pad
                        for run in runs:
                            if not run.text:
                                continue
                            _textout_shadow(x, y, run.text, run.color)
                            seg_w, _ = dc.GetTextExtent(run.text)
                            x += seg_w

                        y += line_h

                    # ---- drag handle (top-left) ----
                    hs = int(getattr(self, "_drag_handle_px", 18))
                    handle_rect = (0, 0, hs, hs)
                    hb = win32gui.CreateSolidBrush(win32api.RGB(40, 40, 40))
                    win32gui.FillRect(hdc_mem, handle_rect, hb)
                    win32gui.DeleteObject(hb)

                    # outline
                    pen = win32gui.CreatePen(win32con.PS_SOLID, 1, win32api.RGB(220, 220, 220))
                    old_pen = win32gui.SelectObject(hdc_mem, pen)
                    old_brush = win32gui.SelectObject(hdc_mem, win32gui.GetStockObject(win32con.NULL_BRUSH))
                    win32gui.Rectangle(hdc_mem, 0, 0, hs, hs)
                    win32gui.SelectObject(hdc_mem, old_brush)
                    win32gui.SelectObject(hdc_mem, old_pen)
                    win32gui.DeleteObject(pen)

                    # ---- resize grip (bottom-right) ----
                    gs = int(getattr(self, "_grip_px", 18))
                    grip_rect = (w - gs, h - gs, w, h)
                    gb = win32gui.CreateSolidBrush(win32api.RGB(40, 40, 40))
                    win32gui.FillRect(hdc_mem, grip_rect, gb)
                    win32gui.DeleteObject(gb)

                    # simple diagonal "grip" lines
                    pen2 = win32gui.CreatePen(win32con.PS_SOLID, 1, win32api.RGB(220, 220, 220))
                    old_pen2 = win32gui.SelectObject(hdc_mem, pen2)
                    for i in range(4, gs, 5):
                        win32gui.MoveToEx(hdc_mem, w - 1 - i, h - 1)
                        win32gui.LineTo(hdc_mem, w - 1, h - 1 - i)
                    win32gui.SelectObject(hdc_mem, old_pen2)
                    win32gui.DeleteObject(pen2)


                    win32gui.BitBlt(hdc, 0, 0, w, h, hdc_mem, 0, 0, win32con.SRCCOPY)
                 

                finally:
                    win32gui.SelectObject(hdc_mem, old_bmp)
                    win32gui.DeleteObject(bmp)
                    win32gui.DeleteDC(hdc_mem)

            finally:
                win32gui.EndPaint(hwnd, ps)
            return 0

        if msg == win32con.WM_DESTROY:
            win32gui.PostQuitMessage(0)
            return 0

        return win32gui.DefWindowProc(hwnd, msg, wparam, lparam)

    def _client_wh(self) -> Tuple[int, int]:
        rc = win32gui.GetClientRect(self.hwnd) if self.hwnd else (0, 0, self.w, self.h)
        return max(1, rc[2] - rc[0]), max(1, rc[3] - rc[1])

    def _hit_test_regions(self, cx: int, cy: int) -> int:
        """
        Return HT* code based on client-space cursor position.
        Only handle + size-grip are interactive; everything else is click-through.
        """
        w, h = self._client_wh()

        # Drag handle: top-left square
        if 0 <= cx < self._drag_handle_px and 0 <= cy < self._drag_handle_px:
            return win32con.HTCAPTION

        # Resize grip: bottom-right square
        if (w - self._grip_px) <= cx < w and (h - self._grip_px) <= cy < h:
            return win32con.HTBOTTOMRIGHT

        return win32con.HTTRANSPARENT

    # -------------------------
    # Text measuring
    # -------------------------
        
    def _max_text_px(self) -> int:
        # client width minus left/right padding
        rc = win32gui.GetClientRect(self.hwnd) if self.hwnd else (0, 0, self.w, self.h)
        w = max(1, rc[2] - rc[0])
        return max(20, w - self._pad_left - self._pad_right)

    def _text_px(self, s: str) -> int:
        if not s:
            return 0
        with self._measure_lock:
            tw, _ = self._measure_dc.GetTextExtent(s)
        return int(tw)

    def _pop_last_chars_from_current_line(self, n: int) -> List[Run]:
        """
        Remove last n characters from the current editable line and return them
        as runs (in correct order) so they can be re-appended on the next line.
        """
        if n <= 0:
            return []
        self._ensure_current_line()
        line = self._lines[-1]
        out_rev: List[Run] = []

        while n > 0 and line:
            run = line[-1]
            if not run.text:
                line.pop()
                continue

            if len(run.text) <= n:
                # take whole run
                out_rev.append(Run(run.text, run.color))
                n -= len(run.text)
                line.pop()
            else:
                # split run
                take = run.text[-n:]
                keep = run.text[:-n]
                out_rev.append(Run(take, run.color))
                run.text = keep
                n = 0
                if not run.text:
                    line.pop()

        # reverse and merge adjacent same-color runs
        out = list(reversed(out_rev))
        merged: List[Run] = []
        for r in out:
            if not r.text:
                continue
            if merged and merged[-1].color == r.color:
                merged[-1].text += r.text
            else:
                merged.append(Run(r.text, r.color))
        return merged

    def _recalc_cur_line_px(self) -> None:
        self._ensure_current_line()
        s = "".join(r.text for r in self._lines[-1])
        self._cur_line_px = self._text_px(s)
