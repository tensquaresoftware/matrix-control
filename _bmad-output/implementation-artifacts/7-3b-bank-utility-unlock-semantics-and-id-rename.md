---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.3b — Bank Utility UNLOCK Semantics and ID Rename
author: BMad Agent
status: done
baseline_commit: uncommitted (Story 7.3 branch work)
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/sprint-change-proposal-2026-06-19.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - Documentation/Development/Plans/2026/06/2026-06-18-Correct-Course-Bank-Utility-UNLOCK-Handoff.md
  - implementation-artifacts/7-3-patchmanageractionhandler-bank-and-internal.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 7.3b: Bank Utility UNLOCK Semantics and ID Rename

Status: done

> **ERRATA (2026-06-19 — Correct Course D-023-R2):** AC #1–3 (lock mirror, cross-bank navigation, `patchManagerBanksLocked` semantics) are **superseded** by Story **7-3c**. AC #4 (ID renames) remains valid. See `Documentation/Development/Plans/2026/06/2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md` and `sprint-change-proposal-2026-06-19-bank-unlock-simplify.md`.


## Story

As a sound designer,
I want the UNLOCK button to send Unlock Bank SysEx only,
so that bank lock behaviour matches Oberheim spec (FR-20 corrected).

## Acceptance Criteria

1. **Given** Story 7.3 Core handler scaffold (INIT/PASTE/STORE, `PatchSelectionMidiSync`, Set Bank / Unlock encode) **When** user clicks **UNLOCK** **Then** `PatchManagerActionHandler` sends **only** Unlock Bank SysEx **0x0C** via `PatchSelectionMidiSync::sendUnlockBank` / `MidiManager::sendUnlockBank`; sets APVTS state `patchManagerBanksLocked` to `false`; does **not** send **0x0A**; does **not** toggle lock state; does **not** change selected bank or patch coordinates.

2. **And** bank select **0–9** and any navigation / `PluginProcessor::handlePatchNumberChange` path that enqueues Set Bank **0x0A** sets `patchManagerBanksLocked` to **`true`** (Oberheim side-effect: Set Bank enables hardware lock). Matrix-6/6R paths that skip Set Bank must **not** write `patchManagerBanksLocked`.

3. **And** navigation wrap continues to read `patchManagerBanksLocked` for `DeviceMemoryLimits::advancePatch(..., banksLocked)` (D-023): locked → wrap within bank; unlocked → wrap across banks. No behaviour change to wrap math — only correct lock derivation.

4. **And** full ID rename aligned with D-022-R1/R2 (grep-clean in `Source/`):
   - Action: `bankUtilityLockBank` → `bankUtilityUnlockBank`; C++ `kLockBank` → `kUnlockBank`; display `"BANK LOCK"` → `"UNLOCK"`.
   - State: `patchManagerBankLock` → `patchManagerBanksLocked`; C++ `kBankLock` → `kBanksLocked`.
   - GUI symbols: `lockBankButton_` → `unlockBankButton_`; `patchManagerLockBankWidth` → `patchManagerUnlockBankWidth`; `kPatchManagerLockBank` → `kPatchManagerUnlockBank`.
   - **Do NOT rename:** `miscBankLockEnable`, `kBankLockEnable`, `SysExConstants::Opcode::kUnlockBank`, `encodeUnlockBank`, `sendUnlockBank`, `sendSetBank`, `encodeSetBank`.

