# Local AW2 ML agent for Campaign and War Room

This branch replaces CPU decision generation in non-fog **Campaign** and **War Room** battles with the local AWBW runE-U83 model. AW2 still executes movement, combat, capture, production, animations, scripted map effects, and turn transitions with its stock command executor. In an active model context, a missing bridge, invalid snapshot, or invalid response stops the CPU turn visibly; it never invokes AW2's decision generator as a fallback.

Fog battles and modes outside Campaign/War Room keep the stock game behavior. Human-controlled armies are never intercepted. In multi-army battles, every native controller marked CPU is model-controlled when its turn is active; native team membership is projected so allied CPU armies are not treated as enemies.

## Revisions and local dependencies

- Upstream base: `Mad-Man-Dan/aw2bhr` commit `7ffc5165faf2e9a6f1e1a4f10eb5d6ba837fa35d`.
- Working branch: `feat/spann-local-agent` in `npiriou/aw2bhr`.
- Canonical US ROM: SHA-1 `14dd0b22c894865867aff89e8116b2dffae25605`, size `0x800000`.
- AWBW root: `X:\dev\awbw`.
- Checkpoint: `data\models\benchmark-runE-U83-vs-U130-100games-20260826\runE-U83-inference.pt`.
- Checkpoint SHA-256: `69b4c12f58f99e4b8532e20052340652ead5ad450b066855161700f9c8791413`.
- Tested mGBA: development build `0.11-9139-3a5bc2462`, commit `3a5bc24629867576b0fb576a5d5a21d3b3d6b576`.
- Tested toolchain: Arm GNU Toolchain 14.2.Rel1, Python 3.12.14, and repository `tools/agbcc` at `da598c1`.

The bridge imports `awbw_native.NativeEnv`, `awbw_ml.inference.MaskedPolicy`, and the checkpoint's existing encoder directly from `X:\dev\awbw`. The convolutional policy accepts the runtime map dimensions; a synthetic 40x28 state was loaded and evaluated with the exact checkpoint. No ROM, checkpoint, savestate, AWBW source, or private asset is copied into this repository.

## Build and run

The unchanged upstream build produces `aw2bhr.gba`:

```powershell
C:\msys64\usr\bin\make.exe -j8 aw2bhr.gba
```

The vanilla output was checked byte-for-byte against `baserom.gba`; both have the canonical SHA-1 above. The reconstruction checks remain unchanged.

Build the separate modified ROM with:

```powershell
X:\dev\awbw\.venv\Scripts\python.exe tools\spann_bridge\build_mod.py
```

The builder rejects a base ROM whose size, SHA-1, or original hook word differs. It writes `build-mod\aw2bhr-spann-local.gba` and `build-mod\build-info.txt`. The current modified ROM SHA-1 is `d9cb06b2cb2f1d164d00b1b0bf7ac971966cbae0`.

Double-click **`START-AW2-ML.cmd`**. It launches the same portable mGBA installation and settings directory used by the previous working Spann launcher, starts the persistent model process, and loads the Lua bridge automatically. `START-SPANN-ML.cmd` remains as a compatibility alias.

```powershell
tools\run_spann_local.ps1 -Mode model
```

`-Mode stub` selects the deterministic bridge, `-Trace` records state transitions, and `-SaveState <path>` loads a local savestate. The internal script name is retained for compatibility; its activation is no longer limited to Spann.

## Activation and native execution

The hook is active when all of these runtime predicates hold:

| Field | Address | Accepted value |
|---|---:|---:|
| mode | `0x03003FC1` | `1` Campaign or `2` War Room |
| fog | `0x03003FCD` | `0` |
| active side | `0x030033EC` | native side `1..4` |
| controller | `0x02023284 + side*0x3c + 0x1b` | `2` CPU |
| dimensions | `0x0201E450` | nonzero and at most `0x508` cells |

There is no map-ID, side-number, or Spann-dimension allowlist. Runtime probes confirmed War Room Spann as mode `2`, map `0x6c`, 15x10; War Room Moji as mode `2`, map `0x6d`, 26x22; and the first Campaign battle as mode `1`, map `0x8a`, 15x10.

The branch changes the phase-2 switch word at ROM offset `0x61750` from `0x0806176c` to the appended payload at `0x08800000`. The payload returns through the original dispatcher epilogue at `0x08061783`.

The action flow is:

1. Phase 2 snapshots the current native state and waits.
2. Lua appends the authoritative unit, player, and special-object tables and writes a session/request-scoped IPC file atomically.
3. The host projects native teams into the model's two-player view, asks `NativeEnv` for legal actions, removes commands the native translator cannot execute, and runs runE-U83 on the remaining mask.
4. The hook verifies the session, request, state seed, actor ownership, coordinates, native movement overlay, path terminator, and target semantics.
5. The hook fills AW2's native command record and hands control to the stock phase-3 executor. End turn enters the stock phase-4 cleanup.
6. A new request is published only after the executor returns to phase 2.

Confirmed native commands are `1` build, `2` wait, `3` capture, `4` attack unit, `5` attack special terrain, `6` APC supply, `7` load, `8` unload, `0x0a` join, `0x0b` dive, `0x0c` surface, `0x0f` COP, `0x10` SCOP, and `0x14` fire missile silo. Command 4 carries the native target unit ID. Commands 5 and `0x14` carry target `(x,y)` in bytes `+6/+7`. Unload bytes `+6/+7` correspond to cargo slots one/two and use `0` no unload, `1` up, `2` right, `3` down, `4` left. Power byte `+6` is the native army selector. Movement directions are `0` left, `1` right, `2` down, `3` up, and `4` terminator.

