"""
ChatGPT.py
Wildcat/AI/ChatGPT.py
Wildcat.AI.ChatGPT
AI Code

Very simple module to create and talk to a ChatGPT instance. Loads and saves context.

Public API

speak(text) -> None - Queue a user message for the next model call.
listen() -> str - Send queued input and return the full assistant reply.
listen_stream(on_delta=None) -> Iterator[str] - Stream the assistant reply as text deltas.
start_session(label=None) - Insert a [SESSION START] marker into history.
end_session() - Insert a [SESSION END] marker into history.
reset() - Clear conversation state (keeps system prompt).
set_model(name_or_model_id) -> str - Switch active model (profile name or raw id).
set_models(models: dict) - Replace model profile map.
save_json(path) - Persist full module state to disk.
load_json(path, *, api_key=None, request_timeout_s=None, auto_resume_session=True) -> ChatGPTModule
    Load state from disk and return a ready instance.
ChatGPTModule(...) - Public constructor for configuring the module.

Notes:
* Uses the OpenAI Responses API via the official Python SDK.
* Streaming event schemas can vary across SDK versions; listen_stream() is defensive.
"""

from __future__ import annotations

import os
import time
import json
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Callable, Dict, Iterable, Iterator, List, Optional

from openai import OpenAI

import base64
import re
from pathlib import Path

from typing import Any


LOG_DIR = Path("logs")
LOG_DIR.mkdir(exist_ok=True)

IMAGE_DIR = Path("generated_images")
IMAGE_DIR.mkdir(parents=True, exist_ok=True)

def _now_iso() -> str:
    # Local timezone, second precision (good enough for audit/troubleshooting)
    return datetime.now().astimezone().isoformat(timespec="seconds")


@dataclass
class ChatTurn:
    role: str  # "system" | "user" | "assistant"
    content: str
    ts: str  # ISO8601 local time


