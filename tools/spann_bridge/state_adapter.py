"""Convert the bounded AW2 mailbox snapshot into an AWBW NativeEnv state."""

from __future__ import annotations

import copy
import math
from typing import Any

from mailbox_layout import REQUEST
from protocol import u16, u32

AW2_TO_AWBW = {
    1: "infantry", 2: "mech", 3: "medium_tank", 5: "tank", 6: "recon",
    7: "apc", 8: "neotank", 10: "artillery", 11: "rockets", 14: "anti_air",
    15: "missiles", 16: "fighter", 17: "bomber", 19: "battle_copter",
    20: "transport_copter", 21: "battleship", 22: "cruiser", 23: "lander",
    24: "submarine",
}
AWBW_TO_AW2 = {name: unit_id for unit_id, name in AW2_TO_AWBW.items()}
UNIT_COMMANDS = {"wait": 2, "capture": 3, "attack": 4}


def _owner_from_property(raw: int) -> int | None:
    kind = raw & 0x1F
    if kind not in (6, 8, 10, 11, 14):
        return None
    owner = raw >> 5
    return owner if owner in (1, 2) else None


def state_from_snapshot(template: dict[str, Any], payload: bytes) -> dict[str, Any]:
    if len(payload) < REQUEST["unit_records"] + REQUEST["unit_record_count"] * REQUEST["unit_record_stride"]:
        raise ValueError("truncated AW2 snapshot")
    if u16(payload, REQUEST["snapshot_version"]) != 1:
        raise ValueError("unsupported AW2 snapshot version")
    width = payload[REQUEST["width"]]
    height = payload[REQUEST["height"]]
    if (width, height) != (15, 10):
        raise ValueError(f"unexpected Spann dimensions {width}x{height}")

    state = copy.deepcopy(template)
    state["day"] = u16(payload, REQUEST["day"])
    state["active_player"] = 2
    funds = (u32(payload, REQUEST["funds_p1"]), u32(payload, REQUEST["funds_p2"]))
    property_plane = payload[
        REQUEST["property_plane"] : REQUEST["property_plane"] + width * height
    ]
    unit_plane = payload[REQUEST["unit_plane"] : REQUEST["unit_plane"] + width * height]

    buildings_by_position = {
        (int(item["position"]["x"]), int(item["position"]["y"])): item
        for item in state["buildings"]
    }
    for (x, y), building in buildings_by_position.items():
        building["owner"] = _owner_from_property(property_plane[y * width + x])
        building["capture_hp"] = 20
        state["map"]["property_owners"][str(building["id"])] = building["owner"]
        state["map"]["property_capture_hp"][str(building["id"])] = 20

    for player in state["players"]:
        player_id = int(player["id"])
        owned = [int(building["id"]) for building in state["buildings"] if building["owner"] == player_id]
        player["funds"] = funds[player_id - 1]
        player["properties"] = owned
        player["income"] = len(owned) * int(state["config"]["funds_per_property"])
        player["power_active"] = "none"

    occupied_ids = set(unit_plane)
    occupied_ids.discard(0)
    units = []
    records_base = REQUEST["unit_records"]
    stride = REQUEST["unit_record_stride"]
    for unit_id in range(REQUEST["unit_record_count"]):
        start = records_base + unit_id * stride
        record = payload[start : start + stride]
        if not record or record[0] == 0 or unit_id not in occupied_ids:
            continue
        unit_type = AW2_TO_AWBW.get(record[0])
        if unit_type is None:
            continue
        x, y = record[2], record[3]
        if x >= width or y >= height or unit_plane[y * width + x] != unit_id:
            continue
        hp = record[4] & 0x7F
        ammo = ((record[5] << 8 | record[4]) >> 7) & 0x0F
        capture_progress = (record[5] >> 3) & 0x1F
        units.append(
            {
                "id": unit_id,
                "owner": (unit_id >> 6) + 1,
                "unit_type": unit_type,
                "position": {"x": x, "y": y},
                "hp_internal": hp,
                "hp_displayed": 0 if hp == 0 else math.ceil(hp / 10),
                "fuel": record[6] & 0x7F,
                "ammo": ammo,
                "moved": bool(record[1] & 1),
                "fired": bool(record[1] & 1),
                "capture_progress": capture_progress or None,
                "hidden": "visible",
                "cargo": [],
            }
        )
    for unit in units:
        progress = unit["capture_progress"]
        if progress is None or unit["unit_type"] not in {"infantry", "mech"}:
            unit["capture_progress"] = None
            continue
        position = unit["position"]
        building = buildings_by_position.get((int(position["x"]), int(position["y"])))
        if building is None or not 0 < int(progress) < 20:
            unit["capture_progress"] = None
            continue
        building["capture_hp"] = int(progress)
        state["map"]["property_capture_hp"][str(building["id"])] = int(progress)
    state["units"] = units
    state["next_unit_id"] = max((int(unit["id"]) for unit in units), default=0) + 1
    state["status"] = {"state": "active"}
    return state