5. **And** unit tests updated in `PatchManagerActionHandlerTests.cpp` (and any test referencing old IDs): unlock sends **0x0C** only; `banksLocked` false after unlock; `banksLocked` true after bank select on Matrix-1000; Matrix-6 bank/unlock actions remain no-ops. Remove toggle semantics tests. Full `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds green.

6. **And** grep verification passes (zero stale anchors in `Source/`):

   ```bash
   rg 'bankUtilityLockBank|kLockBank|patchManagerBankLock|lockBankButton_|patchManagerLockBank|kPatchManagerLockBank' Source/
   ```

7. **And** no visual UX in this story — red dot on Internal Patches bank NumberBox, active bank red text, padlock removal polish → Story **7.5**. GUI renames here are identifier/display alignment only.

8. **And** Story **7.3** remains `review` until this story merges and passes review; then mark 7.3 `done`.

## Tasks / Subtasks

- [x] **Remove BANK LOCK toggle — implement UNLOCK-only handler** (AC: #1, #3)
  - [x] Delete toggle block in `PatchManagerActionHandler.cpp` (lines 112–127: flip `kBankLock`, conditional `sendSetBank`/`sendUnlockBank`)
  - [x] Add private `handleUnlockBank(const DeviceMemoryLimits& limits)` — guard `hasBankConcept()`, `sendUnlockBank`, set `kBanksLocked = false` only
  - [x] Route action on `kUnlockBank` (`bankUtilityUnlockBank`); update Matrix-6 guard to use new action ID
  - [x] Add private `markBanksLockedInApvts()` (or equivalent) — set `kBanksLocked = true`
  - [x] Call `markBanksLockedInApvts()` after every Matrix-1000 path that sends **0x0A**: bank button select, `applyPatchCoordinates`, STORE pre-sync (if `syncSelection` used)
  - [x] Call `markBanksLockedInApvts()` from `PluginProcessor::handlePatchNumberChange` when `syncSelection` runs and `limits.hasBankConcept()` (patch NumberBox bank change)

- [x] **Shared definitions rename** (AC: #4)
  - [x] `PluginIDs.h` — `kUnlockBank`, `kBanksLocked`, APVTS strings
  - [x] `PluginDisplayNames.h` — `kUnlockBank = "UNLOCK"`
  - [x] `PluginDescriptorsPatchManager.cpp` — widget id + display name

- [x] **Core wiring rename** (AC: #4)
  - [x] `ActionPropertyRegistry.cpp`
  - [x] `MidiManager.cpp` — init default property name `kBanksLocked`
  - [x] `PatchManagerActionHandler.cpp` — all readers/writers use `kBanksLocked`; navigation local `banksLocked` variable naming optional cleanup

- [x] **GUI identifier rename (no new visuals)** (AC: #4, #7)
  - [x] `BankUtilityPanel.h/.cpp` — `unlockBankButton_`, click routes to `kUnlockBank`
  - [x] `WidgetDimensions.h`, `DesignAtoms.h`, `DimensionFactory.cpp`, `WidgetDimensionRegistry.cpp`

- [x] **Tests** (AC: #5, #6)
  - [x] `PatchManagerActionHandlerTests.cpp` — rename `lockOff_sendsUnlock` → `unlockBank_sends0CHOnly`; assert bank unchanged; add `bankSelect_setsBanksLockedTrue`
  - [x] `WidgetDimensionRegistryTests.cpp` — update widget id constant
  - [x] `ActionDispatcherTests.cpp` — if `kLockBank` referenced
  - [x] Run full `Matrix-Control_Tests`; build VST3 + Standalone Debug

- [x] **Verification** (AC: #6)
  - [x] Run grep anchors; fix any stragglers in `Source/`
  - [x] Confirm `miscBankLockEnable` / Master Edit BANK LOCK untouched

## Dev Notes

### What Story 7-3b IS — and what it is NOT

| In scope (7-3b) | Out of scope |
|---|---|
| Fix incorrect BANK LOCK **toggle** from Story 7.3 | Red dot / red bank text visuals (**7.5**) |
| UNLOCK-only handler + derived `patchManagerBanksLocked` | Bidirectional sync `miscBankLockEnable` ↔ Bank Utility (deferred) |
| Full APVTS/GUI/test ID rename | Computer Patches / Epic 4 |
| `banksLocked = true` on every **0x0A** path | APVTS migration shim for saved sessions (pre-release — clean rename OK) |
| Grep-clean `Source/` | Planning doc grep in `_bmad-output/` (optional annotation only) |

### Correct behaviour (user mental model)

| User action | MIDI | `patchManagerBanksLocked` |
|---|---|---|
| Click bank **N** (0–9) | **0x0A** + Program Change | `true` |
| Click **UNLOCK** | **0x0C** only | `false` |
| Navigate `<` `>` (unlocked, crosses bank) | **0x0A** + PC when bank changes | `true` on bank change |
| Navigate while locked | PC only (same bank wrap) | unchanged (`true`) |
| Patch NumberBox edit (bank changes) | **0x0A** + PC (M-1000) | `true` |
| Matrix-6/6R bank or UNLOCK click | none | no-op / no property writes |

Reference: Oberheim Set Bank **0AH** locks as side-effect; only **0CH** unlocks. See `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` §0AH, §0CH.

### Current wrong implementation (fix this)

Story 7.3 shipped a **toggle** at `PatchManagerActionHandler.cpp` lines 112–127:

```cpp
// WRONG — remove entirely
const bool newLock = !currentLock;
apvts_.state.setProperty(...kBankLock, newLock, ...);
if (newLock)
    patchSelectionMidiSync_->sendSetBank(...);  // lock-on sends 0AH
