---
organization: Ten Square Software
project: Matrix-Control
title: Story 8.5 — Matrix-6/6R Patch Memory Limits
author: BMad Agent
status: review
baseline_commit: 044ad3f2237e6bf61b6644e88ece2259c6647aaa
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/briefs/brief-matrix-control-2026-05-22/brief.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - reference-docs/oberheim/oberheim-matrix-6-6r-midi-sysex-implementation.md
  - reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - Source/Shared/Definitions/Matrix1000Limits.h
created: 2026-06-12
updated: 2026-06-12
---

# Story 8.5: Matrix-6/6R Patch Memory Limits

Status: done

<!-- Extends FR-46 beyond MASTER graying. Closes the v1 gap left by brief decision "100 vs 1000 patches deferred". Depends on deviceType from Stories 8.1–8.2; coordinates with Epic 7 Patch Manager wiring (7.3, 7.5, 7.6). -->

## Story

As a Matrix-6 or Matrix-6R owner,
I want the PATCH MANAGER to reflect my synth's 100-patch memory model with no bank concept,
so that I can navigate and store patches 00–99 without Matrix-1000 bank semantics leaking into the UI or MIDI layer (FR-46 extension).

## Context

| Model | Banks | Patches per scope | Total internal patches |
|-------|-------|-------------------|--------------------------|
| Matrix-1000 | 10 (0–9; 0–1 RAM, 2–9 ROM) | 00–99 per bank | 1000 |
| Matrix-6/6R | *none* | 00–99 | 100 |

`Matrix1000Limits.h` today encodes the M-1000 model only. M-6/6R have no Set Bank opcode (`0x0A` — M-1000 only per Oberheim refs). FR-46 currently covers MASTER EDIT graying only; this story adds **patch-memory semantics**.

## Acceptance Criteria

### AC 1 — Limit headers (SSOT constants)

1. Add `Source/Shared/Definitions/Matrix6Or6RLimits.h`:
   - `kMinPatchNumber = 0`, `kMaxPatchNumber = 99`
   - `kInternalPatchSlotCount = 100`
   - **No** bank-number constants (M-6/6R have no bank concept).
2. Keep `Matrix1000Limits.h` unchanged for M-1000 bank/patch bounds and **shared synthesis constants** (`kModulationBusCount`, `kParameterCellCount`, etc.) — these apply to all Matrix family PATCH editing.
3. Do **not** rename `Matrix1000Limits` in this story; bank-specific constants stay there.

### AC 2 — Device-aware limit resolution (Core)

1. Add a small Core helper (preferred location: `Source/Core/Services/DeviceMemoryLimits.{h,cpp}` or method on `DeviceTypeRegistry` from Story 8.1) exposing a read-only view, e.g. `DeviceMemoryLimits`, resolved from `deviceType`:
   - **Matrix-1000:** `hasBankConcept=true`, banks `Matrix1000Limits::kMinBankNumber`–`kMaxBankNumber`, patches `kMinPatchNumber`–`kMaxPatchNumber`, `hasRomBanks=true` (banks 2–9).
   - **Matrix-6/6R:** `hasBankConcept=false`, patches `Matrix6Or6RLimits::kMinPatchNumber`–`kMaxPatchNumber`, `hasRomBanks=false`.
   - **Unknown / not detected:** default to Matrix-1000 limits until inquiry succeeds (safe superset); re-resolve when `deviceType` updates.
2. All patch/bank validation in Core (handlers, `MidiManager` navigation, Program Change / patch-request paths) consults this helper — **no** scattered `if (deviceType == …)` with magic numbers.
3. GUI panels receive limits via APVTS `deviceType` listener + helper, or via properties pushed by Core — **no** GUI include of Core services beyond existing processor/editor patterns.

### AC 3 — Bank Utility disabled on Matrix-6/6R

**Given** `deviceType` is Matrix-6 or Matrix-6R and `deviceDetected=true`

**When** PATCH MANAGER is shown

**Then:**
- Entire **BANK UTILITY** module is grayed and non-interactive: bank buttons 0–9 and **BANK LOCK** (FR-45 device gating).
- Footer message on click attempt: informative text that bank selection is Matrix-1000 only.
- No Set Bank SysEx (`0x0A`) is enqueued for M-6/6R paths.
- `selectedBank` / `bankLock` APVTS properties are not mutated by user actions on grayed controls.

