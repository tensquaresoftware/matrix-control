---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.3c — Bank Utility UNLOCK Simplify (MIDI-only)
author: BMad Agent
status: review
baseline_commit: uncommitted (post-7-3b investigation WIP)
sources:
  - planning-artifacts/sprint-change-proposal-2026-06-19-bank-unlock-simplify.md
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - Documentation/Development/Plans/2026/06/2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md
  - implementation-artifacts/7-3b-bank-utility-unlock-semantics-and-id-rename.md
  - implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 7.3c: Bank Utility UNLOCK Simplify (MIDI-only)

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want UNLOCK to send remote SysEx only and patch navigation to stay within the current bank,
so that plugin behaviour matches hardware limits without false lock/cross-bank navigation state (FR-20, FR-22 corrected).

## Acceptance Criteria

1. **Given** Matrix-1000/1000R **When** user clicks **UNLOCK** **Then** `PatchManagerActionHandler::handleUnlockBank` sends **only** Unlock Bank SysEx **0x0C** via `PatchSelectionMidiSync::sendUnlockBank`; sets display lock indicator `patchManagerBanksLocked` to **`false`** (dot off); does **not** change `kCurrentBankNumber`, `kCurrentPatchNumber`, or `kSelectedBank`; does **not** call `clearSyncedBankState()`.

2. **When** user selects bank **0–9** **Then** behaviour unchanged from 7-3b: updates bank APVTS + `syncSelection(..., forceSetBank=true)` + Program Change; sets lock indicator **`true`** (dot on).

3. **When** any plugin path sends Set Bank (`0AH`) — including **cold-start first sync** (`!hasLastSyncedBank_`) and `PluginProcessor::handlePatchNumberChange` when `setBankSent` **Then** lock indicator set **`true`** (dot on). Indicator is **display-only** — must not affect navigation math.

4. **When** user navigates with Prev/Next or patch NumberBox **Then** `DeviceMemoryLimits::advancePatch` wraps **within current bank only** (patch 99 → 0, patch 0 → 99); bank **never** changes via navigation; lock indicator **unchanged** by wrap alone.

5. **And** navigation code (`PatchManagerActionHandler` Prev/Next, `DeviceMemoryLimits::advancePatch`) **must not read** `kBanksLocked`. Remove `bankLocked` parameter from `advancePatch` signature.

6. **And** revert investigation follow-up #4: `PatchSelectionMidiSync::sendUnlockBank` **must not** call `clearSyncedBankState()`.

7. **And** `InternalPatchesPanel::refreshBankLockIndicator` **kept** — dot on Current Bank NumberBox follows `kBanksLocked` (D-023a-R3). GUI wiring may be verified in 7.5; do not remove dot logic in this story unless already broken.

8. **And** unit tests in `PatchManagerActionHandlerTests.cpp`:
   - **Remove:** `navigation_wrapsAcrossBanks`, `unlock_thenNextAtPatch99_sendsSetBank1`, `unlock_onBank1_nextSameBank_resyncsSetBank`
   - **Keep/update:** `unlockBank_sends0CHOnly`, `bankSelect_setsBanksLockedTrue`, `navigation_locked_withinBank_noSetBank` (rename optional → `navigation_withinBank_noSetBank`), `locked_at99_fourNext_staysBank0` (rename optional → `at99_fourNext_staysBank0`)
   - **Add:** `unlock_doesNotChangeApvtsCoordinates`, `at99_next_wrapsToPatch0_sameBank`, `coldStart_setBank_setsLockIndicatorTrue` (first nav from fresh sync sends Set Bank + indicator true)