else
    patchSelectionMidiSync_->sendUnlockBank(...);
```

**Replace with:**

```cpp
void PatchManagerActionHandler::handleUnlockBank(const DeviceMemoryLimits& limits)
{
    if (!limits.hasBankConcept())
        return;

    if (patchSelectionMidiSync_ != nullptr)
        patchSelectionMidiSync_->sendUnlockBank(limits);

    apvts_.state.setProperty(BankUtilityModule::StateProperties::kBanksLocked, false, nullptr);
}
```

Bank select path (after `syncSelection`) must call `markBanksLockedInApvts()`.

### Centralizing `banksLocked = true`

Every **0x0A** enqueue must mirror lock ON in APVTS. Call sites today:

| Location | Method | Sets lock? |
|---|---|---|
| `PatchManagerActionHandler` | bank button → `syncSelection` | **Yes** — add after call |
| `PatchManagerActionHandler` | navigation → `applyPatchCoordinates` → `syncSelection` | **Yes** when M-1000 |
| `PatchManagerActionHandler` | STORE → `syncSelection` | **Yes** when M-1000 |
| `PluginProcessor` | `handlePatchNumberChange` → `syncSelection` | **Yes** when M-1000 |

**Do not** inject APVTS into `PatchSelectionMidiSync` unless you add an optional callback — keep MIDI helper free of GUI/APVTS if a small handler-side helper suffices. Duplicating one-line `setProperty(kBanksLocked, true)` at call sites is acceptable; prefer private `markBanksLockedInApvts()` on handler + same one-liner in `PluginProcessor` to avoid new cross-module types.

**Do not** set `kBanksLocked = true` in `handleUnlockBank`.

### ID rename inventory (mandatory)

#### Bank Utility action (button)

| Current | Target | Primary files |
|---|---|---|
| `"bankUtilityLockBank"` | `"bankUtilityUnlockBank"` | `PluginIDs.h`, descriptors, registry, tests |
| `kLockBank` | `kUnlockBank` | `PluginIDs.h` + all references |
| `"BANK LOCK"` | `"UNLOCK"` | `PluginDisplayNames.h` |
| `lockBankButton_` | `unlockBankButton_` | `BankUtilityPanel.h/.cpp` |
| `patchManagerLockBankWidth` | `patchManagerUnlockBankWidth` | `WidgetDimensions.h`, `DimensionFactory.cpp` |
| `kPatchManagerLockBank` | `kPatchManagerUnlockBank` | `DesignAtoms.h`, `WidgetDimensionRegistry.cpp`, tests |

#### APVTS lock state (not the button)

| Current | Target |
|---|---|
| `"patchManagerBankLock"` | `"patchManagerBanksLocked"` |
| `kBankLock` (StateProperties) | `kBanksLocked` |

#### Do NOT rename

| Symbol | Reason |
|---|---|
| `miscBankLockEnable` / `kBankLockEnable` | Master Edit byte 165 — separate domain |
| `PluginDisplayNames::MasterEditSection::...::kBankLockEnable` | Master parameter label **BANK LOCK** |
| `SysExConstants::Opcode::kUnlockBank` | Oberheim opcode 0CH |
| `MidiManager::sendUnlockBank()` | Already correct verb |

**Namespace collision note:** After rename, `PluginIDs::...::kUnlockBank` (widget action) and `SysExConstants::Opcode::kUnlockBank` (opcode byte) share the English word in different namespaces — **acceptable**; use fully qualified names in Core when ambiguous.

### APVTS property taxonomy (post-rename)

| Property ID | Kind | Handler | Notes |
|---|---|---|---|
| `bankUtilitySelectBank0`…`9` | event | PatchManager | Sets bank + sync MIDI + `banksLocked=true` |
| `bankUtilityUnlockBank` | event | PatchManager | **0x0C** only + `banksLocked=false` |
| `internalPatchesLoadPrevious/Next` | event | PatchManager | Navigate; lock state per sync path |
| `patchManagerSelectedBank` | state | Handler | Bank Utility mirror |
| `patchManagerBanksLocked` | state | Handler + Processor | Derived lock mirror — **not** toggled by button |

### Architecture compliance

- Core **must not** include GUI headers (AD-1).
- APVTS identifiers from `PluginIDs.h` only; display strings from `PluginDisplayNames.h`.
- SysEx enqueue only via `MidiManager` / editor path — never from GUI.
- No new third-party libraries; C++17; JUCE 8.0.12.
- Breaking APVTS property rename (`patchManagerBankLock` → `patchManagerBanksLocked`) — plugin pre-release; no migration shim unless Guillaume requests.

### File structure

```
Source/Shared/Definitions/
├── PluginIDs.h                              (UPDATE — kUnlockBank, kBanksLocked)
├── PluginDisplayNames.h                     (UPDATE — UNLOCK label)
└── PluginDescriptorsPatchManager.cpp        (UPDATE)

