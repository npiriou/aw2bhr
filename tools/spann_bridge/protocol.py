"""Binary file IPC and mailbox payload helpers for the Spann bridge."""

from __future__ import annotations

import os
import struct
from dataclasses import dataclass
from pathlib import Path

from mailbox_layout import ACTIONS, MAGIC, REQUEST, RESPONSE, VERSION

FILE_HEADER = struct.Struct("<IHHIII")
FILE_KIND_REQUEST = 1
FILE_KIND_RESPONSE = 2


@dataclass(frozen=True)
class Envelope:
    kind: int
    session_id: int
    request_id: int
    payload: bytes


def encode_envelope(envelope: Envelope) -> bytes:
    return FILE_HEADER.pack(
        MAGIC,
        VERSION,
        envelope.kind,
        envelope.session_id,
        envelope.request_id,
        len(envelope.payload),
    ) + envelope.payload


def decode_envelope(data: bytes, *, expected_kind: int | None = None) -> Envelope:
    if len(data) < FILE_HEADER.size:
        raise ValueError("truncated IPC envelope")
    magic, version, kind, session_id, request_id, length = FILE_HEADER.unpack_from(data)
    if magic != MAGIC or version != VERSION:
        raise ValueError("IPC magic/version mismatch")
    if expected_kind is not None and kind != expected_kind:
        raise ValueError(f"unexpected IPC kind {kind}")
    payload = data[FILE_HEADER.size :]
    if len(payload) != length:
        raise ValueError("IPC payload length mismatch")
    return Envelope(kind, session_id, request_id, payload)


def atomic_write(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_bytes(data)
    os.replace(temporary, path)


def u16(payload: bytes, offset: int) -> int:
    return struct.unpack_from("<H", payload, offset)[0]


def u32(payload: bytes, offset: int) -> int:
    return struct.unpack_from("<I", payload, offset)[0]


def response_payload(action: dict[str, object], state_seed: int) -> bytes:
    payload = bytearray(RESPONSE["state_seed"] + 4)
    branch = action.get("branch")
    if branch == "build":
        payload[RESPONSE["action"]] = ACTIONS["BUILD"]
        payload[RESPONSE["building_x"]] = int(action["x"])
        payload[RESPONSE["building_y"]] = int(action["y"])
        payload[RESPONSE["unit_type"]] = int(action["aw2_unit_type"])
    elif branch == "end_turn":
        payload[RESPONSE["action"]] = ACTIONS["END_TURN"]
    elif branch == "unit":
        payload[RESPONSE["action"]] = ACTIONS["UNIT"]
        payload[RESPONSE["unit_id"]] = int(action["unit_id"])
        payload[RESPONSE["dest_x"]] = int(action["dest_x"])
        payload[RESPONSE["dest_y"]] = int(action["dest_y"])
        payload[RESPONSE["command"]] = int(action["command"])
        path = bytes(int(step) for step in action.get("path", []))
        if len(path) > RESPONSE["path_capacity"]:
            raise ValueError("native path exceeds mailbox capacity")
        payload[RESPONSE["path_length"]] = len(path)
        payload[RESPONSE["path"] : RESPONSE["path"] + len(path)] = path
    else:
        raise ValueError(f"unsupported action branch {branch!r}")
    struct.pack_into("<I", payload, RESPONSE["state_seed"], state_seed)
    return bytes(payload)


def request_state_seed(payload: bytes) -> int:
    if len(payload) < REQUEST["state_seed"] + 4:
        raise ValueError("truncated request payload")
    return u32(payload, REQUEST["state_seed"])