9. **And** grep verification:

   ```bash
   # Navigation must NOT read lock indicator
   rg 'kBanksLocked|banksLocked' Source/Core/Services/DeviceMemoryLimits Source/Core/Actions/PatchManagerActionHandler.cpp

   # UNLOCK must NOT clear sync cache
   rg 'clearSyncedBankState' Source/Core/MIDI/PatchSelectionMidiSync.cpp

   # Cross-bank tests gone
   rg 'wrapsAcrossBanks|unlock_thenNext|unlock_onBank1_nextSameBank' Tests/
   ```

   Expected: `kBanksLocked` only in indicator write/read paths (handler unlock/bank select, `markBanksLockedInApvts`, `PluginProcessor::handlePatchNumberChange`, `InternalPatchesPanel`, `MidiManager` init default) — **not** in `advancePatch` or Prev/Next branch.

10. **And** Matrix-6/6R: Bank Utility grayed; no `0AH`/`0CH`; indicator not written on bank/unlock actions.

11. **And** full `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds green.

## Tasks / Subtasks

- [x] **Navigation simplify** (AC: 4, 5)
  - [x] `DeviceMemoryLimits::advancePatch` — remove `bankLocked` param; always wrap within bank when patch overflows
  - [x] `PatchManagerActionHandler` Prev/Next — call `advancePatch(current, direction)` without reading `kBanksLocked`
  - [x] Update header + any callers/tests referencing old signature

- [x] **UNLOCK + sync revert** (AC: 1, 6)
  - [x] `PatchSelectionMidiSync::sendUnlockBank` — remove `clearSyncedBankState()` call
  - [x] Confirm `handleUnlockBank` still sets indicator false only (no coordinate writes)

- [x] **Lock indicator (display-only)** (AC: 2, 3)
  - [x] Keep `markBanksLockedInApvts()` on bank select, Set Bank sent paths (`applyPatchCoordinates`, STORE, `handlePatchNumberChange`)
  - [x] Document in code comment: display-only per D-023a-R3; not read by navigation
  - [x] Do **not** remove `kBanksLocked` APVTS property — repurposed as indicator

- [x] **Tests** (AC: 8, 9)
  - [x] Remove cross-bank / unlock-resync tests
  - [x] Add new intra-bank + indicator tests
  - [x] Run grep anchors

- [x] **Build & verify** (AC: 11)
  - [x] `Matrix-Control_Tests`
  - [x] macOS Debug VST3 + Standalone

### Review Findings

- [x] [Review][Patch] Commentaire D-023a-R3 mal placé — le commentaire « navigation must not read kBanksLocked » est sur `markBanksLockedInApvts()` (écriture) au lieu du bloc Prev/Next (lecture supprimée). [PatchManagerActionHandler.cpp:232]

- [x] [Review][Patch] Cas limite non testé `{bank=0, patch=kMinPatchNumber}` direction -1 — l'ancien test inter-banque couvrait ce point ; le remplacement utilise `{bank=1, kMin}`. Ajouter une assertion `{0,0} → {0,99}`. [DeviceMemoryLimitsTests.cpp]

- [x] [Review][Patch] `testAt99_next_wrapsToPatch0_sameBank` n'asserte pas que `kBanksLocked` reste `false` après wrap sans Set Bank. [PatchManagerActionHandlerTests.cpp:441]

- [x] [Review][Patch] AC#1 exige que UNLOCK ne touche pas `kSelectedBank` — aucun test unlock ne l'asserte. Ajouter dans `testUnlock_doesNotChangeApvtsCoordinates`. [PatchManagerActionHandlerTests.cpp:460]

- [x] [Review][Patch] `testUnlockBankSends0CHOnly` et `testUnlock_doesNotChangeApvtsCoordinates` sont quasi-identiques (bank 2/5, locked) — différencier (ex. assert `kSelectedBank` dans l'un, vérifier absence de PC dans l'autre) ou fusionner. [PatchManagerActionHandlerTests.cpp:460-508]

- [x] [Review][Defer] Point rouge transitoire « off » lors d'une sélection de banque — `kCurrentBankNumber` déclenche `refreshDeviceLimits` → `refreshBankLockIndicator` avant `markBanksLockedInApvts()`. État final correct ; risque visuel faible (repaint JUCE). [InternalPatchesPanel.cpp:76-88] — deferred, risque faible

- [x] [Review][Defer] Nom `kBanksLocked` ne reflète plus la sémantique display-only — dette sémantique pré-existante ; rename hors scope 7-3c. — deferred, pré-existant

- [x] [Review][Defer] Commande grep AC#9 dans la story utilise un chemin répertoire inexistant (`DeviceMemoryLimits` sans `.cpp/.h`) — cosmétique doc. — deferred, doc only

- [x] [Review][Defer] `InternalPatchesPanel.cpp/.h` modifiés mais absents du File List story — wiring indicateur (AC#7) ; mettre à jour le File List. — deferred, doc only

## Dev Notes

### Correct Course context (approved 2026-06-19)

Sprint Change Proposal: `_bmad-output/planning-artifacts/sprint-change-proposal-2026-06-19-bank-unlock-simplify.md` (**Approved**).

| Decision | Effect on this story |
|----------|---------------------|
| **D-023-R2** | Navigation always intra-bank |
| **D-022-R4** | UNLOCK = `0CH` + indicator off; no bank/patch/sync cache |
| **D-023a-R3** | Dot on after bank select / Set Bank (incl. cold start); off after UNLOCK |
| **D-022-R6** | No sync after front-panel hardware use — user manual follow-up |

### Current codebase state (pre-implementation)

**Must change:**

| File | Current (wrong for 7-3c) | Target |
|------|--------------------------|--------|
| `DeviceMemoryLimits.cpp/.h` | `advancePatch(..., bankLocked)` cross-bank when unlocked | Always intra-bank wrap |
| `PatchManagerActionHandler.cpp:130-134` | Reads `kBanksLocked` for navigation | Remove read |
| `PatchSelectionMidiSync.cpp:75` | `clearSyncedBankState()` after UNLOCK | Remove |
| `PatchManagerActionHandlerTests.cpp` | Cross-bank + unlock-resync tests | Remove/replace |

**Keep (display-only indicator):**

| File | Behaviour |
|------|-----------|
| `handleUnlockBank` | Sets `kBanksLocked = false` |
| `markBanksLockedInApvts()` | Sets `kBanksLocked = true` on Set Bank paths |
| `InternalPatchesPanel::refreshBankLockIndicator` | Dot from `kBanksLocked` |
| `PluginProcessor::handlePatchNumberChange` | Sets indicator true when `setBankSent` |

### `advancePatch` target logic (Matrix-1000)

When `nextPatch` out of range `[minPatch, maxPatch]`:

```cpp
result.patch = wrapPatchWithinDevice(current, step);  // 99→0 or 0→99
result.bank = current.bank;  // unchanged
return result;
```

Remove entire cross-bank branch (lines 107–140 approx. in current `DeviceMemoryLimits.cpp`).

### `sendUnlockBank` target

```cpp
void PatchSelectionMidiSync::sendUnlockBank(const DeviceMemoryLimits& limits)
{
    if (midiManager_ == nullptr || !limits.hasBankConcept())
        return;

    midiManager_->sendUnlockBank();
    // Do NOT clearSyncedBankState()
}
```

### Indicator vs navigation — critical guardrail

**LLM mistake to prevent:** deleting `kBanksLocked` entirely. Story **removes navigation coupling**, **keeps** display indicator per D-023a-R3.

Navigation grep must show **zero** reads of `kBanksLocked` in:
- `DeviceMemoryLimits.cpp`
- Prev/Next block in `PatchManagerActionHandler.cpp`

### Test plan summary

| Test | Expect |
|------|--------|
| `unlockBank_sends0CHOnly` | `0CH`; indicator false; bank/patch unchanged |
| `bankSelect_setsBanksLockedTrue` | `0AH`; indicator true |
| `at99_fourNext_staysBank0` | 4× Next → bank 0 patch 3; PC only |
| `at99_next_wrapsToPatch0_sameBank` | 1× Next from 99 → patch 0 bank 0 |
| `unlock_doesNotChangeApvtsCoordinates` | UNLOCK from 2/5 → still 2/5 |
| `coldStart_setBank_setsLockIndicatorTrue` | First Next from fresh instance → Set Bank 0 + indicator true |

Manual smoke (handoff §8):

1. Fresh instance → Next → `0AH` + PC; stay bank 0; patch wraps 99→0.
2. Bank **3** → `0AH 03` + PC; dot **on**.
3. **UNLOCK** → `0CH` only; bank/patch unchanged; dot **off**.
4. Next → PC only (same bank).
5. Matrix-6: Bank Utility grayed.

### Previous story intelligence (7-3b)

- ID renames done — **do not rename again** (`bankUtilityUnlockBank`, `kBanksLocked`, etc.).
- `markBanksLockedInApvts()` + suppress hook patterns — reuse.
- Startup policy: instance starts 0/00; first user nav may send Set Bank 0 → indicator on (AC #3).
- Errata on 7-3b AC #1–3 superseded by **this** story.

### Out of scope (Story 7.5)

- Active bank **red text** on Bank Utility 0–9
- `kSelectedBank` sync after navigation
- Optional UNLOCK footer message
- Full GUI sign-off

### Project context reference

- `project-context.md` — C++17, builds under `Builds/`, English source only, no GUI in Core except indicator read in panel (preserve).
- `CONVENTIONS.md` — authoritative style.
- Handoff: `Documentation/Development/Plans/2026/06/2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md`

### References

- [sprint-change-proposal-2026-06-19-bank-unlock-simplify.md]
- [.decision-log.md — D-022-R4, D-022-R5, D-022-R6, D-023-R2, D-023a-R3]
- [7-3b-bank-utility-unlock-semantics-and-id-rename.md]
- [investigation next-patch-after-unlock — follow-up #5]

## Dev Agent Record

### Agent Model Used

Composer (claude-4.6-sonnet-medium-thinking)

### Debug Log References

- Grep AC#9: no `kBanksLocked` in `DeviceMemoryLimits`; Prev/Next no longer reads lock; `clearSyncedBankState` not called from `sendUnlockBank`; cross-bank test names absent from `Tests/`.

### Completion Notes List

- Simplified `DeviceMemoryLimits::advancePatch` — removed `bankLocked` param; navigation always wraps within current bank (D-023-R2).
- `PatchManagerActionHandler` Prev/Next no longer reads `kBanksLocked`; indicator remains display-only with comment on `markBanksLockedInApvts()` (D-023a-R3).
- `PatchSelectionMidiSync::sendUnlockBank` sends `0CH` only — reverted `clearSyncedBankState()` call (AC#6).
- Tests: removed cross-bank/unlock-resync cases; added `unlock_doesNotChangeApvtsCoordinates`, `at99_next_wrapsToPatch0_sameBank`, `coldStart_setBank_setsLockIndicatorTrue`; renamed navigation tests.
- Full `Matrix-Control_Tests` green; macOS Debug VST3 + Standalone build green.

### File List

- Source/Core/Services/DeviceMemoryLimits.cpp
- Source/Core/Services/DeviceMemoryLimits.h
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/Core/MIDI/PatchSelectionMidiSync.cpp
- Tests/Unit/DeviceMemoryLimitsTests.cpp
- Tests/Unit/PatchManagerActionHandlerTests.cpp

## Change Log

- 2026-06-19: Story 7-3c created — Correct Course approved; intra-bank navigation + display-only lock indicator (D-023-R2, D-023a-R3).
- 2026-06-19: Implementation complete — intra-bank `advancePatch`, dumb UNLOCK (no sync cache clear), display-only lock indicator, tests updated; ready for review.
- 2026-06-19: Code review — 5 patch findings applied (comment placement, boundary test, unlock test differentiation, kSelectedBank/kBanksLocked asserts); status → done.