Load and join may legally end on an occupied friendly cell, so the route guard allows an occupied final overlay cell only for commands `7` and `0x0a`; every preceding cell remains subject to AW2's movement overlay. Unload is not considered complete until the native asynchronous executor returns from substate `0x0a` to phase 2/substate 0.

## State projection

The request contains dynamic width/height, mode, map ID, active side, day, fog, current weather, unit/property planes, all 256 twelve-byte unit records, five player records, and the `0x020288B4` special-object HP plane.

Current weather is read from `0x03003FEC`: `0` clear, `1` snow, `2` rain. The resulting AWBW state and movement rules use the matching string rather than assuming clear weather.

AW2 terrain IDs 21..31 are Campaign-specific Black Hole structures. AWBW has no matching terrain classes. A special cell with live HP is projected as an attackable pipe seam; the model's `attack_seam` target is translated to native command 5. A special cell without live HP, including underlay/custom structure cells, is projected as an impassable pipe. Raw AW2 kind and HP remain in bridge-only metadata and are checked again during translation. This preserves obstruction and lets the model target only structures AW2 itself marks destructible.

The model sees the active native team as player 2 and every opposing native team as player 1. Other active-team armies are present but marked moved, and their production sites are not offered to the current side. This prevents one CPU army from moving or building for an ally.

Power state comes from the native player record: `+0x20` is the AW2 meter, converted to AWBW's tenths-of-funds scale; `+0x24` is native readiness (`0`, COP, SCOP); `+0x25` is the prior-use count; and `+0x1f` is the active COP/SCOP state. The readiness byte is checked again in the ROM hook. Cargo comes from unit bytes `+7/+8`, and submerged state is unit flag byte `+1 & 0x20`.

## Mailbox and IPC

The verified mailbox remains `0x02030000..0x02030fff`:

| Offset | Size | Purpose |
|---:|---:|---|
| `0x000` | `0x80` | identity, state, lengths, errors, counters |
| `0x080` | `0xa80` | in-emulator snapshot planes |
| `0xb00` | `0x100` | response action |
| `0xc00` | remainder | debug/reserved |

Lua appends tables that do not fit in the verified 4 KiB region. Protocol-v2 request files are 7,268 bytes: 2,608 mailbox bytes, 3,072 unit-record bytes, 300 player-record bytes, and 1,288 special-HP bytes. The response contains a route of at most 12 direction bytes and the echoed state seed.

Mailbox states are `IDLE`, `REQUEST_READY`, `WAITING`, `RESPONSE_READY`, `EXECUTING`, and `ERROR`. IPC envelopes bind every response to a random emulator session, request ID, and state seed. Invalid input leaves the targeted CPU in `WAITING` or `ERROR`; stale commands cannot be replayed.

## Verification

- Vanilla reconstruction remained byte-identical to the supplied US ROM.
- The deterministic stub completed production and end turn through the stock executor.
- On Spann, runE-U83 completed 12 decisions on day 6, including capture, unit combat, movement, four builds, and end turn; the game reached day 7 without an error or stale response. This regression was repeated after the dynamic-map/weather/special-object changes.
- On Moji, runE-U83 completed two enemy turns on the 26x22 map, including production and movement, and advanced to day 3 without an error.
- Campaign mode/map/dimensions/fog were confirmed at runtime on the first battle. The automated menu driver reached the human turn, but did not complete its menu end-turn sequence, so a full Campaign enemy turn is not claimed as runtime-tested.
- A synthetic Campaign snapshot with rain and a live Deathray is accepted by `NativeEnv`; legal `attack_seam` actions are produced and translated to native command 5 with the target coordinates. The command-5 field layout and executor path are confirmed from upstream source; execution against a real special objective has not yet been observed in mGBA.
- A forced full-meter mGBA run executed SCOP as native command `0x10`, returned to phase 2, issued the next request, completed production, and ended the turn without a mailbox error.
- A deterministic mGBA scenario executed load, unload, and join as commands `7`, `8`, and `0x0a`, ended the turn, and successfully issued another load on day 7. Separate scenarios executed APC supply (`6`), missile-silo firing (`0x14`), and Sub dive (`0x0b`) and continued to later requests without mailbox errors. These scenarios altered RAM only to make the action immediately legal; the bridge and native executor were the production paths.
- Automated coverage includes layout generation, IPC envelopes, stale-state seed binding, dynamic state conversion, weather and native power projection, coalition ownership, capture mirroring, route validation, unit attacks, special-object attacks, production, power, transport, join, supply, hide/surface, and silo translation. Twenty tests pass with the AWBW virtual environment.

Run the automated checks with:

```powershell
X:\dev\awbw\.venv\Scripts\python.exe -m unittest discover -s tests -p 'test_*.py'
```

## Current command boundary

Every runE-U83 action that has an AW2 equivalent is now exposed: build, wait, capture, unit/special-object attack, end turn, COP/SCOP, transport load/unload, join, APC supply, Sub dive/surface, and missile-silo firing. The only filtered unit command is Black Boat repair because AW2 has no Black Boat or selectable repair command; property repair remains AW2's automatic turn-start behavior. AWBW-only units and actions for Black Boat, Carrier, Stealth, and Black Bomb remain unavailable because those units do not exist in Advance Wars 2. No filtered action is delegated to AW2 AI. A targeted non-fog CPU turn fails closed if no translatable action remains.
