---
organization: Ten Square Software
project: Matrix-Control
title: Story 8.5 — Matrix-6/6R Patch Memory Limits
author: BMad Agent
status: ready-for-dev
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/brief.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - reference-docs/oberheim/oberheim-matrix-6-6r-midi-sysex-implementation.md
  - reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - Source/Shared/Definitions/Matrix1000Limits.h
created: 2026-06-12
updated: 2026-06-12
---

# Story 8.5: Matrix-6/6R Patch Memory Limits

Status: ready-for-dev

<!-- Extends FR-46 beyond MASTER graying. Closes the v1 gap left by brief decision "100 vs 1000 patches deferred". Depends on deviceType from Stories 8.1–8.2; coordinates with Epic 7 Patch Manager wiring (7.3, 7.5, 7.6). -->

## Story

As a Matrix-6 or Matrix-6R owner,
I want the PATCH MANAGER to reflect my synth's 100-patch memory model with no bank concept,
so that I can navigate and store patches 00–99 without Matrix-1000 bank semantics leaking into the UI or MIDI layer (FR-46 extension).

## Context

| Model | Banks | Patches per scope | Total internal patches |
|-------|-------|-------------------|--------------------------|
| Matrix-1000 | 10 (0–9; 0–1 RAM, 2–9 ROM) | 00–99 per bank | 1000 |
| Matrix-6 / Matrix-6R | *none* | 00–99 | 100 |

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
   - **Matrix-6 / Matrix-6R:** `hasBankConcept=false`, patches `Matrix6Or6RLimits::kMinPatchNumber`–`kMaxPatchNumber`, `hasRomBanks=false`.
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

- [ ] **T1** — Add `Matrix6Or6RLimits.h` under `Source/Shared/Definitions/`.
- [ ] **T2** — Implement `DeviceMemoryLimits` (or `DeviceTypeRegistry::memoryLimits()`) in Core.
- [ ] **T3** — Wire `InternalPatchesPanel` patch NumberBox min/max from resolved limits; gray bank NumberBox or remove per D-023.
- [ ] **T4** — Wire `BankUtilityPanel` graying from `deviceType` + FR-45 pattern (Story 7.8 graying matrix may share helper).
- [ ] **T5** — Guard `PatchManagerActionHandler` / `MidiManager` bank SysEx and navigation wrap logic with `DeviceMemoryLimits`.
- [ ] **T6** — Add `DeviceMemoryLimitsTests.cpp`; register in CMake test target.
- [ ] **T7** — Manual UAT note: Matrix-6/6R hardware — verify patch 00/99 navigation and STORE; confirm no `0x0A` on bus.

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

—

### Completion Notes

—

### File List

—

## Change Log

- 2026-06-12 — Story 8.5 created: Matrix-6/6R patch memory limits, `Matrix6Or6RLimits.h`, device-aware limit resolution, BANK UTILITY graying, 100-slot navigation.