class ChatGPTModule:
    """
    Conversational wrapper around OpenAI Responses API.

    Public surface:
      - speak(text): queue user input
      - listen() -> str: get assistant reply (non-streaming convenience)
      - listen_stream() -> Iterator[str]: stream assistant reply deltas

    Added features:
      - JSON save/load of state (history + summary + model selection)
      - Model profiles (named models you can switch between)
      - Timestamps on every stored message
      - Session start/end markers preserved in history
    """

    STATE_VERSION = 1

    def __init__(
        self,
        *,
        api_key: Optional[str] = None,
        model: str = "gpt-4.1-mini",
        image_model: str = "gpt-image-1",
        models: Optional[Dict[str, str]] = None,
        system_prompt: str = "You are a helpful assistant.",
        max_turns_kept: int = 500,
        enable_summary: bool = True,
        summarize_trigger_turns: int = 40,
        summarizer_model: Optional[str] = None,
        request_timeout_s: Optional[float] = None,
        auto_start_session: bool = True,
        session_label: str = "default",
    ) -> None:
        self.client = OpenAI(api_key=api_key or os.environ["OPENAI_API_KEY"])
        
        # Model selection
        self.models: Dict[str, str] = dict(models or {})
        self.model = self._resolve_model_id(model)
        self.summarizer_model = self._resolve_model_id(summarizer_model or model)
        self.image_model = self._resolve_model_id(image_model)
        
        # Prompts / context
        self.system_prompt = system_prompt
        self.max_turns_kept = max(4, int(max_turns_kept))
        self.enable_summary = bool(enable_summary)
        self.summarize_trigger_turns = max(self.max_turns_kept + 4, int(summarize_trigger_turns))
        self.request_timeout_s = request_timeout_s

        # State
        self._pending_user: Optional[str] = None
        self._summary: str = ""
        self._turns: List[ChatTurn] = [ChatTurn("system", system_prompt, _now_iso())]

        # Sessions
        self._session_label = session_label
        self._session_open = False
        if auto_start_session:
            self.start_session(label=session_label)
            
        # -------------------------
        # Runtime-only multimodal context (NOT persisted)
        # -------------------------
        self._mm_ctx: List[dict] = []  # each item is a Responses "message" dict
        self._mm_ctx_limit: int = 2    # keep last 2 images by default


    def _log_io(self, *, role: str, content: str) -> None:
        """
        Append a single I/O event to the audit log.
        One JSON object per line for easy parsing.
        """
        try:
            rec = {
                "ts": _now_iso(),
                "session": self._session_label,
                "role": role,
                "content": content,
                "model": self.model,
            }
            path = LOG_DIR / "chat_io.log"
            with path.open("a", encoding="utf-8") as f:
                f.write(json.dumps(rec, ensure_ascii=False) + "\n")
        except Exception:
            # Logging must never break chat
            pass

    # -------------------------
    # Public API: multimodal context memory (runtime-only)
    # -------------------------

    def set_mm_ctx_limit(self, n: int) -> None:
        """Set how many multimodal messages to retain (runtime-only)."""
        try:
            n = int(n)
        except Exception:
            n = 2
        self._mm_ctx_limit = max(0, min(n, 8))  # hard cap for safety
        if self._mm_ctx_limit == 0:
            self._mm_ctx = []
        else:
            self._mm_ctx = self._mm_ctx[-self._mm_ctx_limit :]

    def clear_mm_ctx(self) -> None:
        """Drop all remembered images (runtime-only)."""
        self._mm_ctx = []

    def remember_image(
        self,
        image_path_or_b64: str,
        *,
        prompt: str = "Image context for follow-up questions.",
        detail: str = "auto",
    ) -> None:
        """
        Store an image as runtime-only context so future chat turns can reference it.
        This does NOT call the model. It simply ensures the image is re-sent each turn.
        """
        if not image_path_or_b64:
            return

        data_url = self._image_to_data_url(image_path_or_b64)

        msg = {
            "role": "user",
            "content": [
                {"type": "input_text", "text": (prompt or "Image context.").strip()},
                {"type": "input_image", "image_url": data_url, "detail": detail},
            ],
        }

        self._mm_ctx.append(msg)
        if self._mm_ctx_limit > 0 and len(self._mm_ctx) > self._mm_ctx_limit:
            self._mm_ctx = self._mm_ctx[-self._mm_ctx_limit :]


    # -------------------------
    # Session markers
    # -------------------------

    def start_session(self, *, label: Optional[str] = None) -> None:
        """Append a session-start marker into context (timestamped)."""
        if self._session_open:
            return
        if label is not None:
            self._session_label = label
        self._session_open = True

        ts = _now_iso()
        marker = f"[SESSION START] label={self._session_label} ts={ts}"

        self._turns.append(ChatTurn("system", marker, ts))
        self._log_io(role="system", content=marker)


    def end_session(self) -> None:
        """Append a session-end marker into context (timestamped)."""
        if not self._session_open:
            return
        self._session_open = False

        ts = _now_iso()
        marker = f"[SESSION END] label={self._session_label} ts={ts}"

        self._turns.append(ChatTurn("system", marker, ts))
        self._log_io(role="system", content=marker)




    # -------------------------
    # Model profiles
    # -------------------------
    
    def _resolve_model_id(self, name_or_model_id: Optional[str]) -> str:
        raw = (name_or_model_id or "").strip()
        if not raw:
            return self.model
        return self.models.get(raw, raw)


    def set_model(self, name_or_model_id: str) -> str:
        """
        Switch the active model.

        - If name exists in self.models, uses that model id.
        - Otherwise treats input as a raw model id.

        Returns the resolved model id.
        """
        name_or_model_id = (name_or_model_id or "").strip()
        if not name_or_model_id:
            return self.model

        resolved = self.models.get(name_or_model_id, name_or_model_id)
        self.model = resolved
        return self.model

    def set_models(self, models: Dict[str, str]) -> None:
        self.models = dict(models or {})

    # -------------------------
    # JSON persistence
    # -------------------------

    def save_json(self, path: str | Path) -> None:
        """Save current state (including history, summary, and model config) to JSON."""
        p = Path(path)
        state = {
            "version": self.STATE_VERSION,
            "saved_at": _now_iso(),
            "system_prompt": self.system_prompt,
            "model": self.model,
            "models": self.models,
            "summarizer_model": self.summarizer_model,
            "enable_summary": self.enable_summary,
            "summarize_trigger_turns": self.summarize_trigger_turns,
            "max_turns_kept": self.max_turns_kept,
            "summary": self._summary,
            "session_label": self._session_label,
            "session_open": self._session_open,
            "turns": [asdict(t) for t in self._turns],
        }
        p.write_text(json.dumps(state, ensure_ascii=False, indent=2), encoding="utf-8")

    @classmethod
    def load_json(
        cls,
        path: str | Path,
        *,
        api_key: Optional[str] = None,
        request_timeout_s: Optional[float] = None,
        auto_resume_session: bool = True,
    ) -> "ChatGPTModule":
        """
        Load state from JSON and return a ready module.

        If auto_resume_session is True, appends a resume marker so the model can
        see that a new runtime session began.
        """

        p = Path(path)
        raw = json.loads(p.read_text(encoding="utf-8"))

        system_prompt = raw.get("system_prompt", "You are a helpful assistant.")
        model = raw.get("model", "gpt-4.1-mini")
        models = raw.get("models") or {}

        inst = cls(
            api_key=api_key,
            model=model,
            models=models,
            system_prompt=system_prompt,
            max_turns_kept=int(raw.get("max_turns_kept", 24)),
            enable_summary=bool(raw.get("enable_summary", True)),
            summarize_trigger_turns=int(raw.get("summarize_trigger_turns", 40)),
            summarizer_model=raw.get("summarizer_model") or model,
            request_timeout_s=request_timeout_s,
            auto_start_session=False,
            session_label=raw.get("session_label", "default"),
        )

        inst._summary = raw.get("summary", "") or ""
        inst._session_label = raw.get("session_label", "default")
        inst._session_open = bool(raw.get("session_open", False))

        turns: List[ChatTurn] = []
        for t in raw.get("turns", []):
            try:
                turns.append(ChatTurn(role=t["role"], content=t["content"], ts=t.get("ts") or _now_iso()))
            except Exception:
                # Skip malformed entries
                continue

        if turns:
            inst._turns = turns
        else:
            inst._turns = [ChatTurn("system", system_prompt, _now_iso())]

        if auto_resume_session:
            ts = _now_iso()
            marker = f"[SESSION RESUME] loaded_from={p.name} ts={ts}"
            inst._turns.append(ChatTurn("system", marker, ts))
            inst._log_io(role="system", content=marker)
            inst._session_open = True


        return inst

    # -------------------------
    # Public API
    # -------------------------

    def reset(self) -> None:
        """Clear the conversation state (keeps only the system prompt)."""
        self._pending_user = None
        self._summary = ""
        self._turns = [ChatTurn("system", self.system_prompt, _now_iso())]
        self._session_open = False
        self._mm_ctx = []

    def speak(self, text: str) -> None:
        """Queue a user message to be answered by the next listen() / listen_stream()."""
        text = (text or "").strip()
        if not text:
            return

        # LOG input
        self._log_io(role="user", content=text)

        self._pending_user = text


    def listen(self) -> str:
        """Send the queued user message to the model and return the assistant reply."""
        out: List[str] = []
        for chunk in self.listen_stream():
            out.append(chunk)
        return "".join(out)

    def listen_stream(self, *, on_delta: Optional[Callable[[str], None]] = None) -> Iterator[str]:
        """
        Send the queued user message to the model and stream back text deltas.

        Yields:
          - str: incremental text chunks (deltas)

        Also:
          - if on_delta is provided, it is called for each delta (useful for UI).

        Side effects:
          - Appends the full assistant reply to history as one ChatTurn.
          - Performs summarization and trimming consistent with listen().
        """
        if not self._pending_user:
            raise RuntimeError("listen_stream() called with no pending user input. Call speak(text) first.")

        if not self._session_open:
            # Ensure there is always a visible session boundary
            self.start_session(label=self._session_label)

        user_text = self._pending_user
        self._pending_user = None

        # Timestamped user message
        self._turns.append(ChatTurn("user", user_text, _now_iso()))

        # Manage context growth BEFORE calling the model
        # self._maybe_summarize()
        # self._trim_turns()

        input_messages = self._build_messages_for_request()

        reply_parts: List[str] = []
        t0 = time.perf_counter()

        # Primary path: Responses streaming API
        # The official SDK exposes a context manager for streaming.
        # We keep this defensive because event shapes can differ across SDK versions.
        try:
            with self.client.responses.stream(model=self._resolve_model_id(self.model), input=input_messages) as stream:
                for event in stream:
                    delta = self._extract_text_delta(event)
                    if delta:
                        reply_parts.append(delta)
                        if on_delta is not None:
                            on_delta(delta)
                        yield delta

                # Ensure the stream is fully consumed and errors are surfaced.
                # Many SDKs provide get_final_response(); some do not.
                try:
                    _ = stream.get_final_response()
                except Exception:
                    pass

        except Exception:
            # If we already streamed anything, do NOT replay the full response;
            # just finish using what we already have (prevents duplicate on_delta).
            if reply_parts:
                # Option A: swallow the error and continue (best for UI continuity)
                pass
                # Option B: re-raise to surface real streaming errors (stricter)
                # raise
            else:
                # Fallback: non-streaming call (yields once)
                full = self._call_model(input_messages)
                full = (full or "")
                if full:
                    reply_parts = [full]
                    if on_delta is not None:
                        on_delta(full)
                    yield full


        # Timeout check (best-effort; this measures total stream wall time)
        if self.request_timeout_s is not None:
            if (time.perf_counter() - t0) > float(self.request_timeout_s):
                raise TimeoutError(f"OpenAI request exceeded timeout ({self.request_timeout_s}s).")

        reply = "".join(reply_parts).strip()

        # LOG output
        if reply:
            self._log_io(role="assistant", content=reply)

        self._turns.append(ChatTurn("assistant", reply, _now_iso()))
        self._trim_turns()


    def generate_image(
        self,
        prompt: str,
        *,
        size: str = "1024x1024",
        n: int = 1,
        quality: Optional[str] = None,        # accepted, not forwarded unless you explicitly wire it
        style: Optional[str] = None,          # accepted, not forwarded
        response_format: Optional[str] = None,# accepted, not forwarded
        out_dir: str | Path = IMAGE_DIR,      # you already have IMAGE_DIR
        model: Optional[str] = None,
        **_ignored: object,                   # swallow any extra kwargs safely
    ) -> str:
        """
        Generate an image and save it to disk.
        Returns the saved PNG filepath as a string (or raises on hard failures).
        """
        import uuid
        import base64

        prompt = (prompt or "").strip()
        if not prompt:
            raise ValueError("generate_image(): empty prompt")

        use_model = self._resolve_model_id(model or self.image_model)

        # normalize n (we only return one path for overlay use)
        try:
            n = int(n)
        except Exception:
            n = 1
        n = 1 if n <= 0 else min(n, 8)

        out_path = Path(out_dir)
        out_path.mkdir(parents=True, exist_ok=True)

        # IMPORTANT: Only pass parameters you know the endpoint accepts.
        # Do NOT pass response_format/quality/style unless you have verified support.
        print("\nGENNING IMAGE\n")
        result = self.client.images.generate(
            model=use_model,
            prompt=prompt,
            size=size,
        )
        print("\nGENNING IMAGE FINISHED\n")
        
        # OpenAI images.generate commonly returns base64 JSON in data[0].b64_json
        b64 = getattr(result.data[0], "b64_json", None) if result and getattr(result, "data", None) else None
        if not b64:
            # Defensive: if your SDK returns dicts
            try:
                b64 = result["data"][0].get("b64_json")
            except Exception:
                b64 = None

        if not b64:
            raise RuntimeError("generate_image(): no b64_json returned by images.generate()")

        file_path = out_path / f"{uuid.uuid4().hex}.png"
        file_path.write_bytes(base64.b64decode(b64))
        return str(file_path)


    # -------------------------
    # Public API: vision helper
    # -------------------------

    def _latest_generated_image(self) -> Optional[str]:
        """
        Return the most recently modified image file in IMAGE_DIR,
        or None if no images exist.
        """
        try:
            if not IMAGE_DIR.exists():
                return None

            files = [
                p for p in IMAGE_DIR.iterdir()
                if p.is_file() and p.suffix.lower() in {".png", ".jpg", ".jpeg", ".webp", ".gif"}
            ]
            if not files:
                return None

            latest = max(files, key=lambda p: p.stat().st_mtime)
            return str(latest.resolve())
        except Exception:
            return None

    def see_image(
        self,
        image_path_or_b64: Optional[str] = None,
        prompt: str = "Describe what you see in this image.",
        *,
        detail: str = "auto",
        model: Optional[str] = None,
    ) -> str:
        """
        Send an image + prompt to a vision-capable model and return the assistant text.

        - Persists the exchange in _turns (so it is saved/loaded with save_json/load_json).
        - Writes audit entries via _log_io (user + assistant).
        """
        # If no image specified, fall back to most recent generated image
        if not image_path_or_b64:
            image_path_or_b64 = self._latest_generated_image()
            if not image_path_or_b64:
                raise ValueError("see_image(): no image provided and no generated images found in IMAGE_DIR")

        if not self._session_open:
            self.start_session(label=self._session_label)

        prompt = (prompt or "").strip() or "Describe what you see in this image."

        # Resolve the image to a data URL
        data_url = self._image_to_data_url(image_path_or_b64)

        # Record a compact marker in conversation memory (avoid stuffing base64 into _turns)
        ts_user = _now_iso()
        marker = f"[SEE_IMAGE] prompt={prompt}"
        self._turns.append(ChatTurn("user", marker, ts_user))
        self._log_io(role="user", content=f"{marker} (image={self._redact_image_ref(image_path_or_b64)})")

        use_model = self._resolve_model_id(model or self.model)

        # Build context INCLUDING prior turns, but EXCLUDING the just-added marker.
        input_payload = [{"role": "system", "content": self.system_prompt}]

        if self._summary.strip():
            input_payload.append(
                {
                    "role": "system",
                    "content": "Conversation summary (for continuity):\n" + self._summary.strip(),
                }
            )

        # Include prior turns (without timestamps), excluding:
        # - the system prompt at index 0
        # - the marker we just appended (last turn)
        for t in self._turns[1:-1]:
            input_payload.append({"role": t.role, "content": t.content})

        # Append the actual multimodal message (prompt + image)
        input_payload.append(
            {
                "role": "user",
                "content": [
                    {"type": "input_text", "text": prompt},
                    {"type": "input_image", "image_url": data_url, "detail": detail},
                ],
            }
        )

        # Call model (non-streaming)
        t0 = time.perf_counter()
        resp = self.client.responses.create(
            model=use_model,
            input=input_payload,
        )

        if self.request_timeout_s is not None:
            if (time.perf_counter() - t0) > float(self.request_timeout_s):
                raise TimeoutError(f"OpenAI request exceeded timeout ({self.request_timeout_s}s).")

        reply = (getattr(resp, "output_text", "") or "").strip()

        # Log + persist assistant reply
        if reply:
            self._log_io(role="assistant", content=reply)

        self._turns.append(ChatTurn("assistant", reply, _now_iso()))
        self._trim_turns()

        return reply


    def _image_to_data_url(self, image_path_or_b64: str) -> str:
        """
        Convert an image path / data URL / raw base64 string into a data URL suitable
        for Responses multimodal input.
        """
        s = (image_path_or_b64 or "").strip()
        if not s:
            raise ValueError("_image_to_data_url(): empty input")

        # Already a data URL?
        if s.lower().startswith("data:image/") and "base64," in s.lower():
            return s

        # If it's a file path, read bytes and infer mime from suffix
        p = Path(s)
        if p.exists() and p.is_file():
            data = p.read_bytes()
            b64 = base64.b64encode(data).decode("ascii")
            mime = self._infer_image_mime(p.suffix)
            return f"data:{mime};base64,{b64}"

        # Otherwise treat as raw base64 (no header)
        # Validate lightly by attempting a decode
        try:
            _ = base64.b64decode(s, validate=True)
        except Exception as e:
            raise ValueError(f"_image_to_data_url(): not a file and not valid base64: {e}") from e

        # Default mime when unknown
        return f"data:image/png;base64,{s}"

    @staticmethod
    def _infer_image_mime(suffix: str) -> str:
        ext = (suffix or "").lower().lstrip(".")
        if ext in {"jpg", "jpeg"}:
            return "image/jpeg"
        if ext == "webp":
            return "image/webp"
        if ext == "gif":
            return "image/gif"
        # default
        return "image/png"

    @staticmethod
    def _redact_image_ref(image_path_or_b64: str) -> str:
        """
        Avoid logging base64 blobs; keep logs readable.
        """
        s = (image_path_or_b64 or "").strip()
        if not s:
            return ""
        if s.lower().startswith("data:image/"):
            return "data_url"
        p = Path(s)
        if p.exists():
            try:
                return str(p.resolve())
            except Exception:
                return str(p)
        # raw base64
        return f"base64(len={len(s)})"


    @staticmethod
    def _safe_stamp(ts_iso: str) -> str:
        # Make ISO timestamp filename-safe: 2026-01-07T19:05:22+10:30 -> 20260107_190522p1030
        s = ts_iso
        s = s.replace(":", "").replace("-", "")
        s = s.replace("T", "_")
        s = s.replace("+", "p").replace("Z", "z")
        # Trim fractional if present, keep offset if present
        s = re.sub(r"\.\d+", "", s)
        return s


    # -------------------------
    # Internal: context handling
    # -------------------------

    def _build_messages_for_request(self) -> List[dict]:
        msgs: List[dict] = []

        msgs.append({"role": "system", "content": self.system_prompt})

        if self._summary.strip():
            msgs.append(
                {
                    "role": "system",
                    "content": "Conversation summary (for continuity):\n" + self._summary.strip(),
                }
            )

        # Build from saved turns WITHOUT timestamps
        # We will inject mm context right before the most recent user message (the current prompt)
        turns = [{"role": t.role, "content": t.content} for t in self._turns[1:]]

        if self._mm_ctx and turns:
            last = turns[-1]
            if last.get("role") == "user":
                # everything except the current user prompt
                msgs.extend(turns[:-1])
                # runtime-only image context
                msgs.extend(self._mm_ctx)
                # current user prompt
                msgs.append(last)
                return msgs

        # fallback: no special placement
        msgs.extend(turns)
        if self._mm_ctx:
            msgs.extend(self._mm_ctx)

        return msgs



    def _trim_turns(self) -> None:
        """Keep the system prompt plus a rolling window of recent turns."""
        if not self._turns:
            self._turns = [ChatTurn("system", self.system_prompt, _now_iso())]
            return

        system = self._turns[0] if self._turns[0].role == "system" else ChatTurn("system", self.system_prompt, _now_iso())
        rest = self._turns[1:]

        keep = rest[-2 * self.max_turns_kept :]
        self._turns = [system] + keep

    def _maybe_summarize(self) -> None:
        """Compress older turns into a running summary when history grows."""
        if not self.enable_summary:
            return

        rest = self._turns[1:]
        if len(rest) < self.summarize_trigger_turns:
            return

        keep_tail = 2 * self.max_turns_kept
        old = rest[: max(0, len(rest) - keep_tail)]
        if not old:
            return

        old_text = self._format_turns_for_summary(old)

        summary_prompt = [
            {"role": "system", "content": "You compress chat logs into a concise, factual summary for future context."},
            {
                "role": "user",
                "content": (
                    "Update the running summary based on the additional chat log.\n\n"
                    "Rules:\n"
                    "- Keep it compact.\n"
                    "- Preserve user goals, preferences, decisions, and key facts.\n"
                    "- Preserve any important names, file paths, constraints.\n"
                    "- Keep explicit session boundaries (SESSION START/END/RESUME).\n"
                    "- Do not include irrelevant chatter.\n\n"
                    f"Current summary:\n{self._summary.strip() or '(none)'}\n\n"
                    f"Additional chat log:\n{old_text}\n"
                ),
            },
        ]

        new_summary = self._call_model(summary_prompt, model=self.summarizer_model)
        new_summary = (new_summary or "").strip()
        if new_summary:
            self._summary = new_summary

        # Drop summarized turns, keep tail
        self._turns = [self._turns[0]] + rest[-keep_tail:]

    @staticmethod
    def _format_turns_for_summary(turns: List[ChatTurn]) -> str:
        lines: List[str] = []
        for t in turns:
            role = t.role.upper()
            lines.append(f"[{t.ts}] {role}: {t.content}")
        return "\n".join(lines)

    # -------------------------
    # Internal: streaming helpers
    # -------------------------

    @staticmethod
    # Important note: Sometimes duplicate deltas can come through here if you are too permissive for some reason
    def _extract_text_delta(event: object) -> str:
        def _get(obj: object, key: str, default: object = None) -> object:
            if isinstance(obj, dict):
                return obj.get(key, default)
            return getattr(obj, key, default)

        etype = (_get(event, "type", "") or "").strip()

        # Only accept explicit output_text delta events.
        if etype == "response.output_text.delta":
            delta = _get(event, "delta", "")
            return delta if isinstance(delta, str) else ""

        # Optional: if your SDK uses a slightly different naming, add it here,
        # but do it explicitly (do not fall back to generic fields).
        # if etype == "response.text.delta":
        #     ...

        return ""


    # -------------------------
    # Internal: model call
    # -------------------------

    def _call_model(self, input_messages: List[Dict[str, str]], model: Optional[str] = None) -> str:
        t0 = time.perf_counter()
        use_model = model or self.model
        use_model = self._resolve_model_id(model or self.model)
        resp = self.client.responses.create(model=use_model, input=input_messages)
        if self.request_timeout_s is not None:
            if (time.perf_counter() - t0) > float(self.request_timeout_s):
                raise TimeoutError(f"OpenAI request exceeded timeout ({self.request_timeout_s}s).")
        return (resp.output_text or "")


