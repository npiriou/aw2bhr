# Spann Island local ML agent

This branch replaces enemy decision generation on **War Room -> Spann Island** with a local file-IPC bridge to the existing AWBW model. AW2 still performs movement, capture, production, animations, and turn transitions through its original phase-3 executor. The hook falls through to the original AI everywhere outside the verified Spann enemy context.

## Revisions and local dependencies

- Upstream: `Mad-Man-Dan/aw2bhr` at `7ffc5165faf2e9a6f1e1a4f10eb5d6ba837fa35d` (`upstream/main` when this work started).
- Working branch: `feat/spann-local-agent` in `npiriou/aw2bhr`.
- Canonical US ROM SHA-1: `14dd0b22c894865867aff89e8116b2dffae25605`, size `0x800000`.
- AWBW root: `X:\dev\awbw`.
- Checkpoint: `data\models\benchmark-runE-U83-vs-U130-100games-20260826\runE-U83-inference.pt`.
- Checkpoint SHA-256: `69b4c12f58f99e4b8532e20052340652ead5ad450b066855161700f9c8791413`.
- The host imports `awbw_native.NativeEnv` and the existing `awbw_live.runner.policy_selector(checkpoint, device=...)`. The model is loaded once by the persistent bridge.
- Tested mGBA: development build `0.11-9139-3a5bc2462`, commit `3a5bc24629867576b0fb576a5d5a21d3b3d6b576`. Stable mGBA 0.10.5 does not expose the required `--script` option; its GUI can load `tools/mgba/bootstrap.lua` manually.
- Tested toolchain: Arm GNU Toolchain 14.2.Rel1 (`arm-none-eabi-gcc 14.2.1`), Python 3.12.14, and repository `tools/agbcc` at `da598c1`.

No ROM, model checkpoint, savestate, AWBW engine source, or other private asset is stored in this repository. Paths in the example configuration point to the existing local files.

## Build and run

The upstream build remains unchanged and produces `aw2bhr.gba`. On the tested MSYS2 setup:

```powershell
C:\msys64\usr\bin\make.exe -j8 aw2bhr.gba
```

The result was compared byte-for-byte with `baserom.gba`; both have SHA-1 `14dd0b22c894865867aff89e8116b2dffae25605`. The upstream `make compare` recipe currently reads the CRLF in `aw2bhr.sha1` as part of the filename under this MSYS setup. The reconstruction check was not changed or bypassed; SHA-1 and byte equality were checked independently.

Build the separate modified ROM with:

```powershell
X:\dev\awbw\.venv\Scripts\python.exe tools\spann_bridge\build_mod.py
```

The builder rejects a base ROM whose size, SHA-1, or original hook word differs. It writes `build-mod\aw2bhr-spann-local.gba` and `build-mod\build-info.txt`. The tested build has SHA-1 `bc2865a70f4f22dc86883f79ae6ae2f4e3a124d5`; generated ROMs remain ignored by Git.

Edit `spann-bridge.example.json` if the AWBW root, checkpoint, runtime directory, or device differs. Then launch both processes:

```powershell
tools\run_spann_local.ps1 -Mode model
```

Use `-Mode stub` for the deterministic bridge, `-Trace` for transition logging, and `-SaveState <path>` for local testing. The launcher shows mGBA, starts the host bridge in the background, and stops the bridge when mGBA exits.

## Verified activation and hook

Runtime instrumentation confirmed these identifiers on the US ROM:

| Field | Address | Required value |
|---|---:|---:|
| game mode | `0x03003FC1` | `0x02` (War Room) |
| map id | `0x03003FC2` | `0x6c` (Spann Island) |
| active side | `0x030033EC` | `0x02` (enemy) |
| width x height | `0x0201E450` | `15 x 10` |

The branch changes the phase-2 switch word at ROM offset `0x61750` from `0x0806176c` to the appended payload at `0x08800000`. The payload is entered as an internal switch arm and returns through the original dispatcher epilogue at `0x08061783`. If any activation predicate fails, it calls the original phase-2 arm at `0x0805d438` and returns normally.

The target flow is:

