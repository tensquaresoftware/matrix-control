---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.11 — Internal Patches INIT SysEx Device Rules
author: BMad Agent
status: review
baseline_commit: d309fa8a88a81aa139a46d89e699610789b21d65
sources:
  - planning-artifacts/sprint-change-proposal-2026-07-14-init-sysex-and-hierarchical-history-combobox.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - implementation-artifacts/7-3-patchmanageractionhandler-bank-and-internal.md
  - _local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md
  - _local/Documents/References/MD/oberheim-matrix-6-6r-midi-sysex-implementation.md
  - project-context.md
created: 2026-07-14
updated: 2026-07-14
---

# Story 7.11: Internal Patches INIT SysEx Device Rules

Status: done

## Story

As a sound designer,
I want Internal Patches INIT to load the init template into the plugin **and** push it to the connected synth using the correct SysEx for my device type,
so that INIT matches my Matrix-1000 Editor prototype on M-1000 and writes the current patch on Matrix-6/6R (FR-24, D-044-R2).

## Acceptance Criteria

1. **Given** Story 7.3 INIT template path (`PatchInitService`, suppress flags, footer) **When** user triggers `internalPatchesInit` on **Matrix-1000** (`limits.hasBankConcept() == true`) **Then** handler loads template → APVTS (unchanged suppress pattern) **and** enqueues **one** SysEx **0x0D** (Single Patch Data to Edit Buffer) via `MidiManager::sendPatchToEditBuffer` **And** **no** 0x01 patch-number message on INIT.

2. **And** **Given** device type **Matrix-6/6R** (`!limits.hasBankConcept()`) **When** user triggers INIT **Then** handler loads template → APVTS **and** enqueues **0x01** Single Patch Data to **current patch number** (same packed buffer path as PASTE/STORE) **And** **never** sends 0x0D or 0x0E.

3. **And** INIT is **blocked** on ROM banks (same FR-23 gating as PASTE/STORE — UI grayed + handler defense-in-depth); footer info/warning from `InitTemplateLoadResult` unchanged on allowed banks.

3b. **And** INIT is **blocked** while Compare mode is active (UI grayed like STORE; handler no-op if action reaches Core).

4. **And** **STORE** and **PASTE** behaviour **unchanged** from 7.3 (0x01 to current patch slot; ROM gates intact).

5. **And** SysEx infrastructure:
   - `SysExConstants::Opcode::kSinglePatchToEditBuffer = 0x0D`
   - `SysExEncoder::encodePatchToEditBufferSysEx(const juce::uint8* packedData)` — `F0 10 06 0D <data> <checksum> F7` (no patch number byte)
   - `MidiManager::sendPatchToEditBuffer(const juce::uint8* packedData)`
   - Unit tests in `SysExEncoderTests` for 0x0D envelope bytes + checksum

6. **And** `PatchManagerActionHandlerTests`:
   - M-1000 limits fake → INIT calls `sendPatchToEditBuffer` once; no `sendPatch`
   - M-6 limits fake → INIT calls `sendPatch(currentPatch, data)` once; no edit-buffer send
   - Existing INIT APVTS/template assertions stay green

7. **And** full `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds green.

8. **And** **no GUI changes** — panel wiring remains 7.6; UAT grid item M6-4.1 / 7-6 smoke item 8 updated in Dev Notes.

## Tasks / Subtasks

- [x] **SysEx encode + MidiManager** (AC: #5)
  - [x] Add opcode constant and encoder (mirror `encodePatchSysEx` without patch number nibble)
  - [x] Add `sendPatchToEditBuffer` on editor path queue
  - [x] `SysExEncoderTests` golden bytes for minimal valid buffer

- [x] **Handler device branch** (AC: #1, #2, #3, #4)
  - [x] In `handleInternalPatchInit`: after `pushPatchModelToApvtsWithSuppress`, resolve limits from current device type
  - [x] M-1000: `apvtsPatchMapper_->apvtsToBuffer()` then `sendPatchToEditBuffer`
  - [x] M-6/6R: `apvtsToBuffer()` then `sendPatch(currentPatchNumber, data)`
  - [x] Do not alter PASTE/STORE handlers

- [x] **Tests + docs** (AC: #6, #7, #8)
  - [x] Extend `PatchManagerActionHandlerTests` with device-type INIT MIDI fakes
  - [x] Note errata on story 7-3 AC #4 in completion notes
  - [x] Run full test suite

## Dev Notes

### Correct Course context (D-044-R2)

Story **7.3** deliberately made INIT editor-only. UAT on **7-6** (2026-07-14) showed mismatch with Matrix-1000 Editor prototype: INIT must load **edit buffer** (0x0D) immediately. Updated M-6/6R reference doc confirms **no** 0x0D/0x0E — INIT uses **0x01** to current patch.

### Oberheim reference

| Opcode | Matrix-1000 | Matrix-6/6R |
|--------|-------------|-------------|
| 0x01 Single Patch Data | To patch number in current bank | To patch number 0–99 |
| 0x0D Edit Buffer | Full patch → edit buffer | **Not supported** |
| 0x0E Store Edit Buffer | Edit buffer → bank/patch | **Not supported** |

**STORE v1:** keep **0x01** (UAT smoke 10 validated). 0x0E is a future enhancement if needed.

### Execution flow (target)

```
handleInternalPatchInit()
  → patchInitService_->initFullPatch()
  → pushPatchModelToApvtsWithSuppress(...)
  → propagateInitTemplateFooterMessage(result)
  → limits = DeviceMemoryLimits::resolve(deviceType)
  → apvtsPatchMapper_->apvtsToBuffer()
  → if limits.hasBankConcept()
        midiManager_->sendPatchToEditBuffer(patchModel_->data())
     else
        midiManager_->sendPatch(currentPatch, patchModel_->data())
