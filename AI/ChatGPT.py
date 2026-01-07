
"""
ChatGPT.py
Wildcat/AI/ChatGPT.py
Wildcat.AI.ChatGPT
AI Code

Very simple module to create and talk to a ChatGPT instance. Loads and saves context.
"""

from __future__ import annotations

import os
import time
import json
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional

from openai import OpenAI


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
      - listen() -> str: get assistant reply

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
        models: Optional[Dict[str, str]] = None,
        system_prompt: str = "You are a helpful assistant.",
        max_turns_kept: int = 24,
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
        self.model = model
        self.summarizer_model = summarizer_model or model

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
    # Session markers
    # -------------------------

    def start_session(self, *, label: Optional[str] = None) -> None:
        """Append a session-start marker into context."""
        if self._session_open:
            return
        if label is not None:
            self._session_label = label
        self._session_open = True
        self._turns.append(
            ChatTurn(
                "system",
                f"[SESSION START] label={self._session_label} ts={_now_iso()}",
                _now_iso(),
            )
        )

    def end_session(self) -> None:
        """Append a session-end marker into context."""
        if not self._session_open:
            return
        self._session_open = False
        self._turns.append(
            ChatTurn(
                "system",
                f"[SESSION END] label={self._session_label} ts={_now_iso()}",
                _now_iso(),
            )
        )

    # -------------------------
    # Model profiles
    # -------------------------

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

        turns = []
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
            inst._turns.append(
                ChatTurn(
                    "system",
                    f"[SESSION RESUME] loaded_from={p.name} ts={_now_iso()}",
                    _now_iso(),
                )
            )
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

    def speak(self, text: str) -> None:
        """Queue a user message to be answered by the next listen()."""
        text = (text or "").strip()
        if not text:
            return
        self._pending_user = text

    def listen(self) -> str:
        """Send the queued user message to the model and return the assistant reply."""
        if not self._pending_user:
            raise RuntimeError("listen() called with no pending user input. Call speak(text) first.")

        if not self._session_open:
            # Ensure there is always a visible session boundary
            self.start_session(label=self._session_label)

        user_text = self._pending_user
        self._pending_user = None

        # Timestamped user message
        self._turns.append(ChatTurn("user", user_text, _now_iso()))

        # Manage context growth BEFORE calling the model
        self._maybe_summarize()
        self._trim_turns()

        reply = self._call_model(self._build_messages_for_request())

        reply = (reply or "").strip()
        self._turns.append(ChatTurn("assistant", reply, _now_iso()))
        self._trim_turns()
        return reply

    # -------------------------
    # Internal: context handling
    # -------------------------

    def _build_messages_for_request(self) -> List[Dict[str, str]]:
        """Assemble messages for the request."""
        msgs: List[Dict[str, str]] = []

        # Base system prompt
        msgs.append({"role": "system", "content": self.system_prompt})

        # Running summary (system)
        if self._summary.strip():
            msgs.append(
                {
                    "role": "system",
                    "content": f"Conversation summary (for continuity):
{self._summary.strip()}",
                }
            )

        # Include timestamped turns as-is (including system markers)
        for t in self._turns[1:]:
            # Put timestamp into the content for better chronological grounding.
            # This is intentionally simple and model-friendly.
            content = f"[{t.ts}] {t.content}"
            msgs.append({"role": t.role, "content": content})

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
                    "Update the running summary based on the additional chat log.

"
                    "Rules:
"
                    "- Keep it compact.
"
                    "- Preserve user goals, preferences, decisions, and key facts.
"
                    "- Preserve any important names, file paths, constraints.
"
                    "- Keep explicit session boundaries (SESSION START/END/RESUME).
"
                    "- Do not include irrelevant chatter.

"
                    f"Current summary:
{self._summary.strip() or '(none)'}

"
                    f"Additional chat log:
{old_text}
"
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
        return "
".join(lines)

    # -------------------------
    # Internal: model call
    # -------------------------

    def _call_model(self, input_messages: List[Dict[str, str]], model: Optional[str] = None) -> str:
        use_model = model or self.model
        t0 = time.perf_counter()
        resp = self.client.responses.create(
            model=use_model,
            input=input_messages,
        )
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

    print("ChatGPTModule demo. Commands: /reset /save /load /model <nameOrId> /end /exit
")

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
            print("(reset)
")
            continue

        if user.lower() == "/end":
            gpt.end_session()
            print("(session ended)
")
            continue

        if user.lower() == "/save":
            gpt.save_json(state_path)
            print(f"(saved to {state_path})
")
            continue

        if user.lower() == "/load":
            if state_path.exists():
                gpt = ChatGPTModule.load_json(state_path, auto_resume_session=True)
                print(f"(loaded from {state_path})
")
            else:
                print(f"(no state file at {state_path})
")
            continue

        if user.lower().startswith("/model "):
            target = user.split(" ", 1)[1].strip()
            resolved = gpt.set_model(target)
            print(f"(model set to: {resolved})
")
            continue

        gpt.speak(user)
        reply = gpt.listen()
        print(f"
GPT: {reply}
")


if __name__ == "__main__":
    _demo()
