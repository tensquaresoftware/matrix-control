---
organization: Ten Square Software
project: Matrix-Control
title: Story U-12 — HierarchicalComboBox Widget
author: BMad Agent
status: ready-for-dev
sources:
  - planning-artifacts/sprint-change-proposal-2026-07-14-init-sysex-and-hierarchical-history-combobox.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - Source/GUI/Widgets/ComboBox.h
  - Source/GUI/Widgets/ComboBox.cpp
  - project-context.md
created: 2026-07-14
updated: 2026-07-14
---

# Story U-12: HierarchicalComboBox Widget

Status: ready-for-dev

## Story

As a GUI developer,
I want a reusable **`TSS::HierarchicalComboBox`** widget with primary items and hover-activated submenus,
so that two-level selections (e.g. Patch Mutator History M → R) use one control with the existing ComboBox look (FR-54 UX, D-082-R2, Epic U).

## Acceptance Criteria

1. **Given** existing `TSS::ComboBox` look pipeline (`ComboBoxLook`, `PopupMenuLook`, skin Black green tones) **When** `HierarchicalComboBox` is implemented **Then** closed state matches standard ComboBox (same height, border, font, `setUiScale` behaviour) **And** popup uses the same menu look as `ComboBox::setPopupMenuLook`.

2. **And** popup interaction (IDE-style hierarchical menu):
   - Primary column lists top-level items (e.g. M00, M01, …)
   - Items **with children** show a **`>`** chevron on the right when hovered/selected
   - Hovering a parent item opens a **secondary submenu** to the right with child items (e.g. `—`, R00, R01, …)
   - Selecting a **leaf** in the submenu commits selection and closes popup
   - Selecting a **leaf** primary item (no children) commits immediately
   - Empty/disabled sentinel state supported (`<EMPTY>` text, non-openable)

3. **And** public API (minimal, panel-agnostic):
   - `clear()`, `addPrimaryItem(id, label)`, `addChildItem(primaryId, id, label)`
   - `setSelectedPrimaryId` / `setSelectedChildId` (or combined `setSelectedIds`) + `getSelectedPrimaryId` / `getSelectedChildId`
   - `onChange` callback when selection commits
   - `setEnabled`, `setTextWhenNothingSelected`, `setInactiveAppearance` optional parity with Button if needed for compare-graying

4. **And** implementation uses **`juce::PopupMenu`** nested submenus (or equivalent JUCE 8 pattern) — **no** global `AffineTransform` scaling; width/height from constructor + `setUiScale`.

5. **And** registered in build:
   - `Source/GUI/Widgets/HierarchicalComboBox.h`
   - `Source/GUI/Widgets/HierarchicalComboBox.cpp`
   - `CMakeLists.txt` PLUGIN_SOURCES
   - Optional: `TestComponent` demo section for manual visual QA (follow U-2 sandbox pattern)

6. **And** **DesignAtoms** — add `Atoms::Widths::ComboBox::kPatchMutatorHistory = 48` (same as `Atoms::Widths::Slider::kPatchMutator`); **do not** remove M/R atoms until Story **6-14** migrates panel (deprecation comment OK).

7. **And** no Patch Mutator panel wiring in this story (owned by **6-14**).

8. **And** builds green; no new automated GUI pixel tests (project policy).

## Tasks / Subtasks

- [ ] **Widget skeleton** (AC: #1, #4, #5)
  - [ ] Create `HierarchicalComboBox` extending `juce::Component` (or composing inner label/button like ComboBox)
  - [ ] Reuse look builders: `comboBoxLookFromSkin`, `popupMenuLookFromSkin`
  - [ ] `setUiScale`, `setLook`, `paint`, `mouseDown` → show menu

- [ ] **Popup menu model** (AC: #2, #3)
  - [ ] Internal item tree: primary id → optional child list
  - [ ] Build nested `PopupMenu` on show; chevron via menu item properties or custom paint if needed
  - [ ] Wire selection → stored ids → `onChange`

- [ ] **Design atom + factory hook** (AC: #6)
  - [ ] Add `kPatchMutatorHistory = 48` to `DesignAtoms.h`
  - [ ] Wire in `DimensionFactory` as `patchMutatorHistoryWidth` (or reuse slider width field in panel dims — document in 6-14)

- [ ] **TestComponent demo** (AC: #5, #8)
  - [ ] Minimal M/R sample data for manual hover/submenu verification at 50%/100%/150% UI scale

## Dev Notes

### Naming rationale (D-082-R2)

Class name **`HierarchicalComboBox`** expresses the IDE-style primary → secondary submenu pattern. Namespace: `TSS::` alongside `ComboBox`, `Button`, `Slider`.

### Reference UX

Cursor IDE: **View** (primary) → hover **Appearance** (chevron `>`) → secondary list (**Full Screen**, …). Patch Mutator History maps:

| Level | Content |
|-------|---------|
| Primary | M00–M99 (sorted) |
| Secondary (per M) | `—` (root-only), R00–R99 |

### Relationship to `TSS::ComboBox`

Do **not** break existing flat ComboBox. New class may share helpers (look, scaled height) but owns popup construction. Consider private helper in `SkinHelpers` only if duplication exceeds ~15 lines.

### Width constraint (for 6-14)

History control width @ 100 % = **48 px** = RANDOM slider width (`Atoms::Widths::Slider::kPatchMutator`). Frees horizontal space so COMPARE aligns with RANDOM row action column.

### JUCE notes

- `PopupMenu::addSubMenu()` for secondary column
- `ComboBox` in JUCE does not natively support hover submenus — custom `showMenuAsync` from this widget
- Mouse hover submenu: default JUCE submenu opens on hover when configured on parent item

### Out of scope

- Patch Mutator APVTS wiring (6-14)
- INIT SysEx (7-11)

## Dev Agent Record

### Agent Model Used

{{agent_model_name_version}}

### Completion Notes List

### File List

## Change Log

- 2026-07-14: Story U-12 created — HierarchicalComboBox widget (Correct Course D-082-R2).