1. Phase 2 publishes an authoritative snapshot and returns to the game loop.
2. Lua atomically writes a session/request-scoped IPC file and changes `REQUEST_READY` to `WAITING`.
3. The persistent host converts the snapshot into the checked-in AWBW Spann fixture, enumerates legal actions with `NativeEnv`, and selects through runE-U83.
4. Lua accepts only the matching response filename and envelope, copies it into EWRAM, and publishes `RESPONSE_READY` last.
5. The hook rechecks the response session, request, state seed, bounds, unit, action, and route terminator.
6. The hook creates AW2's native command state and sets phase 3 / executor substate 0. Unit routes are packed with AW2's `sub_08034400`; movement context and fuel cost come from `sub_080202A4` and `sub_0802042C`.
7. The stock executor completes the action. Only after phase 3 returns to phase 2 does the hook clear `EXECUTING` and publish the next snapshot.
8. End-turn goes to the original phase-4 cleanup; the model path skips the built-in phase-5 production decision.

Confirmed native meanings in this flow are command `1` = build, `2` = unit wait, and `3` = unit capture. A model end-turn response enters phase 4. Unit movement directions in the uncompressed native route are `0` left, `1` right, `2` down, `3` up, with `4` as the terminator.

## Mailbox and IPC

The mailbox occupies `0x02030000..0x02030fff`. It was zero in all available Spann savestates, had no references in the current upstream disassembly, and was checked with an mGBA watchpoint during vanilla play. `mod/mailbox_layout.json` is the source of truth; `generate_layout.py` produces matching assembly, Python, and Lua constants.

| Offset | Size | Purpose |
|---:|---:|---|
| `0x000` | `0x30` | magic/version, session and request IDs, state, lengths, errors, counters |
| `0x080` | `0x800` | request snapshot |
| `0x880` | `0x180` | response action |
| `0xa00` | remainder | debug/reserved |

Mailbox states are `IDLE`, `REQUEST_READY`, `WAITING`, `RESPONSE_READY`, `EXECUTING`, and `ERROR`. The request contains mode/map/side/day/controller state, funds, the 15x10 unit and property planes, all 128 twelve-byte unit records, and a state seed. The response contains an action, unit/building fields, a native route of at most 12 bytes, and the echoed state seed.

IPC files add a 20-byte little-endian envelope: magic/version, kind, session ID, request ID, and payload length. Writers use a temporary file plus atomic rename. Lua deletes accepted response files. Reset/start invalidates mailbox magic, and Lua assigns a new host-random session before the first request of each emulator session. Every response is bound to that session, request, and state seed. A missing or invalid response leaves the game fail-closed in `WAITING` or `ERROR`; it never replays an earlier command.

## Tests performed

- Vanilla ROM reconstruction: byte-for-byte equal to the supplied US ROM.
- Modified ROM boot: mGBA loaded the appended payload and reported the patched hook word and expected ROM size.
- Deterministic stub: produced four infantry builds and end-turn through stock phase 3, reaching the next human day.
- Real runE-U83: from the day-1 Spann savestate, completed enemy days 1 through 5 and reached human day 6. The run processed 50 matched requests: builds, waits, captures (including partial and completed captures), and five end-turns. Every unit/build action passed through phase 3 substates 0 -> 1 -> 2 -> phase 2; each turn ended through phase 4. No duplicate or stale response was consumed.
- Desynchronization checks: capture progress and property-owner mirror changes were observed in the next real snapshot and accepted by `NativeEnv`. An intentionally retained failed request remained in `WAITING`; resuming that exact request after fixing the adapter did not consume a stale response.
- Non-target boot: mGBA ran the normal title/menu context (`mode 0x03`, map `0x01`) while mailbox magic remained unset. The assembly activation guard also has negative tests by inspection for mode, map, side, and dimensions; a full enemy turn on another map was not available in the supplied savestates.
- Automated tests: mailbox bounds/generated-file synchronization, envelope identity and kind validation, state-seed echo, invalid route rejection, AW2 snapshot conversion, partial-capture mirrors accepted by `NativeEnv`, build translation, and unit-route translation.

Run the automated checks with:

```powershell
X:\dev\awbw\.venv\Scripts\python.exe -m unittest discover -s tests -p 'test_*.py'
```

## Current boundary

The exercised POC translates build, wait, capture, and end-turn decisions. Combat, powers, transports, supply, join, and silo actions have not yet occurred in the tested five-turn run and are deliberately rejected rather than guessed. The bridge currently models visible Spann state; fog-hidden enemy knowledge and weather transitions have not been exercised. The implementation is intentionally fixed to the US ROM hash and the verified upstream revision.
