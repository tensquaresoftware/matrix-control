---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.3 — PatchManagerActionHandler Bank and Internal
author: BMad Agent
status: done
baseline_commit: a1e1bbc
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - implementation-artifacts/7-1-actiondispatcher-and-handler-interfaces.md
  - implementation-artifacts/7-2-moduleactionhandler-i-c-p-and-matrix-mod-init.md
  - implementation-artifacts/5-1-clipboardservice-compatibility-matrix.md
  - implementation-artifacts/8-5-matrix-6-6r-patch-memory-limits.md
  - project-context.md
created: 2026-06-18
updated: 2026-06-18
---

# Story 7.3: PatchManagerActionHandler Bank and Internal

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want bank and internal patch actions dispatched to Core,
so that Patch Manager buttons behave per PRD (FR-19–FR-24).

## Acceptance Criteria

> **ERRATA (2026-06-19 — Correct Course D-022-R1):** AC #3 (BANK LOCK toggle) is **superseded**. Correct implementation (`handleUnlockBank` + derived lock state) is tracked in Story **7-3b**. Do not mark 7.3 `done` until 7-3b merges.

1. **Given** Story 7.1 (`ActionDispatcher` + `PatchManagerActionHandler` scaffold), Story 7.2 (module I/C/P complete), Epic 2 MIDI enqueue paths, and Story 8.5 (`DeviceMemoryLimits`) **When** user triggers Bank Utility bank select / lock, Internal Patches navigation, INIT, PASTE, or STORE **Then** `PatchManagerActionHandler` executes the action — **no stubs**, no early `return` for these IDs (except Computer Patches file ops — see AC #7).

2. **And** bank selection + navigation (already partially implemented) **also enqueue synth selection MIDI**:
   - **Matrix-1000** (`limits.hasBankConcept()`): on bank change → Set Bank SysEx **0x0A** with bank 0–9; on patch change → Program Change with patch 0–99; order **Set Bank before Program Change** when both change in one action.
   - **Matrix-6 / Matrix-6R**: Program Change only (patch 0–99); **never** enqueue Set Bank **0x0A** or Unlock **0x0C**.
   - Bank Utility buttons remain no-ops when `!limits.hasBankConcept()` (existing guard preserved).

3. **And** **UNLOCK** action (FR-20 corrected — see Story **7-3b**):
   - Action `bankUtilityUnlockBank` sends Unlock Bank **0x0C** only; sets `patchManagerBanksLocked` false.
   - Bank select / navigation Set Bank sets `patchManagerBanksLocked` true.
   - No toggle semantics.

4. **And** **Internal Patches INIT** (`internalPatchesInit`):
   - Loads full patch via `InitTemplateLoader::loadPatch` from Settings folder (`PluginIDs::Settings::kInitTemplatesFolderPath` supplier — same pattern as `PatchModuleInitService`).
   - Writes `PatchModel` → APVTS via `ApvtsPatchMapper::bufferToApvts()` with **both** `suppressPatchParameterSysEx_` and `suppressMatrixModParameterSysEx_` true during bulk push.
   - Propagates `InitTemplateLoadResult::infoMessage` to footer (info vs warning — reuse `ModuleActionHandler::propagateInitTemplateFooterMessage` pattern or extract shared helper).
   - **Editor-only reset** — does **not** enqueue patch **0x01** SysEx (STORE is the explicit write-to-synth path per FR-24 / D-044).
   - Works on **all** banks including ROM (FR-23 restricts PASTE/STORE only).

5. **And** **Internal Patches PASTE** (`internalPatchesPaste`):
   - Defensive gate: `limits.isPasteStoreAllowed(currentBank)` — if false → footer warning, return (FR-23, D-023, D-060).
   - If `!clipboardService_->canPasteFullPatch()` → no-op (no footer).
   - On success: `pasteFullPatch(*patchModel_)` → `apvtsToBuffer()` first if needed → suppress hooks → `bufferToApvts()` → clear suppress → enqueue **one** full patch SysEx **0x01** via `MidiManager::sendPatch(patchNumber, packedData)` for current patch slot.
   - Does **not** alter clipboard mode or `internalPatchesPasteEnabled` mirror.

6. **And** **Internal Patches STORE** (`internalPatchesStore`):
   - ROM gate same as PASTE (footer warning, no SysEx).
   - `apvtsPatchMapper_->apvtsToBuffer()` then `sendPatch(currentPatchNumber, patchModel_->data())` — patch number is **within-bank** 0–99; Set Bank must precede if bank differs from synth (handled by selection sync helper when coordinates change before STORE).
   - RAM banks 0–1 only per FR-24 (enforced by `isPasteStoreAllowed` — banks 0–1 allowed, 2–9 ROM blocked).

7. **And** **Computer Patches** action IDs remain **explicit no-op stubs** with comment `Epic 4` — do **not** implement `PatchFileService` here.

8. **And** **COPY** stays in `ModuleActionHandler` (`internalPatchesCopy`) — no regression (Story 7.2).

9. **And** no GUI changes in this story (visual bank lock / red text → Story **7.5**; Internal Patches panel footer-on-hover → Story **7.6**). Core ROM gate + footer on attempted PASTE/STORE is **required** even when GUI buttons are grayed (D-060).

10. **And** unit tests in `Tests/Unit/PatchManagerActionHandlerTests.cpp` cover handler logic without `PluginProcessor`:
    - ROM gate blocks PASTE/STORE with footer properties set.
    - INIT loads template + `bufferToApvts` invoked (recording fake mapper).
    - PASTE calls `pasteFullPatch` + `sendPatch` when allowed.
    - STORE calls `apvtsToBuffer` + `sendPatch` when allowed.
    - Bank select on Matrix-1000 enqueues Set Bank; Matrix-6 path does not.
    - Navigation updates APVTS coordinates via `DeviceMemoryLimits::advancePatch`.
    - Full `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds remain green.

## Tasks / Subtasks

- [x] **MIDI infrastructure — Set Bank / Unlock Bank** (AC: #2, #3)
  - [x] Add `SysExConstants::Opcode::kUnlockBank = 0x0C` if absent
  - [x] `SysExEncoder::encodeSetBank(juce::uint8 bank)` — `F0 10 06 0A <bank> F7` per Oberheim ref
  - [x] `SysExEncoder::encodeUnlockBank()` — `F0 10 06 0C F7`
  - [x] `MidiManager::sendSetBank(int bank)` / `sendUnlockBank()` → `editorPath_.enqueueSysEx`
  - [x] Unit tests in `SysExEncoderTests` or `MidiManagerTests` for message bytes

- [x] **`PatchSelectionMidiSync` helper** (AC: #2, #6)
  - [x] `Source/Core/MIDI/PatchSelectionMidiSync.{h,cpp}` — deps: `DeviceMemoryLimits`, `MidiManager*` (nullable)
  - [x] `syncSelection(int bank, int patch)` — Matrix-1000: Set Bank + PC; M-6/6R: PC only
  - [x] Wire from `PatchManagerActionHandler` after bank select, navigation, lock-on
  - [x] Wire from `PluginProcessor::handlePatchNumberChange` (patch NumberBox state edits) so PC + Set Bank stay consistent outside action events

- [x] **`PatchInitService` — full patch INIT** (AC: #4)
  - [x] `Source/Core/Init/PatchInitService.{h,cpp}` — mirror `PatchModuleInitService` but whole buffer via `InitTemplateLoader::loadPatch`
  - [x] `initFullPatch()` returns `InitTemplateLoadResult`; caller owns suppress flags around `bufferToApvts`
  - [x] Register in `CMakeLists.txt`

- [x] **Extend `PatchManagerActionHandler`** (AC: #1–#7)
  - [x] Expand ctor injection: `PatchModel*`, `ApvtsPatchMapper*`, `ClipboardService*`, `PatchInitService*`, `PatchSelectionMidiSync*` (or `MidiManager*` + limits supplier), footer helper callback
  - [x] Implement `handleInternalPatchInit`, `handleInternalPatchPaste`, `handleInternalPatchStore`
  - [x] Add `propagateRomBlockedFooter()` using new `PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kRomBankPasteStoreFooterMessage` constant
  - [x] Refactor `applyPatchCoordinates` → set APVTS properties then call `syncSelection`
  - [x] Bank select path → update APVTS + `syncSelection`
  - [x] Lock toggle → APVTS + Set Bank or Unlock Bank SysEx
  - [x] Remove `// Story 7.3` stub returns for Internal Patches INIT/PASTE/STORE

- [x] **`PluginProcessor` wiring** (AC: #2, #4)
  - [x] Construct `patchInitService_`, `patchSelectionMidiSync_`
  - [x] Pass new deps into `PatchManagerActionHandler` ctor
  - [x] Extend `ActionExecutionHooks` if matrix-mod suppress needed in handler (reuse existing lambdas)

- [x] **Unit tests** (AC: #10)
  - [x] `Tests/Unit/PatchManagerActionHandlerTests.cpp` — recording fakes (mapper, clipboard, midi, init service)
  - [x] `Tests/Unit/PatchInitServiceTests.cpp` — optional if logic thin; otherwise cover via handler tests
  - [x] Run full `Matrix-Control_Tests`; build VST3 + Standalone

- [x] **Self-review** (AC: #8, #9)
  - [x] No GUI diffs
  - [x] Computer Patches stubs unchanged
  - [x] Module copy/paste/init regression smoke
  - [x] Handlers ≤ Clean Code limits

## Dev Notes

### What Story 7.3 IS — and what it is NOT

| In scope (7.3) | Out of scope (later stories) |
|---|---|
| Complete `PatchManagerActionHandler` INIT/PASTE/STORE + MIDI sync | Computer Patches `PatchFileService` (**Epic 4**) |
| Set Bank **0x0A** / Unlock **0x0C** encode + enqueue | Bank Utility red text / padlock visuals (**7.5**) |
| Core ROM gate + footer on PASTE/STORE attempt | Internal Patches hover tooltip / gray refresh polish (**7.6**) |
| `PatchInitService` + `PatchSelectionMidiSync` | Dirty-patch navigation warning (**Epic 9**) |
| `PatchManagerActionHandlerTests` | `internalPatchesCopy` (**7.2** — `ModuleActionHandler`) |

### Current brownfield state (read before editing)

`PatchManagerActionHandler.cpp` already implements (migrated from Story 7.1):

- `<` / `>` navigation → `DeviceMemoryLimits::advancePatch` → `applyPatchCoordinates` (APVTS only)
- Bank buttons 0–9 → `kSelectedBank` + `kCurrentBankNumber`
- Bank lock toggle → `kBankLock` bool flip
- Matrix-6/6R guard → ignore bank actions when `!hasBankConcept()`

**Still stubbed (lines 43–48):**

```cpp
if (propertyId == InternalPatchesModule::StandaloneWidgets::kInitPatch
    || propertyId == InternalPatchesModule::StandaloneWidgets::kPastePatch
    || propertyId == InternalPatchesModule::StandaloneWidgets::kStorePatch)
{
    return; // Story 7.3
}
```

**Known MIDI gap (Story 8.5 review, deferred to 7.3):** bank changes never enqueue Set Bank **0x0A**; only `handlePatchNumberChange` sends Program Change when patch **state** property changes.

### APVTS property taxonomy — Patch Manager actions

| Property ID | Kind | Handler | Notes |
|---|---|---|---|
| `bankUtilitySelectBank0`…`9` | event | PatchManager | Sets bank + sync MIDI |
| `bankUtilityLockBank` | event | PatchManager | Toggle lock + 0x0A/0x0C |
| `internalPatchesLoadPrevious/Next` | event | PatchManager | Navigate + sync MIDI |
| `internalPatchesInit` | event | PatchManager | Full patch init |
| `internalPatchesPaste` | event | PatchManager | Full patch paste + 0x01 |
| `internalPatchesStore` | event | PatchManager | STORE + 0x01 |
| `internalPatchesCopy` | event | **Module** | Clipboard — do not move |
| `internalPatchesCurrentBankNumber` | **state** | Processor clamp | Display + sync; not action event |
| `internalPatchesCurrentPatchNumber` | **state** | Processor + PC | NumberBox edits |
| `patchManagerSelectedBank` | **state** | Set by handler | Bank Utility mirror |
| `patchManagerBankLock` | **state** | Set by handler | Lock mirror |
| `internalPatchesPasteEnabled` | uiMirror | Processor | From `ClipboardPasteEnabledResolver` |

### Internal Patches INIT — execution flow

Mirror `PatchModuleInitService` but whole buffer:

1. `patchInitService_->initFullPatch()` internally:
   - `initTemplateLoader_.loadPatch(patchModel_, templatesFolder_())`
   - return `InitTemplateLoadResult`
2. Caller (`PatchManagerActionHandler`):
   - `hooks_.setSuppressPatchSysEx(true)` + `setSuppressMatrixModSysEx(true)`
   - `apvtsPatchMapper_->bufferToApvts()`
   - clear suppress flags
   - `propagateInitTemplateFooterMessage(result)`
3. **No** `sendPatch` — user auditions edited patch; STORE writes RAM.

Reference: D-034, D-040, FR-36, `InitTemplateLoader::loadPatch`.

### Internal Patches PASTE — execution flow

1. Read `currentBank` from `internalPatchesCurrentBankNumber`.
2. If `!limits.isPasteStoreAllowed(currentBank)` → `propagateRomBlockedFooter()` → return.
3. If `!clipboardService_->canPasteFullPatch()` → return silently.
4. `apvtsPatchMapper_->apvtsToBuffer()` (ensure model matches APVTS before paste replaces model).
5. `clipboardService_->pasteFullPatch(*patchModel_)`.
6. Suppress both SysEx flags → `bufferToApvts()` → clear suppress.
7. `midiManager_->sendPatch(currentPatchNumber, patchModel_->data())`.

Reference: Story 5.1 `pasteFullPatch`; architecture AD-4 full buffer → **0x01**.

### Internal Patches STORE — execution flow

1. ROM gate (same as paste).
2. `apvtsPatchMapper_->apvtsToBuffer()`.
3. `sendPatch(currentPatchNumber, patchModel_->data())`.

Ensure synth bank matches APVTS bank before STORE (call `syncSelection` if needed, or document that user must select bank first — prefer explicit `syncSelection(bank, patch)` at start of STORE handler).

### ROM gating (FR-23, D-023, D-060)

| Bank (Matrix-1000) | PASTE/STORE |
|---|---|
| 0–1 RAM | Allowed |
| 2–9 ROM | Blocked — Core footer warning |

Use `DeviceMemoryLimits::isPasteStoreAllowed(bank)` — **never** duplicate `bank >= 2` magic numbers.

Add footer string constant (English):

```cpp
// PluginDisplayNames.h — InternalPatchesModule namespace
constexpr const char* kRomBankPasteStoreFooterMessage =
    "Paste and Store are not available on ROM banks 2–9.";
```

Set `uiMessageText` + `uiMessageSeverity` = `"warning"` (same pattern as init footer in `ModuleActionHandler`).

GUI already grays buttons via `InternalPatchesPanel::updatePasteStoreEnabled` (Story 5.2) — Core gate is defense in depth.

### Set Bank / Unlock Bank — Oberheim semantics

From `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md`:

| Opcode | Message | Effect |
|---|---|---|
| **0x0A** | `F0 10 06 0A <bank> F7` | Select bank 0–9; **enables bank lock on synth** |
| **0x0C** | `F0 10 06 0C F7` | Disables bank lock |

When user clicks bank button: enqueue **0x0A** after APVTS update. When user unlocks via BANK LOCK button: enqueue **0x0C**. When user locks: enqueue **0x0A** for current bank.

**Matrix-6/6R:** skip both opcodes entirely (Story 8.5 AC).

### PatchSelectionMidiSync — shared processor/handler helper

Extract to avoid duplicating MIDI rules:

```cpp
void PatchSelectionMidiSync::syncSelection(int bank, int patch, const DeviceMemoryLimits& limits)
{
    if (midiManager_ == nullptr) return;
    if (limits.hasBankConcept())
        midiManager_->sendSetBank(bank);
    midiManager_->sendProgramChange(patch);
}
```

Call sites:

| Trigger | Caller |
|---|---|
| Bank button / navigation / lock-on | `PatchManagerActionHandler` |
| Patch NumberBox state edit | `PluginProcessor::handlePatchNumberChange` |

For NumberBox-only patch change when bank unchanged: sending Set Bank again is acceptable (idempotent on hardware) — keeps one code path. Alternative: track last synced bank in helper; prefer simplicity unless hardware tests show issues.

### PatchManagerActionHandler — updated dependency injection

| Dependency | Used for |
|---|---|
| `juce::AudioProcessorValueTreeState&` | coordinates, footer |
| `DeviceMemoryLimitsSupplier` | limits (existing) |
| `PatchModel*` | paste/store buffer |
| `ApvtsPatchMapper*` | apvtsToBuffer / bufferToApvts |
| `ClipboardService*` | pasteFullPatch / canPasteFullPatch |
| `PatchInitService*` | internal INIT |
| `PatchSelectionMidiSync*` | bank/patch MIDI |
| `ActionExecutionHooks` | suppress flags during bulk APVTS push |
| `std::function<void(const InitTemplateLoadResult&)>` | footer for INIT (optional — may call shared static helper) |

Construct new services in `PluginProcessor` after `initTemplateLoader_` (same zone as `patchModuleInitService_`).

### Architecture compliance

- Core **must not** include GUI headers (AD-1).
- `PluginProcessor` remains composition root (AD-2).
- Init services in `Source/Core/Init/`; MIDI helpers in `Source/Core/MIDI/`.
- APVTS identifiers from `PluginIDs.h` only.
- SysEx enqueue only via `MidiManager` / `EditorPath` — never from GUI.
- Full patch replace → **0x01** only (AD-4); INIT is local reset unless user STOREs.
- One primary class per `.h/.cpp` pair.

### File structure

```
Source/Core/Init/
└── PatchInitService.h/.cpp                 (NEW)

Source/Core/MIDI/
├── PatchSelectionMidiSync.h/.cpp           (NEW)
├── SysEx/SysExEncoder.h/.cpp               (UPDATE — encodeSetBank, encodeUnlockBank)
├── SysEx/SysExConstants.h                  (UPDATE — kUnlockBank if needed)
├── MidiManager.h/.cpp                      (UPDATE — sendSetBank, sendUnlockBank)

Source/Core/Actions/
├── PatchManagerActionHandler.h/.cpp        (UPDATE — full implementation)

Source/Core/
├── PluginProcessor.h/.cpp                  (UPDATE — wiring, handlePatchNumberChange)

Source/Shared/Definitions/
└── PluginDisplayNames.h                    (UPDATE — ROM footer message)

Tests/Unit/
├── PatchManagerActionHandlerTests.cpp      (NEW)
└── PatchInitServiceTests.cpp               (NEW — optional)
```

Update `CMakeLists.txt` for new `.cpp` files in plugin + `Matrix-Control_Tests`.

### Testing requirements

Follow `ModuleActionHandlerTests` / `DeviceMemoryLimitsTests` patterns — `juce::UnitTest`, recording fakes, no `PluginProcessor`.

| Test | Assert |
|---|---|
| `initFullPatch_loadsTemplate` | `bufferToApvts` called; footer set on missing file |
| `paste_romBank_blocked` | No paste/SysEx; warning footer |
| `paste_ramBank_success` | `pasteFullPatch` + `sendPatch` invoked |
| `store_romBank_blocked` | No `sendPatch`; warning footer |
| `store_ramBank_success` | `apvtsToBuffer` + `sendPatch` |
| `bankSelect_matrix1000_setBank` | Fake midi records 0x0A with bank index |
| `bankSelect_matrix6_noSetBank` | No 0x0A; PC only when patch changes |
| `navigation_wrapsAcrossBanks` | Coordinates match `advancePatch` when unlocked |
| `lockOff_sendsUnlock` | 0x0C enqueued on Matrix-1000 |

Manual smoke (dev agent):

1. Matrix-1000: select bank 3 → verify Set Bank on MIDI monitor; patch NumberBox → Program Change.
2. INIT on bank 5 (ROM) → editor resets; footer info if template missing.
3. COPY patch → PASTE on bank 0 → patch data replaces editor + 0x01 sent.
4. STORE on bank 2 → footer warning, no SysEx.
5. Matrix-6 device type → bank buttons grayed (GUI); handler ignores bank IDs; patch 99→0 wrap.

### Previous story intelligence (7.2)

- `ActionExecutionHooks` includes `setSuppressPatchSysEx` — reuse for `bufferToApvts` bulk push (matrix mod suppress too).
- `internalPatchesCopy` stays in `ModuleActionHandler` — do not relocate.
- `propagateInitTemplateFooterMessage` in `ModuleActionHandler` — extract to shared `InitTemplateFooter.h` or duplicate minimally; prefer small shared helper in `Core/Init/` to avoid handler cross-dependency.
- Review deferred test debt from 7.2 (suppress-hook order) — apply to 7.3 tests where practical.

### Previous story intelligence (7.1)

- `PatchManagerActionHandler` ctor today: only `apvts` + `deviceMemoryLimits` supplier.
- Review deferred: bank select without device clamp — preserve existing behaviour; optional clamp in 7.3 using `jlimit(min,max,bankIndex)`.
- Computer Patches IDs registered — keep no-op bodies.

### Git intelligence (recent commits)

| Commit | Relevance |
|---|---|
| `a1e1bbc` | Story 7.2 done — handler patterns, suppress hooks, tests |
| `577cd83` | ModuleActionHandler paste/init wiring — mirror for PatchManager |
| `0c7a3c2` | Story 7.1 — PatchManager scaffold + navigation migration |
| `8-5` (in branch) | `DeviceMemoryLimits`, Matrix-6 no bank concept |

### Latest tech / framework notes

- **JUCE 8.0.12** — `bufferToApvts` / `setValueNotifyingHost` on message thread only.
- **C++17** — no new third-party libraries.
- Action properties remain timestamp `juce::var` int64 — handler ignores `newValue` body.
- SysEx **0x0A** / **0x0C** are Matrix-1000-only — guard with `limits.hasBankConcept()`.

### Project context reference

- `project-context.md` — C++17, builds under `Builds/`, English source only, no GUI in Core.
- `CONVENTIONS.md` — authoritative style; Clean Code function length.
- Oberheim ref: `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` § 0AH, 0CH.

### References

- [epics.md — Story 7.3, FR-19–FR-24]
- [prd.md — FR-19–FR-24, FR-36]
- [.decision-log.md — D-023, D-034, D-040, D-044, D-060]
- [architecture.md — AD-4, AD-5, PatchManagerActionHandler]
- [7-1-actiondispatcher-and-handler-interfaces.md — handler scaffold, registry]
- [7-2-moduleactionhandler-i-c-p-and-matrix-mod-init.md — suppress hooks, init service pattern]
- [5-1-clipboardservice-compatibility-matrix.md — pasteFullPatch semantics]
- [8-5-matrix-6-6r-patch-memory-limits.md — Set Bank deferral, DeviceMemoryLimits]

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Debug Log References

### Completion Notes List

- Implemented Set Bank (0x0A) / Unlock Bank (0x0C) SysEx encode + MidiManager enqueue; SysExEncoderTests cover byte layout.
- Added `PatchSelectionMidiSync` (Matrix-1000: Set Bank + PC; Matrix-6/6R: PC only) wired in handler + `handlePatchNumberChange`.
- Added `PatchInitService` + shared `InitTemplateFooter`; Internal Patches INIT/PASTE/STORE fully implemented with ROM gate footer.
- `PatchManagerActionHandlerTests` (9 cases) + full `Matrix-Control_Tests` green; VST3 + Standalone Debug build green.
- Story 7-3b landed (UNLOCK-only, ID rename, conditional Set Bank in `syncSelection`); manual smoke Matrix-1000 UAT pass (Guillaume, 2026-06-19).

### File List

- Source/Core/Actions/PatchManagerActionHandler.h
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/Core/Actions/ModuleActionHandler.cpp
- Source/Core/Init/InitTemplateFooter.h
- Source/Core/Init/PatchInitService.h
- Source/Core/Init/PatchInitService.cpp
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/MIDI/PatchSelectionMidiSync.h
- Source/Core/MIDI/PatchSelectionMidiSync.cpp
- Source/Core/MIDI/SysEx/SysExConstants.h
- Source/Core/MIDI/SysEx/SysExEncoder.h
- Source/Core/MIDI/SysEx/SysExEncoder.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Source/Shared/Definitions/PluginDisplayNames.h
- Tests/Unit/PatchManagerActionHandlerTests.cpp
- Tests/Unit/SysExEncoderTests.cpp
- CMakeLists.txt
- _bmad-output/implementation-artifacts/sprint-status.yaml

## Change Log

- 2026-06-18: Story 7.3 created — PatchManagerActionHandler bank/internal comprehensive developer guide.
- 2026-06-18: Story 7.3 implemented — bank/internal patch actions, MIDI sync, INIT/PASTE/STORE, unit tests.
- 2026-06-19: Story 7-3b merged (UNLOCK semantics, ID rename, syncSelection fix); manual UAT Matrix-1000 pass; status → done.
