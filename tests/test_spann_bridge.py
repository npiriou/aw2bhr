from __future__ import annotations

import json
import struct
import sys
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BRIDGE = ROOT / "tools" / "spann_bridge"
sys.path.insert(0, str(BRIDGE))
sys.path.insert(0, str(ROOT / "tools"))

from generate_layout import generated_outputs
from mailbox_layout import ACTIONS, BASE, FIELDS, REQUEST, RESPONSE, SIZE
from protocol import (
    FILE_KIND_REQUEST,
    Envelope,
    decode_envelope,
    encode_envelope,
    request_state_seed,
    response_payload,
)
from state_adapter import state_from_snapshot, translate_model_action
from sync_mgba_controller import sync as sync_mgba_controller


class ControllerConfigTests(unittest.TestCase):
    def test_mapping_moves_to_portable_slot_without_replacing_device(self) -> None:
        source = """\
[gba.input.SDLB]
device0=normal-guid
keyA=0
keyB=1
keyStart=7
axisLeftAxis=-0
hat0Up=6

[gba.input-profile.(Controller)]
keyA=0
keyB=1
keyStart=7
axisLeftAxis=-0
hat0Up=6
"""
        target = """\
[gba.input.SDLC]
device0=portable-guid
keyA=11
keyB=10

[gba.input-profile.(Controller)]
keyA=11
keyB=10

[gba.input-profile.portable-guid]
gyroAxisX=0
"""
        with tempfile.TemporaryDirectory() as directory:
            source_path = Path(directory) / "source.ini"
            target_path = Path(directory) / "target.ini"
            source_path.write_text(source, encoding="utf-8")
            target_path.write_text(target, encoding="utf-8")

            sections = sync_mgba_controller(source_path, target_path)
            result = target_path.read_text(encoding="utf-8")

        self.assertEqual(
            sections,
            [
                "gba.input.SDLC",
                "gba.input-profile.(Controller)",
                "gba.input-profile.portable-guid",
            ],
        )
        self.assertIn("device0=portable-guid", result)
        self.assertIn("keyA=0", result)
        self.assertIn("keyStart=7", result)
        self.assertNotIn("device0=normal-guid", result)


class LayoutTests(unittest.TestCase):
    def test_layout_stays_inside_reserved_4k(self) -> None:
        self.assertEqual(BASE, 0x02030000)
        self.assertEqual(SIZE, 0x1000)
        self.assertLessEqual(FIELDS["request"] + REQUEST["capacity"], FIELDS["response"])
        self.assertLessEqual(FIELDS["response"] + RESPONSE["capacity"], FIELDS["debug"])
        self.assertLessEqual(FIELDS["debug"], SIZE)

    def test_generated_layout_files_are_current(self) -> None:
        for path, expected in generated_outputs().items():
            self.assertEqual(path.read_text(encoding="utf-8"), expected, str(path))


class ProtocolTests(unittest.TestCase):
    def test_envelope_round_trip_keeps_session_and_request(self) -> None:
        source = Envelope(FILE_KIND_REQUEST, 0x12345678, 17, b"snapshot")
        self.assertEqual(decode_envelope(encode_envelope(source), expected_kind=FILE_KIND_REQUEST), source)

    def test_response_contains_matching_state_seed(self) -> None:
        seed = 0xAABBCCDD
        payload = response_payload(
            {"branch": "build", "x": 13, "y": 1, "aw2_unit_type": 1}, seed
        )
        self.assertEqual(payload[RESPONSE["action"]], ACTIONS["BUILD"])
        self.assertEqual(struct.unpack_from("<I", payload, RESPONSE["state_seed"])[0], seed)

    def test_bad_kind_is_rejected(self) -> None:
        data = encode_envelope(Envelope(FILE_KIND_REQUEST, 1, 2, b""))
        with self.assertRaises(ValueError):
            decode_envelope(data, expected_kind=2)

    def test_oversized_native_path_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "path exceeds"):
            response_payload(
                {
                    "branch": "unit", "unit_id": 65, "dest_x": 1, "dest_y": 1,
                    "command": 2, "path": [0] * (RESPONSE["path_capacity"] + 1),
                },
                1,
            )


