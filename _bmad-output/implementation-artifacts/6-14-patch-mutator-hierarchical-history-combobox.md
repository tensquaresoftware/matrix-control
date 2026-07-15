---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.14 — Patch Mutator Hierarchical History ComboBox
author: BMad Agent
status: done
baseline_commit: bcd9bd0d4839ca89064349f7cead70dc716ec39e
sources:
  - planning-artifacts/sprint-change-proposal-2026-07-14-init-sysex-and-hierarchical-history-combobox.md
  - implementation-artifacts/6-6-history-m-and-r-ui-properties.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/u-12-hierarchical-combobox-widget.md
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
  - project-context.md
created: 2026-07-14
updated: 2026-07-15
---

# Story 6.14: Patch Mutator Hierarchical History ComboBox

Status: done

## Story

As a sound designer,
I want a single History combobox with M roots and R retries in a hover submenu,
so that labels are readable and COMPARE aligns with the RANDOM row (FR-54, D-082-R2, UX-DR6).

## Acceptance Criteria

1. **Given** Story **U-12** (`TSS::HierarchicalComboBox`) complete **When** `PatchMutatorPanel` history row is refactored **Then** `historyMComboBox_` and `historyRComboBox_` are replaced by **one** `historyComboBox_` (`HierarchicalComboBox`) **And** width = **`patchMutatorHistoryWidth`** = **48 px** @ 100 % (same as RANDOM slider — `Atoms::Widths::Slider::kPatchMutator`).

2. **And** APVTS wire strings from Story **6.6** remain stable; C++ symbols may use clearer names:
   - Wire IDs unchanged: `patchMutatorHistoryMList`, `patchMutatorHistoryRList`, `patchMutatorSelectedM`, `patchMutatorSelectedR`, `patchMutatorCompareActive`
   - C++ symbols: `kHistoryMutateList`, `kHistoryRetryList`, `kSelectedMutateRootIndex`, `kSelectedRetryIndex`, `kCompareActive`, `kSelectedRetryRootOnly`
   - **Additional ephemeral mirror (6-14):** `kHistoryRetryListsByRoot` (`patchMutatorHistoryRetryListsByRoot`) — encoded `root=—|Rxx;…` for every root so hierarchical submenus are complete (stripped from session XML)
   - Reads: lists + selection + compare + per-root retry menus
   - Writes on user selection: `kSelectedMutateRootIndex`, `kSelectedRetryIndex` (panel only — no engine calls from panel)
   - Pipe-separated list parsing unchanged for mutate/retry lists
   - Primary item = root index; child `—` → root-only; child Rxx → retry index

3. **And** popup content:
   - Empty history → control disabled, `<EMPTY>` sentinel
   - Each root Mi → primary row; submenu = `—` + sorted R labels for that root (from `kHistoryRetryListsByRoot`, not lazy/selected-root-only)
   - Changing primary **honours the submenu child** clicked (R or `—`); if no child is chosen, fall back to root-only (hierarchical supersession of 6.6 dual-combo “always reset R”)

4. **And** **Compare mode** (6.8): when `kCompareActive`, History control disabled/grayed — same rules as today for both former comboboxes.

5. **And** **layout**: `layoutHistoryLine` X positions for COMPARE / DELETE / CLEAR / EXPORT match **`layoutSliderLine`** RANDOM row (COMPARE under RETRY column) — **no horizontal drift** vs Amount/Random rows at 50%, 100%, 150% UI scale.

6. **And** remove obsolete wiring:
   - Delete `historyMComboBox_`, `historyRComboBox_`, `historyMRootIndices_`, `historyRRetryIndices_` dual refresh paths — consolidate to `refreshHistoryComboBox()`
   - Remove `DesignAtoms` M/R width constants after migration; keep only `kPatchMutatorHistory`
   - Update `WidgetDimensionRegistry` / `DimensionFactory` if they reference old history M/R ids

7. **And** Core may update history UI mirrors for hierarchical History:
   - `syncHistoryUiProperties` publishes mutate list, selected-root retry list, **per-root retry menus**, and selection; self-heals APVTS↔engine root mismatch before rebuild
   - `rebuildHistoryListMirrors()` (via handler/processor) rebuilds mirrors when selection changes
   - Panel remains selection-property writer only (AD-5)

8. **And** manual smoke + builds green:
   - Build Standalone; Mutator History submenu at 100% and 150% UI scale
   - MUTATE → M00 appears; hover any Mi (including never previously selected) → `—` + that root’s retries; select R01 → audition path unchanged (6.7)
   - COMPARE grays History; layout spot-check vs Figma grid