def translate_model_action(action: dict[str, Any], state: dict[str, Any]) -> dict[str, object]:
    branch = action.get("branch")
    if branch == "build":
        building_id = int(action["building"])
        building = next((item for item in state["buildings"] if int(item["id"]) == building_id), None)
        if building is None:
            raise ValueError(f"model selected unknown building {building_id}")
        unit_type = str(action["unit_type"])
        try:
            aw2_type = AWBW_TO_AW2[unit_type]
        except KeyError as exc:
            raise ValueError(f"AWBW unit type {unit_type!r} has no AW2 mapping") from exc
        return {
            "branch": "build",
            "x": int(building["position"]["x"]),
            "y": int(building["position"]["y"]),
            "aw2_unit_type": aw2_type,
        }
    if branch == "end_turn":
        return {"branch": "end_turn"}
    if branch == "unit":
        unit_id = int(action["unit"])
        unit = next((item for item in state["units"] if int(item["id"]) == unit_id), None)
        if unit is None or int(unit["owner"]) != 2:
            raise ValueError(f"model selected invalid enemy unit {unit_id}")
        positions = action.get("path", {}).get("positions", [])
        if not positions:
            raise ValueError("unit action has no path")
        source = (int(unit["position"]["x"]), int(unit["position"]["y"]))
        parsed = [(int(item["x"]), int(item["y"])) for item in positions]
        if parsed[0] != source:
            raise ValueError("unit path does not start at the native unit position")
        directions = []
        direction_by_delta = {(-1, 0): 0, (1, 0): 1, (0, 1): 2, (0, -1): 3}
        for first, second in zip(parsed, parsed[1:]):
            delta = (second[0] - first[0], second[1] - first[1])
            try:
                directions.append(direction_by_delta[delta])
            except KeyError as exc:
                raise ValueError(f"non-cardinal native path step {first}->{second}") from exc
        directions.append(4)
        command_name = str(action.get("action", {}).get("command"))
        try:
            command = UNIT_COMMANDS[command_name]
        except KeyError as exc:
            raise ValueError(f"unit command {command_name!r} is not runtime-confirmed yet") from exc
        destination = parsed[-1]
        translated = {
            "branch": "unit",
            "unit_id": unit_id,
            "dest_x": destination[0],
            "dest_y": destination[1],
            "command": command,
            "path": directions,
        }
        if command_name == "attack":
            target_id = int(action.get("action", {}).get("target"))
            target = next((item for item in state["units"] if int(item["id"]) == target_id), None)
            if target is None or int(target["owner"]) != 1:
                raise ValueError(f"model selected invalid attack target {target_id}")
            translated["param0"] = target_id
            translated["param1"] = 0
        return translated
    raise ValueError(f"model selected unsupported branch {branch!r}")
