---
organization: Ten Square Software
project: Matrix-Control
title: Passation — Code Review T.1 / Matrix-Simulator
author: BMad Agent
status: ready-for-code-review
sources:
  - implementation-artifacts/t-1-matrix-device-inquiry-simulator.md
  - implementation-artifacts/sprint-status.yaml
created: 2026-07-18
updated: 2026-07-18
---

# Passation — Code Review: Story T.1 + Matrix-Simulator extract

**Audience:** fresh `bmad-code-review` (or adversarial review) session — **different LLM than the implementer recommended.**  
**Story:** `t-1-matrix-device-inquiry-simulator` — status **review** in Matrix-Control sprint.  
**Owner ask (Guillaume, 2026-07-18):** Review the Device Inquiry work and the **detached** simulator app. Implementation and operator UAT are done; UI polish and project extract happened after the story was marked review.

---

## How to launch (fresh chat)

```text
/bmad-code-review T-1
```

Attach this passation. Open **both** workspaces if needed (multi-root) or review each repo separately as below.

---

## Two deliverables (do not confuse)

| Deliverable | Repo / path | Role |
|-------------|-------------|------|
| **A — Product Core** | `/Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Control` | Shared reply encoder + unit goldens (stays in plugin/tests) |
| **B — Standalone app** | `/Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Simulator` | macOS GUI IAC responder (extracted from former `Tools/`) |

**Critical:** The simulator is **no longer** under Matrix-Control. `Tools/` was deleted; `MATRIX_BUILD_DEVICE_SIMULATOR` CMake target was removed. Do **not** expect `Tools/MatrixDeviceInquirySimulator/` in Matrix-Control.

---

## A — Matrix-Control (Core slice)

### Paths to review

- `Source/Core/MIDI/SysEx/SysExEncoder.h` — `encodeDeviceInquiryReply(...)`
- `Source/Core/MIDI/SysEx/SysExEncoder.cpp` — implementation (constants-only literals)
- `Tests/Unit/SysExEncoderTests.cpp` — M-1000 / provisional M-6 goldens + round-trip decode/registry
- `CMakeLists.txt` — confirm **no** simulator target / no `Tools/` sources in `PLUGIN_SOURCES`
- Story: `_bmad-output/implementation-artifacts/t-1-matrix-device-inquiry-simulator.md`
- Sprint: `_bmad-output/implementation-artifacts/sprint-status.yaml` → `t-1-matrix-device-inquiry-simulator: review`

### Baseline

- Story `baseline_commit`: `62e67c61a5a6e41bceedd47743c5a0bdb38b99eb`
- Diff Core vs baseline for the files above (uncommitted work may still be present — check `git status`).

### AC focus (Core)

- Reply shape: `F0 7E <chan> 06 02 10 06 00 <memb-lo> <memb-hi> <rev0..3> F7`
- No duplicated manuf/family/member magic outside `SysExConstants::DeviceInquiry`
- Tests green without live IAC
- Plugin binaries must not link simulator UI

---

## B — Matrix-Simulator (detached app)

### Root path

```text
/Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Simulator
```

### Layout

```text
Matrix-Simulator/
├── CMakeLists.txt
├── CMakeUserPresets.json
├── README.md
├── .gitignore
└── Source/
    ├── Main.cpp              # GUI + MIDI listen/reply + persistence
    └── DeviceInquiry.h       # Local copy of inquiry request match + reply encode
```

### Build / binary

```bash
cd /Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Simulator
cmake --preset macos-debug-arm64
cmake --build --preset macos-debug-arm64
```

App:

```text
Builds/macOS/ARM/Debug/Matrix-Simulator_artefacts/Debug/Matrix-Simulator.app
```

### Behaviour to verify in review

- Profiles: Matrix-1000 (`0x02 0x00`) / Matrix-6 provisional (`0x01 0x00`)
- UI: Synth Profile, Firmware, **MIDI From** then **MIDI To**, Options → Active MIDI inputs/outputs checkboxes
- Window: fixed **500×500**, not resizable; JUCE title bar + Options menu
- Persistence: `~/Library/Application Support/Ten Square Software/Matrix-Simulator.settings`
- Threading: MIDI callback → `sendMessageNow`; UI log via `callAsync`; state under `CriticalSection`
- **Drift risk:** `Source/DeviceInquiry.h` must stay aligned with Matrix-Control `SysExConstants::DeviceInquiry` (intentional local SSOT for a separate repo — flag if sync story missing)

### GitHub

- Repo being created by Guillaume; description proposed:  
  *“macOS MIDI tool that answers Universal Device Inquiry as a Matrix-1000 or Matrix-6 for Matrix-Control development without hardware.”*
- Local git may have **no commits yet** at review time — review working tree / first commit as available.

---

## Operator UAT (already done — do not re-block)

Guillaume confirmed (2026-07-18):

- No MIDI → Matrix-Control UI locked/gray
- Matrix-6 profile → Bank Utility gray + Bank NumberBox hidden
- Matrix-1000 → Bank Utility active

Manual IAC smoke is **accepted**; review should focus on code quality, security of MIDI handling, persistence edge cases, and Core/app constant drift — not re-running full UAT unless something looks broken.

---

## Suggested review order

1. Matrix-Control `encodeDeviceInquiryReply` + `SysExEncoderTests` goldens  
2. Matrix-Control CMake isolation (no Tools / no simulator in plugin)  
3. Matrix-Simulator `DeviceInquiry.h` vs Core constants parity  
4. Matrix-Simulator `Main.cpp` — port open/close, inquiry match, reply send, persistence  
5. README accuracy vs current UI labels (Synth Profile, MIDI From/To order)

---

## Out of scope (still)

- Full patch/master dump emulator  
- Invented Matrix-6R inquiry member pattern  
- Shipping simulator inside AU/VST3 / product Standalone  
- Windows/Linux virtual MIDI  

---

## Chat tip for the review agent

- Speak **French** to Guillaume (project constitution).  
- Story / this passation stay **English**.  
- Present findings in plain language first (code-review presentation contract).
