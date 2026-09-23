"""Convert an authoritative AW2 mailbox snapshot into an AWBW model state."""

from __future__ import annotations

import math
from typing import Any

from mailbox_layout import REQUEST
from protocol import u16

AW2_TO_AWBW = {
    1: "infantry", 2: "mech", 3: "medium_tank", 5: "tank", 6: "recon",
    7: "apc", 8: "neotank", 10: "artillery", 11: "rockets", 14: "anti_air",
    15: "missiles", 16: "fighter", 17: "bomber", 19: "battle_copter",
    20: "transport_copter", 21: "battleship", 22: "cruiser", 23: "lander",
    24: "sub",
}
AWBW_TO_AW2 = {name: unit_id for unit_id, name in AW2_TO_AWBW.items()}
UNIT_COMMANDS = {
    "wait": 2,
    "capture": 3,
    "attack": 4,
    "attack_seam": 5,
    "supply": 6,
    "load": 7,
    "unload": 8,
    "join": 0x0A,
    "hide": 0x0B,
    "fire_silo": 0x14,
}
SUPPORTED_UNIT_COMMANDS = frozenset(UNIT_COMMANDS)
AW2_CO_NAMES = (
    "Nell", "Andy", "Max", "Olaf", "Sami", "Grit", "Kanbei", "Sonja",
    "Eagle", "Drake", "Sturm", "Flak", "Lash", "Adder", "Hawke", "Hachi",
    "Colin", "Jess", "Sensei",
)
TERRAIN_NAMES = {
    1: "plain", 2: "river", 3: "mountain", 4: "wood", 5: "road",
    7: "sea", 12: "bridge", 13: "shoal", 15: "pipe", 16: "pipe_seam",
    17: "silo", 18: "silo_empty", 19: "reef",
}
PROPERTY_KINDS = {6: "city", 8: "hq", 10: "airport", 11: "port", 14: "base", 20: "lab"}
SPECIAL_TERRAIN_KINDS = frozenset(range(9, 10)) | frozenset(range(21, 32))
WEATHER_NAMES = {0: "clear", 1: "snow", 2: "rain"}


def _player_record(payload: bytes, side: int) -> bytes:
    stride = REQUEST["player_record_stride"]
    start = REQUEST["player_records"] + side * stride
    return payload[start : start + stride]


def _native_side(unit_id: int) -> int:
    return (unit_id >> 6) + 1


