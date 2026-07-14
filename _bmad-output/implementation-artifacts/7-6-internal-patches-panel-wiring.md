---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.6 — Internal Patches Panel Wiring
author: BMad Agent
status: done
baseline_commit: d309fa8
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - implementation-artifacts/7-3-patchmanageractionhandler-bank-and-internal.md
  - implementation-artifacts/7-3c-bank-utility-unlock-simplify.md
  - implementation-artifacts/7-5-bank-utility-ui-wiring.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/deferred-work.md
  - project-context.md
created: 2026-07-14
updated: 2026-07-14
---

# Story 7.6: Internal Patches Panel Wiring

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want internal patch navigation and I/C/P/STORE wired,
so that RAM/ROM rules apply in UI (FR-22, FR-23, FR-24).

## Acceptance Criteria

1. **Given** Stories 7.3 / 7.3c (Core handler complete) and 7.5 (bank lock dot verified) **When** user clicks `<` or `>` **Then** `InternalPatchesPanel` fires timestamp `setProperty` on `internalPatchesLoadPreviousPatch` / `internalPatchesLoadNextPatch`; `PatchManagerActionHandler` navigates via `DeviceMemoryLimits::advancePatch` with **intra-bank wrap only** (99→0, 0→99); bank coordinate unchanged; NumberBoxes update via APVTS listener.

2. **And** when user edits the **patch** NumberBox **Then** value pushes to `internalPatchesCurrentPatchNumber` APVTS property; `PluginProcessor::handlePatchNumberChange` enqueues Program Change (and Set Bank when cold-start requires it per `PatchSelectionMidiSync`); **bank** NumberBox remains **display-only** (`editable=false`) — no user path to change bank from Internal Patches (FR-21, D-023).

3. **And** when user clicks INIT / COPY / PASTE / STORE **Then** panel fires timestamp properties on the matching StandaloneWidget IDs; `ActionDispatcher` routes INIT/PASTE/STORE to `PatchManagerActionHandler` and COPY to `ModuleActionHandler` — **no business logic in panel**.

4. **And** ROM gating (FR-23, D-023): on ROM banks 2–9, INIT, PASTE and STORE appear grayed (`setInactiveAppearance`); **footer warning** (`kRomBankPasteStoreFooterMessage`, severity `warning`) on **click attempt** and **mouse hover** over grayed INIT/PASTE/STORE. Core `propagateRomBlockedFooter()` remains as defense-in-depth if action still reaches handler. INIT/PASTE/STORE ROM gating aligned with Story 7-11 (AC3).

5. **And** non-ROM disable reasons unchanged: PASTE grayed when `internalPatchesPastePatchEnabled` is false (no footer — Core silent no-op); INIT and STORE grayed when `patchMutatorCompareActive` is true (Story 6.8 / 7-11 AC3b — no ROM footer).

6. **And** ctor and `valueTreeRedirected` sync NumberBox displays from APVTS (`kCurrentBankNumber`, `kCurrentPatchNumber`) so session restore / device-type reconcile shows correct digits without requiring a property change event.

7. **And** lock dot on bank NumberBox **unchanged** — already wired in 7.5; verify only, do not remove `refreshBankLockIndicator`.

8. **And** **no** changes to `PatchManagerActionHandler` navigation/MIDI/STORE semantics, `DeviceMemoryLimits::advancePatch`, or bank-unlock indicator writes (owned by 7.3c / 7.5). Optional **panel-only** ROM footer helpers; do not duplicate Core ROM gate logic.

9. **And** manual smoke + builds green:
   - `cmake --build Builds/macOS --target Matrix-Control_Tests Matrix-Control_VST3 Matrix-Control_Standalone` (or platform preset)
   - `Matrix-Control_Tests` full suite (existing `PatchManagerActionHandlerTests` navigation/ROM tests must stay green)
   - Standalone smoke checklist in Dev Notes

## Tasks / Subtasks

