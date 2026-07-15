---
organization: Ten Square Software
project: Matrix-Control
title: Story U-13 — ComboBox / HierarchicalComboBox Popup Infrastructure Dedup
author: BMad Agent
status: backlog
sources:
  - implementation-artifacts/u-12-hierarchical-combobox-widget.md
  - implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md
  - Source/GUI/Widgets/ComboBox.h
  - Source/GUI/Widgets/ComboBox.cpp
  - Source/GUI/Widgets/HierarchicalComboBox.h
  - Source/GUI/Widgets/HierarchicalComboBox.cpp
  - Source/GUI/Widgets/HierarchicalPopupMenu.h
  - Source/GUI/Widgets/HierarchicalPopupMenu.cpp
  - Source/GUI/Widgets/PopupMenuBase.h
  - Source/GUI/Widgets/PopupMenuBase.cpp
  - Source/GUI/Widgets/ComboBoxControlPainter.h
  - Source/GUI/Widgets/PopupMenuRenderer.h
  - _bmad-output/project-context.md
created: 2026-07-14
updated: 2026-07-14
---

# Story U-13: ComboBox / HierarchicalComboBox Popup Infrastructure Dedup

Status: backlog

<!-- Refactoring / DRY hygiene in Epic U. Follows U-12 (widget) and ideally 6-14 (panel wiring). Zero visual change — behaviour and TestComponent QA must remain identical. -->

## Story

As a developer maintaining ComboBox-family widgets,
I want shared popup and control infrastructure extracted from `ComboBox` and `HierarchicalComboBox`,
so that duplicated lifecycle, layout constants, and popup plumbing are centralized without regressing closed-state or hierarchical popup rendering.

## Problem Statement

Story **U-12** already extracted:

- **`ComboBoxControlPainter`** — closed-state paint (background, text, triangle, focus border)
- **`PopupMenuRenderer`** — item label, hover, chevron, background (shared with `MultiColumnPopupMenu` / `ScrollablePopupMenu`)

Remaining duplication:

| Area | Today |
|------|--------|
| **Popup component base** | `PopupMenuBase` is coupled to `ComboBox&`; `HierarchicalPopupMenu` is a parallel ~500-line `juce::Component` with overlapping modal/keyboard/close patterns |
| **Control boilerplate** | `setUiScale`, `setLook`, `setPopupMenuLook`, `focusGained`/`focusLost`, `isPopupOpen_`, `hasFocus_`, `mouseDown` → show popup — near-identical in both controls |
| **Layout helpers** | Border thickness / item height helpers duplicated between `HierarchicalPopupMenu` and `PopupMenuBase` |

**Out of scope:**

- Staircase N1/N2 border geometry in `HierarchicalPopupMenu` (unique UX; do not merge into generic `drawBorder`)
- Patch Mutator panel wiring (**6-14**)
- `ScrollablePopupMenu` / `MultiColumnPopupMenu` behaviour changes beyond shared-base adoption
- New automated GUI pixel tests (project policy)
- ButtonLike ComboBox style in `HierarchicalComboBox`

## Acceptance Criteria

### AC 1 — Shared popup host abstraction (no `ComboBox&` lock-in)

1. Introduce a small host interface or base type (e.g. `IPopupMenuHost` or refactored `PopupMenuBase`) exposing what popup menus need:
   - `getUiScale()`, `getPopupMenuLook()`, `getBaseComponentWidth()` (or equivalent anchor width)
   - Popup open/close notification hooks
2. `PopupMenuBase` (or successor) consumes the host interface; **`ComboBox` implements it** without behaviour change.
3. **`HierarchicalComboBox` implements the same interface** so popup code can share close/modal/keyboard patterns where applicable.
4. **`HierarchicalPopupMenu` adopts shared pieces** (renderer setup, escape handling, close lifecycle, layout constant access, aligned dual-panel borders, per-column scroll) — full merge into `PopupMenuBase` inheritance is optional if dual-panel layout keeps a dedicated subclass.

### AC 2 — Control lifecycle dedup

1. Extract shared closed-control behaviour (focus tracking, popup-open repaint flag, `setUiScale` early-return, optional `setInactiveAppearance` hook) into a reusable helper or thin base/mixin used by both widgets.
2. **`ComboBox::paint` and `HierarchicalComboBox::paint`** remain thin delegates to `ComboBoxControlPainter::paintClosedState`.
3. No change to public API of either widget.

### AC 3 — Zero visual / behavioural regression

1. Manual QA via TestComponent:
   - Standard `ComboBox` (single + multi-column popup) at 50%–200%
   - `HierarchicalComboBox` aligned dual-panel popup (transparent void, N1/N2 borders, hover M01/M02)
2. Full unit test suite green.
3. No diff in closed-state or popup appearance at 100% UI scale (reviewer sign-off).

### AC 4 — Build & hygiene

1. New/refactored sources registered in `CMakeLists.txt` if files are added.
2. Functions respect project Clean Code limits (`CONVENTIONS.md`); extract helpers rather than growing monoliths.
3. Story File List documents touched files.

## Tasks / Subtasks

- [ ] **Inventory & design** (AC: #1, #2)
  - [ ] Map overlapping APIs between `PopupMenuBase`, `HierarchicalPopupMenu`, both controls
  - [ ] Choose approach: host interface vs templated base vs composition helper (document in Dev Notes)

- [ ] **Popup host refactor** (AC: #1)
  - [ ] Extract host interface / decouple `PopupMenuBase` from concrete `ComboBox`
  - [ ] Wire `HierarchicalComboBox` + reduce duplication in `HierarchicalPopupMenu`

- [ ] **Control boilerplate extract** (AC: #2)
  - [ ] Shared focus + popup-open state helper
  - [ ] Both widgets delegate without API change

- [ ] **Regression pass** (AC: #3, #4)
  - [ ] TestComponent manual QA checklist
  - [ ] `cmake --build` + unit tests green
  - [ ] Update File List and completion notes

## Dev Notes

### Sequencing

- **Blocked by:** U-12 `done` (widget landed)
- **Recommended after:** **6-14** (panel migration validates production wiring before structural refactor)
- **Parallel-safe with:** U-3…U-9 layout audits (orthogonal)

### Already shared (do not re-extract)

- `ComboBoxControlPainter` — closed state
- `PopupMenuRenderer` — item/background/chevron drawing
- `PopupMenuPositioner` — anchor positioning
- `ComboBox::getPopupLayoutDimensions()` — popup layout SSOT

### Keep separate / share carefully

- Transparent `hitTest` for dual-panel void space — hierarchical-only (may remain)
- **Staircase borders are obsolete** (Story **6-15**): do **not** preserve `drawStaircasePanelBorders`. DRY work should share the **aligned dual-panel** border path (N1/N2 top- or bottom-aligned; shared vertical edge) plus per-column scroll/viewport patterns with `ScrollablePopupMenu`

### Estimated effort

~0.5 day (hygiene; no product AC)

## File List

<!-- Populated during dev-story -->

## Change Log

- 2026-07-14: Story U-13 created — ComboBox / HierarchicalComboBox popup infrastructure DRY (post U-12 defer).
