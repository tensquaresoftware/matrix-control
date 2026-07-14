---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.14 — Patch Mutator Hierarchical History ComboBox
author: BMad Agent
status: ready-for-dev
sources:
  - planning-artifacts/sprint-change-proposal-2026-07-14-init-sysex-and-hierarchical-history-combobox.md
  - implementation-artifacts/6-6-history-m-and-r-ui-properties.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/u-12-hierarchical-combobox-widget.md
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
  - project-context.md
created: 2026-07-14
updated: 2026-07-14
---

# Story 6.14: Patch Mutator Hierarchical History ComboBox

Status: ready-for-dev

## Story

As a sound designer,
I want a single History combobox with M roots and R retries in a hover submenu,
so that labels are readable and COMPARE aligns with the RANDOM row (FR-54, D-082-R2, UX-DR6).

## Acceptance Criteria

1. **Given** Story **U-12** (`TSS::HierarchicalComboBox`) complete **When** `PatchMutatorPanel` history row is refactored **Then** `historyMComboBox_` and `historyRComboBox_` are replaced by **one** `historyComboBox_` (`HierarchicalComboBox`) **And** width = **`patchMutatorHistoryWidth`** = **48 px** @ 100 % (same as RANDOM slider — `Atoms::Widths::Slider::kPatchMutator`).

2. **And** APVTS contract from Story **6.6** unchanged:
   - Reads: `kHistoryMList`, `kHistoryRList`, `kSelectedM`, `kSelectedR`, `kCompareActive`
   - Writes on user selection: `kSelectedM`, `kSelectedR` (panel only — no engine calls)
   - Pipe-separated list parsing unchanged
   - Primary item = root index; child `—` → `kRootOnly`; child Rxx → retry index

3. **And** popup content mirrors 6.6 semantics:
   - Empty history → control disabled, `<EMPTY>` sentinel
   - Each root Mi → primary row; submenu = `—` + sorted R labels for that root
   - Changing primary selection resets R to root-only when M changes (same as 6.6 dual-combo behaviour)

4. **And** **Compare mode** (6.8): when `kCompareActive`, History control disabled/grayed — same rules as today for both former comboboxes.

5. **And** **layout**: `layoutHistoryLine` X positions for COMPARE / DELETE / CLEAR / EXPORT match **`layoutSliderLine`** RANDOM row (COMPARE under RETRY column) — **no horizontal drift** vs Amount/Random rows at 50%, 100%, 150% UI scale.

6. **And** remove obsolete wiring:
   - Delete `historyMComboBox_`, `historyRComboBox_`, `historyMRootIndices_`, `historyRRetryIndices_` dual refresh paths — consolidate to `refreshHistoryComboBox()`
   - Remove `DesignAtoms` M/R width constants after migration; keep only `kPatchMutatorHistory`
   - Update `WidgetDimensionRegistry` / `DimensionFactory` if they reference old history M/R ids

7. **And** **no Core/engine changes** — `PatchMutatorEngine::syncHistoryUiProperties` untouched.

8. **And** manual smoke + builds green:
   - Build Standalone; Mutator History submenu at 100% and 150% UI scale
   - MUTATE → M00 appears; hover M00 → `—` + retries; select R01 → audition path unchanged (6.7)
   - COMPARE grays History; layout spot-check vs Figma grid

## Tasks / Subtasks

- [ ] **Prerequisite gate** (AC: #1)
  - [ ] Confirm Story U-12 merged or `HierarchicalComboBox` available on branch

- [ ] **Panel refactor** (AC: #1–#4, #6)
  - [ ] Replace dual ComboBox members with `historyComboBox_`
  - [ ] Implement `refreshHistoryComboBox()` from APVTS lists
  - [ ] Selection handler writes `kSelectedM` / `kSelectedR`
  - [ ] Update `valueTreePropertyChanged` / `valueTreeRedirected` listeners
  - [ ] Compare-mode enable/disable

- [ ] **Layout fix** (AC: #5)
  - [ ] History combobox width = slider width; single control + gap before COMPARE
  - [ ] Verify `resized()` at uiScale 0.5, 1.0, 1.5 — COMPARE column aligned with RETRY

- [ ] **Cleanup** (AC: #6)
  - [ ] Remove M/R design atoms and dimension registry entries
  - [ ] Grep cleanup: `patchMutatorHistoryM`, `patchMutatorHistoryR`, `historyMComboBox`, `historyRComboBox`

- [ ] **Smoke** (AC: #8)
  - [ ] Manual checklist in Dev Notes
  - [ ] Full unit suite (no regression in PatchMutatorEngineTests)

## Dev Notes

### Depends on

- **U-12** (blocking) — widget must exist before panel swap
- **6.6** (done) — APVTS property model
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

Story 6.6 AC #5 mandated dual comboboxes. This story replaces **panel wiring only**; engine/list properties remain valid.

### Anti-patterns

- No SysEx from panel
- No changes to `MutationHistoryStore` or `PatchMutatorEngine` selection logic
- Do not reintroduce `<` `>` navigation (D-026)

### Manual smoke

1. Empty history → `<EMPTY>`, disabled
2. MUTATE → open History → M00 visible with readable label
3. Hover M00 → submenu with `—`, R00…
4. Select M02 → R list updates to M02 retries
5. COMPARE on → History grayed
6. UI scale 150% → COMPARE vertically aligned with RETRY above

## Dev Agent Record

### Agent Model Used

{{agent_model_name_version}}

### Completion Notes List

### File List

## Change Log

- 2026-07-14: Story 6-14 created — hierarchical History combobox panel (Correct Course D-082-R2).