```

### Inter-message delay

Respect existing `SysExDelayProfile` / queue consumer (Story 2.2). 0x0D is full patch (~275 bytes) — same class as 0x01.

### Files (expected)

```
Source/Core/MIDI/SysEx/SysExConstants.h
Source/Core/MIDI/SysEx/SysExEncoder.h
Source/Core/MIDI/SysEx/SysExEncoder.cpp
Source/Core/MIDI/MidiManager.h
Source/Core/MIDI/MidiManager.cpp
Source/Core/Actions/PatchManagerActionHandler.cpp
Tests/Unit/SysExEncoderTests.cpp
Tests/Unit/PatchManagerActionHandlerTests.cpp
```

### Manual smoke (post-impl)

- **M-1000:** INIT → init sound audible immediately; MIDI monitor shows `F0 10 06 0D … F7` (not 0x01)
- **M-6 type (Settings/sim):** INIT → `F0 10 06 01 <patch> … F7` with current patch number

## Dev Agent Record

### Agent Model Used

Composer 2.5

### Completion Notes List

- Added `kSinglePatchToEditBuffer` (0x0D) opcode, `encodePatchToEditBufferSysEx`, and `MidiManager::sendPatchToEditBuffer` on the editor-path queue.
- `handleInternalPatchInit` now branches on `DeviceMemoryLimits::hasBankConcept()`: M-1000 sends 0x0D edit buffer; M-6/6R sends 0x01 to current patch slot. PASTE/STORE unchanged.
- Added `kPatchToEditBufferMessageLength` (274 bytes — one fewer header byte than 0x01).
- **Story 7-3 AC #4 errata:** INIT was deliberately editor-only in 7.3; Correct Course D-044-R2 supersedes that — INIT must push SysEx to the synth per device type.
- **AC #3 errata (code review 2026-07-14):** INIT on ROM banks allowed in original 7-11 AC; superseded — INIT now gated like PASTE/STORE (SysEx to synth is a write). Compare-mode INIT grayed like STORE.
- Full `Matrix-Control_Tests` green; macOS Debug VST3 + Standalone build green.

### File List

- Source/Core/MIDI/SysEx/SysExConstants.h
- Source/Core/MIDI/SysEx/SysExEncoder.h
- Source/Core/MIDI/SysEx/SysExEncoder.cpp
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.h
- Tests/Unit/SysExEncoderTests.cpp
- Tests/Unit/PatchManagerActionHandlerTests.cpp

## Change Log

- 2026-07-14: Story 7-11 created — Correct Course D-044-R2 INIT device-aware SysEx.
- 2026-07-14: Implementation complete — device-aware INIT SysEx (0x0D M-1000, 0x01 M-6/6R), tests, builds green.
- 2026-07-14: Code review — ROM + Compare INIT gating (GUI + handler), exactly-one SysEx test assertion, ROM/compare handler tests; status → done.

### Review Findings

- [x] [Review][Decision] INIT pendant Compare → griser le bouton (comme STORE) + garde handler — resolved: GUI `wirePasteStoreButton` sur INIT avec `!compareActive` ; handler no-op si `kCompareActive`.

- [x] [Review][Patch] Tests INIT : assertion « exactement un » message SysEx [Tests/Unit/PatchManagerActionHandlerTests.cpp] — resolved: `patchSysExCount`.

- [x] [Review][Patch] Test INIT bloqué sur banque ROM [Tests/Unit/PatchManagerActionHandlerTests.cpp] — resolved: `testInitRomBankBlocked` + AC3 errata (INIT gated like PASTE/STORE).

- [x] [Review][Defer] Footer succès INIT affiché avant envoi MIDI / si `midiManager_` null [Source/Core/Actions/PatchManagerActionHandler.cpp:250-253] — deferred, pre-existing (pattern 7.3)

- [x] [Review][Defer] Pas de garde « port MIDI ouvert » avant enqueue [Source/Core/MIDI/MidiManager.cpp:231-242] — deferred, pre-existing (identique à `sendPatch`)

- [x] [Review][Defer] `apvtsToBuffer()` sans vérification de succès avant envoi [Source/Core/Actions/PatchManagerActionHandler.cpp:256] — deferred, pre-existing (PASTE/STORE)

- [x] [Review][Defer] `getCurrentPatch()` non clampé avant cast `uint8` [Source/Core/Actions/PatchManagerActionHandler.cpp:261,737-741] — deferred, pre-existing

- [x] [Review][Defer] Type device `kUnknown` → branche M-1000 / 0x0D [DeviceMemoryLimits.cpp] — deferred, pre-existing

- [x] [Review][Defer] Pas de tests unitaires `MidiManager::sendPatchToEditBuffer` — deferred, hors AC5 (couvert par SysExEncoderTests + handler tests)
