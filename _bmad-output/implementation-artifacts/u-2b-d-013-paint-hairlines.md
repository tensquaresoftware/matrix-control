---
organization: Ten Square Software
project: Matrix-Control
title: Story U-2b — D-013 Paint Hairlines Compliance
author: BMad Agent
status: done
baseline_commit: 5a4c9887
parent_story: u-2-transversal-widgets-scale-audit
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md (D-013)
  - Source/GUI/Layout/ScaledDrawing.h
  - CONVENTIONS.md
created: 2026-06-10
updated: 2026-06-17
---

# Story U.2b: D-013 Paint Hairlines Compliance

Status: done

<!-- Follow-up to U-2. Layout/bounds scaling is signed off (Guillaume, 2026-06-09). This story covers paint()-time stroke/hairline policy only. -->

## Story

As a performer,
I want every hairline and decorative stroke in custom widgets to stay crisp and correctly centred at all UI scale presets,
so that separators, headers, and display curves match D-013 (1 physical px @ 50–150 %, 2 physical px @ 200 % on Retina).

## Acceptance Criteria

### AC 1 — ScaledDrawing for hairlines (regression guard)

1. **`grep 'std::max(1.0f.*uiScale_' Source/GUI/Widgets/`** → zero hits for stroke thickness in `paint()` (font scaling `* uiScale_` is OK).
2. **`grep 'snappedStrokeThicknessFromDesign' Source/GUI/Widgets/HorizontalSeparator.cpp`** → at least one hit.
3. Every 1 px design hairline uses `ScaledDrawing::snappedStrokeThicknessFromDesign` + `systemDisplayScaleForComponent(*this)`.
4. Hairlines are drawn with **`fillRect`** on a centred rectangle (not `drawLine`) unless a widget has a documented exception (e.g. diagonal display curves).

### AC 2 — Known outliers migrated (minimum scope)

| Widget | File | Current gap | Target pattern |
|--------|------|-------------|----------------|
| HorizontalSeparator | `HorizontalSeparator.cpp` | manual `* uiScale_`, `drawLine` | ScaledDrawing + centred `fillRect` (mirror `VerticalSeparator.cpp`) |
| SectionHeader | `SectionHeader.cpp` | `lineHeight = max(1, kLineHeight_ * uiScale_)` | ScaledDrawing + centred fill |
| EnvelopeDisplay | `EnvelopeDisplay.cpp` | curve `lineThickness` manual | ScaledDrawing for curve stroke (keep `drawLine` for polyline OK) |
| TrackGeneratorDisplay | `TrackGeneratorDisplay.cpp` | same as Envelope | same |
| ScrollablePopupMenu | `ScrollablePopupMenu.cpp` | `getBorderThicknessDesign() * uiScale_` inset | ScaledDrawing for border inset if hairline |

Reference implementations (do not regress): `Button.cpp`, `VerticalSeparator.cpp`, `ModuleHeader.cpp`, `GroupLabel.cpp`.

### AC 3 — Centring rules

1. **HorizontalSeparator:** line centred vertically in component bounds (8 px design band @ 100 %).
2. **SectionHeader:** decorative lines centred vertically in content area.
3. **VerticalSeparator:** already compliant — verify only.

### AC 4 — Snap policy

| Use case | `StrokeSnapPolicy` |
|----------|-------------------|
| Decorative hairlines (separators, header rules) | `kRound` |
| Control borders (Button, ComboBox, etc.) | `kFloor` (already set — do not change) |

### AC 5 — Manual UAT (D-013 column)

Fill hairline row @ 50, 75, 100, 125, 150, 175, 200 %:

| Widget | 50 | 75 | 100 | 125 | 150 | 175 | 200 | Notes |
|--------|:--:|:--:|:---:|:---:|:---:|:---:|:---:|-------|
| HorizontalSeparator | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | |
| VerticalSeparator | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | regression |
| SectionHeader | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | |
| GroupLabel | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | regression |
| ModuleHeader | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | regression |
| ParameterCell separator | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | via HorizontalSeparator |
| ModulationBusCell separator | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | via HorizontalSeparator |

**Pass criteria:** no visibly thick/blurry hairlines; 200 % shows 2 px physical stroke on Retina; no vertical drift vs controls above.

### AC 6 — Verify

- [x] `cmake --build Builds/macOS` — BUILD SUCCEEDED
- [x] Unit tests pass (`Matrix-Control_Tests`) — 1 pre-existing MIDI activity failure unrelated to GUI
- [x] Grep proofs in Completion Notes

## Tasks / Subtasks

- [x] **NumberBox dot baseline** (from U-2 code review) — `calculateDotPosition()` uses scaled font metrics