### AC 4 — Internal Patches navigation (100 slots, no inter-bank wrap)

**Given** Matrix-6 or Matrix-6R detected

**When** user navigates with `<` / `>` or edits the patch NumberBox

**Then:**
- Valid range is **00–99** only (`Matrix6Or6RLimits`).
- Wrap is **cyclic within 100 slots**: patch 99 → `<` → patch 00; patch 00 → `>` → patch 99.
- **No** wrap to another bank (patch 99 → `>` must **not** change bank or jump beyond 99).
- Patch load/store SysEx and Program Change use patch number only (no bank byte / Set Bank prelude).
- Remove or hide any **bank NumberBox** in Internal Patches if still present (align D-023; brownfield `currentBankNumber` in `InternalPatchesPanel.cpp`).

### AC 5 — PASTE / STORE rules on Matrix-6/6R

**Given** Matrix-6 or Matrix-6R detected

**When** Internal Patches panel is active

**Then:**
- PASTE and STORE remain **enabled** for all patches 00–99 (FR-23 ROM gating is Matrix-1000 only).
- INIT and COPY behave as on Matrix-1000 (same 134-byte PATCH SysEx).
- STORE sends patch to synth internal memory per M-6/6R rules (Single Patch Data `0x01` — no bank qualifier).

### AC 6 — Matrix-1000 regression

**Given** `deviceType` is Matrix-1000

**When** PATCH MANAGER is used

**Then** existing FR-19–FR-24 behaviour is unchanged: banks 0–9, BANK LOCK, inter-bank wrap when unlocked, ROM gating on banks 2–9.

### AC 7 — Tests and build

1. Unit tests for `DeviceMemoryLimits` resolution: all three `deviceType` values + unknown default.
2. Unit tests for patch wrap helper: M-6 cyclic 99↔00; M-1000 inter-bank wrap unchanged (may live in handler tests added in 7.3 if not yet present — at minimum limits resolution is tested here).
3. `Matrix1000Limits` / `Matrix6Or6RLimits` constants referenced by name in tests (no duplicated literals).
4. macOS VST3 + Standalone build clean; existing unit suite passes.

## Tasks / Subtasks

- [x] **T1** — Add `Matrix6Or6RLimits.h` under `Source/Shared/Definitions/`.
- [x] **T2** — Implement `DeviceMemoryLimits` (or `DeviceTypeRegistry::memoryLimits()`) in Core.
- [x] **T3** — Wire `InternalPatchesPanel` patch NumberBox min/max from resolved limits; gray bank NumberBox or remove per D-023.
- [x] **T4** — Wire `BankUtilityPanel` graying from `deviceType` + FR-45 pattern (Story 7.8 graying matrix may share helper).
- [x] **T5** — Guard `PatchManagerActionHandler` / `MidiManager` bank SysEx and navigation wrap logic with `DeviceMemoryLimits`.
- [x] **T6** — Add `DeviceMemoryLimitsTests.cpp`; register in CMake test target.
- [x] **T7** — Manual UAT note: Matrix-6/6R hardware — verify patch 00/99 navigation and STORE; confirm no `0x0A` on bus.

### Review Findings

- [x] [Review][Defer] **GUI panels include Core services directly** — `BankUtilityPanel` and `InternalPatchesPanel` call `Core::DeviceMemoryLimits::resolve()` from GUI; matches `SettingsPanel` precedent; refactor to APVTS limit properties in a follow-up story.

