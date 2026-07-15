---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.15 — History ComboBox Popup UX
author: BMad Agent
status: done
baseline_commit: df0c96ba69985c35622c4ce77183c194fdb13f4e
sources:
  - implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md
  - implementation-artifacts/u-12-hierarchical-combobox-widget.md
  - implementation-artifacts/u-13-combobox-popup-infrastructure-dedup.md
  - planning-artifacts/sprint-change-proposal-2026-07-14-init-sysex-and-hierarchical-history-combobox.md
  - Source/GUI/Widgets/HierarchicalPopupMenu.cpp
  - Source/GUI/Widgets/PopupMenuPositioner.cpp
  - Source/GUI/Widgets/ScrollablePopupMenu.cpp
  - Source/GUI/Layout/Design/DesignAtoms.h
  - project-context.md
created: 2026-07-15
updated: 2026-07-15
---

# Story 6.15: History ComboBox Popup UX

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a sound designer,
I want the hierarchical History popup to open like a normal ComboBox (aligned N1/N2, capped height with scroll, clear M/R labels, stable green borders),
so that long mutation histories stay usable and match the rest of the plugin’s ComboBox UX (FR-54 / D-082-R2 polish after Story 6-14).

## Acceptance Criteria

1. **Given** Story **6-14** hierarchical History is wired **When** the History popup opens **Then** placement follows the same **above/below** screen-fit rules as base `TSS::ComboBox` (`PopupMenuPositioner`) **And** N1 (primary) and N2 (secondary) panels are **edge-aligned** (not staircase):
   - Popup opens **below** the closed control → N1 and N2 share the **same top** edge
   - Popup opens **above** the closed control → N1 and N2 share the **same bottom** edge
   - History sits at the bottom of the Standalone window → both columns open **above**, bottom-aligned

2. **And** staircase layout is removed: delete / stop using `drawStaircasePanelBorders` and row-tied secondary Y offset (`getSecondaryPanelBounds` must not start at the hovered primary row). Transparent “void” hit-testing may remain only for space outside both rects.

3. **And** each column (N1 and N2 independently) shows at most **10** visible items, then **vertical scroll** using the same scrollbar interaction model as `ScrollablePopupMenu` / Computer Patches ComboBox (wheel + thumb). Shared SSOT: derive max viewport height from design atoms — **10 × `PopupMenu::kItemHeight`** (= **200** design px when `kItemHeight == 20`). Update `Atoms::Widths::PopupMenu::kMaxScrollHeight` from **300 → 200** so all ComboBox popups (including Computer Patches) show **10** visible rows instead of **15**.

4. **And** submenu presentation (panel / widget display — **APVTS wire contracts unchanged**):
   - Root **with no retries** → **no N2**, **no chevron**; click N1 `Mxx` commits root-only (`kSelectedRetryRootOnly`)
   - Root **with ≥1 retry** → chevron + N2 containing **`Mxx`** first (recall root), then **`Mxx-Ryy`** rows (`MutationNaming::formatPatchName` style) — **not** em-dash `—` + bare `Ryy`
   - Engine may still publish `—|R00|…` in `kHistoryRetryListsByRoot` / `kHistoryRetryList`; **transform labels in the panel** (or a thin GUI helper) when calling `addChildItem`
   - Closed-control text must stay coherent after label change: root → `Mxx`; retry → `Mxx-Ryy` (must **not** render `M01 M01-R00`)

5. **And** popup column widths may exceed the closed **48 px** control as needed so longest labels (`M99`, `M99-R99`) + N1 chevron + padding remain readable without clipping; closed History width stays `kPatchMutatorHistory` (**48**). Prefer measuring from font metrics (or a documented design min width for N2).

6. **And** border colour bug fix: while either N1/N2 is open, borders stay **`PopupMenuLook.border`** (skin green / `#00DDAD` in Black skin) for the entire popup lifetime — including hover on the **last** N2 row. Borders must **not** flash or fall back to dark grey `#161616` (`ColourChart::kDarkGrey1`). Match base ComboBox border stability (paint order / clipped highlights / shared-edge drawing — fix root cause, do not hard-code colours in the widget).

7. **And** behaviour preserved from 6-14 / 6-7 / 6-8:
   - Empty history → `<EMPTY>`, disabled
   - Compare mode → History grayed / non-openable
   - Selection still writes only `kSelectedMutateRootIndex` / `kSelectedRetryIndex` (AD-5); audition debounce unchanged
   - Per-root submenu data still comes from `kHistoryRetryListsByRoot` (complete menus for never-selected roots)

