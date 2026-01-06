"""
IndexTTS2-TestClient.py
AI code

Interactive command-line client for the IndexTTS2 TTS server.

Purpose
-------
Provides a simple, reliable way to:
- Connect to a running IndexTTS2-Server instance.
- Send text-to-speech requests.
- Exercise server control commands.
- Validate end-to-end audio playback behavior.

This client is intended primarily for:
- Manual testing.
- Debugging server behavior.
- Verifying queueing, interruption, and voice changes.

Key Capabilities
----------------
- Persistent TCP connection with newline-delimited JSON protocol.
- Convenience wrappers for common server commands:
    * speak
    * ping
    * get_state
    * stop_all
    * stop_and_clear
    * set_voice_prompt
- Interactive REPL-style interface with slash commands.

Supported Console Commands
--------------------------
/quit, /q, /exit      - Close the client
/ping                - Check server liveness
/state               - Query playback and stream state
/stop                - Stop current playback
/clear               - Stop playback and clear all queues
/voice <wav_path>    - Change the active voice prompt

Any other input is sent as:
    {"cmd": "speak", "text": "<your input>"}

Protocol Notes
--------------
- Communication uses blocking sockets.
- Requests and responses are newline-delimited JSON objects.
- Each request expects exactly one response.

Threading / I/O Model
---------------------
- Single-threaded, synchronous I/O.
- Designed for interactive use rather than high throughput.
- Server-side concurrency is handled entirely by the TTS server.

Design Constraints
------------------
- Assumes a local or trusted network environment.
- Not hardened for hostile inputs or public exposure.
- Intended as a diagnostic and development tool, not a production client.
"""

from __future__ import annotations

import json
import socket
import sys
from typing import Any, Dict, Optional

HOST = "127.0.0.1"
PORT = 17777


class TTSClient:
    def __init__(self, host: str = HOST, port: int = PORT, timeout_s: float = 10.0) -> None:
        self.host = host
        self.port = port
        self.timeout_s = float(timeout_s)
        self.sock: Optional[socket.socket] = None
        self.f_r = None
        self.f_w = None

    def connect(self) -> None:
        if self.sock is not None:
            return
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(self.timeout_s)
        s.connect((self.host, self.port))
        s.settimeout(None)  # switch back to blocking for interactive use

        self.sock = s
        self.f_r = s.makefile("r", encoding="utf-8", newline="\n")
        self.f_w = s.makefile("w", encoding="utf-8", newline="\n")

    def close(self) -> None:
        try:
            if self.f_r:
                self.f_r.close()
        except Exception:
            pass
        try:
            if self.f_w:
                self.f_w.close()
        except Exception:
            pass
        try:
            if self.sock:
                self.sock.close()
        except Exception:
            pass
        self.sock = None
        self.f_r = None
        self.f_w = None

    def request(self, payload: Dict[str, Any]) -> Dict[str, Any]:
        if self.sock is None:
            self.connect()

        assert self.f_w is not None and self.f_r is not None

        self.f_w.write(json.dumps(payload, ensure_ascii=False) + "\n")
        self.f_w.flush()

        line = self.f_r.readline()
        if not line:
            raise ConnectionError("Server closed connection")
        return json.loads(line)

    # Convenience wrappers
    def speak(self, text: str) -> Dict[str, Any]:
        return self.request({"cmd": "speak", "text": text})

    def ping(self) -> Dict[str, Any]:
        return self.request({"cmd": "ping"})

    def get_state(self) -> Dict[str, Any]:
        return self.request({"cmd": "get_state"})

    def stop_all(self) -> Dict[str, Any]:
        return self.request({"cmd": "stop_all"})

    def stop_and_clear(self) -> Dict[str, Any]:
        return self.request({"cmd": "stop_and_clear"})

    def set_voice_prompt(self, wav_path: str) -> Dict[str, Any]:
        return self.request({"cmd": "set_voice_prompt", "wav_path": wav_path})


def main() -> int:
    client = TTSClient()

    try:
        client.connect()
        resp = client.ping()
        if not resp.get("ok"):
            print("Ping failed:", resp)
            return 1
    except Exception as e:
        print(f"Could not connect to TTS server at {HOST}:{PORT}: {e}")
        return 1

    print(f"Connected to TTS server at {HOST}:{PORT}")
    print("Type text and press Enter to speak.")
    print("Commands: /quit, /ping, /state, /stop, /clear, /voice <wav_path>")
    print()

    try:
        while True:
            try:
                line = input("> ")
            except EOFError:
                # Ctrl+Z then Enter on Windows, Ctrl+D on Unix
                break

            line = line.strip()
            if not line:
                continue

            if line in ("/q", "/quit", "/exit"):
                break

            # simple slash-commands
            if line == "/ping":
                print(client.ping())
                continue
            if line == "/state":
                print(client.get_state())
                continue
            if line == "/stop":
                print(client.stop_all())
                continue
            if line == "/clear":
                print(client.stop_and_clear())
                continue
            if line.startswith("/voice "):
                wav = line[len("/voice ") :].strip().strip('"')
                if not wav:
                    print("Usage: /voice <wav_path>")
                    continue
                print(client.set_voice_prompt(wav))
                continue

            # normal speak
            resp = client.speak(line)
            if not resp.get("ok"):
                print("Server error:", resp)

    except KeyboardInterrupt:
        pass
    finally:
        client.close()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