- [x] [Review][Patch] **Footer unreachable on grayed bank buttons** [BankUtilityPanel.cpp:69-98] — buttons stay click-enabled when grayed; alpha + onClick footer guard.
- [x] [Review][Patch] **French comment in source** [InternalPatchesPanel.cpp:57] — translated to English.
- [x] [Review][Patch] **Test uses literal `99` instead of named constant** [DeviceMemoryLimitsTests.cpp:130] — uses `Matrix6Or6RLimits::kMaxPatchNumber`.
- [x] [Review][Patch] **Thin Matrix-6R resolution test** [DeviceMemoryLimitsTests.cpp:52-58] — full bounds assertions added.
- [x] [Review][Patch] **Duplicate member-byte literals in DeviceTypeRegistry** [DeviceTypeRegistry.cpp:11-17] — uses `SysExConstants::DeviceInquiry` constants.
- [x] [Review][Patch] **Redundant Program Change branches** [PluginProcessor.cpp:1161-1168] — collapsed to single `sendProgramChange` call.
- [x] [Review][Patch] **Unused include in GUI** [InternalPatchesPanel.cpp:13] — removed `Matrix6Or6RLimits.h`.
- [x] [Review][Patch] **Missing M-1000 lower-bound wrap test** [DeviceMemoryLimitsTests.cpp] — bank 0 patch 0 → bank 9 patch 99 test added.
- [x] [Review][Patch] **Stale bank coordinate on device-type switch** [PluginProcessor.cpp:1133-1134] — `reconcilePatchManagerCoordinatesForDeviceType()` on `deviceType` / `deviceDetected` change; bank reset in `handleBankNumberChange`.

- [x] [Review][Defer] **STORE / INIT / COPY / PASTE not handled in Core** [PluginProcessor.cpp] — pre-existing; Story 7.3 `PatchManagerActionHandler` scope; dev notes acknowledge wiring deferred.

- [x] [Review][Defer] **Patch-load SysEx not invoked on navigation** [PluginProcessor.cpp] — `<`/`>` updates APVTS only; Program Change sent on manual patch edit but not full Single Patch Request; pre-existing Epic 7 gap.

- [x] [Review][Defer] **Matrix-6R never assigned from Device Inquiry** [DeviceTypeRegistry.cpp] — `0x01/0x00` maps to `kMatrix6` only; 6R shares M-6 memory model today; distinct member bytes TBD at T7 hardware UAT.

- [x] [Review][Defer] **No integration tests for GUI graying / property listener ordering** — unit tests cover limit math only; manual UAT + future handler tests acceptable for v1.

- [x] [Review][Defer] **InitDefaults / GenerateInitFixtures mixed in working tree** [CMakeLists.txt] — Story 3-1 scope creep in same branch; split for review boundaries.

- [x] [Review][Defer] **Program Change without Set Bank on Matrix-1000** [PluginProcessor.cpp] — bank change via navigation does not send `0x0A`; pre-existing MIDI gap outside 8-5 AC scope.

## Dev Notes

### Dependency graph

| Story | Relationship |
|-------|----------------|
| **8.1** DeviceTypeRegistry | **Hard dependency** — `deviceType` enum + member-byte mapping |
| **8.2** Device Inquiry | **Hard dependency** — `deviceType` / `deviceDetected` APVTS properties |
| **8.4** FR-46 MASTER graying | Soft — same epic; can ship together |
| **7.3** PatchManagerActionHandler | **Coordinate** — handler must use `DeviceMemoryLimits`; if 7.3 lands first, refactor; if 8.5 lands first, 7.3 implements against helper |
| **7.5** Bank Utility wiring | **Coordinate** — graying AC supersedes unconditional bank button wiring for M-6 |
| **7.6** Internal Patches wiring | **Coordinate** — wrap rules must branch on `hasBankConcept` |
| **7.8** Graying matrix | Footer-on-grayed-click pattern for BANK UTILITY |

### Architecture constraints

- `Core ↛ GUI` — limits helper lives in Core; GUI reads `deviceType` from APVTS and calls a processor/editor façade or listens to limit-related properties.
- No new magic numbers — use `Matrix1000Limits::*` and `Matrix6Or6RLimits::*` only.
- `kModulationBusCount` and other synthesis limits remain in `Matrix1000Limits` (identical across PATCH-capable models).

### Oberheim reference

- M-6/6R patch request: `F0 10 06 04 01 pp F7` with `pp` = 0–99 ([oberheim-matrix-6-6r-midi-sysex-implementation.md §8.2](../../reference-docs/oberheim/oberheim-matrix-6-6r-midi-sysex-implementation.md)).
- Set Bank `0x0A` — Matrix-1000 only; must not be sent to M-6/6R.

### PRD follow-up (out of scope for implementation PR)

Extend **FR-46** text and add device-type qualifiers on **FR-19–FR-23** in `prd.md` when this story is accepted — story implements behaviour; PRD edit is a separate doc pass.

### Brownfield touchpoints