8. **And** **U-13** conflict resolved in notes: U-13 currently says “keep staircase N1/N2 borders unique.” After this story, staircase is obsolete — **amend U-13** Dev Notes / out-of-scope bullet so DRY work shares the **aligned dual-panel** border path (do **not** implement U-13 in this story).

9. **And** manual smoke + builds green (no new automated GUI pixel tests — project policy):
   - Standalone: History near bottom → N1+N2 above, bottom-aligned; no staircase
   - Force below (e.g. TestComponent combo near top) → N1+N2 top-aligned under control
   - ≥11 roots → N1 scrolls at 10 visible; root with ≥11 retries → N2 scrolls
   - Computer Patches popup also caps at 10 visible rows
   - Root without retries: click N1 selects root; no N2
   - Root with retries: N2 shows `Mxx` then `Mxx-R00…`; select each → closed text + audition OK
   - Hover last N2 row → borders stay green
   - Full unit suite green (`Matrix-Control_Tests`)

## Tasks / Subtasks

- [x] **Aligned dual-column layout** (AC: #1, #2)
  - [x] Refactor `HierarchicalPopupMenu::getSecondaryPanelBounds` / `getPreferredContentSize` for top- or bottom-align with primary
  - [x] Plumb “opens above vs below” from `PopupMenuPositioner` (or equivalent) into the hierarchical popup so alignment matches placement
  - [x] Remove `drawStaircasePanelBorders`; use shared rectangular borders (possibly shared edge without staircase)

- [x] **Scroll + max visible rows** (AC: #3)
  - [x] Change `DesignAtoms` `PopupMenu::kMaxScrollHeight` **300 → 200**; confirm `DimensionFactory` / `ComboBox::getPopupLayoutDimensions` pick it up
  - [x] Add per-column scrolling to hierarchical N1/N2 (reuse `ScrollablePopupMenu` patterns / scrollbar look — avoid duplicating thumb math if a small shared helper exists; full U-13 DRY is out of scope)
  - [x] Verify Computer Patches flat ComboBox now shows 10 rows

- [x] **Submenu labels & no empty N2** (AC: #4, #5)
  - [x] In `PatchMutatorPanel::addRetryChildrenForPrimary` (or helper): skip children when only sentinel; when retries exist, emit `formatPatchName(root, rootOnly)` then `formatPatchName(root, Ryy)` display strings
  - [x] Update `HierarchicalComboBox::getDisplayText` (and selection mapping) for new child labels
  - [x] Size N2 (and N1 if needed) for longest label + chevron

- [x] **Border stability** (AC: #6)
  - [x] Reproduce hover-last-row grey border; fix paint/hit/resize so border colour never switches to editor background grey
  - [x] Regression-check at 100% and 150% UI scale

- [x] **Docs / consumers** (AC: #8)
  - [x] Amend `u-13-combobox-popup-infrastructure-dedup.md` staircase out-of-scope note
  - [x] Update `TestHierarchicalComboBoxes` sample tree to exercise scroll + new labels + above/below placement if needed

- [x] **Smoke + tests** (AC: #7, #9)
  - [x] Manual checklist above on Standalone + TestComponent
  - [x] Run unit tests; no Core contract breaks (`kHistoryRetryListsByRoot` format may stay)

### Review Findings

- [x] [Review][Patch] Re-run PopupMenuPositioner when popup size grows after open [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp:558`] — decision: re-fit via positioner (may flip above/below)
- [x] [Review][Patch] Display-label / retry-index desync when tokens are skipped [`Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp:631`]
- [x] [Review][Patch] ComboBox static default still `maxScrollHeight = 300` [`Source/GUI/Widgets/ComboBox.cpp:19`]
- [x] [Review][Patch] Hierarchical popup uses unscaled vertical margin [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp:986`]
- [x] [Review][Patch] Stale N2 highlight when pointer returns to same N1 row [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp:689`]
- [x] [Review][Patch] Restore out-of-range guard in `getSecondaryItemBounds` [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp:529`]
- [x] [Review][Patch] Fix misleading TestHierarchicalComboBoxes root-count comment [`Source/GUI/Tests/TestHierarchicalComboBoxes.cpp:92`]
- [x] [Review][Defer] Duplicated CustomScrollBar vs ScrollablePopupMenu [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp`] — deferred, pre-existing
- [x] [Review][Defer] Permanent 20 Hz scrollbar timers while popup open [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp`] — deferred, pre-existing

## Dev Notes

### Origin (no epics.md body)

Story key `6-15-history-combobox-popup-ux` was added to `sprint-status.yaml` on 2026-07-14 after hierarchical History smoke. Formal BDD was **not** in `epics.md`. ACs here are authored from Guillaume’s UAT briefs:

- 2026-07-14: no N2 without retries; N2 labels `Mxx` / `Mxx-Ryy` (bug #1 wrong-root attachment was fixed during 6-14 review — do not re-open)
- 2026-07-15: drop staircase; ComboBox-like placement; align N1/N2; 10-item scroll; Computer Patches 15→10; green border on last-row hover

Supersedes sprint-change-proposal submenu wording “`—` + R00…” for **display only**.

### Current state (must read before editing)

| Area | Today | This story |
|------|--------|------------|
| `HierarchicalPopupMenu::getSecondaryPanelBounds` | Secondary Y = hovered primary row → **staircase** | Align tops or bottoms with N1 |
| `drawStaircasePanelBorders` | Shared-edge staircase stroke | Remove |
| Scroll | None — full list height | Cap 10 rows + scroll per column |
| `kMaxScrollHeight` | **300** (= 15×20) | **200** (= 10×20) — affects **all** ComboBox popups |
| Engine `kHistoryRetryListsByRoot` | Always prefixes `—` | Keep wire format; panel transforms |
| Panel children | Passes engine labels through | Filter sentinel-only; display `Mxx` / `Mxx-Ryy` |
| Closed text | `Mxx` or `Mxx` + `" "` + child | Must handle `Mxx-Ryy` child labels without doubling |

### Architecture & AD compliance

- **AD-5:** Panel writes selection properties only; no SysEx from GUI
- **AD-6 / FR-54:** Two-level History model unchanged; this story is presentation
- **Core ↛ GUI:** Prefer panel-side label mapping; if sharing `MutationNaming::formatPatchName`, include from Core headers already used by panel/tests — do **not** move naming into GUI
- **No global AffineTransform** UI scale; use existing `setUiScale` + design atoms
- **English only** in source / comments

### Implementation guardrails

1. **Do not invent a third History control** — extend `HierarchicalComboBox` / `HierarchicalPopupMenu`
2. **Do not change** APVTS wire IDs or engine list encoding unless absolutely required (prefer panel transform)
3. **Do not implement U-13** DRY merge here — only amend U-13 docs so next story does not preserve staircase
4. **Reuse** `PopupMenuPositioner`, `PopupMenuRenderer`, `PopupMenuLook`, `ScrollablePopupMenu` scrollbar patterns
5. **Closed width** stays 48 px (layout alignment with RANDOM / COMPARE from 6-14 is out of scope to reopen)
6. Fragile `substring(1,3)` parsing remains deferred (`deferred-work.md`) unless you touch that path for display mapping — then use root index from `mutateRootIndices_` already available

### Suggested approach (non-normative)

1. After `PopupMenuPositioner::calculateDimensions`, detect whether the final Y is above or below the anchor; store `opensAbove` on the popup.
2. Layout: `secondary.x = primary.right - sharedBorder`; if `opensAbove` then `secondary.bottom = primary.bottom` else `secondary.top = primary.top`.
3. For scroll: viewport height = `min(itemCount, 10) * itemHeight`; content larger → scrollbar (see `ScrollablePopupMenu::show` / custom thumb).
4. Label helper in panel:

```cpp
// Pseudocode — presentation only
if (retryLabels == onlySentinel) // no real Ryy
  // no addChildItem
else
  addChild(rootOnlyId, MutationNaming::formatPatchName(root, kRootOnly)); // "M01"
  for each Ryy: addChild(..., MutationNaming::formatPatchName(root, Ryy));  // "M01-R00"
```

5. Border bug: last-row hover likely exposes `#161616` editor bg through incomplete staircase edges or highlight painting over border after resize — after aligned rects, paint **background → items → full rect borders** last with `popupLook.border`.

### Files likely to touch

| Path | Change |
|------|--------|
| `Source/GUI/Widgets/HierarchicalPopupMenu.h/.cpp` | Align layout, scroll, border paint |
| `Source/GUI/Widgets/HierarchicalComboBox.h/.cpp` | `getDisplayText` / selection display |
| `Source/GUI/Widgets/PopupMenuPositioner.h/.cpp` | Optional: expose open-above vs below to callers |
| `Source/GUI/Layout/Design/DesignAtoms.h` | `kMaxScrollHeight` 300→200 |
| `Source/GUI/Factories/DimensionFactory.cpp` | Confirm mapping (likely auto) |
| `Source/GUI/Panels/.../PatchMutatorPanel.cpp` | Child label transform / empty-N2 |
| `Source/GUI/Tests/TestHierarchicalComboBoxes.*` | Sample data + smoke helpers |
| `_bmad-output/implementation-artifacts/u-13-….md` | Drop “preserve staircase” |
| `CMakeLists.txt` | Only if new helper `.cpp` added |

### Testing

- **Unit:** Existing PatchMutator engine / naming tests must stay green; add/adjust unit tests only if Core/panel helpers with pure logic are extracted (optional)
- **Manual:** AC #9 checklist; TestComponent hierarchical page at 50%–200%
- **No** automated GUI pixel tests (NFR-1 / project-context)

### Previous story intelligence (6-14)

- Per-root menus via `kHistoryRetryListsByRoot` are required — keep complete
- Selection honours submenu child when changing M
- U-12 deltas already in tree: `onBeforeShowPopup`, soft missing-child, chevron when children exist
- Deferrals in `deferred-work.md` (parsing, no GUI auto-tests) — leave unless blocking

### Git intelligence

Recent: `880b11e` Complete Story 6-14; `84b11b5` Add HierarchicalComboBox (U-12). Build on those — do not reinvent popup hosting.

### Project context reference

Follow `_bmad-output/project-context.md`: GUI→Core direction, design atoms SSOT, manual GUI QA, English source, no French in code.

## Dev Agent Record

### Agent Model Used

Composer (Cursor Agent)

### Debug Log References

- JUCE 8: `Font::getStringWidthFloat` unavailable — use `GlyphArrangement::getStringWidth` for column width measurement.

### Completion Notes List

- Aligned N1/N2 via `PopupMenuDimensions::opensAbove`; removed staircase layout and `drawStaircasePanelBorders`.
- Per-column scroll capped by `kMaxScrollHeight` 200 (10 rows); CustomScrollBar wheel/thumb mirrors ScrollablePopupMenu.
- `MutationNaming::buildHistorySubmenuDisplayLabels` transforms engine `—|Ryy` into `Mxx` / `Mxx-Ryy`; sentinel-only roots get no N2.
- `getDisplayText` uses full child patch names without doubling (`M01 M01-R00`).
- Borders painted last with `popupLook.border` over clipped item highlights.
- U-13 docs updated: share aligned dual-panel path, not staircase.
- Unit suite green including new MutationNaming label tests.
- Review patches: `buildHistorySubmenuDisplay` lockstep indices; `applyPreferredSize` re-runs `PopupMenuPositioner`; ComboBox default `maxScrollHeight` 200; scaled vertical margin; clear stale N2 highlight; secondary bounds guard.

### File List

- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Widgets/PopupMenuPositioner.h
- Source/GUI/Widgets/PopupMenuPositioner.cpp
- Source/GUI/Widgets/HierarchicalPopupMenu.h
- Source/GUI/Widgets/HierarchicalPopupMenu.cpp
- Source/GUI/Widgets/HierarchicalComboBox.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
- Source/GUI/Tests/TestHierarchicalComboBoxes.cpp
- Source/Core/Services/PatchMutator/MutationNaming.h
- Source/Core/Services/PatchMutator/MutationNaming.cpp
- Tests/Unit/MutationNamingTests.cpp
- _bmad-output/implementation-artifacts/u-13-combobox-popup-infrastructure-dedup.md
- _bmad-output/implementation-artifacts/6-15-history-combobox-popup-ux.md
- _bmad-output/implementation-artifacts/sprint-status.yaml

### Implementation Plan

1. Core label helper + unit tests (RED→GREEN).
2. Atom `kMaxScrollHeight` 300→200 + `opensAbove` on positioner.
3. Rewrite HierarchicalPopupMenu: aligned panels, scroll, border paint order, measured widths.
4. Panel transform + closed text; TestComponent sample data; amend U-13.

## Change Log

- 2026-07-15: Story created — popup UX polish (aligned N1/N2, scroll@10, labels, border fix, Computer Patches 15→10).
- 2026-07-15: Implemented — aligned dual-panel History popup UX; status → review.
- 2026-07-15: Code review — 7 patches applied (positioner re-fit, label/index lockstep, maxScrollHeight default, scaled margin, highlight clear, bounds guard, test comment); 2 deferred to U-13; status → done.
