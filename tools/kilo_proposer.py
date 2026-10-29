#!/usr/bin/env python3
"""Bridge the offline worker protocol to the Kilo Code gateway.

The upstream ``tools/local_proposer.py`` accepts loopback endpoints only, because
retail-derived material must not leave the machine by default. This project-local
adapter is the explicit opt-in for a remote gateway: it reads the API key from
the environment (never from a file in the repository) and posts the same JSON
request the worker builds.

Usage:

    export KILOCODE_API_KEY=...
    python3 tools/kilo_proposer.py --model deepseek/deepseek-v4.1-flash-1017

The worker sends one JSON request on stdin and expects one JSON response with
``source``, ``symbols`` and ``note`` on stdout.
"""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.request

DEFAULT_URL = "https://api.kilo.ai/api/gateway/chat/completions"


def unwrap_json(text: str) -> str:
    text = text.strip()
    if not text:
        raise ValueError("gateway returned empty content (the token budget was "
                         "consumed by reasoning; raise --max-tokens or lower the effort)")
    if text.startswith("```"):
        lines = text.splitlines()
        if lines and lines[0].startswith("```"):
            lines.pop(0)
        if lines and lines[-1].strip() == "```":
            lines.pop()
        text = "\n".join(lines).strip()
    try:
        value = json.loads(text)
    except json.JSONDecodeError:
        start, end = text.find("{"), text.rfind("}")
        if start < 0 or end <= start:
            raise
        value = json.loads(text[start:end + 1])
    if not isinstance(value, dict):
        raise ValueError("generated response is not a JSON object")
    return json.dumps(value)


def prompt(request: dict) -> str:
    return (
        "Perform the requested byte-matching C reconstruction. Study the "
        "assembly oracle and improve the prior candidate using its compiler "
        "score and mismatch evidence. Emit exactly one JSON object with source, "
        "symbols, and note fields. Do not emit markdown or assembly source.\n\n"
        + json.dumps(request, indent=2, sort_keys=True)
    )


def query(url: str, model: str, request: dict, temperature: float,
          max_tokens: int, timeout: int, key: str, effort: str = "low") -> str:
    body = json.dumps({
        "model": model,
        "messages": [
            {"role": "system", "content": (
                "You reconstruct maintainable ANSI C for a PlayStation matching "
                "decompilation with GCC 2.7/2.8 PSX. Reason from the MIPS "
                "assembly oracle to C that compiles to the same bytes. Return "
                "only the requested JSON object."
            )},
            {"role": "user", "content": prompt(request)},
        ],
        "temperature": temperature,
        "max_tokens": max_tokens,
        # Reasoning models otherwise burn the whole completion budget on
        # reasoning_tokens and return empty content (finish_reason=length).
        "reasoning": {"effort": effort},
    }).encode()
    http_request = urllib.request.Request(
        url, data=body,
        headers={"Content-Type": "application/json", "Authorization": f"Bearer {key}"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(http_request, timeout=timeout) as response:
            reply = json.load(response)
    except urllib.error.HTTPError as error:
        detail = error.read().decode(errors="replace")[-2000:]
        raise RuntimeError(f"gateway returned HTTP {error.code}: {detail}") from error
    try:
        content = reply["choices"][0]["message"]["content"]
    except (KeyError, IndexError, TypeError) as error:
        raise ValueError("gateway response has no generated message") from error
    if not isinstance(content, str):
        raise ValueError("gateway message is not text")
    finish = (reply["choices"][0] or {}).get("finish_reason")
    if finish == "length" and not content.strip():
        raise ValueError("gateway stopped on the token limit with empty content; "
                         "lower --reasoning-effort or raise --max-tokens")
    return unwrap_json(content)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default=os.environ.get("KILO_URL", DEFAULT_URL))
    parser.add_argument("--model", default="deepseek/deepseek-v4.1-flash-1017")
    parser.add_argument("--temperature", type=float, default=0.3)
    parser.add_argument("--max-tokens", type=int, default=8192)
    parser.add_argument("--reasoning-effort", default=os.environ.get("KILO_REASONING_EFFORT", "low"),
                        choices=["none", "low", "high", "max"])
    parser.add_argument("--timeout", type=int, default=600)
    args = parser.parse_args()

    key = os.environ.get("KILOCODE_API_KEY", "").strip()
    if not key:
        print("KILOCODE_API_KEY is not set", file=sys.stderr)
        return 1
    try:
        request = json.load(sys.stdin)
        if not isinstance(request, dict):
            raise ValueError("worker request is not a JSON object")
        print(query(args.url, args.model, request, args.temperature,
                    args.max_tokens, args.timeout, key, args.reasoning_effort))
    except (json.JSONDecodeError, OSError, RuntimeError, ValueError) as error:
        print(str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