def state_from_snapshot(payload: bytes) -> dict[str, Any]:
    if len(payload) != REQUEST["file_length"]:
        raise ValueError(f"AW2 snapshot length {len(payload)} != {REQUEST['file_length']}")
    if u16(payload, REQUEST["snapshot_version"]) != 2:
        raise ValueError("unsupported AW2 snapshot version")
    width = payload[REQUEST["width"]]
    height = payload[REQUEST["height"]]
    cells = u16(payload, REQUEST["map_cells"])
    if not width or not height or width * height != cells or cells > 0x508:
        raise ValueError(f"invalid AW2 map dimensions {width}x{height} / {cells} cells")
    if payload[REQUEST["fog"]] != 0:
        raise ValueError("fog-of-war snapshot reached the non-fog bridge")
    try:
        weather = WEATHER_NAMES[payload[REQUEST["weather"]]]
    except KeyError as exc:
        raise ValueError(f"unsupported AW2 weather {payload[REQUEST['weather']]}") from exc
    active_side = payload[REQUEST["active_side"]]
    if active_side not in range(1, 5):
        raise ValueError(f"invalid active native side {active_side}")

    records = {side: _player_record(payload, side) for side in range(1, 5)}
    active_record = records[active_side]
    if len(active_record) != REQUEST["player_record_stride"] or active_record[0x1B] != 2:
        raise ValueError("active AW2 side is not a CPU controller")
    active_team = active_record[0x2A]

    def virtual_owner(native_owner: int) -> int | None:
        if native_owner not in records or records[native_owner][0x1B] == 0:
            return None
        return 2 if records[native_owner][0x2A] == active_team else 1

    unit_plane = payload[REQUEST["unit_plane"] : REQUEST["unit_plane"] + cells]
    property_plane = payload[REQUEST["property_plane"] : REQUEST["property_plane"] + cells]
    special_hp_plane = payload[REQUEST["special_hp_plane"] : REQUEST["special_hp_plane"] + cells]
    occupied_ids = set(unit_plane) - {0}
    native_records: dict[int, bytes] = {}
    stride = REQUEST["unit_record_stride"]
    for unit_id in range(REQUEST["unit_record_count"]):
        start = REQUEST["unit_records"] + unit_id * stride
        record = payload[start : start + stride]
        if len(record) == stride and record[0] != 0:
            native_records[unit_id] = record
    cargo_ids = {cargo for record in native_records.values() for cargo in record[7:9] if cargo}
    represented_ids = occupied_ids | cargo_ids
    cargo_parent = {
        cargo_id: transport_id
        for transport_id, record in native_records.items()
        for cargo_id in record[7:9]
        if cargo_id
    }

    units: list[dict[str, Any]] = []
    for unit_id in sorted(represented_ids):
        record = native_records.get(unit_id)
        if record is None:
            raise ValueError(f"map/cargo references missing AW2 unit record {unit_id}")
        if record[0] not in AW2_TO_AWBW:
            raise ValueError(f"unsupported AW2 unit type {record[0]} in record {unit_id}")
        unit_type = AW2_TO_AWBW[record[0]]
        native_side = _native_side(unit_id)
        owner = virtual_owner(native_side)
        if owner is None:
            continue
        transport = cargo_parent.get(unit_id)
        if transport is None:
            x, y = record[2], record[3]
            if x >= width or y >= height or unit_plane[y * width + x] != unit_id:
                continue
            position: dict[str, int] | None = {"x": x, "y": y}
        else:
            position = None
        hp = record[4] & 0x7F
        ammo = ((record[5] << 8 | record[4]) >> 7) & 0x0F
        capture_progress = (record[5] >> 3) & 0x1F
        moved = bool(record[1] & 1) or (owner == 2 and native_side != active_side)
        units.append({
            "id": unit_id, "owner": owner, "unit_type": unit_type, "position": position,
            "hp_internal": hp, "hp_displayed": 0 if hp == 0 else math.ceil(hp / 10),
            "fuel": record[6] & 0x7F, "ammo": ammo, "moved": moved, "fired": moved,
            "capture_progress": capture_progress or None,
            "hidden": "submerged" if unit_type == "sub" and record[1] & 0x20 else "visible",
            "cargo": [int(value) for value in record[7:9] if value in represented_ids],
            "transport": transport, "native_side": native_side,
        })

    terrain: list[str] = []
    buildings: list[dict[str, Any]] = []
    building_by_position: dict[tuple[int, int], dict[str, Any]] = {}
    pipes: list[dict[str, int]] = []
    pipe_seams: list[dict[str, int]] = []
    silos: list[dict[str, int]] = []
    hqs: list[dict[str, int]] = []
    labs: list[dict[str, int]] = []
    special_terrain: list[dict[str, int]] = []
    for index, raw in enumerate(property_plane):
        x, y = index % width, index // width
        kind = raw & 0x1F
        position = {"x": x, "y": y}
        if kind in PROPERTY_KINDS:
            terrain.append("property")
            native_owner = raw >> 5
            owner = virtual_owner(native_owner)
            building_kind = PROPERTY_KINDS[kind]
            if owner == 2 and native_owner != active_side and building_kind in {"base", "airport", "port"}:
                building_kind = "city"
            building = {
                "id": len(buildings) + 1, "kind": building_kind, "position": position,
                "owner": owner, "capture_hp": 20, "native_owner": native_owner,
            }
            buildings.append(building)
            building_by_position[(x, y)] = building
            if kind == 8:
                hqs.append(position)
            elif kind == 20:
                labs.append(position)
        else:
            special_hp = special_hp_plane[index]
            if kind in SPECIAL_TERRAIN_KINDS:
                # AWBW has no campaign-object terrain classes. Preserve static
                # objects as impassable pipe and expose only AW2 cells with a
                # live special-object HP entry as attackable pipe seams.
                terrain_name = "pipe_seam" if special_hp else "pipe"
                special_terrain.append({"x": x, "y": y, "kind": kind, "hp": special_hp})
            else:
                try:
                    terrain_name = TERRAIN_NAMES[kind]
                except KeyError as exc:
                    raise ValueError(f"unsupported AW2 terrain {kind} at ({x},{y})") from exc
                if kind == 16 and special_hp:
                    special_terrain.append({"x": x, "y": y, "kind": kind, "hp": special_hp})
            terrain.append(terrain_name)
            if kind == 15:
                pipes.append(position)
            elif kind == 16 or (kind in SPECIAL_TERRAIN_KINDS and special_hp):
                pipe_seams.append(position)
            elif kind in SPECIAL_TERRAIN_KINDS:
                pipes.append(position)
            elif kind == 17:
                silos.append(position)

    for unit in units:
        progress = unit["capture_progress"]
        position = unit["position"]
        if progress is None or position is None or unit["unit_type"] not in {"infantry", "mech"}:
            unit["capture_progress"] = None
            continue
        building = building_by_position.get((int(position["x"]), int(position["y"])))
        if building is None or not 0 < int(progress) < 20:
            unit["capture_progress"] = None
        else:
            building["capture_hp"] = int(progress)

    properties = {
        player: [int(item["id"]) for item in buildings if item["owner"] == player]
        for player in (1, 2)
    }
    enemy_sides = [side for side, record in records.items() if record[0x1B] and virtual_owner(side) == 1]
    enemy_side = enemy_sides[0] if enemy_sides else next(side for side in range(1, 5) if side != active_side)

    def co_name(side: int) -> str:
        co_id = records[side][0x1D]
        return AW2_CO_NAMES[co_id] if co_id < len(AW2_CO_NAMES) else "Andy"

    def funds(side: int) -> int:
        return int.from_bytes(records[side][0:4], "little")

    def power_meter(side: int) -> int:
        # AW2 stores charge in funds; AWBW persists tenths of funds.
        return int.from_bytes(records[side][0x20:0x24], "little") * 10

    def power_active(side: int) -> str:
        return {1: "cop", 2: "scop"}.get(records[side][0x1F], "none")

    state: dict[str, Any] = {
        "schema_version": 1,
        "config": {
            "map_id": (payload[REQUEST["mode"]] << 8) | payload[REQUEST["map_id"]],
            "funds_per_property": 1000, "starting_funds": 0, "powers_enabled": True,
            "weather": weather, "fog_enabled": False, "tags_enabled": False,
            "unit_limit": 64, "capture_limit": None, "day_limit": None,
            "banned_units": [], "banned_cos": [], "lab_units": [], "ghosted_properties": [],
        },
        "day": u16(payload, REQUEST["day"]), "active_player": 2,
        "players": [
            {"id": 1, "funds": funds(enemy_side), "income": len(properties[1]) * 1000,
             "co": co_name(enemy_side), "power_meter": power_meter(enemy_side),
             "power_active": power_active(enemy_side), "power_uses": records[enemy_side][0x25],
             "properties": properties[1], "team": 1,
             "eliminated": False, "turn_order": 1},
            {"id": 2, "funds": funds(active_side), "income": len(properties[2]) * 1000,
             "co": co_name(active_side), "power_meter": power_meter(active_side),
             "power_active": power_active(active_side), "power_uses": records[active_side][0x25],
             "properties": properties[2], "team": 2,
             "eliminated": False, "turn_order": 2},
        ],
        "units": units, "buildings": buildings,
        "map": {
            "width": width, "height": height, "terrain": terrain,
            "building_positions": [item["position"] for item in buildings],
            "property_owners": {str(item["id"]): item["owner"] for item in buildings},
            "property_capture_hp": {str(item["id"]): item["capture_hp"] for item in buildings},
            "pipes": pipes, "pipe_seams": pipe_seams,
            "pipe_seam_hp": [
                {"position": {"x": item["x"], "y": item["y"]}, "hp": min(item["hp"], 99)}
                for item in special_terrain if item["hp"]
            ],
            "silos": silos,
            "hqs": hqs, "labs": labs, "comtowers": [],
            "predeployed_units": [int(item["id"]) for item in units if item["position"] is not None],
        },
        "weather": {"current": weather, "remaining_turns": None, "source_co": None, "source_player": None},
        "next_unit_id": max((int(item["id"]) for item in units), default=0) + 1,
        "status": {"state": "active"},
        "_aw2": {
            "mode": payload[REQUEST["mode"]], "map_id": payload[REQUEST["map_id"]],
            "active_side": active_side, "active_team": active_team,
            "weather": weather, "special_terrain": special_terrain,
            "power_readiness": active_record[0x24],
        },
    }
    return state


