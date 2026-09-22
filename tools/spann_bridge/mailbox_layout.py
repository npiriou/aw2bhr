"""Load the shared local-AW2-agent mailbox definition."""

from __future__ import annotations

import json
from pathlib import Path

LAYOUT_PATH = Path(__file__).resolve().parents[2] / "mod" / "mailbox_layout.json"
LAYOUT = json.loads(LAYOUT_PATH.read_text(encoding="utf-8"))
BASE = int(LAYOUT["base"])
SIZE = int(LAYOUT["size"])
MAGIC = int(LAYOUT["magic"])
VERSION = int(LAYOUT["version"])
STATES = LAYOUT["states"]
ACTIONS = LAYOUT["actions"]
FIELDS = LAYOUT["fields"]
REQUEST = LAYOUT["request"]
RESPONSE = LAYOUT["response"]
