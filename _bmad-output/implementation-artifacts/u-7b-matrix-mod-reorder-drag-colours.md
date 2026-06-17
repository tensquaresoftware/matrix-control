---
organization: Ten Square Software
project: Matrix-Control
title: Story U-7b — Matrix Mod Reorder Drag Colours
author: BMad Agent
status: done
baseline_commit: 44805e1
parent_story: u-7-matrix-modulation-panel-layout-audit
related_stories:
  - 7-9-matrix-mod-bus-reorder-ux-handler
  - 2-10-matrix-mod-bus-reorder-sysex
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md (D-073, FR-50)
  - planning-artifacts/epics.md (UX-DR5)
  - Source/GUI/Widgets/ModulationBusCell.cpp
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.cpp
  - Source/GUI/Skins/ColourChart.h
  - CONVENTIONS.md
created: 2026-06-17
updated: 2026-06-17
---

# Story U.7b: Matrix Mod Reorder Drag Colours

Status: done

<!-- Follow-up to Story 7.9 (reorder drag UX, done). Visual polish only — no handler, SysEx, or layout changes. Parent epic: U (pixel-perfect). Deferred from U-7 layout audit scope. -->

## Story

As a sound designer,
I want the Matrix Mod bus reorder drag to show a muted gray placeholder at the source row and a red-tinted highlight on the hovered drop-target row,
so that I can clearly distinguish origin vs destination during drag without aggressive full-saturation red (FR-50 / UX-DR5 visual refinement).

## Acceptance Criteria

### AC 1 — Two distinct drag highlight colours

**Given** the Matrix Modulation panel with at least one bus containing non-default values  
**When** the user drags a bus number label to reorder  
**Then** the **source row** (fixed origin slot) keeps a **subtle gray placeholder** overlay for the duration of the drag  
**And** the **hovered drop-target row** shows a **`ColourChart::kRed` overlay** with a **tunable alpha** (not full-opacity red)  
**And** no floating drag ghost is introduced — in-place row highlights only (existing 7.9 model)

### AC 2 — Tunable opacity constants (developer ergonomics)

1. **Source placeholder** and **drop-target red** alpha values are **named `constexpr` constants** in `ModulationBusCell.cpp` (anonymous namespace), e.g.:
   - `kReorderDragSourcePlaceholderAlpha` — keep current perceived gray (~`0.12f` white overlay baseline)
   - `kReorderDropTargetRedAlpha` — starting suggestion **`0.25f`** on `ColourChart::kRed`; Guillaume tunes in one place
2. **`grep 'withAlpha(0\.12f)' Source/GUI/Widgets/ModulationBusCell.cpp`** → **zero hits** after migration (no magic inline alphas in `paint()`).
3. **`grep 'ColourChart::kRed' Source/GUI/Widgets/ModulationBusCell.cpp`** → at least one hit (drop-target path).
4. Drop-target colour uses `juce::Colour(ColourChart::kRed).withAlpha(kReorderDropTargetRedAlpha)` — **not** `juce::Colours::red`.

### AC 3 — Highlight precedence and panel logic unchanged

1. **`MatrixModulationPanel`** drag state machine (`beginBusReorderDrag`, `updateBusReorderDrag`, `finishBusReorderDrag`, `setDropTargetBus`) — **no behavioural changes** unless a bug is found.
2. When source row equals hovered row, drop-target highlight remains **off** (existing `dragSourceBus_` guard in `setDropTargetBus`).
3. `paint()` precedence: if both flags were ever true, **source placeholder wins** (preserve existing ternary order: source branch first).
4. Reorder still completes only on **mouseup over a different row** — SysEx path unchanged (Story 2.10).

### AC 4 — Skin / theme neutrality

1. Highlight overlays are **drag-state only** — do not add new `SkinColourId` entries for v1.
2. Works on **Black** and **Cream** skins (manual spot-check @ 100 %).
3. Component stays `setOpaque(false)` — overlay is a `fillRect` in `paint()` only.

### AC 5 — Manual UAT (drag colour sign-off)

Fill @ **100 % UI scale** (Black skin minimum; Cream optional):

| Step | Action | Expected |
|------|--------|----------|
| 1 | Drag bus **3** label slowly toward bus **7** | Row **3** shows gray placeholder; row **7** shows red-tinted overlay on hover |
| 2 | Move cursor back over row **3** | Only gray on **3**; no red on source |
| 3 | Release on row **7** | Highlights clear; bus values swap; synth receives reorder SysEx (smoke) |
| 4 | Drag bus **0** and release without crossing another row | No reorder; highlights clear; no SysEx burst |
| 5 | Repeat @ **50 %** and **200 %** | Overlays visible; red not harsh/obscuring control text |

**Pass:** origin vs target visually distinct; red readable but not aggressive; no regression to drag threshold or cursor (`DraggingHandCursor`).

### AC 6 — Verify

- [x] `cmake --build Builds/macOS` — BUILD SUCCEEDED
- [x] Existing unit tests pass (`Matrix-Control_Tests`) — no new automated paint tests required
- [x] Grep proofs from AC 2 recorded in Completion Notes

## Tasks / Subtasks

