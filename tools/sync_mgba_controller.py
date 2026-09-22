#!/usr/bin/env python3
"""Copy the installed mGBA controller mapping into a portable mGBA config.

mGBA builds using different SDL revisions can assign the same controller to a
different SDL input slot and expose a slightly different device GUID.  Replacing
the whole config therefore loses the portable build's detected slot.  This tool
keeps the target device/slot and only transfers the user-facing button, hat and
axis mapping.
"""

from __future__ import annotations

import argparse
import configparser
import re
import shutil
from pathlib import Path


SDL_SECTION = re.compile(r"^gba\.input\.SDL[A-Z]$")
PROFILE_PREFIX = "gba.input-profile."
CONTROL_PREFIXES = ("key", "axis", "hat")


def read_ini(path: Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None, strict=False)
    parser.optionxform = str
    with path.open("r", encoding="utf-8-sig") as handle:
        parser.read_file(handle)
    return parser


def control_mapping(section: configparser.SectionProxy) -> dict[str, str]:
    return {
        key: value
        for key, value in section.items()
        if key.startswith(CONTROL_PREFIXES)
    }


def best_mapping(source: configparser.ConfigParser) -> tuple[str, dict[str, str]]:
    candidates: list[tuple[int, str, dict[str, str]]] = []
    for name in source.sections():
        if not (SDL_SECTION.match(name) or name.startswith(PROFILE_PREFIX)):
            continue
        mapping = control_mapping(source[name])
        if "keyA" in mapping and "keyB" in mapping:
            # The named profile is the durable user mapping. SDL[A-Z] is the
            # currently assigned player slot and can change between SDL versions.
            profile_bonus = 100 if name.startswith(PROFILE_PREFIX) else 0
            candidates.append((profile_bonus + len(mapping), name, mapping))
    if not candidates:
        raise ValueError("No configured SDL controller mapping was found in the installed mGBA config")
    _, name, mapping = max(candidates, key=lambda item: item[0])
    return name, mapping


def sync(source_path: Path, target_path: Path) -> list[str]:
    source = read_ini(source_path)
    target = read_ini(target_path)
    source_name, mapping = best_mapping(source)

    target_sections: list[str] = []
    target_devices: list[str] = []
    for name in target.sections():
        if SDL_SECTION.match(name) and target[name].get("device0"):
            target_sections.append(name)
            target_devices.append(target[name]["device0"])

    source_profile_name = source_name.removeprefix(PROFILE_PREFIX)
    matching_profile = PROFILE_PREFIX + source_profile_name
    if matching_profile in target and matching_profile not in target_sections:
        target_sections.append(matching_profile)

    for device in target_devices:
        device_profile = PROFILE_PREFIX + device
        if device_profile in target and device_profile not in target_sections:
            target_sections.append(device_profile)

    if not target_sections:
        raise ValueError("The portable mGBA config has no detected SDL controller slot")

    changed = False
    for name in target_sections:
        for key, value in mapping.items():
            if target[name].get(key) != value:
                target[name][key] = value
                changed = True

    if changed:
        backup = target_path.with_suffix(target_path.suffix + ".before-controller-sync")
        if not backup.exists():
            shutil.copy2(target_path, backup)
        with target_path.open("w", encoding="utf-8", newline="\n") as handle:
            target.write(handle, space_around_delimiters=False)

    return target_sections


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("target", type=Path)
    args = parser.parse_args()
    sections = sync(args.source, args.target)
    print("Controller mapping synchronized to: " + ", ".join(sections))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
