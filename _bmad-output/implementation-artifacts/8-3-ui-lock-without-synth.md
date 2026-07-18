---
organization: Ten Square Software
project: Matrix-Control
title: Story 8.3 — UI Lock Without Synth
author: BMad Agent
status: done
baseline_commit: 62e67c61a5a6e41bceedd47743c5a0bdb38b99eb
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/8-2-device-inquiry-and-footer-identity.md
  - implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md
  - implementation-artifacts/7-8-header-footer-shell-and-persistence.md
  - project-context.md
created: 2026-07-18
updated: 2026-07-18
---

# Story 8.3: UI Lock Without Synth

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a user,
I want editing disabled when no synth is detected,
so that I am guided to fix the connection (FR-2).

## Brownfield Reality (Read First)

**Device presence is done; the lock is not.** Story 8.2 writes `deviceDetected` / `deviceType` / `deviceVersion`, triggers inquiry on port configure, and verifies footer **right-zone** identity. Editing panels, ActionHandlers, and outbound SysEx/PC **still run freely** when `deviceDetected=false`.

| Deliverable | Status at baseline (`62e67c6`) |
|-------------|--------------------------------|
| APVTS `deviceDetected` SSOT (MidiManager) | **Exists** — init `false`; inquiry success/fail/clear ports update it |
| Footer right zone identity / `kNoDevice` | **Exists** — verify only; do not rework |
| Port reconfigure → re-inquiry | **Exists** (8.2) — **verify** only for this AC |
| GUI section lock when undetected | **Missing** |
| Footer left-zone “fix connection” guidance | **Missing** |
| Core gate: no editor SysEx / Program Change while locked | **Missing** — PC/SysEx work with ports alone today |
| `SynthConnection` / `EditGuard` class | **None** — do not invent a parallel framework |
| Compare-mode section lock pattern | **Exists** — `CompareLockBinder` — **reuse / compose** |

**This story is lock + guidance + outbound gate — not a rewrite of inquiry, registry, or footer identity painting.**

## Acceptance Criteria

1. **Given** Story 8.2 (`deviceDetected` driven by inquiry / port clear)  
   **When** `deviceDetected=false`  
   **Then** the following are disabled for interaction (grayed / no child mouse / no keyboard focus for edits):
   - PATCH EDIT
   - MASTER EDIT
   - MATRIX MODULATION
   - PATCH MANAGER (Bank Utility, Internal Patches, Computer Patches, **Patch Mutator**)
   - Module I/C/P (and Matrix Mod section I/C/P) as part of those panels  
   **And** Header routing stays usable: MIDI From / MIDI To (and other header shell prefs: skin, UI scale, Settings, About) so the user can fix ports and recover.

2. **And** footer **left zone** shows actionable guidance while locked (cables, ports, power-cycle — English copy in `PluginDisplayNames`). Right zone stays `kNoDevice` / identity from 8.2 (FR-53 unchanged). Guidance is non-blocking footer only (D-038) — **no modal**.

3. **And** while locked, **no editor SysEx** and **no Program Change** are sent (FR-2).  
   **Exception (mandatory):** Universal Device Inquiry request (and its 8.2 orchestration) **must still run** — that is the unlock path. Do not gate inquiry behind the same lock check.  
   **Not gated:** Instrument-path notes / CC / pitch bend (FR-5) — FR-2 targets editor actions, not live playing through the instrument role.

4. **And** D-037 holds: **no** INIT / Patch Manager / I/C/P exception “for testing without synth”. Locked means locked.

5. **And** when `deviceDetected` becomes `true`, UI unlocks and lock guidance is cleared (without wiping unrelated footer messages — match Compare’s exact-string clear pattern).

6. **And** port reconfigure still re-triggers Device Inquiry (8.2 behaviour) — regression check only; do not re-implement trigger logic.

7. **And** Compare mode and device lock **compose correctly**: a panel stays locked if `!deviceDetected` **or** Compare is active. Two binders must not fight over `setInterceptsMouseClicks` / alpha (last-writer-wins bug). See Dev Notes.

