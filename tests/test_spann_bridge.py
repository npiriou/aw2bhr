from __future__ import annotations

import json
import struct
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BRIDGE = ROOT / "tools" / "spann_bridge"
sys.path.insert(0, str(BRIDGE))

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
from bridge import _model_state, _remove_untranslatable_actions
from state_adapter import is_translatable_action, state_from_snapshot, translate_model_action


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
        payload = bytearray(REQUEST["file_length"])
        struct.pack_into("<H", payload, REQUEST["snapshot_version"], 2)
        payload[REQUEST["width"]] = 15
        payload[REQUEST["height"]] = 10
        payload[REQUEST["mode"]] = 2
        payload[REQUEST["map_id"]] = 0x6C
        payload[REQUEST["active_side"]] = 2
        struct.pack_into("<H", payload, REQUEST["day"], 1)
        struct.pack_into("<H", payload, REQUEST["map_cells"], 150)
        struct.pack_into("<I", payload, REQUEST["state_seed"], 0x10203040)
        payload[REQUEST["property_plane"] : REQUEST["property_plane"] + 150] = bytes([1]) * 150
        properties = ((0, 0, 8, 1), (1, 0, 14, 1), (13, 3, 14, 2),
                      (14, 9, 8, 2), (9, 2, 6, 1))
        for x, y, kind, owner in properties:
            payload[REQUEST["property_plane"] + y * 15 + x] = kind | owner << 5
        for side, controller, team, co in ((1, 1, 1, 2), (2, 2, 2, 1)):
            offset = REQUEST["player_records"] + side * REQUEST["player_record_stride"]
            struct.pack_into("<I", payload, offset, 5000)
            payload[offset + 0x1B] = controller
            payload[offset + 0x1D] = co
            payload[offset + 0x2A] = team
        return bytes(payload)

    def test_snapshot_builds_a_native_compatible_state(self) -> None:
        awbw = Path(r"X:\dev\awbw")
        snapshot = self.snapshot()
        state = state_from_snapshot(snapshot)
        self.assertEqual(state["active_player"], 2)
        self.assertEqual(state["players"][1]["funds"], 5000)
        self.assertEqual((state["map"]["width"], state["map"]["height"]), (15, 10))
        self.assertEqual(request_state_seed(snapshot), 0x10203040)
        sys.path.insert(0, str(awbw / "python"))
        from awbw_native import NativeEnv

        env = NativeEnv(json.dumps(_model_state(state), separators=(",", ":")))
        legal = _remove_untranslatable_actions(env)
        self.assertTrue(all(is_translatable_action(action) for action in legal))
        self.assertTrue(any(action["branch"] == "build" for action in legal))
        translated = translate_model_action(
            next(action for action in legal if action["branch"] == "build" and action["unit_type"] == "infantry"),
            state,
        )
        self.assertEqual(translated["aw2_unit_type"], 1)

    def test_campaign_weather_and_special_object_are_projected(self) -> None:
        payload = bytearray(self.snapshot())
        payload[REQUEST["mode"]] = 1
        payload[REQUEST["weather"]] = 2
        x, y = 5, 4
        index = y * 15 + x
        payload[REQUEST["property_plane"] + index] = 30  # Deathray
        payload[REQUEST["special_hp_plane"] + index] = 73
        state = state_from_snapshot(bytes(payload))
        self.assertEqual(state["config"]["weather"], "rain")
        self.assertEqual(state["weather"]["current"], "rain")
        self.assertEqual(state["map"]["terrain"][index], "pipe_seam")
        self.assertIn({"x": x, "y": y}, state["map"]["pipe_seams"])
        self.assertIn(
            {"position": {"x": x, "y": y}, "hp": 73},
            state["map"]["pipe_seam_hp"],
        )

    def test_zero_native_capture_bits_become_no_active_capture(self) -> None:
        awbw = Path(r"X:\dev\awbw")
        payload = bytearray(self.snapshot())
        unit_id = 65
        x, y = 13, 3
        payload[REQUEST["unit_plane"] + y * 15 + x] = unit_id
        offset = REQUEST["unit_records"] + unit_id * REQUEST["unit_record_stride"]
        payload[offset : offset + 12] = bytes((1, 0, x, y, 100, 0, 99, 0, 0, 0, 0, 0))
        state = state_from_snapshot(bytes(payload))
        unit = next(item for item in state["units"] if item["id"] == unit_id)
        self.assertIsNone(unit["capture_progress"])

    def test_native_capture_bits_update_unit_and_property_mirror(self) -> None:
        awbw = Path(r"X:\dev\awbw")
        payload = bytearray(self.snapshot())
        unit_id = 65
        x, y = 9, 2
        payload[REQUEST["unit_plane"] + y * 15 + x] = unit_id
        offset = REQUEST["unit_records"] + unit_id * REQUEST["unit_record_stride"]
        payload[offset : offset + 12] = bytes((1, 1, x, y, 100, 0x50, 96, 0, 0, 0, 0, 0))
        state = state_from_snapshot(bytes(payload))
        unit = next(item for item in state["units"] if item["id"] == unit_id)
        building = next(item for item in state["buildings"] if item["position"] == {"x": x, "y": y})
        self.assertEqual(unit["capture_progress"], 10)
        self.assertEqual(building["capture_hp"], 10)
        self.assertEqual(state["map"]["property_owners"][str(building["id"])], building["owner"])
        self.assertEqual(state["map"]["property_capture_hp"][str(building["id"])], 10)
        sys.path.insert(0, str(awbw / "python"))
        from awbw_native import NativeEnv

        NativeEnv(json.dumps(_model_state(state), separators=(",", ":")))

    def test_capture_action_translates_to_native_destination_and_path(self) -> None:
        state = {
            "_aw2": {"active_side": 2},
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
        state = {"_aw2": {"active_side": 2}, "units": [{"id": 65, "owner": 2, "position": {"x": 11, "y": 1}}]}
        with self.assertRaisesRegex(ValueError, "non-cardinal"):
            translate_model_action(
                {
                    "branch": "unit", "unit": 65,
                    "path": {"positions": [{"x": 11, "y": 1}, {"x": 10, "y": 2}]},
                    "action": {"command": "wait"},
                },
                state,
            )

    def test_attack_translates_target_id_to_native_param0(self) -> None:
        state = {
            "_aw2": {"active_side": 2},
            "units": [
                {"id": 65, "owner": 2, "position": {"x": 8, "y": 4}},
                {"id": 3, "owner": 1, "position": {"x": 7, "y": 4}},
            ],
        }
        translated = translate_model_action(
            {
                "branch": "unit",
                "unit": 65,
                "path": {"positions": [{"x": 8, "y": 4}]},
                "action": {"command": "attack", "target": 3},
            },
            state,
        )
        self.assertEqual(translated["command"], 4)
        self.assertEqual(translated["param0"], 3)
        payload = response_payload(translated, 0x10203040)
        self.assertEqual(payload[RESPONSE["command"]], 4)
        self.assertEqual(payload[RESPONSE["param0"]], 3)

    def test_special_object_attack_translates_target_coordinates(self) -> None:
        state = {
            "_aw2": {
                "active_side": 2,
                "special_terrain": [{"x": 6, "y": 7, "kind": 30, "hp": 50}],
            },
            "units": [{"id": 65, "owner": 2, "position": {"x": 6, "y": 8}}],
        }
        translated = translate_model_action(
            {
                "branch": "unit", "unit": 65,
                "path": {"positions": [{"x": 6, "y": 8}]},
                "action": {"command": "attack_seam", "target": {"x": 6, "y": 7}},
            },
            state,
        )
        self.assertEqual(translated["command"], 5)
        self.assertEqual((translated["param0"], translated["param1"]), (6, 7))


if __name__ == "__main__":
    unittest.main()