class AdapterTests(unittest.TestCase):
    def snapshot(self) -> bytes:
        template = json.loads(
            (Path(r"X:\dev\awbw") / "maps" / "aw2-war-room" / "spann-island" / "initial-state.json").read_text(encoding="utf-8")
        )
        payload = bytearray(REQUEST["unit_records"] + REQUEST["unit_record_count"] * REQUEST["unit_record_stride"])
        struct.pack_into("<H", payload, REQUEST["snapshot_version"], 1)
        payload[REQUEST["width"]] = 15
        payload[REQUEST["height"]] = 10
        payload[REQUEST["mode"]] = 2
        payload[REQUEST["map_id"]] = 0x6C
        payload[REQUEST["active_side"]] = 2
        struct.pack_into("<H", payload, REQUEST["day"], 1)
        struct.pack_into("<I", payload, REQUEST["funds_p1"], 5000)
        struct.pack_into("<I", payload, REQUEST["funds_p2"], 5000)
        struct.pack_into("<I", payload, REQUEST["state_seed"], 0x10203040)
        kinds = {"city": 6, "hq": 8, "airport": 10, "port": 11, "base": 14}
        for building in template["buildings"]:
            position = building["position"]
            owner = int(building["owner"] or 0)
            offset = REQUEST["property_plane"] + int(position["y"]) * 15 + int(position["x"])
            payload[offset] = kinds[building["kind"]] | owner << 5
        return bytes(payload)

    def test_snapshot_builds_a_native_compatible_state(self) -> None:
        awbw = Path(r"X:\dev\awbw")
        template = json.loads((awbw / "maps" / "aw2-war-room" / "spann-island" / "initial-state.json").read_text(encoding="utf-8"))
        snapshot = self.snapshot()
        state = state_from_snapshot(template, snapshot)
        self.assertEqual(state["active_player"], 2)
        self.assertEqual(state["players"][1]["funds"], 5000)
        self.assertEqual(request_state_seed(snapshot), 0x10203040)
        sys.path.insert(0, str(awbw / "python"))
        from awbw_native import NativeEnv

        legal = json.loads(NativeEnv(json.dumps(state, separators=(",", ":"))).legal_actions_json())
        self.assertTrue(any(action["branch"] == "build" for action in legal))
        translated = translate_model_action(
            next(action for action in legal if action["branch"] == "build" and action["unit_type"] == "infantry"),
            state,
        )
        self.assertEqual(translated["aw2_unit_type"], 1)

    def test_zero_native_capture_bits_become_no_active_capture(self) -> None:
        awbw = Path(r"X:\dev\awbw")
        template = json.loads((awbw / "maps" / "aw2-war-room" / "spann-island" / "initial-state.json").read_text(encoding="utf-8"))
        payload = bytearray(self.snapshot())
        unit_id = 65
        x, y = 13, 3
        payload[REQUEST["unit_plane"] + y * 15 + x] = unit_id
        offset = REQUEST["unit_records"] + unit_id * REQUEST["unit_record_stride"]
        payload[offset : offset + 12] = bytes((1, 0, x, y, 100, 0, 99, 0, 0, 0, 0, 0))
        state = state_from_snapshot(template, bytes(payload))
        unit = next(item for item in state["units"] if item["id"] == unit_id)
        self.assertIsNone(unit["capture_progress"])

    def test_native_capture_bits_update_unit_and_property_mirror(self) -> None:
        awbw = Path(r"X:\dev\awbw")
        template = json.loads((awbw / "maps" / "aw2-war-room" / "spann-island" / "initial-state.json").read_text(encoding="utf-8"))
        payload = bytearray(self.snapshot())
        unit_id = 65
        x, y = 9, 2
        payload[REQUEST["unit_plane"] + y * 15 + x] = unit_id
        offset = REQUEST["unit_records"] + unit_id * REQUEST["unit_record_stride"]
        payload[offset : offset + 12] = bytes((1, 1, x, y, 100, 0x50, 96, 0, 0, 0, 0, 0))
        state = state_from_snapshot(template, bytes(payload))
        unit = next(item for item in state["units"] if item["id"] == unit_id)
        building = next(item for item in state["buildings"] if item["position"] == {"x": x, "y": y})
        self.assertEqual(unit["capture_progress"], 10)
        self.assertEqual(building["capture_hp"], 10)
        self.assertEqual(state["map"]["property_owners"][str(building["id"])], building["owner"])
        self.assertEqual(state["map"]["property_capture_hp"][str(building["id"])], 10)
        sys.path.insert(0, str(awbw / "python"))
        from awbw_native import NativeEnv

        NativeEnv(json.dumps(state, separators=(",", ":")))

    def test_capture_action_translates_to_native_destination_and_path(self) -> None:
        state = {
            "units": [{"id": 65, "owner": 2, "position": {"x": 11, "y": 1}}],
        }
        translated = translate_model_action(
            {
                "branch": "unit",
                "unit": 65,
                "path": {"positions": [
                    {"x": 11, "y": 1}, {"x": 11, "y": 2},
                    {"x": 10, "y": 2}, {"x": 9, "y": 2},
                ]},
                "action": {"command": "capture"},
            },
            state,
        )
        self.assertEqual(translated["command"], 3)
        self.assertEqual((translated["dest_x"], translated["dest_y"]), (9, 2))
        self.assertEqual(translated["path"], [2, 0, 0, 4])

    def test_non_cardinal_model_path_is_rejected(self) -> None:
        state = {"units": [{"id": 65, "owner": 2, "position": {"x": 11, "y": 1}}]}
        with self.assertRaisesRegex(ValueError, "non-cardinal"):
            translate_model_action(
                {
                    "branch": "unit", "unit": 65,
                    "path": {"positions": [{"x": 11, "y": 1}, {"x": 10, "y": 2}]},
                    "action": {"command": "wait"},
                },
                state,
            )


if __name__ == "__main__":
    unittest.main()