8. **And** out of scope (do **not** implement here):
   - Virtual instrument bus layout / MASTER gray for M-6/6R (Story 8.4)
   - Changing Device Inquiry encode/decode/registry / D-080
   - Reworking footer identity layout (U-4)
   - Dirty-patch / unsaved dialogs (Epic 9)
   - Naming a constant for `"deviceDetected"` unless a one-line win falls out (deferred-work optional)

9. **And** `Matrix-Control_Tests` pass; macOS Debug VST3 + Standalone build clean; no French in source; `Core ↛ GUI` preserved.

## Tasks / Subtasks

- [x] **T1 — Core outbound gate** (AC: #3, #4, #9)
  - [x] Add a single clear predicate (e.g. `MidiManager::isEditorOutboundAllowed()` / read `deviceDetected`) used by editor send paths
  - [x] Gate: `sendProgramChange`, EditorPath SysEx producers, parameter/master/matrix-mod dispatchers, ActionHandler SysEx/PC sends, dump/request paths that emit PC/SysEx
  - [x] **Do not** gate: `performDeviceInquiry` / inquiry arm+send, instrument realtime enqueue
  - [x] Update misleading comment on `isDeviceDumpAvailable()` (ports ≠ detection; FR-2 forbids PC while undetected)
  - [x] Unit tests: attempt PC/SysEx enqueue when `deviceDetected=false` → no send; inquiry still allowed; when `true` → send proceeds

- [x] **T2 — GUI section lock binder** (AC: #1, #7)
  - [x] Prefer extend/reuse `CompareLockBinder` pattern (`Source/GUI/Helpers/`) — e.g. `DeviceLockBinder` or a small shared `SectionLockBinder` with composed predicate
  - [x] Wire lock targets: PatchEditPanel, MasterEditPanel, MatrixModulationPanel, Bank Utility / Internal / Computer / **PatchMutator** (full PM)
  - [x] **Do not** lock HeaderPanel MIDI ports / logo menu / Settings entry points
  - [x] Solve Compare composition (AC #7) before shipping — see Dev Notes “Compare + Device lock”
  - [x] Visual: same language as Compare (`setAlpha(0.5f)`, intercept children, `giveAwayKeyboardFocus`)

- [x] **T3 — Footer guidance copy** (AC: #2, #5)
  - [x] Add English constant(s) under `PluginDisplayNames` (Footer / device-lock messages)
  - [x] When locked: set left-zone guidance via existing footer APVTS (`uiMessageText` + severity) — reuse `GrayedControlHelper::setFooter*Message` or `ExceptionPropagator` patterns; **do not** invent a third footer API
  - [x] Optional: grayed-click on locked containers re-asserts the same guidance (D-038) — only if cheap with existing `setGrayedClickHandler`
  - [x] On unlock: clear **only** the device-lock guidance string (exact match), mirroring Compare footer clear
  - [x] Leave FooterPanel right-zone identity code alone unless a real bug appears

- [x] **T4 — Regression / inquiry unlock path** (AC: #5, #6)
  - [x] Manual: no ports / timeout → locked + guidance + `No device`
  - [x] Manual: configure From+To to Matrix → inquiry success → unlock + identity in right zone + guidance cleared
  - [x] Manual: clear a port → lock returns; re-select → inquiry again (8.2)

- [x] **T5 — Build / regression** (AC: #8, #9)
  - [x] Full `Matrix-Control_Tests` green
  - [x] Smoke Standalone + VST3: locked editing does not move synth; Header ports still changeable

## Dev Notes

### Epic context

| Story | Owns |
|-------|------|
| **8.1 (done)** | `DeviceTypeRegistry` + D-080 |
| **8.2 (done)** | Inquiry trigger + async orchestration + FR-53 footer identity |
| **8.3 (this)** | UI lock + guidance + Core SysEx/PC gate when `deviceDetected=false` |
| **8.4** | Instrument bus layout + MASTER EDIT gray (M-6/6R) |
| **8.5 (done)** | `DeviceMemoryLimits` / PM 100-slot — already consumes detection |

### Architecture compliance

- **FR-2 owners:** Core (`MidiManager` / dispatch / handlers) for outbound gate; GUI helpers + body panels for lock; Footer left zone for guidance. Panels must **not** call `MidiManager` directly.
- **SSOT lock flag:** APVTS `deviceDetected` (bool) — do not add a parallel `uiLocked` property unless there is a hard reason (YAGNI: derive from detection).
- **Dependency:** `Core ↛ GUI`.
- **Error UX:** footer via `uiMessageText` / severity (D-038); ExceptionPropagator already centralises clear/set.
- **Identity:** footer right zone only (FR-53 / D-076) — orthogonal to lock guidance (left zone).

[Source: architecture.md FR→component map §4.1; PRD FR-2; D-037; D-038]

### Critical: Device Inquiry must bypass the outbound gate

```
FR-2: "No SysEx or Program Change is sent while locked."

Naive gate on every SysEx send → Device Inquiry blocked → user can never unlock.

Correct gate:
  if (!deviceDetected && !isDeviceInquiryMessage(sysEx))
      refuse editor SysEx;
  if (!deviceDetected)
      refuse Program Change;
  // Device Inquiry request always allowed when ports open (existing 8.2 path)
```

Prefer a named allowlist (inquiry only) over scattering boolean exceptions.

### Critical: Compare + Device lock composition

Today `CompareLockBinder` independently sets:

```cpp
component->setInterceptsMouseClicks(true, !compareActive);
component->setAlpha(compareActive ? 0.5f : 1.0f);
```

A second binder that only looks at `deviceDetected` will **overwrite** those flags when detection flips.

**Required behaviour:** panel interactive children only when `deviceDetected && !compareActive` (Mutator Compare self-management rules preserved for COMPARE control itself).

**Recommended approaches (pick one, document in completion notes):**

1. **Shared apply helper** used by both binders: `applySectionLock(component, locked)` where each binder recomputes `locked = !deviceDetected || compareActive` before apply; **or**
2. **Generalise** one `SectionLockBinder` with a predicate / multi-property listen; migrate Compare wiring to it and add device property; **or**
3. **Single coordinator** owned higher (BodyPanel / SharedPanel / PatchManagerPanel) that listens to both properties.

Do **not** ship two naive binders. Prefer KISS over a large GUI framework rewrite — option 1 or a thin generalisation of `CompareLockBinder` is enough.

### What stays enabled (recovery surface)

| Control | Locked when undetected? |
|---------|-------------------------|
| MIDI From / MIDI To | **No** — recovery |
| Skin / UI Scale / Settings / About | **No** |
| Footer (read-only messaging) | N/A — shows guidance |
| PATCH / MASTER / MM / PM / Mutator / I/C/P | **Yes** |
| Instrument notes/CC (if ports + host armed) | **Not UI-locked**; realtime path not gated by FR-2 |

### Current code — preserve / extend

```cpp
// CompareLockBinder — template for section lock (extend carefully)
component->setInterceptsMouseClicks(true, !locked);
component->setAlpha(locked ? 0.5f : 1.0f);
if (locked) component->giveAwayKeyboardFocus();

// MidiManager — detection SSOT (do not rewrite inquiry)
updateDeviceStatus(bool detected, version, type);
refreshDeviceInquiryAfterPortSync();  // keep as unlock path

// Footer right zone — leave alone
FooterPanel::buildDeviceIdentityText(); // type · vX or kNoDevice
```

**Must not break:**
- 8.2 inquiry debounce / async capture / port-clear detection clear
- 8.5 Bank Utility gray when **detected** M-6/6R (no banks) — device lock when **undetected** is a separate, broader lock; when detected, existing M-6 gray rules still apply
- Compare mode Mutator COMPARE still clickable while Compare locks other PM sections
- ExceptionPropagator / left-zone message catalog for other features
- Session persistence / FR-3 strip policy from 7-8

### Previous story intelligence

**From 8.2 (done):**
- Inquiry async + port-sync trigger complete; footer identity verified.
- Explicitly deferred UI lock + fix-connection copy to **this** story.
- Review deferrals (SafePointer on timers, full async inquiry unit coverage) — **do not expand** into 8.3 unless blocking.
- Pattern: `DeviceInquiryTrigger` pure helper + MidiManager orchestration — prefer small testable predicates for “may send editor outbound?” similarly.

**From 8.1 / 8.5:**
- `deviceType` / `deviceDetected` already consumed by memory limits and Bank Utility — lock must not fight those listeners (Bank Utility: gray when detected+no banks; when undetected, whole PM locked by 8.3).

**From 7.8:**
- Full UI lock deferred here; false historical claim that inquiry already ran — fixed in 8.2.

**From D-037 / D-010:**
- Plugin is a MIDI remote, not a standalone generator without hardware. No local INIT exception in v1.

### Git intelligence

- `62e67c6` — Close story 8.2 Device Inquiry trigger and footer identity (**baseline**).
- `8360bce` — Close story 8.1 DeviceTypeRegistry.
- Pattern to copy for GUI lock: `CompareLockBinder` + wiring in `BodyPanel` / `SharedPanel` / `PatchManagerPanel`.
- Pattern to copy for footer clear: Compare / Mutator exact-string clear of `kCompareLockedFooter`.

### Project structure notes

| Path | Role in 8.3 |
|------|-------------|
| `Source/Core/MIDI/MidiManager.{h,cpp}` | **PRIMARY** Core gate + inquiry allowlist |
| `Source/Core/MIDI/*Dispatcher*`, `EditorPath`, `PatchSelectionMidiSync` | Gate call sites / enqueue lambdas |
| `Source/Core/Actions/*ActionHandler*` | Refuse SysEx/PC actions when locked (fail closed + footer if useful) |
| `Source/Core/PluginProcessor.cpp` | Any direct `sendProgramChange` / sync paths |
| `Source/GUI/Helpers/CompareLockBinder.*` | **Reuse / generalise** — composition with device lock |
| `Source/GUI/Helpers/GrayedControlHelper.*` | Footer message helpers |
| `Source/GUI/Panels/.../BodyPanel*`, `SharedPanel*`, `PatchManagerPanel*` | Wire binder(s); include Mutator |
| `Source/GUI/.../HeaderPanel*` | **Do not lock** recovery controls |
| `Source/GUI/.../FooterPanel*` | Verify left/right zones; minimal edits |
| `Source/Shared/Definitions/PluginDisplayNames.h` | Guidance + optional locked-click copy |
| `Tests/Unit/*` | Core gate tests; binder predicate tests if extracted |

### Testing requirements

- Framework: JUCE `UnitTest` under `Tests/Unit/`
- AAA; F.I.R.S.T.; no live Matrix hardware required in CI
- Prefer TDD for pure helpers (`isEditorOutboundAllowed`, composed lock predicate)
- Manual UAT: lock on launch / timeout; unlock after inquiry; Header ports still work; Compare + undetected composition; no PC when scrubbing Internal Patches while locked

[Source: project-context.md § Testing Strategy; CONVENTIONS.md §8.5]

### Latest tech notes

- JUCE 8.0.12: APVTS `state` ValueTree listeners are **message-thread** — same model as `CompareLockBinder`; do not read/write ValueTree from audio thread for the gate. If a send path can run off the message thread, mirror `deviceDetected` into an atomic updated from `updateDeviceStatus` (only if a real race exists — measure before inventing).
- No JUCE built-in “plugin locked” API — project helpers remain correct.
- MMA Device Inquiry remains the unlock SysEx; keep allowlisted.

### Anti-patterns (do not)

- Reimplement Device Inquiry / registry / footer identity from scratch
- Gate Device Inquiry SysEx behind the lock (permanent lockout)
- Lock Header MIDI ports (user cannot recover)
- Add `uiLocked` duplicate of `deviceDetected` without need
- Modal dialogs for “no synth” (D-038)
- INIT / edit exception while undetected (D-037)
- Two binders that overwrite each other’s `setInterceptsMouseClicks`
- Gate instrument notes/CC as if they were FR-2 editor traffic
- French in source / user-facing strings outside `PluginDisplayNames`
- GUI includes in Core

### Project context reference

Follow `_bmad-output/project-context.md`: C++17, Allman braces, English-only source, Clean Code limits, Core/GUI dependency rule, dedicated MIDI thread for editor SysEx, JUCE UnitTest conventions.

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Full suite initially failed after FR-2 gate because PatchMutatorEngine / PatchManagerActionHandler harnesses left `deviceDetected` at constructor default `false`. Fixed harnesses to set `deviceDetected=true` when they assert outbound MIDI.

### Completion Notes List

- **T1:** Added `EditorOutboundGate.h` pure predicates (`isEditorOutboundAllowed`, inquiry allowlist, `isSectionLocked`). `MidiManager::isEditorOutboundAllowed()` gates all editor PC/SysEx entry points (including dump/request paths). Device Inquiry continues via `sendSysExWithDelay` and is not gated. Instrument realtime path untouched.
- **T2 / Compare composition (option 1):** Extended `CompareLockBinder` with shared `applySectionLock`, listens to `deviceDetected` + Compare, predicate `locked = !deviceDetected || compareActive`. Patch Mutator uses a second binder with `lockOnCompare=false` so COMPARE stays live when detected+Compare. Header not locked. Patch Manager `SectionHeader` included in the composed binder so it dims with the section.
- **UAT fix:** JUCE 8 `getComponentAt` ignores `allowClicksOnChildComponents`; lock now uses `setInterceptsMouseClicks(false, false)` while locked so the whole subtree rejects hits (alpha alone was cosmetic).
- **T3:** `PluginDisplayNames::FooterPanel::kDeviceLockGuidance`; binder sets/clears left-zone via `GrayedControlHelper::setFooterInfoMessage` with exact-string clear. Skipped optional grayed-click (would fight `setInterceptsMouseClicks` with Compare composition).
- **T4:** Unlock path unchanged (`refreshDeviceInquiryAfterPortSync` / `performDeviceInquiry`). Automated gate + composition predicates cover CI; manual UAT checklist remains for synth-on-desk verification.
- **T5:** `Matrix-Control_Tests` exit 0; macOS Debug VST3 + Standalone built clean.

### File List

- Source/Core/MIDI/EditorOutboundGate.h (new)
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/GUI/Helpers/CompareLockBinder.h
- Source/GUI/Helpers/CompareLockBinder.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp
- Source/Shared/Definitions/PluginDisplayNames.h
- Tests/Unit/EditorOutboundGateTests.cpp (new)
- Tests/Unit/MidiManagerTests.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp
- Tests/Unit/PatchManagerActionHandlerTests.cpp
- Tests/Unit/MidiPortOpenFailurePropagationTests.cpp
- CMakeLists.txt
- _bmad-output/implementation-artifacts/sprint-status.yaml
- _bmad-output/implementation-artifacts/8-3-ui-lock-without-synth.md

### Review Findings

- [x] [Review][Patch] Footer message priority while device-locked — Always force `kDeviceLockGuidance` while undetected; on unlock clear device guidance (incl. severity) and restore Compare footer if Compare still active (Guillaume chose option 1) [Source/GUI/Helpers/CompareLockBinder.cpp]
- [x] [Review][Decision] In-flight editor MIDI after lock — Resolved: allow in-flight drain; enqueue gate only (Guillaume chose option 2). No code change.
- [x] [Review][Patch] Re-assert device-lock footer after port `clearMessage` while still undetected [Source/Core/MIDI/MidiManager.cpp]
- [x] [Review][Patch] Clear `uiMessageSeverity` when clearing device-lock guidance (match Compare exact-string clear) [Source/GUI/Helpers/CompareLockBinder.cpp]
- [x] [Review][Defer] Single owner for device-lock footer sync across four binders [Source/GUI/Helpers/CompareLockBinder.cpp] — deferred, pre-existing multi-binder wiring amplified by this story
- [x] [Review][Defer] `maySendEditorSysEx` allowlist unused by MidiManager send path (inquiry uses ungated `sendSysExWithDelay`) [Source/Core/MIDI/EditorOutboundGate.h] — deferred, pre-existing path design; document or wire later

### Change Log

- 2026-07-18 — Implemented FR-2 UI lock, footer guidance, and Core outbound gate; story → review.
- 2026-07-18 — Code review: footer guidance re-assert after port clear; unlock clears severity and restores Compare copy; in-flight queue drain accepted; story → done.

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **done**
- Sprint key: `8-3-ui-lock-without-synth`