Source/Core/Actions/
├── PatchManagerActionHandler.h/.cpp         (UPDATE — handleUnlockBank, markBanksLocked)
└── ActionPropertyRegistry.cpp               (UPDATE)

Source/Core/MIDI/
└── MidiManager.cpp                          (UPDATE — init property name)

Source/Core/
└── PluginProcessor.cpp                      (UPDATE — handlePatchNumberChange lock mirror)

Source/GUI/Panels/.../BankUtilityPanel.h/.cpp (UPDATE — rename only)
Source/GUI/Layout/WidgetDimensions.h          (UPDATE)
Source/GUI/Layout/Design/DesignAtoms.h        (UPDATE)
Source/GUI/Factories/DimensionFactory.cpp     (UPDATE)
Source/GUI/Layout/WidgetDimensionRegistry.cpp (UPDATE)

Tests/Unit/
├── PatchManagerActionHandlerTests.cpp       (UPDATE)
├── WidgetDimensionRegistryTests.cpp         (UPDATE)
└── ActionDispatcherTests.cpp                (UPDATE if needed)
```

No `CMakeLists.txt` changes expected unless new helper `.cpp` added (prefer inline private methods).

### Testing requirements

Follow existing `PatchManagerActionHandlerTests` patterns — `juce::UnitTest`, real `MidiOutboundQueue`, no `PluginProcessor`.

| Test | Assert |
|---|---|
| `unlockBank_sends0CHOnly` | **0x0C** enqueued; no **0x0A**; `kBanksLocked == false`; bank/patch unchanged |
| `bankSelect_setsBanksLockedTrue` | After bank 3 select on M-1000: `kBanksLocked == true`; **0x0A** with bank 3 |
| `bankSelect_matrix6_noSetBank` | (existing) unchanged behaviour |
| `navigation_wrapsAcrossBanks` | (existing) uses `banksLocked=false` init — still valid |
| `navigation_locked_wrapsWithinBank` | Optional: init `banksLocked=true`, patch 99→0 stays same bank |

Update `initializePatchManagerState` helper to use `kBanksLocked`.

Manual smoke (post-implementation):

1. Matrix-1000 + MIDI monitor: bank **3** → `F0 10 06 0A 03 F7` + PC.
2. **UNLOCK** → `F0 10 06 0C F7` only; bank still 3.
3. `<` `>` after UNLOCK wraps across banks; **0x0A** when bank changes.
4. Matrix-6: bank + UNLOCK grayed; no **0x0A**/**0x0C**.

### Previous story intelligence (7.3)

- Story 7.3 implemented INIT/PASTE/STORE, `PatchSelectionMidiSync`, Set Bank/Unlock encode — **reuse**; do not reimplement MIDI infrastructure.
- **Errata:** AC #3 toggle semantics were wrong; this story is the fix. Do not mark 7.3 `done` until 7-3b review passes.
- `PatchManagerActionHandlerTests` has 9 cases including `lockOff_sendsUnlock` — rename and extend, do not delete coverage.
- Review deferred from 7.3: none blocking 7-3b except the toggle bug itself.

### Git intelligence (branch state)

Uncommitted work on branch includes Story 7.3 implementation files — 7-3b edits apply on top:

- `PatchManagerActionHandler.cpp/.h`
- `PatchSelectionMidiSync.cpp/.h`
- `MidiManager.cpp/.h`
- `PatchManagerActionHandlerTests.cpp`
- `PluginIDs.h`, `PluginDisplayNames.h`

Recent commits on mainline: 7.2 handler patterns (`a1e1bbc`), 7.1 dispatcher (`0c7a3c2`).

### Latest tech / framework notes

- **JUCE 8.0.12** — APVTS `setProperty` on message thread only.
- SysEx **0x0A** / **0x0C** Matrix-1000-only — always guard with `limits.hasBankConcept()`.
- U-IDs story (`u-ids-widget-id-harmonization`) renamed `unlockBank` → `bankUtilityLockBank` — **this story reverts that decision** per D-022-R1; do not treat U-IDs doc as authoritative for Bank Utility naming.

### Project context reference

- `project-context.md` — C++17, builds under `Builds/`, English source only, no GUI in Core.
- `CONVENTIONS.md` — authoritative style.
- Correct Course handoff: `Documentation/Development/Plans/2026/06/2026-06-18-Correct-Course-Bank-Utility-UNLOCK-Handoff.md`
- Sprint Change Proposal: `_bmad-output/planning-artifacts/sprint-change-proposal-2026-06-19.md`

### References

- [epics.md — Story 7-3b, FR-20]
- [sprint-change-proposal-2026-06-19.md — D-022-R1/R2/R3, rename table]
- [.decision-log.md — D-022-R1, D-022-R2, D-023]
- [7-3-patchmanageractionhandler-bank-and-internal.md — errata, MIDI infra already done]
- [Correct-Course-Bank-Utility-UNLOCK-Handoff.md — grep anchors, test plan]

## Dev Agent Record

### Agent Model Used

Auto (Cursor Agent)

### Debug Log References

- Fixed compile error: `handleUnlockBank` used `BankUtilityModule` outside `using namespace` scope — qualified with full `PluginIDs::PatchManagerSection::...` path.

### Completion Notes List

- Replaced Story 7.3 BANK LOCK toggle with UNLOCK-only handler: `handleUnlockBank` sends **0x0C** only and sets `patchManagerBanksLocked = false`; no toggle, no **0x0A** on unlock.
- Added `markBanksLockedInApvts()`; called after bank select, navigation (`applyPatchCoordinates`), STORE sync, and `PluginProcessor::handlePatchNumberChange` sync on Matrix-1000.
- Full ID rename per D-022-R1/R2: `bankUtilityUnlockBank`, `kUnlockBank`, `patchManagerBanksLocked`, `kBanksLocked`, `unlockBankButton_`, dimension/registry symbols. Master Edit `miscBankLockEnable` untouched.
- Tests: `unlockBank_sends0CHOnly`, `bankSelect_setsBanksLockedTrue`; grep-clean `Source/` (word-boundary check). Full `Matrix-Control_Tests` green; VST3 + Standalone Debug build green.

### File List

- Source/Core/Actions/ActionPropertyRegistry.cpp
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/Core/Actions/PatchManagerActionHandler.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/PluginProcessor.cpp
- Source/GUI/Factories/DimensionFactory.cpp
- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Layout/WidgetDimensionRegistry.cpp
- Source/GUI/Layout/WidgetDimensions.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.h
- Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginIDs.h
- Tests/Unit/PatchManagerActionHandlerTests.cpp
- Tests/Unit/WidgetDimensionRegistryTests.cpp
- _bmad-output/implementation-artifacts/sprint-status.yaml

## Change Log

- 2026-06-19: Story 7-3b created — UNLOCK semantics fix + full ID rename developer guide (Correct Course D-022-R1).
- 2026-06-19: Code review — `syncSelection` Set Bank on bank change only; tests Matrix-6 + locked navigation; duplicate MIDI suppress hook.
- 2026-06-19: **Manual UAT (Guillaume, Matrix-1000):** smoke 7-3b validated — blocks A/B/C pass; D4/D5 clarified; hardware lock after UNLOCK+STORE confirmed on synth display.
- 2026-06-19: **Startup policy (post-UAT):** Internal Patches always reset to bank 0 / patch 00 on instance start and after APVTS restore (standalone + DAW); no MIDI until user navigates. `clearSyncedBankState()` replaces session `resetLastSyncedBank` at boot. Spec: `spec-internal-patch-startup-coordinates.md`.

### Review Findings

- [x] [Review][Decision] Navigation verrouillée envoie toujours Set Bank (0x0A) — **Résolu (option 1)** : `syncSelection` n’envoie 0x0A que si `forceSetBank` ou banque changée (`lastSyncedBank_`) ; `markBanksLockedInApvts` seulement quand 0x0A envoyé ; suppress listener pour éviter MIDI dupliqué sur navigation.

- [x] [Review][Patch] Tests Matrix-6 UNLOCK no-op manquants [`Tests/Unit/PatchManagerActionHandlerTests.cpp`] — Ajouté `unlockBank_matrix6_noOp`.

- [x] [Review][Patch] Test bank select Matrix-6 ne vérifie pas `kBanksLocked` [`Tests/Unit/PatchManagerActionHandlerTests.cpp:291-301`] — Assertion `kBanksLocked` false ajoutée.

- [x] [Review][Patch] Fichiers sources/tests non suivis par git — `git add` appliqué pour `PatchManagerActionHandlerTests.cpp`, `PatchSelectionMidiSync.cpp/.h`, `PatchInitService.*`, `InitTemplateFooter.h`.

- [x] [Review][Defer] MIDI dupliqué sur navigation prev/next [`PatchManagerActionHandler.cpp:242-258` + `PluginProcessor.cpp:1220-1221`] — `applyPatchCoordinates` appelle `syncSelection`, puis `setProperty(kCurrentPatchNumber)` déclenche `valueTreePropertyChanged` → `handlePatchNumberChange` → second `syncSelection`. Dette architecture 7.3 ; 7-3b ajoute seulement `markBanksLocked` (idempotent). — deferred, pre-existing

- [x] [Review][Defer] Renommage APVTS `patchManagerBankLock` → `patchManagerBanksLocked` sans migration [`PluginIDs.h`, `MidiManager.cpp`] — sessions sauvegardées perdent l’état lock ; spec 7-3b accepte explicitement l’absence de shim pre-release. — deferred, pre-existing

- [x] [Review][Defer] `kSelectedBank` non mis à jour sur navigation cross-banque [`PatchManagerActionHandler.cpp:242-252`] — seul `kCurrentBankNumber` est mis à jour ; `patchManagerSelectedBank` reste sur l’ancienne banque jusqu’à un clic bouton banque. Impact UI Bank Utility → story 7.5. — deferred, pre-existing

- [x] [Review][Defer] Flags suppress SysEx sans RAII sur exception [`PatchManagerActionHandler.cpp:139-190`] — même pattern que stories 7.1/7.2 ; hors périmètre 7-3b. — deferred, pre-existing
