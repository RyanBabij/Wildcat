
"""
Whisper_Client.py
Wildcat/AI/Whisper_Client.py
Wildcat.AI.Whisper_Client
AI Code

Lightweight TCP client for the standalone whisper.py transcription service.

Protocol
---------
Request:
  {"cmd":"transcribe","audio_f32_b64":"...","sample_rate":16000}

Response:
  {"ok":true,"text":"...","language":"en","confidence":0.98}

Public API
----------
class WhisperClient
    __init__(
        host: str = "127.0.0.1",
        port: int = 17778,
        sample_rate: int = 16000,
        timeout_s: float = 30.0
    )
        Configure connection details for the Whisper TCP service.

    transcribe_audio_array(audio: np.ndarray) -> Tuple[str, str, float]
        Send a mono float32 audio array to the server and return:
          - text (str): transcription result
          - language (str): detected language code
          - confidence (float): confidence score

Notes
-----
- Audio must be a NumPy array convertible to float32.
- Networking uses a simple newline-delimited JSON protocol.
- All socket operations are blocking and respect timeout_s.
"""


from __future__ import annotations

import base64
import json
import socket
from typing import Tuple

import numpy as np


def _json_dumps(obj: dict) -> bytes:
    return (json.dumps(obj, ensure_ascii=False) + "\n").encode("utf-8")


def _json_loads(b: bytes) -> dict:
    return json.loads(b.decode("utf-8", errors="strict"))


def _encode_audio_f32_b64(audio: np.ndarray) -> str:
    a = np.asarray(audio, dtype=np.float32).reshape(-1)
    return base64.b64encode(a.tobytes()).decode("ascii")


class WhisperClient:
    """
    Client for the standalone whisper.py TCP service.

    Request:
      {"cmd":"transcribe","audio_f32_b64":"...","sample_rate":16000}

    Response:
      {"ok":true,"text":"...","language":"en","confidence":0.98}
    """

    def __init__(
        self,
        host: str = "127.0.0.1",
        port: int = 17778,
        sample_rate: int = 16000,
        timeout_s: float = 30.0,
    ):
        self.host = host
        self.port = int(port)
        self.sample_rate = int(sample_rate)
        self.timeout_s = float(timeout_s)

    def transcribe_audio_array(self, audio: np.ndarray) -> Tuple[str, str, float]:
        req = {
            "cmd": "transcribe",
            "audio_f32_b64": _encode_audio_f32_b64(audio),
            "sample_rate": self.sample_rate,
        }

        with socket.create_connection((self.host, self.port), timeout=self.timeout_s) as s:
            s.settimeout(self.timeout_s)
            s.sendall(_json_dumps(req))

            buf = b""
            while b"\n" not in buf:
                chunk = s.recv(65536)
                if not chunk:
                    break
                buf += chunk

        line = buf.split(b"\n", 1)[0].strip()
        if not line:
            raise RuntimeError("Whisper server returned no response")

        resp = _json_loads(line)
        if not resp.get("ok"):
            raise RuntimeError(resp.get("error", "Whisper transcribe failed"))

        text = str(resp.get("text", "") or "").strip()
        lang = str(resp.get("language", "") or "").strip()
        conf = float(resp.get("confidence", 0.0) or 0.0)
        return text, lang, conf