# -------------------------
# Tiny interactive test
# -------------------------


def _demo() -> None:
    state_path = Path("chat_state.json")

    if state_path.exists():
        gpt = ChatGPTModule.load_json(state_path, auto_resume_session=True)
    else:
        gpt = ChatGPTModule(
            system_prompt="You are a helpful assistant.",
            models={
                "fast": "gpt-4.1-mini",
                "quality": "gpt-4.1",
            },
            model="fast",
            auto_start_session=True,
            session_label="cmd",
        )

    print(
        "ChatGPTModule demo. Commands: /reset /save /load /model <nameOrId> /end /exit\n"
        "Streaming: replies are printed as they arrive.\n"
    )

    while True:
        user = input("You: ").strip()
        if not user:
            continue

        if user.lower() in {"/exit", "/quit"}:
            gpt.end_session()
            gpt.save_json(state_path)
            break

        if user.lower() == "/reset":
            gpt.reset()
            print("(reset)\n")
            continue

        if user.lower() == "/end":
            gpt.end_session()
            print("(session ended)\n")
            continue

        if user.lower() == "/save":
            gpt.save_json(state_path)
            print(f"(saved to {state_path})\n")
            continue

        if user.lower() == "/load":
            if state_path.exists():
                gpt = ChatGPTModule.load_json(state_path, auto_resume_session=True)
                print(f"(loaded from {state_path})\n")
            else:
                print(f"(no state file at {state_path})\n")
            continue

        if user.lower().startswith("/model "):
            target = user.split(" ", 1)[1].strip()
            resolved = gpt.set_model(target)
            print(f"(model set to: {resolved})\n")
            continue

        gpt.speak(user)
        print("\nGPT: ", end="", flush=True)
        for chunk in gpt.listen_stream():
            print(chunk, end="", flush=True)
        print("\n")


if __name__ == "__main__":
    _demo()
