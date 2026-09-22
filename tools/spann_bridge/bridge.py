#!/usr/bin/env python3
"""Persistent local stub/model process for the AW2 Spann mailbox bridge."""

from __future__ import annotations

import argparse
import json
import logging
import sys
import time
from pathlib import Path
from typing import Any, Callable

from mailbox_layout import REQUEST
from protocol import (
    FILE_KIND_REQUEST,
    FILE_KIND_RESPONSE,
    Envelope,
    atomic_write,
    decode_envelope,
    encode_envelope,
    request_state_seed,
    response_payload,
)
from state_adapter import state_from_snapshot, translate_model_action

LOG = logging.getLogger("spann-bridge")


def stub_selector(state: dict[str, Any], legal: list[dict[str, Any]], _env: object) -> int:
    del state
    for index, action in enumerate(legal):
        if action.get("branch") == "build" and action.get("unit_type") == "infantry":
            return index
    for index, action in enumerate(legal):
        if action.get("branch") == "end_turn":
            return index
    raise RuntimeError("stub found neither infantry build nor end turn")


def load_runtime(awbw_root: Path, checkpoint: Path | None, mode: str, device: str) -> tuple[type, Callable]:
    python_root = awbw_root / "python"
    if str(python_root) not in sys.path:
        sys.path.insert(0, str(python_root))
    from awbw_native import NativeEnv

    if mode == "stub":
        return NativeEnv, stub_selector
    if checkpoint is None:
        raise ValueError("model mode requires --checkpoint")
    from awbw_live.runner import policy_selector

    LOG.info("loading model once from %s", checkpoint)
    return NativeEnv, policy_selector(checkpoint, device=device)


def process_request(path: Path, runtime_dir: Path, template: dict[str, Any], native_env: type, selector: Callable) -> Path:
    envelope = decode_envelope(path.read_bytes(), expected_kind=FILE_KIND_REQUEST)
    state = state_from_snapshot(template, envelope.payload)
    env = native_env(json.dumps(state, separators=(",", ":")))
    legal = json.loads(env.legal_actions_json())
    selected = int(selector(state, legal, env))
    legal = json.loads(env.legal_actions_json())
    if not 0 <= selected < len(legal):
        raise ValueError("selector returned an invalid legal-action index")
    model_action = legal[selected]
    native_action = translate_model_action(model_action, state)
    payload = response_payload(native_action, request_state_seed(envelope.payload))
    response = Envelope(FILE_KIND_RESPONSE, envelope.session_id, envelope.request_id, payload)
    response_path = runtime_dir / f"response-{envelope.session_id:08x}-{envelope.request_id:08x}.bin"
    atomic_write(response_path, encode_envelope(response))
    LOG.info(
        "session=%08x request=%d day=%d action=%s native=%s",
        envelope.session_id,
        envelope.request_id,
        state["day"],
        model_action,
        native_action,
    )
    path.unlink(missing_ok=True)
    return response_path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, default=Path("spann-bridge.example.json"))
    parser.add_argument("--mode", choices=("stub", "model"))
    parser.add_argument("--once", action="store_true")
    args = parser.parse_args()
    config = json.loads(args.config.read_text(encoding="utf-8"))
    logging.basicConfig(level=getattr(logging, str(config.get("log_level", "INFO")).upper()))
    mode = args.mode or str(config.get("mode", "stub"))
    awbw_root = Path(config["awbw_root"])
    checkpoint = Path(config["checkpoint"]) if config.get("checkpoint") else None
    runtime_dir = Path(config.get("runtime_dir", "runtime")).resolve()
    runtime_dir.mkdir(parents=True, exist_ok=True)
    template_path = awbw_root / "maps" / "aw2-war-room" / "spann-island" / "initial-state.json"
    template = json.loads(template_path.read_text(encoding="utf-8"))
    native_env, selector = load_runtime(awbw_root, checkpoint, mode, str(config.get("device", "auto")))
    LOG.info("ready: mode=%s runtime=%s", mode, runtime_dir)
    while True:
        requests = sorted(runtime_dir.glob("request-*.bin"))
        if requests:
            for path in requests:
                try:
                    process_request(path, runtime_dir, template, native_env, selector)
                except Exception:
                    LOG.exception("request failed: %s", path)
                    path.rename(path.with_suffix(".error"))
            if args.once:
                return
        elif args.once:
            raise SystemExit("no request file found")
        time.sleep(0.01)


if __name__ == "__main__":
    main()