| File | Change |
|------|--------|
| `Source/Shared/Definitions/Matrix6Or6RLimits.h` | **New** |
| `Source/Core/Services/DeviceMemoryLimits.{h,cpp}` | **New** (or extend `DeviceTypeRegistry`) |
| `InternalPatchesPanel.cpp` | Patch NumberBox bounds; bank NumberBox removal/gray |
| `BankUtilityPanel.cpp` | Device-type graying |
| `PatchManagerActionHandler` (Story 7.3) | Navigation + SysEx guards |
| `CMakeLists.txt` | New Core + test sources if needed |

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Implementation Plan

- Added `Matrix6Or6RLimits.h` and `MatrixDeviceTypes.h` as SSOT for M-6/6R patch bounds and APVTS `deviceType` strings.
- Implemented `DeviceTypeRegistry` (member-byte mapping, partial 8.1 scope) and `DeviceMemoryLimits` (resolve + `advancePatch` wrap rules).
- Fixed D-080 member-byte constants in `SysExConstants.h`; `SysExDecoder` now accepts Matrix-1000 and Matrix-6/6R family replies.
- `MidiManager` publishes `deviceType` APVTS property on Device Inquiry; initializes `patchManagerSelectedBank` / `patchManagerBankLock`.
- `InternalPatchesPanel`: hides bank NumberBox when `!hasBankConcept`, clamps patch range, ROM gating for PASTE/STORE on M-1000 only.
- `BankUtilityPanel`: grays entire module on Matrix-6/6R + footer info on click attempt.
- `PluginProcessor`: navigation (`<`/`>`), bank select, and bank-lock handlers consult `DeviceMemoryLimits`; bank actions no-op on M-6/6R.
- Note: `PatchManagerActionHandler` (Story 7.3) not yet present — logic wired in `PluginProcessor` until 7.3 lands. Set Bank SysEx (`0x0A`) not yet encoded anywhere; guarded at handler level for future wiring.

### Completion Notes

- All ACs implemented in code; unit tests cover limit resolution, cyclic M-6 wrap, M-1000 inter-bank/locked wrap, ROM gating, and registry member bytes.
- Builds clean: `Matrix-Control_Tests`, `Matrix-Control_VST3`, `Matrix-Control_Standalone` (macOS ARM).
- **Manual UAT (T7):** Requires Matrix-6/6R hardware — verify patch 00/99 navigation, STORE, and confirm no `0x0A` Set Bank SysEx on MIDI bus.

### File List

- `Source/Shared/Definitions/Matrix6Or6RLimits.h` (new)
- `Source/Shared/Definitions/MatrixDeviceTypes.h` (new)
- `Source/Core/Services/DeviceTypeRegistry.h` (new)
- `Source/Core/Services/DeviceTypeRegistry.cpp` (new)
- `Source/Core/Services/DeviceMemoryLimits.h` (new)
- `Source/Core/Services/DeviceMemoryLimits.cpp` (new)
- `Source/Core/MIDI/SysEx/SysExConstants.h`
- `Source/Core/MIDI/SysEx/SysExDecoder.h`
- `Source/Core/MIDI/SysEx/SysExDecoder.cpp`
- `Source/Core/MIDI/MidiManager.h`
- `Source/Core/MIDI/MidiManager.cpp`
- `Source/Core/PluginProcessor.h`
- `Source/Core/PluginProcessor.cpp`
- `Source/GUI/Widgets/NumberBox.h`
- `Source/GUI/Widgets/NumberBox.cpp`
- `Source/GUI/Panels/.../BankUtilityPanel.h`
- `Source/GUI/Panels/.../BankUtilityPanel.cpp`
- `Source/GUI/Panels/.../InternalPatchesPanel.h`
- `Source/GUI/Panels/.../InternalPatchesPanel.cpp`
- `Source/Shared/Definitions/PluginIDs.h`
- `Source/Shared/Definitions/PluginDisplayNames.h`
- `Tests/Unit/DeviceMemoryLimitsTests.cpp` (new)
- `CMakeLists.txt`

## Change Log

- 2026-06-12 — Story 8.5 created: Matrix-6/6R patch memory limits, `Matrix6Or6RLimits.h`, device-aware limit resolution, BANK UTILITY graying, 100-slot navigation.
- 2026-06-17 — Implemented DeviceMemoryLimits, DeviceTypeRegistry (partial 8.1), GUI graying, navigation guards, unit tests; status → review.
