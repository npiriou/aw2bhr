#!/usr/bin/env python3
"""Persistent local stub/model process for the AW2 local-agent mailbox bridge."""

from __future__ import annotations

import argparse
import copy
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
from state_adapter import is_translatable_action, state_from_snapshot, translate_model_action

LOG = logging.getLogger("aw2-bridge")


def stub_selector(state: dict[str, Any], legal: list[dict[str, Any]], _env: object) -> int:
    del state
    for power in ("scop", "cop"):
        for index, action in enumerate(legal):
            if action.get("branch") == "power" and action.get("power") == power:
                return index
    for command in ("unload", "load", "join", "fire_silo", "supply", "hide"):
        for index, action in enumerate(legal):
            if (
                action.get("branch") == "unit"
                and action.get("action", {}).get("command") == command
            ):
                return index
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
    LOG.info("loading model once from %s", checkpoint)
    from awbw_ml.encoding import EncodedObservation, packed_from_buffer
    from awbw_ml.inference import MaskedPolicy

    policy = MaskedPolicy.load(checkpoint, device)
    encoder = policy.encoder

    def dynamic_policy_selector(state: dict[str, Any], legal: list[dict[str, Any]], env: Any) -> int:
        del legal
        height = int(state["map"]["height"])
        width = int(state["map"]["width"])
        max_height = max(encoder.max_height, height)
        max_width = max(encoder.max_width, width)
        kept, packed, spatial, globals_, encoded_height, encoded_width = env.prepare_encoded_packed(
            max_height,
            max_width,
            True,
            encoder.cargo_v6,
            encoder.cargo_v7,
            encoder.cargo_v8,
        )
        observation = encoder.condition_encoded(
            EncodedObservation.from_compact_buffers(
                spatial,
                globals_,
                encoded_height,
                encoded_width,
                max_height,
                max_width,
            )
        )
        selected = policy.choose_encoded_packed_indices_batch(
            [observation],
            [packed_from_buffer(packed, len(kept))],
        )[0]
        return int(kept[selected])

    return NativeEnv, dynamic_policy_selector


def _model_state(state: dict[str, Any]) -> dict[str, Any]:
    """Remove bridge-only metadata before handing the state to NativeEnv."""
    result = copy.deepcopy(state)
    result.pop("_aw2", None)
    for unit in result["units"]:
        unit.pop("native_side", None)
    for building in result["buildings"]:
        building.pop("native_owner", None)
    return result


def _remove_untranslatable_actions(env: Any, state: dict[str, Any] | None = None) -> list[dict[str, Any]]:
    legal = json.loads(env.legal_actions_json())
    rejected = [index for index, action in enumerate(legal) if not is_translatable_action(action, state)]
    for index in reversed(rejected):
        env.exclude_legal_action(index)
    filtered = json.loads(env.legal_actions_json())
    if not filtered:
        raise RuntimeError("no AW2-translatable legal action remains")
    return filtered


def process_request(path: Path, runtime_dir: Path, native_env: type, selector: Callable) -> Path:
    envelope = decode_envelope(path.read_bytes(), expected_kind=FILE_KIND_REQUEST)
    state = state_from_snapshot(envelope.payload)
    env = native_env(json.dumps(_model_state(state), separators=(",", ":")))
    legal = _remove_untranslatable_actions(env, state)
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
        "session=%08x request=%d mode=%d map=%d side=%d day=%d size=%dx%d action=%s native=%s",
        envelope.session_id,
        envelope.request_id,
        state["_aw2"]["mode"],
        state["_aw2"]["map_id"],
        state["_aw2"]["active_side"],
        state["day"],
        state["map"]["width"],
        state["map"]["height"],
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
    native_env, selector = load_runtime(awbw_root, checkpoint, mode, str(config.get("device", "auto")))
    LOG.info("ready: mode=%s runtime=%s", mode, runtime_dir)
    while True:
        requests = sorted(runtime_dir.glob("request-*.bin"))
        if requests:
            for path in requests:
                try:
                    process_request(path, runtime_dir, native_env, selector)
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
