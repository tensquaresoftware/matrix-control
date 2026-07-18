---
organization: Ten Square Software
project: Matrix-Control
title: Story T.1 — Matrix Device Inquiry Simulator
author: BMad Agent
status: done
baseline_commit: 62e67c61a5a6e41bceedd47743c5a0bdb38b99eb
sources:
  - implementation-artifacts/passation-matrix-device-inquiry-simulator.md
  - implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md
  - implementation-artifacts/8-2-device-inquiry-and-footer-identity.md
  - implementation-artifacts/8-3-ui-lock-without-synth.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - Source/Core/MIDI/SysEx/SysExConstants.h
  - Source/Core/Services/DeviceTypeRegistry.h
  - project-context.md
created: 2026-07-18
updated: 2026-07-18
---

# Story T.1: Matrix Device Inquiry Simulator

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a developer travelling without Matrix hardware,
I want a small macOS app that answers Universal Device Inquiry as a Matrix-1000 or Matrix-6 on IAC ports,
so that Matrix-Control can detect a device, unlock the UI, and show the correct footer identity / device-type gating.

## Placement

| Field | Value |
|-------|--------|
| Track | **Epic T — Dev Tools** (tooling / travel; **not** product Epic 8 burn-down) |
| Story ID | `T.1` |
| Sprint key | `t-1-matrix-device-inquiry-simulator` |
| Story file | `implementation-artifacts/t-1-matrix-device-inquiry-simulator.md` |