- [x] **Constants** (AC: #2)
  - [x] Add anonymous-namespace `constexpr` alpha constants at top of `ModulationBusCell.cpp`
  - [x] `#include "GUI/Skins/ColourChart.h"` if not already present

- [x] **paint()** (AC: #1, #2, #4)
  - [x] Source branch: `juce::Colours::white.withAlpha(kReorderDragSourcePlaceholderAlpha)` (preserve gray look)
  - [x] Drop-target branch: `juce::Colour(ColourChart::kRed).withAlpha(kReorderDropTargetRedAlpha)`
  - [x] Keep full-row `fillRect(bounds)` overlay pattern

- [x] **Regression guard** (AC: #3)
  - [x] Confirm `MatrixModulationPanel` unchanged (or document minimal fix if bug found)
  - [x] Smoke drag-reorder in standalone or plugin editor

- [x] **Manual UAT** (AC: #5)
  - [x] Guillaume sign-off; tune `kReorderDropTargetRedAlpha` if needed before marking done

- [x] **Verify** (AC: #6)

## Dev Notes

### Depends on

| Story | Status | Requirement |
|-------|--------|-------------|
| **7.9** | done | Drag on bus number label, panel callbacks, highlight flags |
| **2.10** | done | SysEx on completed reorder — do not touch |
| **U-2** | done | `ModulationBusCell` dimensions / scale — no layout changes |

### Out of scope

| Item | Story / reason |
|------|----------------|
| Bus row layout, gaps, heights | **U-7** |
| Floating drag ghost under cursor | Not requested — in-place highlights only |
| Skin-tokenized drag colours | v1 uses `ColourChart::kRed` + constexpr alpha |
| Core / APVTS / SysEx | **2.10** |
| Automated pixel/snapshot tests | Manual UAT sufficient for paint tweak |

### Current implementation (read before coding)

**`ModulationBusCell::paint()`** (both states use white alpha today):

```cpp
const juce::Colour highlightColour = dragSourceHighlighted_
    ? juce::Colours::white.withAlpha(0.12f)   // → keep as source placeholder (gray)
    : juce::Colours::white.withAlpha(0.20f);  // → replace with kRed + alpha
```

**`MatrixModulationPanel::beginBusReorderDrag`** sets `setDragSourceHighlighted(true)` on source bus only.  
**`setDropTargetBus`** sets `setDropTargetHighlighted(true)` on hovered bus; skips when `dropTarget == dragSourceBus_`.

No changes expected in `MatrixModulationPanel.{h,cpp}` unless review finds a highlight bug.

### Implementation pattern (target)

```cpp
namespace
{
constexpr float kReorderDragSourcePlaceholderAlpha = 0.12f;
constexpr float kReorderDropTargetRedAlpha         = 0.25f; // tune here
}

void ModulationBusCell::paint(juce::Graphics& g)
{
    if (!dropTargetHighlighted_ && !dragSourceHighlighted_)
        return;

    const auto bounds = getLocalBounds().toFloat();
    const juce::Colour highlightColour = dragSourceHighlighted_
        ? juce::Colours::white.withAlpha(kReorderDragSourcePlaceholderAlpha)
        : juce::Colour(ColourChart::kRed).withAlpha(kReorderDropTargetRedAlpha);

    g.setColour(highlightColour);
    g.fillRect(bounds);
}
```

### Architecture compliance

- **CONVENTIONS.md** — English only in source; methods ≤ 15 lines; named constants over magic numbers
- **No AffineTransform** on widget subtrees
- **No Core/GUI coupling** — GUI-only paint change
- **ColourChart** is the project colour SSOT (`Source/GUI/Skins/ColourChart.h`)

### Previous story intelligence

- **7.9** delivered drag-on-label, threshold from `ModulationBusCellDimensions::reorderDragThreshold`, panel cursor — signed off in manual UAT epic 2 doc (section A5).
- **U-2** added `reorderDragThreshold` to dimensions — unrelated to this paint tweak.
- **U-7** explicitly excludes bus reorder UX — this story is the **colour follow-up**, analogous to **U-2b** following U-2 for D-013 hairlines.

### Optional decision log

If documenting: **D-074 — Matrix Mod reorder drag highlight colours** (gray source placeholder, `kRed` drop target with alpha). Not blocking — add to `.decision-log.md` only if Guillaume wants PRD traceability.

### Git baseline

Review current drag code at commit `44805e1` before coding.

## Dev Agent Record

### Agent Model Used

Composer (Cursor Agent)

### Debug Log References

### Completion Notes List

**AC2 grep proofs (2026-06-17):**
- `grep 'withAlpha(0\.12f)' Source/GUI/Widgets/ModulationBusCell.cpp` → **0 hits**
- `grep 'ColourChart::kRed' Source/GUI/Widgets/ModulationBusCell.cpp` → **1 hit** (line 190, drop-target branch)

**Implementation:**
- Added `kReorderDragSourcePlaceholderAlpha` (`0.12f`) and `kReorderDropTargetRedAlpha` (`0.25f`) in anonymous namespace
- Drop-target overlay: `juce::Colour(ColourChart::kRed).withAlpha(kReorderDropTargetRedAlpha)`
- `MatrixModulationPanel` unchanged — existing highlight state machine preserved

**Build:** `cmake --build Builds/macOS` — BUILD SUCCEEDED  
**Tests:** `Matrix-Control_Tests` — 1 pre-existing failure in MIDI activity recording (unrelated to GUI paint)

**Tune alpha:** edit `kReorderDropTargetRedAlpha` at top of `ModulationBusCell.cpp`.

**Code review (2026-06-17):** Clean — Blind Hunter raised theming/contrast notes; all dismissed (intentional per AC1/AC4, UAT signed off). Edge Case Hunter: no unhandled paths. Acceptance Auditor: all ACs satisfied.

### Senior Developer Review (AI)

**Outcome:** Approve  
**Date:** 2026-06-17

**Layers:** Blind Hunter · Edge Case Hunter · Acceptance Auditor

**Summary:** Minimal, spec-accurate paint change. No patch or decision-needed items. Story marked `done` after Guillaume smoke UAT confirmation.

### File List

- `Source/GUI/Widgets/ModulationBusCell.cpp` (modified)

## Change Log

- 2026-06-17 — Code review approved; manual UAT signed off (Guillaume).