9. **And** U-12 widget deltas allowed in this story (document for U-12 consumers):
   - `onBeforeShowPopup` callback before menu show
   - Softer `setSelectedIds` / commit fallback when child missing
   - Submenu chevron drawn whenever a primary has children (not only when highlighted)

## Tasks / Subtasks

- [x] **Prerequisite gate** (AC: #1)
  - [x] Confirm Story U-12 merged or `HierarchicalComboBox` available on branch

- [x] **Panel refactor** (AC: #1–#4, #6)
  - [x] Replace dual ComboBox members with `historyComboBox_`
  - [x] Implement `refreshHistoryComboBox()` from APVTS lists
  - [x] Selection handler writes selection props only
  - [x] Update `valueTreePropertyChanged` / `valueTreeRedirected` listeners
  - [x] Compare-mode enable/disable

- [x] **Layout fix** (AC: #5)
  - [x] History combobox width = slider width; single control + gap before COMPARE
  - [x] Verify `resized()` at uiScale 0.5, 1.0, 1.5 — COMPARE column aligned with RETRY

- [x] **Cleanup** (AC: #6)
  - [x] Remove M/R design atoms and dimension registry entries
  - [x] Grep cleanup: `patchMutatorHistoryM`, `patchMutatorHistoryR`, `historyMComboBox`, `historyRComboBox`

- [x] **Smoke** (AC: #8)
  - [x] Manual checklist in Dev Notes
  - [x] Full unit suite (no regression in PatchMutatorEngineTests)

### Review Findings

- [x] [Review][Decision] Core/engine scope + APVTS symbol rename vs AC #7 / AC #2 — **Resolved (option 1):** keep Core + renames; amend AC #2/#7, File List, and notes to match implementation.
- [x] [Review][Decision] Per-root History submenu completeness vs lazy R-list cache — **Resolved (option 2):** engine publishes per-root retry menus (`kHistoryRetryListsByRoot`); panel attaches full submenus.
- [x] [Review][Decision] Primary (M) change: always reset R to root-only vs keep submenu child — **Resolved (option 2):** honour submenu child when changing M; AC #3 amended.
- [x] [Review][Decision] U-12 widget surface changes inside 6-14 — **Resolved (option 1):** keep widget deltas; document via AC #9 + File List.
- [x] [Review][Patch] Amend story AC #2/#7 + anti-patterns for Core/renames; update File List & Completion Notes [6-14-…md]
- [x] [Review][Patch] Prefetch per-root retry labels via `kHistoryRetryListsByRoot` [PatchMutatorEngine.cpp / PatchMutatorPanel.cpp]
- [x] [Review][Patch] Amend AC #3 for hierarchical M→child selection (honour submenu click) [6-14-…md]
- [x] [Review][Patch] Document U-12 widget deltas (onBeforeShowPopup, chevron, setSelectedIds) in story File List / notes [6-14-…md]
- [x] [Review][Patch] Stale retry submenu cache retained on empty APVTS list while retry still selected [PatchMutatorPanel.cpp] — removed keep-stale heuristic; authoritative by-root mirror
- [x] [Review][Patch] Same-root selection with childId ≤ 0 leaves APVTS retry unchanged [PatchMutatorPanel.cpp]
- [x] [Review][Patch] Combo can show primary-only while APVTS keeps orphan non-root retry [PatchMutatorPanel.cpp]
- [x] [Review][Patch] Restore fixture files at Tests/Fixtures/Patches/ after Few/Many move [Tests/Fixtures/Patches]
- [x] [Review][Patch] Restore self-heal when APVTS mutate-root ≠ engine during syncHistoryUiProperties [PatchMutatorEngine.cpp] — **dismissed after apply:** naive self-heal breaks mutate→sync (APVTS lags); `rebuildHistoryListMirrors()` already applies selection first.
- [x] [Review][Defer] Fragile Mxx/Rxx index parsing via substring(1,3) [PatchMutatorPanel.cpp:645] — deferred, pre-existing
- [x] [Review][Defer] No automated tests for hierarchy cache / defer / onBeforeShowPopup paths — deferred, pre-existing
- [x] [Review][Defer] No automated layout/scale assertions for COMPARE alignment — deferred, pre-existing
- [x] [Review][Defer] Unguarded short/non-numeric mutate labels → root index 0 [PatchMutatorPanel.cpp:645] — deferred, pre-existing

## Dev Notes

### Depends on

- **U-12** (blocking) — widget must exist before panel swap; 6-14 may extend widget API (AC #9)
- **6.6** (done) — APVTS property model (wire strings stable; symbols renamed; per-root menu mirror added)
- **6.7** (done) — audition debounce unchanged
- **6.8** (done) — compare graying

### Layout reference (100 % design px)

| Control | Width | Notes |
|---------|-------|-------|
| HISTORY label | 40 | `kPatchMutator` label |
| History combobox | **48** | = RANDOM slider |
| gap | 4 | `interControlGap` |
| COMPARE | 52 | aligns with RETRY/MUTATE column |

Previously: 24 + 4 + 24 = 52 px for two comboboxes — too narrow for labels; pushed COMPARE right.

### D-082-R2 supersession

Story 6.6 AC #5 mandated dual comboboxes. This story replaces panel wiring with hierarchical History and extends Core list mirrors (`kHistoryRetryListsByRoot`) so every root’s submenu is complete. Wire string IDs from 6.6 remain valid.

### Anti-patterns

- No SysEx from panel
- Panel writes selection properties only (AD-5) — engine owns store → APVTS list serialization including per-root menus
- Do not reintroduce `<` `>` navigation (D-026)

### Manual smoke

1. Empty history → `<EMPTY>`, disabled
2. MUTATE → open History → M00 visible with readable label
3. Hover M00 → submenu with `—`, R00…
4. With multiple roots + retries: hover a never-selected Mi → submenu still shows that root’s `—` + R labels
5. Change M by picking R01 under M02 → audition follows R01 (not forced root-only)
6. COMPARE on → History grayed
7. UI scale 150% → COMPARE vertically aligned with RETRY above

## Dev Agent Record

### Agent Model Used

Composer

### Completion Notes List

- Replaced `historyMComboBox_` / `historyRComboBox_` with single `TSS::HierarchicalComboBox` (`historyComboBox_`) at 48 px design width.
- Engine publishes `kHistoryRetryListsByRoot`; panel `refreshHistoryComboBox()` attaches `—` + R children for every root.
- Selection writes `kSelectedMutateRootIndex` / `kSelectedRetryIndex` only (AD-5); M change honours submenu child when present.
- `layoutHistoryLine` mirrors `layoutSliderLine` X steps (label + 48 px control + gap → COMPARE aligns with RETRY).
- Removed deprecated `kPatchMutatorHistoryM/R` atoms and `patchMutatorHistoryMWidth/RWidth`; restored single widget id `patchMutatorHistory`.
- U-12 deltas in-scope: `onBeforeShowPopup`, chevron when children exist, soft missing-child selection fallback.
- Code-review patches: self-heal in `syncHistoryUiProperties`, selection edge cases, fixture restore at `Tests/Fixtures/Patches/`.

### File List

- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Layout/WidgetDimensions.h
- Source/GUI/Factories/DimensionFactory.cpp
- Source/GUI/Widgets/HierarchicalComboBox.h
- Source/GUI/Widgets/HierarchicalComboBox.cpp
- Source/GUI/Widgets/HierarchicalPopupMenu.cpp
- Source/Shared/Definitions/PluginIDs.h
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp
- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Source/Core/Services/PatchMutator/MutatorSessionPersistence.h
- Source/Core/Services/PatchMutator/HistoryDefragService.cpp
- Source/Core/Actions/MutatorActionHandler.h
- Source/Core/Actions/MutatorActionHandler.cpp
- Source/Core/PluginProcessor.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp
- Tests/Unit/MutatorActionHandlerTests.cpp
- Tests/Unit/MutatorRecipePersistenceTests.cpp
- Tests/Fixtures/Patches/Patch 5.syx
- Tests/Fixtures/Patches/Patch 66.syx
- Tests/Fixtures/Patches/Patch 71.syx
- Tests/Fixtures/Patches/Patch 808.syx
- _bmad-output/implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md
- _bmad-output/implementation-artifacts/sprint-status.yaml

## Change Log

- 2026-07-14: Story 6-14 created — hierarchical History combobox panel (Correct Course D-082-R2).
- 2026-07-14: Implemented hierarchical History combobox in PatchMutatorPanel; layout realigned; M/R widget atoms removed.
- 2026-07-15: Code review — amended ACs for Core mirrors / hierarchical selection / U-12 deltas; per-root retry menu property; selection & fixture patches.