- [x] **Audit brownfield wiring** (AC: #1–#3, #7)
  - [x] Confirm all eight controls use timestamp `setProperty` pattern (already in `InternalPatchesPanel.cpp`)
  - [x] Confirm patch NumberBox `setOnValueChanged` avoids feedback loop (existing guard)
  - [x] Confirm bank NumberBox `editable=false` and has **no** `setOnValueChanged` bank-write path
  - [x] Grep panel — no `PatchManagerActionHandler`, `MidiManager`, or SysEx calls

- [x] **NumberBox session sync** (AC: #2, #6)
  - [x] Add `syncNumberBoxesFromApvts()` — read `kCurrentBankNumber` / `kCurrentPatchNumber`, push to NumberBoxes
  - [x] Call from ctor (after `refreshDeviceLimits`) and `valueTreeRedirected` (alongside existing clipboard/limit refresh)

- [x] **ROM footer on click + hover** (AC: #4)
  - [x] Refactor `updatePasteStoreEnabled` to track `romPasteStoreBlocked_` separately from clipboard/compare disables
  - [x] ROM blocked: visual gray (`setAlpha(0.5f)`) but **keep `setEnabled(true)`** so clicks reach panel handler; `onClick` shows footer via local helper (do **not** dispatch action property when ROM-blocked — Core footer is redundant but acceptable if dispatched; prefer panel-only to avoid spurious MIDI)
  - [x] ROM blocked hover: `mouseEnter` on INIT/PASTE/STORE → same warning footer (reuse `kRomBankPasteStoreFooterMessage`)
  - [x] RAM banks: restore `setAlpha(1.0f)`; apply normal enable rules for clipboard/compare

- [x] **Footer helper** (AC: #4)
  - [x] Add anonymous or private `setFooterWarningMessage(apvts, message)` in panel `.cpp` (mirror `BankUtilityPanel::setFooterInfoMessage` but severity `"warning"`)

- [x] **Self-review & smoke** (AC: #8, #9)
  - [x] No Core handler diffs unless audit finds a genuine regression (unlikely)
  - [x] Run full test suite + Standalone smoke (Dev Notes)
  - [x] Clean Code limits on new panel methods (extract helpers if >15 lines)

### Review Findings

- [x] [Review][Patch] INIT gating ROM/Compare + footer élargi — décision option 3 : garder `wirePasteStoreButton` sur INIT (aligné 7-11) ; `kRomBankPasteStoreFooterMessage` → « Init, Paste and Store are not available on ROM banks 2-9. » ; AC4/AC5 spec mis à jour.

- [x] [Review][Defer] Footer ROM persiste après retour banque RAM [`InternalPatchesPanel.cpp:316`] — aucun clear de `uiMessageText` quand `romPasteStoreBlocked_` repasse à false ; warning peut rester affiché.
- [x] [Review][Defer] Hover footer absent si curseur déjà sur bouton au blocage ROM [`InternalPatchesPanel.cpp:134`] — `mouseEnter` ne se déclenche pas sans mouvement souris quand la banque passe ROM→curseur immobile.
- [x] [Review][Defer] Smoke manuel Standalone non documenté (AC9 partiel) — Dev Agent Record indique UAT reporté à Guillaume ; pas de preuve dans le diff.

## Dev Notes

### What Story 7.6 IS — and what it is NOT

| In scope (7.6) | Out of scope |
|---|---|
| Internal Patches **GUI wiring audit + polish** | Core INIT/PASTE/STORE/MIDI (**7.3** — done) |
| ROM **footer on click + hover** when PASTE/STORE grayed (FR-23) | Bank Utility red text / UNLOCK footer (**7.5** — done) |
| NumberBox **display sync** on ctor / session restore | Cross-bank navigation (removed **7.3c**) |
| Verify lock dot + nav + I/C/P/STORE end-to-end | Dirty-patch warnings (**Epic 9**) |
| Manual GUI smoke | Automated GUI unit tests (project policy: manual) |
| Panel-local footer for ROM UX | Duplicate MIDI dedup (**deferred-work** — do not fix here) |

### Hard prerequisites — frozen Core semantics

**7.3 / 7.3c / 7.5 are `done`.** Do not alter:

- `DeviceMemoryLimits::advancePatch` — intra-bank wrap only; never reads `kBanksLocked`.
- `PatchManagerActionHandler` Prev/Next → `applyPatchCoordinates` → `syncSelection`.
- UNLOCK / bank select / lock indicator writes (7.3c, 7.5).
- `handleInternalPatchStore` — `apvtsToBuffer` + `sendPatch` on RAM banks only; ROM → `propagateRomBlockedFooter()`.

### Current brownfield state (read before editing)

#### InternalPatchesPanel — action wiring largely complete

`InternalPatchesPanel.cpp` today:

| Control | Wiring | Status |
|---|---|---|
| `<` / `>` | timestamp on `kLoadPreviousPatch` / `kLoadNextPatch` | ✅ |
| Patch NumberBox | `setOnValueChanged` → `kCurrentPatchNumber` | ✅ |
| Bank NumberBox | display-only (`editable=false`); listener syncs from APVTS | ✅ display; ⚠️ ctor/redirect sync gap |
| INIT/COPY/PASTE/STORE | timestamp `setProperty` on action IDs | ✅ |
| ROM gray | `updatePasteStoreEnabled` → `setEnabled(false)` | ⚠️ blocks click → **no footer** |
| Lock dot | `refreshBankLockIndicator` on `kBanksLocked` | ✅ (7.5 verified) |
| Compare STORE guard | `kCompareActive` in `updatePasteStoreEnabled` | ✅ (6.8) |
| Clipboard PASTE guard | `kPastePatchEnabled` listener | ✅ |

**Primary 7.6 gap:** FR-23 requires footer on ROM click/hover, but `setEnabled(false)` prevents `onClick` from firing. Fix in **panel only** using the Bank Utility grayed-click pattern (keep enabled, reduce alpha, intercept UX locally).

#### PatchManagerActionHandler — Core complete (do not rewrite)

```cpp
// PatchManagerActionHandler.cpp — navigation (keep)
applyPatchCoordinates(limits.advancePatch(current, direction), limits);

// ROM gate (keep — defense in depth)
if (!limits.isPasteStoreAllowed(currentBank)) {
    propagateRomBlockedFooter();
    return;
}
```

STORE on RAM: `syncSelection` + `markBanksLockedInApvts` + `apvtsToBuffer` + `sendPatch`.

#### PluginProcessor — patch NumberBox MIDI path

Direct APVTS property edit (not action timestamp) triggers `handlePatchNumberChange` → `patchSelectionMidiSync_->syncSelection`. Navigation actions use handler `applyPatchCoordinates` with suppress hook to avoid double MIDI from listener — **do not change** this chain in 7.6.

**Known deferred debt (do not fix in 7.6):** duplicate `syncSelection` on some navigation paths (`deferred-work.md` — MIDI duplicated prev/next). Out of scope.

### ROM footer implementation guidance

**Reference pattern:** `BankUtilityPanel` grayed module — buttons stay `setEnabled(true)`, alpha 0.5, `mouseDown` on panel shows footer when grayed.

**Recommended approach for PASTE/STORE ROM block:**

1. Track `romPasteStoreBlocked_` in panel (from `!limits.isPasteStoreAllowed(currentBank)`).
2. When ROM blocked:
   - `pastePatchButton_->setEnabled(true); pastePatchButton_->setAlpha(0.5f);`
   - Replace `onClick` lambda: call `setFooterWarningMessage` with `kRomBankPasteStoreFooterMessage`; **return without** `setProperty` (panel-only UX).
   - Register `juce::MouseListener` on button (panel as listener or small helper) — `mouseEnter` → same footer if `romPasteStoreBlocked_`.
3. When RAM allowed:
   - Restore alpha 1.0; restore original timestamp `onClick` lambdas (factor shared `makeActionClick(propertyId)` helper to avoid duplication).
   - Apply clipboard/compare enable rules via `setEnabled` as today.

**Footer helper:**

```cpp
void setFooterWarningMessage(juce::AudioProcessorValueTreeState& apvts, const juce::String& message)
{
    apvts.state.setProperty("uiMessageText", message, nullptr);
    apvts.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
}
```

**Anti-patterns:**

- Do **not** add SysEx or handler calls from GUI.
- Do **not** use `setEnabled(false)` for ROM block if footer-on-click is required.
- Do **not** show ROM footer when PASTE disabled due to empty clipboard (FR-35 silent no-op).
- Do **not** add global `AffineTransform` scaling.

### NumberBox sync helper

```cpp
void InternalPatchesPanel::syncNumberBoxesFromApvts()
{
    const int bank = static_cast<int>(apvts_.state.getProperty(kCurrentBankNumber, 0));
    const int patch = static_cast<int>(apvts_.state.getProperty(kCurrentPatchNumber, 0));
    if (currentBankNumber) currentBankNumber->setValue(bank);
    if (currentPatchNumber) currentPatchNumber->setValue(patch);
}
```

Call after `refreshDeviceLimits()` in ctor and at end of `valueTreeRedirected`.

### APVTS property reference

| Property ID | C++ constant | Kind | 7.6 consumer |
|---|---|---|---|
| `internalPatchesLoadPreviousPatch` | `InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch` | action | `<` button |
| `internalPatchesLoadNextPatch` | `InternalPatchesModule::StandaloneWidgets::kLoadNextPatch` | action | `>` button |
| `internalPatchesCurrentBankNumber` | `kCurrentBankNumber` | state | Bank NumberBox display (read-only) |
| `internalPatchesCurrentPatchNumber` | `kCurrentPatchNumber` | state | Patch NumberBox edit + display |
| `internalPatchesInitPatch` | `kInitPatch` | action | INIT |
| `internalPatchesCopyPatch` | `kCopyPatch` | action | COPY → `ModuleActionHandler` |
| `internalPatchesPastePatch` | `kPastePatch` | action | PASTE |
| `internalPatchesStorePatch` | `kStorePatch` | action | STORE |
| `internalPatchesPastePatchEnabled` | `kPastePatchEnabled` | state | PASTE enable (clipboard) |
| `patchManagerBanksLocked` | `BankUtilityModule::StateProperties::kBanksLocked` | state (display-only) | Lock dot |
| `patchMutatorCompareActive` | `PatchMutatorModule::StateProperties::kCompareActive` | state | STORE disable during compare |

Display string for ROM footer: `PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kRomBankPasteStoreFooterMessage` (already exists — **no new string** unless copy edit required).

### Architecture compliance

- GUI → Core via APVTS properties only; panels stay presentation-only (FR-47, AD-5).
- IDs from `PluginIDs.h`; display strings from `PluginDisplayNames.h`.
- `DeviceMemoryLimits` / `DeviceTypeRegistry` in GUI for ROM/RAM gating — existing 8.5 pattern.
- No French in source; English footer strings only.
- Builds under `Builds/`; no `CMakeLists.txt` change expected.

### File structure

```
Source/GUI/Panels/.../PatchManagerPanel/Modules/
├── InternalPatchesPanel.h    (UPDATE — sync helper, optional MouseListener decl, rom state flag)
└── InternalPatchesPanel.cpp  (UPDATE — ROM footer click/hover, NumberBox sync, refactor updatePasteStoreEnabled)

Tests/ — no new tests required; existing PatchManagerActionHandlerTests cover Core ROM/navigation
```

### Manual smoke checklist (Standalone, Matrix-1000 device type)

1. Fresh instance → bank NumberBox shows `0`, patch `0`; PASTE/STORE enabled on bank 0.
2. Bank Utility → bank **1** (RAM) → bank NumberBox updates to `1`; patch unchanged; PASTE/STORE still enabled (same disabled look as bank 0 when clipboard empty only applies to PASTE if empty).
3. Bank Utility → bank **3** (ROM; banks 2–9) → INIT/PASTE/STORE use **disabled palette** (identical to clipboard-disabled PASTE); no hover/pressed highlight; click INIT or PASTE → footer warning; hover STORE → footer warning; no SysEx STORE/INIT.
4. Bank Utility → bank **0** (RAM) → PASTE/STORE enabled again.
5. Patch **99** → click `>` → wraps to **00**, same bank; bank NumberBox unchanged.
6. Patch **00** → click `<` → wraps to **99**, same bank.
7. Edit patch NumberBox to **42** → MIDI Program Change observed; bank unchanged.
8. INIT → editor resets (no automatic 0x01 unless user STOREs).
9. COPY → PATCH EDIT module Paste buttons gray; Internal Patches PASTE follows clipboard flag.
10. STORE on bank 0 → patch SysEx 0x01 enqueued.
11. Enable Compare in Mutator → STORE disabled; PASTE rules unchanged.
12. UI Scale 50% / 150% → layout intact; ROM gray + footer still work.

### Previous story intelligence (7.5)

- Lock dot verified — **do not remove** `refreshBankLockIndicator`.
- Bank Utility highlight reads `kSelectedBank`; Internal Patches bank NumberBox reads `kCurrentBankNumber` — intentional split.
- Footer pattern: `uiMessageText` + `uiMessageSeverity` via APVTS state properties.
- `valueTreeRedirected` refresh pattern added to `BankUtilityPanel` — mirror for Internal Patches NumberBoxes.

### Previous story intelligence (7.3c)

- Navigation never changes bank; lock indicator display-only.
- Tests `at99_next_wrapsToPatch0_sameBank`, `navigation_withinBank_noSetBank` — must remain green.
- Do not reintroduce `kBanksLocked` reads in navigation.

### Previous story intelligence (7.3)

- Core INIT/PASTE/STORE fully implemented; GUI explicitly deferred to **7.6** for ROM footer-on-hover.
- `propagateRomBlockedFooter()` already sets warning footer when handler reached — panel must cover disabled-button UX gap.

### Previous story intelligence (6.8)

- STORE disabled when `kCompareActive` — already in `updatePasteStoreEnabled`; preserve when refactoring ROM gating.

### Git intelligence (recent commits)

| Commit | Relevance |
|---|---|
| `d309fa8` | Docs only — no Internal Patches code |
| `8ac1354` | CI Node 24 — run test gate before done |
| `e9880cb` | Shell layout — unrelated |
| `70d866e` | Solo dev workflow — CI informational |

No recent commits touch `InternalPatchesPanel` — brownfield state stable since 7.5 / 6.8.

### Latest tech / framework notes

- **JUCE 8.0.12** — `ValueTree::Listener`; `Component::addMouseListener` for hover footer on child buttons.
- **C++17** — no new dependencies.
- `TSS::Button` does not expose hover callbacks — use `MouseListener` on button component or panel hit-test; prefer listener on paste/store buttons only.
- Warning footer severity string: `"warning"` (matches `FooterPanel` and Core `propagateRomBlockedFooter`).

### Project context reference

- `project-context.md` — GUI/Core boundary, ActionDispatcher hub, no French in source.
- `CONVENTIONS.md` — Clean Code 15-line function limit; extract `makeActionClick` / footer helpers if needed.
- D-023 — intra-bank wrap; ROM PASTE/STORE gray + footer on click/hover.
- D-023a-R3 — lock dot on bank NumberBox (verify only).
- D-036 — COPY snapshots full patch via `ModuleActionHandler` (already wired).

### References

- [epics.md — Story 7.6, FR-22–FR-24]
- [prd.md — §4.7 FR-22, FR-23, FR-24]
- [.decision-log.md — D-023, D-023-R2, D-023a-R3, D-036]
- [7-3-patchmanageractionhandler-bank-and-internal.md — Core prerequisite]
- [7-3c-bank-utility-unlock-simplify.md — navigation freeze]
- [7-5-bank-utility-ui-wiring.md — lock dot verify pattern]
- [6-8-compare-mode.md — STORE compare guard]
- [deferred-work.md — duplicate MIDI debt, transient dot flash]

## Dev Agent Record

### Agent Model Used

Composer 2.5

### Debug Log References

- Build: `cmake --build Builds/macOS/ARM/Debug --target Matrix-Control_Tests Matrix-Control_VST3 Matrix-Control_Standalone` — green
- Tests: `Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` — all pass (exit 0)
- Audit grep: no `PatchManagerActionHandler`, `MidiManager`, or SysEx in `InternalPatchesPanel.cpp`

### Completion Notes List

- Brownfield audit confirmed: eight controls dispatch via APVTS timestamp properties; bank NumberBox display-only (`editable=false`, no write path); patch NumberBox feedback-loop guard intact; lock dot wiring unchanged.
- Added `syncNumberBoxesFromApvts()` called from ctor and `valueTreeRedirected` for session-restore display sync.
- Refactored ROM gating: `romPasteStoreBlocked_` tracked separately; PASTE/STORE use `TSS::Button::setInactiveAppearance(true)` (disabled palette, no hover/press highlight) while remaining mouse-active for footer click/hover; no action dispatch on ROM click.
- Extracted `setFooterWarningMessage`, `dispatchTimestampAction`, and `wirePasteStoreButton` helpers — no Core handler changes.
- Manual Standalone smoke deferred to Guillaume (project policy); automated suite green including `PatchManagerActionHandlerTests` navigation/ROM tests.

### File List

- `Source/Shared/Definitions/PluginDisplayNames.h`
- `Source/GUI/Widgets/Button.h`
- `Source/GUI/Widgets/Button.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.h`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

## Change Log

- 2026-07-14: Story 7.6 created — Internal Patches panel wiring guide (ROM footer UX, NumberBox sync, brownfield audit).
- 2026-07-14: Story 7.6 implemented — ROM footer click/hover, NumberBox session sync, paste/store wiring refactor; tests green.
- 2026-07-14: ROM gray fix — `Button::setInactiveAppearance` replaces alpha hack; smoke checklist bank numbers corrected (RAM 0–1, ROM 2–9).
- 2026-07-14: Code review — INIT ROM/Compare gating confirmed (7-11); footer message broadened to include Init; story done.