def is_translatable_action(action: dict[str, Any], state: dict[str, Any] | None = None) -> bool:
    branch = action.get("branch")
    if branch in {"build", "end_turn"}:
        return branch != "build" or str(action.get("unit_type")) in AWBW_TO_AW2
    if branch == "power":
        power = str(action.get("power"))
        readiness = int((state or {}).get("_aw2", {}).get("power_readiness", 2))
        return (power == "cop" and readiness >= 1) or (power == "scop" and readiness >= 2)
    if branch != "unit":
        return False
    unit_action = action.get("action", {})
    command = str(unit_action.get("command"))
    if command not in SUPPORTED_UNIT_COMMANDS:
        return False
    if state is not None and command in {"load", "join"}:
        key = "transport" if command == "load" else "target"
        target_id = int(unit_action.get(key, 0))
        if _native_side(target_id) != int(state.get("_aw2", {}).get("active_side", 0)):
            return False
    return True


def translate_model_action(action: dict[str, Any], state: dict[str, Any]) -> dict[str, object]:
    branch = action.get("branch")
    active_side = int(state.get("_aw2", {}).get("active_side", 2))
    if branch == "build":
        building_id = int(action["building"])
        building = next((item for item in state["buildings"] if int(item["id"]) == building_id), None)
        if building is None or int(building.get("native_owner", active_side)) != active_side:
            raise ValueError(f"model selected unavailable native building {building_id}")
        unit_type = str(action["unit_type"])
        if unit_type not in AWBW_TO_AW2:
            raise ValueError(f"AWBW unit type {unit_type!r} has no AW2 mapping")
        return {"branch": "build", "x": int(building["position"]["x"]),
                "y": int(building["position"]["y"]), "aw2_unit_type": AWBW_TO_AW2[unit_type]}
    if branch == "end_turn":
        return {"branch": "end_turn"}
    if branch == "power":
        power = str(action.get("power"))
        readiness = int(state.get("_aw2", {}).get("power_readiness", 0))
        if power == "cop" and readiness >= 1:
            return {"branch": "power", "command": 0x0F, "native_side": active_side}
        if power == "scop" and readiness >= 2:
            return {"branch": "power", "command": 0x10, "native_side": active_side}
        raise ValueError(f"model selected unavailable native power {power!r}")
    if branch != "unit":
        raise ValueError(f"model selected unsupported branch {branch!r}")
    unit_id = int(action["unit"])
    unit = next((item for item in state["units"] if int(item["id"]) == unit_id), None)
    if unit is None or int(unit["owner"]) != 2 or _native_side(unit_id) != active_side:
        raise ValueError(f"model selected invalid active native unit {unit_id}")
    positions = action.get("path", {}).get("positions", [])
    if not positions or unit["position"] is None:
        raise ValueError("unit action has no native path")
    source = (int(unit["position"]["x"]), int(unit["position"]["y"]))
    parsed = [(int(item["x"]), int(item["y"])) for item in positions]
    if parsed[0] != source:
        raise ValueError("unit path does not start at the native unit position")
    directions = []
    direction_by_delta = {(-1, 0): 0, (1, 0): 1, (0, 1): 2, (0, -1): 3}
    for first, second in zip(parsed, parsed[1:]):
        delta = (second[0] - first[0], second[1] - first[1])
        if delta not in direction_by_delta:
            raise ValueError(f"non-cardinal native path step {first}->{second}")
        directions.append(direction_by_delta[delta])
    directions.append(4)
    command_name = str(action.get("action", {}).get("command"))
    if command_name not in UNIT_COMMANDS:
        raise ValueError(f"unit command {command_name!r} is not runtime-confirmed yet")
    destination = parsed[-1]
    translated: dict[str, object] = {
        "branch": "unit", "unit_id": unit_id, "dest_x": destination[0],
        "dest_y": destination[1], "command": UNIT_COMMANDS[command_name], "path": directions,
    }
    if command_name == "attack":
        target_id = int(action.get("action", {}).get("target"))
        target = next((item for item in state["units"] if int(item["id"]) == target_id), None)
        if target is None or int(target["owner"]) != 1:
            raise ValueError(f"model selected invalid attack target {target_id}")
        translated["param0"] = target_id
        translated["param1"] = 0
    elif command_name == "attack_seam":
        target = action.get("action", {}).get("target")
        if not isinstance(target, dict):
            raise ValueError("special-terrain attack has no target position")
        target_x, target_y = int(target["x"]), int(target["y"])
        special = state.get("_aw2", {}).get("special_terrain", [])
        if not any(
            int(item["x"]) == target_x and int(item["y"]) == target_y and int(item["hp"]) > 0
            for item in special
        ):
            raise ValueError(f"model selected invalid AW2 special target ({target_x},{target_y})")
        translated["param0"] = target_x
        translated["param1"] = target_y
    elif command_name in {"load", "join"}:
        key = "transport" if command_name == "load" else "target"
        target_id = int(action.get("action", {}).get(key))
        target = next((item for item in state["units"] if int(item["id"]) == target_id), None)
        if (
            target is None
            or int(target["owner"]) != 2
            or target.get("position") != {"x": destination[0], "y": destination[1]}
        ):
            raise ValueError(f"model selected invalid {command_name} target {target_id}")
        translated["param0"] = target_id
        translated["param1"] = 0
    elif command_name == "unload":
        unloads = action.get("action", {}).get("unloads")
        if not isinstance(unloads, list) or not 1 <= len(unloads) <= 2:
            raise ValueError("unload requires one or two cargo orders")
        cargo = [int(value) for value in unit.get("cargo", [])]
        params = [0, 0]
        direction_by_delta = {(0, -1): 1, (1, 0): 2, (0, 1): 3, (-1, 0): 4}
        seen: set[int] = set()
        for order in unloads:
            cargo_id = int(order["unit"])
            if cargo_id in seen or cargo_id not in cargo:
                raise ValueError(f"model selected invalid unload cargo {cargo_id}")
            seen.add(cargo_id)
            drop = order.get("destination")
            delta = (int(drop["x"]) - destination[0], int(drop["y"]) - destination[1])
            if delta not in direction_by_delta:
                raise ValueError(f"invalid unload destination for cargo {cargo_id}")
            slot = cargo.index(cargo_id)
            if slot > 1:
                raise ValueError("AW2 transport exposes only two cargo slots")
            params[slot] = direction_by_delta[delta]
        translated["param0"], translated["param1"] = params
    elif command_name == "hide":
        hidden = bool(action.get("action", {}).get("hidden"))
        translated["command"] = 0x0B if hidden else 0x0C
    elif command_name == "fire_silo":
        target = action.get("action", {}).get("target")
        if not isinstance(target, dict):
            raise ValueError("missile-silo action has no target position")
        target_x, target_y = int(target["x"]), int(target["y"])
        width, height = int(state["map"]["width"]), int(state["map"]["height"])
        if not (0 <= target_x < width and 0 <= target_y < height):
            raise ValueError(f"missile-silo target ({target_x},{target_y}) is outside the map")
        if {"x": destination[0], "y": destination[1]} not in state["map"].get("silos", []):
            raise ValueError("missile-silo action does not end on a live silo")
        translated["param0"], translated["param1"] = target_x, target_y
    return translated