- [x] **Inventory** (AC: #1)
  - [x] Run grep commands from AC 1; list any remaining manual stroke scaling in `paint()`
  - [x] Record results in Completion Notes

- [x] **HorizontalSeparator** (AC: #2, #3)
  - [x] Migrate `paint()` to ScaledDrawing + centred `fillRect`
  - [x] Keep `kLineThickness_ = 1` design constant

- [x] **SectionHeader** (AC: #2, #3)
  - [x] Replace manual line height scaling with ScaledDrawing
  - [x] Preserve left/right line geometry relative to cached text width

- [x] **Display curve strokes** (AC: #2)
  - [x] EnvelopeDisplay + TrackGeneratorDisplay: ScaledDrawing for `curveLineThickness`
  - [x] Do not change hit-zone math in this story

- [x] **Popup border inset** (AC: #2) — only if grep shows inconsistency
  - [x] ScrollablePopupMenu: align inset with ScaledDrawing policy

- [x] **Manual UAT** (AC: #5)
  - [x] Guillaume sign-off @ 7 presets (focus 50 / 150 / 200 %)

- [x] **Verify** (AC: #6)

## Dev Notes

### Depends on

| Story | Status | Requirement |
|-------|--------|-------------|
| **U-2** | done | Layout/bounds scaling, Factory injection, widget inventory |
| **U-0b** | done | No Design* in widgets |

### Out of scope

| Item | Story |
|------|-------|
| Panel placement / column gaps | U-5…U-9 |
| New ScaledDrawing helpers (unless 3+ identical 5-line blocks) | YAGNI per CONVENTIONS |
| Font weight / typography | done |
| Envelope/Track interactive editing | Epic 10 |

### Reference pattern (HorizontalSeparator target)

Mirror `VerticalSeparator.cpp`:

```cpp
const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
const float lineThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
    static_cast<float>(kLineThickness_),
    uiScale_,
    systemDisplayScale,
    ScaledDrawing::StrokeSnapPolicy::kRound);

auto line = bounds;
line.setHeight(lineThickness);
line.setCentreY(bounds.getCentreY());
g.fillRect(line);
```

### D-013 policy (decision log)

- Presets: 50–200 % prod
- Hairline: **1 physical px** @ 50–150 % inclusive; **2 physical px** @ 200 % on Retina
- Implemented in `ScaledDrawing::snappedStrokeThicknessFromDesign` — do not duplicate logic

### Architecture compliance

- **CONVENTIONS.md** authoritative — design before code, methods ≤ 15 lines, no magic numbers
- **English only** in source
- **No AffineTransform** on widget subtrees
- **No Design* includes** in widgets

### Previous story intelligence (U-2)

- Guillaume validated **layout scaling** @ all presets (2026-06-09) — prod GUI signed off for bounds/gaps
- `ModulationBusCell::getHeight()` override removed — separator placement fixed
- Most atomic controls already use ScaledDrawing for borders
- Paint hairlines explicitly deferred from U-2 to this story

### Git baseline

Review U-2 implementation before coding: commit `5a4c988` (*Adopt fixed design-pixel UI scaling across panels and widgets*).

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Completion Notes List

**AC1 grep proofs (2026-06-11):**
- `grep 'std::max(1.0f.*uiScale_' Source/GUI/Widgets/` → **0 hits** (was 5: HorizontalSeparator, SectionHeader×2, EnvelopeDisplay, TrackGeneratorDisplay, ScrollablePopupMenu thumb)
- `grep 'snappedStrokeThicknessFromDesign' Source/GUI/Widgets/HorizontalSeparator.cpp` → **1 hit** (line 35)

**Migrations:**
- `HorizontalSeparator`: `drawLine` → centred `fillRect` + `snappedStrokeThicknessFromDesign` (`kRound`)
- `SectionHeader`: shared `lineThickness` in `drawLines`, centred via `setY(centreY - thickness/2)` (JUCE `Rectangle<float>` has no `setCentreY`)
- `EnvelopeDisplay` / `TrackGeneratorDisplay`: curve stroke via `snappedStrokeThicknessFromDesign` (`kRound`); `drawLine` retained for polyline
- `ScrollablePopupMenu`: border inset in `resized()` + `show()` aligned with `paint()`; thumb inset via `logicalInsetPixelsFromDesign`

**Build:** `cmake --build Builds/macOS` — BUILD SUCCEEDED
**Tests:** 1 pre-existing failure in MIDI activity recording (unrelated to this story)

**Manual UAT (2026-06-17):** Guillaume sign-off @ 50, 75, 100, 125, 150, 175, 200 % — D-013 hairlines pass (no thick/blurry strokes; 2 px physical @ 200 % Retina).

### File List

- Source/GUI/Widgets/HorizontalSeparator.cpp
- Source/GUI/Widgets/SectionHeader.cpp
- Source/GUI/Widgets/SectionHeader.h
- Source/GUI/Widgets/EnvelopeDisplay.cpp
- Source/GUI/Widgets/TrackGeneratorDisplay.cpp
- Source/GUI/Widgets/ScrollablePopupMenu.cpp

### Review Findings

- [x] [Review][Patch] ScrollablePopupMenu border uses `kRound` instead of `kFloor` (AC 4) [`ScrollablePopupMenu.cpp:210,223,428`] — fixed in code review

- [x] [Review][Defer] `MultiColumnPopupMenu` manual border calc bypasses ScaledDrawing [`MultiColumnPopupMenu.cpp`] — deferred, pre-existing, out of AC 2 scope
- [x] [Review][Defer] ScrollablePopupMenu thumb inset baked at ctor with `displayScale` fallback 1.0 [`ScrollablePopupMenu.cpp:259-263`] — deferred, needs CustomScrollBar API to refresh; cosmetic on Retina
- [x] [Review][Defer] EnvelopeDisplay / TrackGeneratorDisplay drag handlers lack zero-bounds guard mid-gesture — deferred, pre-existing, out of story scope
- [x] [Review][Defer] SectionHeader `contentArea` height can exceed component height at high uiScale — deferred, pre-existing layout (U-2)
- [x] [Review][Defer] `systemDisplayScaleForComponent` in `paint()` per frame — deferred, established pattern (VerticalSeparator, Button)