**Do not** fold into 8.4 (virtual instrument bus) or invent Matrix-6R inquiry member bytes (blocked by 8.1 / PRD §9 #6).

This tool is **follow-on UAT tooling** for 8.2 / 8.3 / 8.5 — **not** a blocker for closing 8.3 review.

## Brownfield Reality (Read First)

| Deliverable | Status |
|-------------|--------|
| Inquiry **request** bytes + encoder | **Exists** — `SysExConstants::DeviceInquiry::kRequestMessage` + `SysExEncoder::encodeDeviceInquiry()` |
| Inquiry **reply** decoder + registry | **Exists** — `SysExDecoder::decodeDeviceId` + `DeviceTypeRegistry::fromDeviceInquiry` |
| Golden reply byte arrays (M-1000 / M-6) | **Exists** — `Tests/Unit/DeviceTypeRegistryTests.cpp` |
| Inquiry **reply encoder** | **Missing** — primary Core gap for this story |
| Product inquiry trigger / unlock / footer | **Exists** (8.2 / 8.3) — simulator only needs to reply correctly |
| `Tools/` or `Apps/` top-level tree | **Missing** — only precedent is `Tests/Tools/GenerateInitFixtures.cpp` |
| Full synth / patch dump emulator | **Out of scope** (tier 2+) |

**This story builds a separate tier-1 IAC responder + reply encoder. It does not change Matrix-Control unlock semantics.**

## Acceptance Criteria

1. **Given** macOS IAC Driver enabled and the simulator listening on the configured MIDI input  
   **When** Matrix-Control sends Universal Device Inquiry (`F0 7E 7F 06 01 F7`) to the simulator’s input  
   **Then** the simulator replies on its output with a Device ID response that Matrix-Control accepts (`deviceDetected=true`).

2. **And** the operator can select at least two profiles before/while running:
   - **Matrix-1000** — member `0x02 0x00` (`SysExConstants::DeviceInquiry::kExpectedMemberLow/High`)
   - **Matrix-6 (provisional)** — member `0x01 0x00` (`kMatrix6MemberLow/High`) → registry → `kMatrix6`  
   UI label must say provisional for M-6 until hardware capture.

3. **And** firmware / version bytes in the reply are configurable **or** fixed documented defaults (four ASCII chars, e.g. `1.11`) sufficient for footer `deviceVersion` display.

4. **And** a short English README (or in-app help) documents Audio MIDI Setup + Matrix-Control port mapping:
   - Matrix-Control **MIDI To** → simulator **In** (IAC)
   - Matrix-Control **MIDI From** ← simulator **Out** (IAC)  
   Warn against selecting the same endpoint for both From and To if the OS merges them oddly.

5. **And** Matrix-Control Standalone with From/To pointed at those ports: inquiry success → `deviceDetected=true`, footer identity shows type/version, UI unlocks (smoke against current 8.2/8.3 behaviour). Switching simulator profile to Matrix-6 and re-triggering inquiry yields `deviceType` Matrix-6 behaviour (e.g. Bank Utility gray per 8.5). Quitting the simulator / clearing ports returns Matrix-Control to locked / `No device`.

6. **And** out of scope (do **not** implement):
   - Full patch / master dump emulation
   - Remote Parameter Edit / Program Change behaviour beyond ignore-or-log
   - Distinct **Matrix-6R** inquiry member pattern (unknown on hardware — do **not** invent)
   - Windows / Linux virtual MIDI (macOS IAC first)
   - Shipping the simulator inside the VST3 / AU / product Standalone binary

7. **And** the tool builds as a **separate** CMake target (console app or minimal Standalone under `Tools/`). Core SysEx constants / reply encoder may be shared read-only from Core. Plugin binary must not gain simulator UI or IAC listener code.

8. **And** unit tests cover reply encoder bytes for both profiles (golden parity with `DeviceTypeRegistryTests`); no live IAC required in CI. English-only source / README / UI strings. `Core ↛ GUI` preserved (tool may link Core; Core must not depend on tool or GUI).

## Tasks / Subtasks

- [x] **T1 — Reply encoder (SSOT)** (AC: #1, #2, #3, #8)
  - [x] Add `SysExEncoder::encodeDeviceInquiryReply(...)` (or equivalent Core helper) that builds:
    `F0 7E <chan> 06 02 10 06 00 <memb-lo> <memb-hi> <rev0..3> F7`
  - [x] Use **only** `SysExConstants::DeviceInquiry` for manuf/family/member literals — no duplicated magic numbers
  - [x] Default channel `0x00` is fine (decoder accepts any channel byte); version default `"1.11"` (4 chars) unless operator overrides
  - [x] Unit-test encoder output against golden arrays in `DeviceTypeRegistryTests` (M-1000 + provisional M-6); optionally round-trip via `SysExDecoder::decodeDeviceId` + `DeviceTypeRegistry::fromDeviceInquiry`

- [x] **T2 — Tool skeleton + CMake** (AC: #7, #8)
  - [x] Create `Tools/MatrixDeviceInquirySimulator/` (new top-level `Tools/` is intentional for Epic T)
  - [x] Add CMake option e.g. `MATRIX_BUILD_DEVICE_SIMULATOR` (recommended default: `ON` on Apple, `OFF` elsewhere) — **do not** bury the target only under `MATRIX_BUILD_TESTS` (daily UAT without full test suite)
  - [x] `juce_add_console_app` (preferred) or tiny Standalone; compile defs mirror `GenerateInitFixtures` (`JUCE_WEB_BROWSER=0`, `JUCE_USE_CURL=0`, `JUCE_STANDALONE_APPLICATION=1`)
  - [x] Link `juce_audio_devices` (+ needed JUCE modules for MIDI In/Out + minimal UI if any); include `Source/` for SysEx headers/encoder
  - [x] Register new `.cpp` sources in CMake; keep target **out of** `PLUGIN_SOURCES`

- [x] **T3 — MIDI listen + reply** (AC: #1, #2, #5)
  - [x] Open operator-selected MIDI input; detect inquiry via exact request match (`kRequestMessage` / same check as `EditorOutboundGate::isDeviceInquirySysEx`)
  - [x] On match, send encoded reply on operator-selected MIDI output
  - [x] Profile combo: Matrix-1000 | Matrix-6 (provisional); changing profile affects subsequent replies
  - [x] Ignore / log non-inquiry MIDI; do not crash on floods
  - [x] KISS threading: message-thread or dedicated MIDI callback with thread-safe send — document choice in Dev Agent Record

- [x] **T4 — Minimal operator UI + docs** (AC: #2, #3, #4)
  - [x] Minimal UI: profile selector, MIDI In / Out selectors, status log (“Inquiry received → replied as Matrix-1000”)
  - [x] Version field optional if defaults documented; if present, clamp/pad to 4 ASCII chars
  - [x] `Tools/MatrixDeviceInquirySimulator/README.md` (English): IAC enable steps, From/To wiring diagram, profile notes, provisional M-6 warning, out-of-scope list

- [x] **T5 — Manual UAT with Matrix-Control** (AC: #5)
  - [x] Enable IAC; launch simulator (M-1000); wire Standalone From/To; confirm unlock + footer
  - [x] Switch to Matrix-6; re-trigger inquiry (reselect port or restart inquiry path); confirm Matrix-6 gating
  - [x] Quit simulator / clear ports; confirm lock + `No device`
  - [x] Document results in Dev Agent Record (no CI hardware dependency)

- [x] **T6 — Build / regression** (AC: #6, #7, #8)
  - [x] Simulator target builds on macOS Debug
  - [x] Existing `Matrix-Control_Tests` green (especially DeviceTypeRegistry / SysEx encoder tests)
  - [x] Confirm plugin AU/VST3 artefacts unchanged in composition (no simulator sources linked)

## Dev Notes

### Why this exists

Guillaume owns a Matrix-1000 only; Matrix-6/6R hardware is unavailable. Travel MacBook needs a portable stand-in for Device Inquiry so FR-2 unlock, footer identity (FR-53), and Matrix-6 memory-limit gating (8.5) can be exercised without hardware.

**Approved scope:** tier 1 only — Universal Device Inquiry **responder**. No full synth emulator.

### Protocol (authoritative)

| Item | Bytes / rule |
|------|----------------|
| Request (Matrix-Control → sim) | `F0 7E 7F 06 01 F7` |
| Reply shape | `F0 7E <chan> 06 02 10 06 00 <memb-lo> <memb-hi> <rev0> <rev1> <rev2> <rev3> F7` |
| Manufacturer | `0x10` (Oberheim) |
| Family | `0x06 0x00` |
| Member M-1000 | `0x02 0x00` |
| Member M-6 (provisional) | `0x01 0x00` → `kMatrix6` |
| Min length | decoder rejects `< 15` bytes |
| Matrix-6R | **Not distinguishable** from inquiry today — do **not** add a third invented member pattern |

[Source: `Source/Core/MIDI/SysEx/SysExConstants.h` L64–91; `SysExDecoder.cpp`; `DeviceTypeRegistry.cpp`]

### Golden replies (copy these for tests)

From `Tests/Unit/DeviceTypeRegistryTests.cpp`:

```cpp
// M-1000 + version "1.11"
{ 0xF0, 0x7E, 0x00, 0x06, 0x02, 0x10, 0x06, 0x00, 0x02, 0x00, '1', '.', '1', '1', 0xF7 }

// Provisional M-6 + version "1.11"
{ 0xF0, 0x7E, 0x00, 0x06, 0x02, 0x10, 0x06, 0x00, 0x01, 0x00, '1', '.', '1', '1', 0xF7 }
```

### Product integration (consume, do not rewrite)

| Concern | Owner | What simulator enables |
|---------|-------|------------------------|
| Inquiry send / decode / APVTS | `MidiManager::performDeviceInquiry` / `refreshDeviceInquiryAfterPortSync` (8.2) | Valid reply bytes |
| UI unlock (FR-2) | `deviceDetected` + `EditorOutboundGate` (8.3) | Successful detection |
| Footer identity | `FooterPanel` (7-8 / 8.2) | `deviceType` + `deviceVersion` |
| Bank Utility gray on M-6 | `DeviceMemoryLimits` (8.5) | Provisional M-6 member → `kMatrix6` |

**Critical product note:** inquiry **request** must remain allowed when UI is locked (`EditorOutboundGate::maySendEditorSysEx` bypasses for inquiry SysEx). Simulator does not change that gate — it only answers.

### CMake precedent

Follow `GenerateInitFixtures` under `MATRIX_BUILD_TESTS` for slim console-app shape, but gate the **simulator** with its **own** option so UAT does not require the full unit-test suite:

```cmake
option(MATRIX_BUILD_DEVICE_SIMULATOR "Build Matrix Device Inquiry Simulator (macOS IAC tool)" ${APPLE})
```

Pattern reference: `CMakeLists.txt` ~L496–533 (`juce_add_console_app`, `JUCE_STANDALONE_APPLICATION=1`, include `Source/`).

### Project structure notes

| Path | Role |
|------|------|
| `Tools/MatrixDeviceInquirySimulator/` | **NEW** — main + UI + README |
| `Source/Core/MIDI/SysEx/SysExEncoder.{h,cpp}` | **UPDATE** — add reply encoder |
| `Tests/Unit/` | **UPDATE** — encoder golden tests (extend existing SysEx/DeviceType tests or add focused file) |
| `CMakeLists.txt` | **UPDATE** — option + target + sources |
| Plugin `PLUGIN_SOURCES` | **Do not touch** for simulator UI |

Conflict: root had no `Tools/` folder — adding it is an explicit Epic T decision (Guillaume approved tooling track placement).

### Anti-patterns (do not)

- Invent Matrix-6R member bytes or a third “6R” profile that claims hardware truth
- Embed simulator into AU/VST3 / product Standalone
- Duplicate member literals outside `SysExConstants`
- Scope-creep to patch dump / RPE / note synth (tier 2)
- French strings in source, README, or UI
- Put GUI code into `Source/Core/`
- Require live IAC in CI

### Manual UAT checklist

1. Enable IAC Driver; create/use one bus (or two ports if split In/Out naming).
2. Launch simulator → profile Matrix-1000 → open ports.
3. Launch Matrix-Control Standalone → MIDI To = sim In, MIDI From = sim Out.
4. Expect: unlock + footer identity Matrix-1000 + version.
5. Switch simulator to Matrix-6 → re-trigger inquiry (reselect port or restart inquiry path).
6. Expect: `deviceType` Matrix-6 behaviour (e.g. Bank Utility gray per 8.5).
7. Quit simulator / clear ports → Matrix-Control returns to locked / `No device`.

### Risks

| Risk | Mitigation |
|------|------------|
| IAC loopback / same-port confusion | README: distinct From vs To; warn if same device selected |
| Provisional M-6 bytes | Label UI “Matrix-6 (provisional inquiry)” |
| 6R request creep | Push back unless hardware bytes known; APVTS override is product-side |
| Scope creep to tier 2 | Keep AC hard on inquiry-only |
| Reply encoder drift | Golden tests + SSOT constants |

### Previous story / git intelligence

Recent closes: `62e67c6` Close story 8.2; `8360bce` Close story 8.1. Product inquiry path and registry are stable — reuse, do not re-derive member bytes. Story 8.3 is in **review**; simulator is optional UAT aid, not a review gate.

### Project context reference

- Chat French / artifacts English / source English
- `Core ↛ GUI`; KISS / YAGNI; no French in public docs
- Builds under `Builds/`; commits English only on explicit request
- [Source: `_bmad-output/project-context.md`; `CONVENTIONS.md`]

### References

- [Source: `implementation-artifacts/passation-matrix-device-inquiry-simulator.md`]
- [Source: `Source/Core/MIDI/SysEx/SysExConstants.h` DeviceInquiry]
- [Source: `Source/Core/MIDI/SysEx/SysExEncoder.cpp` encodeDeviceInquiry]
- [Source: `Source/Core/Services/DeviceTypeRegistry.cpp` fromDeviceInquiry]
- [Source: `Tests/Unit/DeviceTypeRegistryTests.cpp` golden replies]
- [Source: `Source/Core/MIDI/EditorOutboundGate.h` inquiry bypass]
- [Source: stories 8-1, 8-2, 8-3; PRD §9 #6]

## Suggested Review Order

1. `SysExEncoder` reply API + unit golden tests  
2. CMake target isolation (not in plugin)  
3. Simulator MIDI listen/reply + profile switching  
4. README wiring / IAC warnings  
5. Manual UAT notes vs AC #5  

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

- JUCE Debug assertion on non-ASCII UI strings (`→` / `←` / em dash) in `juce::String` ctor — fixed by ASCII-only log/UI literals.
- Simulator Debug launch: window opens; no assertion after ASCII fix.
- Agent host initially had empty MIDI profile; operator UAT completed later on Guillaume's machine.

### Implementation Plan

1. Add `SysExEncoder::encodeDeviceInquiryReply` using only `SysExConstants::DeviceInquiry` literals; golden + round-trip tests in `SysExEncoderTests`.
2. New CMake option `MATRIX_BUILD_DEVICE_SIMULATOR` (default `${APPLE}`) outside `MATRIX_BUILD_TESTS`; `juce_add_console_app` target.
3. Minimal JUCE window: profile, firmware (4 chars), MIDI In/Out, status log; MIDI callback replies with encoded SysEx.
4. English README with IAC wiring and out-of-scope list.

### Completion Notes List

- **T1:** `encodeDeviceInquiryReply(memberLow, memberHigh, firmwareVersion="1.11", channel=0x00)` — version clamped/padded to 4 ASCII bytes. Goldens match `DeviceTypeRegistryTests`; round-trip decode+registry covered.
- **T2/T6:** Target `MatrixDeviceInquirySimulator` builds macOS Debug arm64. `PLUGIN_SOURCES` unchanged (no simulator sources). Full `Matrix-Control_Tests` exit 0 (including new SysExEncoder reply tests).
- **T3 threading:** Inquiry handled on the **MIDI input callback thread**; reply via `MidiOutput::sendMessageNow`. Profile / firmware / output pointer guarded by `CriticalSection`. UI log lines posted with `MessageManager::callAsync`. Non-inquiry SysEx logged and ignored.
- **T4:** UI + `Tools/MatrixDeviceInquirySimulator/README.md` (English).
- **T5 (operator UAT 2026-07-18):** Confirmed by Guillaume — no MIDI → UI locked/gray; Matrix-6 profile → Bank Utility gray + Bank NumberBox hidden; Matrix-1000 → Bank Utility active. Follow-up UX: combos MIDI To/From restored; Options > Audio/MIDI Settings uses input/output enable checkboxes (no duplicate settings button).

### File List

- `Source/Core/MIDI/SysEx/SysExEncoder.h`
- `Source/Core/MIDI/SysEx/SysExEncoder.cpp`
- `Tests/Unit/SysExEncoderTests.cpp`
- `CMakeLists.txt` (simulator target removed after extract)
- `_bmad-output/implementation-artifacts/t-1-matrix-device-inquiry-simulator.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`
- **Moved out of repo:** standalone app now at `/Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Simulator` (was `Tools/MatrixDeviceInquirySimulator/`)

### Change Log

- 2026-07-18: Implemented Device Inquiry reply encoder, simulator GUI app, CMake option, unit goldens.
- 2026-07-18: Operator UAT passed (M-1000 / M-6 / lock). UX: MIDI To/From combos + Options checkboxes for enabled ports; story → review.
- 2026-07-18: Simulator extracted to standalone project `/Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Simulator`; `Tools/` removed from Matrix-Control. Core `encodeDeviceInquiryReply` remains in Matrix-Control.
- 2026-07-18: Code review — accepted detached placement + independent DeviceInquiry SSOT; patched MIDI send lock, SafePointer logs, port desync, 7-bit/empty firmware, menu/README; story → done.

## Open Questions (non-blocking)

None blocking start. Optional later: Windows/Linux virtual MIDI; tier-2 editor SysEx stubs — separate stories only if Guillaume asks.

### Review Findings

- [x] [Review][Decision] Accept detached Matrix-Simulator as T-1 placement — **Resolved: option 1** — detached app is the accepted close shape for T-1 (Guillaume 2026-07-18).
- [x] [Review][Decision] Constant-drift sync process — **Resolved: option 3** — Matrix-Simulator stays fully independent of Matrix-Control; Guillaume will update Device Inquiry bytes manually in the simulator if they ever change (2026-07-18). No sync story / CI parity check.
- [x] [Review][Patch] Hold MidiOutput safely across MIDI-callback send [Matrix-Simulator/Source/Main.cpp] — fixed: encode + `sendMessageNow` under `stateLock_`.
- [x] [Review][Patch] Use SafePointer in callAsync log lambdas [Matrix-Simulator/Source/Main.cpp] — fixed: `SafePointer` on inquiry logs and port-filter dialog callback.
- [x] [Review][Patch] Close ports when timer refresh drops selection to (none) [Matrix-Simulator/Source/Main.cpp] — fixed: `refreshPortLists(false)` reopens/closes when UI selection diverges from open ports.
- [x] [Review][Patch] Clamp firmware version bytes to 7-bit MIDI data [SysExEncoder.cpp + DeviceInquiry.h] — fixed: `& 0x7f` on version bytes; unit coverage added.
- [x] [Review][Patch] Align empty firmware handling in Core with simulator [SysExEncoder.cpp] — fixed: empty → `"1.11"`; unit coverage added.
- [x] [Review][Patch] Rename Options menu item to match MIDI-only panel [Matrix-Simulator/Source/Main.cpp] — fixed: "Active MIDI Ports...".
- [x] [Review][Patch] Expand README out-of-scope bullets [Matrix-Simulator/README.md] — fixed: RPE/PC ignore + not shipped in plugin; independence note on protocol bytes.
- [x] [Review][Defer] No automated tests in Matrix-Simulator repo — deferred, pre-existing (post-extract; goldens only in Matrix-Control SysExEncoderTests)
- [x] [Review][Defer] Hard-coded machine-local JUCE paths in Matrix-Simulator CMakeLists — deferred, pre-existing (same pattern as Matrix-Control fallbacks)
- [x] [Review][Defer] Sticky port filter / stale enabled-ID accumulation — deferred, pre-existing UX polish (no reset-to-all once `hasPortFilter_` is true)
- [x] [Review][Defer] Unbounded callAsync log on SysEx flood — deferred, pre-existing (UAT tool; rate-limit not required for AC)
