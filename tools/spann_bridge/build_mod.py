#!/usr/bin/env python3
"""Build and patch the local Campaign/War Room agent ROM payload."""

from __future__ import annotations

import argparse
import hashlib
import os
import shutil
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CANONICAL_SHA1 = "14dd0b22c894865867aff89e8116b2dffae25605"
HOOK_OFFSET = 0x61750
HOOK_ORIGINAL = bytes.fromhex("6c 17 06 08")
PAYLOAD_ADDRESS = 0x08800000
BASE_ROM_SIZE = 0x800000


def tool(name: str) -> str:
    found = shutil.which(name)
    if found:
        return found
    root = os.environ.get("ARM_GNU_TOOLCHAIN")
    if root:
        candidate = Path(root) / "bin" / f"{name}.exe"
        if candidate.exists():
            return str(candidate)
    standard = Path(r"C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1\bin") / f"{name}.exe"
    if standard.exists():
        return str(standard)
    raise FileNotFoundError(f"{name} not found; put Arm GNU Toolchain on PATH or set ARM_GNU_TOOLCHAIN")


def run(command: list[str]) -> None:
    subprocess.run(command, cwd=ROOT, check=True)


def build(base_rom: Path, output: Path) -> None:
    source = base_rom.read_bytes()
    digest = hashlib.sha1(source).hexdigest()
    if digest != CANONICAL_SHA1:
        raise ValueError(f"base ROM SHA-1 {digest} does not match required {CANONICAL_SHA1}")
    if len(source) != BASE_ROM_SIZE:
        raise ValueError(f"base ROM size is 0x{len(source):x}; expected 0x{BASE_ROM_SIZE:x}")
    if source[HOOK_OFFSET : HOOK_OFFSET + 4] != HOOK_ORIGINAL:
        raise ValueError("phase-2 jump-table hook bytes do not match the verified upstream ROM")

    build_dir = ROOT / "build-mod"
    build_dir.mkdir(parents=True, exist_ok=True)
    obj = build_dir / "spann_hook.o"
    elf = build_dir / "spann_hook.elf"
    payload = build_dir / "spann_hook.bin"
    map_path = build_dir / "spann_hook.map"
    run([tool("arm-none-eabi-as"), "-mcpu=arm7tdmi", "-mthumb", "-I", str(ROOT), "mod/spann_hook.s", "-o", str(obj)])
    run([tool("arm-none-eabi-ld"), "-T", "mod/spann_mod.ld", "-Map", str(map_path), str(obj), "-o", str(elf)])
    run([tool("arm-none-eabi-objcopy"), "-O", "binary", str(elf), str(payload)])
    payload_bytes = payload.read_bytes()
    if not payload_bytes or len(payload_bytes) > 0x10000:
        raise ValueError(f"invalid payload size 0x{len(payload_bytes):x}")

    patched = bytearray(source)
    patched[HOOK_OFFSET : HOOK_OFFSET + 4] = PAYLOAD_ADDRESS.to_bytes(4, "little")
    patched.extend(payload_bytes)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(patched)
    (build_dir / "build-info.txt").write_text(
        "\n".join(
            [
                f"base_sha1={digest}",
                f"hook_offset=0x{HOOK_OFFSET:x}",
                f"hook_original={HOOK_ORIGINAL.hex()}",
                f"hook_target=0x{PAYLOAD_ADDRESS:08x}",
                f"payload_size=0x{len(payload_bytes):x}",
                f"output_sha1={hashlib.sha1(patched).hexdigest()}",
                "",
            ]
        ),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base-rom", type=Path, default=ROOT / "baserom.gba")
    parser.add_argument("--output", type=Path, default=ROOT / "build-mod" / "aw2bhr-spann-local.gba")
    args = parser.parse_args()
    build(args.base_rom.resolve(), args.output.resolve())


if __name__ == "__main__":
    main()
