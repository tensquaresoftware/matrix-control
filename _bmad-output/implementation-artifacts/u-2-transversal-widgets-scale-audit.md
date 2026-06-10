---
organization: Ten Square Software
project: Matrix-Control
title: Story U-2 — Transversal Widgets Scale Audit
author: BMad Agent
status: in-progress
baseline_commit: c07fe2597b11135040eab3c0191a4485c658c5fd
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-0b-factory-dimension-registry-and-descriptor-decoupling.md
  - Source/GUI/Layout/ScaledLayout.h
  - Source/GUI/Layout/ScaledDrawing.h
  - Source/GUI/Layout/WidgetDimensions.h
  - Source/GUI/Widgets/
created: 2026-06-09
updated: 2026-06-09
---

# Story U.2: Transversal Widgets Scale Audit

Status: ready-for-dev

<!-- Requires U-0b (Factory injection complete). Blocks U-6…U-9 panel audits that depend on widget geometry. U-1 (TestComponent) is parallel — use prod panels or partial TestComponent if available for isolation. -->

## Story

As a performer,
I want every custom widget to respect design dimensions and ScaledDrawing at all UI scale presets,
so that controls look crisp and aligned inside any panel (D-013, UX-DR8).

## Acceptance Criteria

### AC 1 — No Design* in widgets (regression guard)

1. **`grep '#include.*Design'`** in `Source/GUI/Widgets/` → **zero hits** (already true post-U-0b — must stay zero).
2. **`grep PluginDesignDimensions`** in `Source/GUI/Widgets/` → **zero hits**.
3. Widgets receive dimensions via ctor-injected scalars or `*Dimensions` structs from Factory/panel callers — never re-read Design* at runtime.

### AC 2 — ScaledLayout + ScaledDrawing consistency

1. **Layout geometry** in `resized()` (or parent `setBounds` chains) uses `ScaledLayout::scaledInt` / distribute helpers — not ad-hoc `roundToInt(design * uiScale)` where a helper exists.
2. **Stroke thickness, insets, hairlines** in `paint()` use `ScaledDrawing::snappedStrokeThicknessFromDesign` (D-013 policy: crisp 1 px @ 50–150 %, 2 px @ 200 % on Retina).
3. **No `AffineTransform`** on widget subtrees.
4. **No raw pixel assumptions tied to 100 % only** — every design constant multiplied by `uiScale_` or passed through ScaledLayout/ScaledDrawing helpers.
5. **`setUiScale(float)`** on every custom widget propagates to children in composites (`ParameterCell`, `ModulationBusCell`, display panels).

### AC 3 — Brownfield widget inventory (minimum scope)

For **each** widget below, code review + fix gaps; manual UAT @ all seven presets (§ Manual UAT):

| Widget | Files | Priority gaps (pre-audit) |
|--------|-------|-------------------------|
| Button | `Button.cpp/.h` | Reference implementation for ScaledDrawing borders |
| Slider | `Slider.cpp/.h` | Reference for ScaledDrawing insets |
| ComboBox | `ComboBox.cpp/.h` | Border not via ScaledDrawing; `kVerticalMargin_=4` unscaled → popup position |
| NumberBox | `NumberBox.cpp/.h` | `updateTextWidthCache()` uses unscaled font; `kDotXOffset_` unscaled |
| Toggle | `Toggle.cpp/.h` | Border via manual `* uiScale_`, not ScaledDrawing |
| Label | `Label.cpp/.h` | Low risk — verify font scaling |
| GroupLabel | `GroupLabel.cpp/.h` | `kTextSpacing_=8`, `kLineThickness_=1` local constants |
| ParameterCell | `ParameterCell.cpp/.h` | Good ScaledLayout base; `getTotalHeight()` returns design px |
| ModuleHeader | `ModuleHeader.cpp/.h` | TitleOnly path lacks struct; layout uses `roundToInt` not ScaledLayout |
| ModulationBusCell | `ModulationBusCell.cpp/.h` | `kGap_=5` outside struct; drag threshold `4px` unscaled |
| ModulationBusHeader | `ModulationBusHeader.cpp/.h` | **P0** — column widths 15/65/65/110 hardcoded, no Factory struct |
| SectionHeader | `SectionHeader.cpp/.h` | Local constants 20/12/20/8 |
| HorizontalSeparator | `HorizontalSeparator.cpp/.h` | Acceptable if UAT passes |
| VerticalSeparator | `VerticalSeparator.cpp/.h` | Paddings 4/2 hardcoded outside `SeparatorDimensions` |
| ActivityLed | `ActivityLed.cpp/.h` | ScaledDrawing OK; no `setSize` at ctor; no struct in `WidgetDimensions.h` |
| PeakIndicator | `PeakIndicator.cpp/.h` | Same as ActivityLed |
| PatchNameDisplay | `PatchNameDisplay.cpp/.h` | Panel has `PatchNameDisplayDimensions`; widget only gets w/h |
| EnvelopeDisplay | `EnvelopeDisplay.cpp/.h` | Many local `k*` constants; `DisplayBandDimensions` not passed to widget |
| TrackGeneratorDisplay | `TrackGeneratorDisplay.cpp/.h` | Same family as EnvelopeDisplay |

**Popup subsystem (AC 3 extension):**

| File | Priority gaps |
|------|---------------|
| `PopupMenuBase.cpp/.h` | `kItemHeight_=20`, borders — verify scaling path |
| `PopupMenuRenderer.cpp/.h` | Manual border `1.0f * uiScale` |
| `MultiColumnPopupMenu.cpp/.h` | Item height, separator — verify @ 175 % / 200 % |
| `ScrollablePopupMenu.cpp/.h` | **`kMinThumbHeightPx_=20` not scaled** |
| `PopupMenuPositioner.cpp/.h` | Uses `ComboBox::getVerticalMargin()` = **4 px fixed** |

### AC 4 — Dimension struct completeness

1. Move remaining **magic numbers** into `WidgetDimensions.h` + `DimensionFactory` population from `DesignAtoms` / `DesignRecipes`:
   - `ModulationBusHeaderDimensions` (column widths, padding, line thickness) — **new struct**
   - `interControlGap` + `reorderDragThreshold` on `ModulationBusCellDimensions`
   - `ModuleHeaderDimensions`: add `textAreaHeight`, `lineThickness` if missing
   - `ActivityLedDimensions` / reuse atom size (12×12) — optional small struct
   - Popup constants struct or fields on `ComboBoxDimensions` (`itemHeight`, `verticalMargin`, `scrollbarWidth`, `minThumbHeight`, `maxScrollHeight`)
   - Display padding/hit-zone fields on `DisplayBandDimensions` passed to Envelope/TrackGenerator ctors
2. **`grep` widget `.h/.cpp` for `inline constexpr static int k[A-Z]`** — each hit either maps to an injected field or is documented as non-dimensional logic (e.g. `kColumnThreshold_=10`).

### AC 5 — Manual UAT (seven presets)

1. Complete checklist below for **each widget type** (prod UI or Debug TestComponent if U-1 landed).
2. **Build:** Standalone Debug · **Skin:** Black primary + Cream spot-check @ 100 %.
3. **Tester:** Guillaume · Sign-off date recorded in Completion Notes.

### AC 6 — Build & regression

1. Standalone Debug + unit tests compile and pass.
2. **`grep AffineTransform`** in `Source/GUI/Widgets/` → zero hits.
3. Launch @ **100 % UI scale** — no visual regression vs pre-U-2 (layout placement is panel scope; widget-internal paint may improve).
4. Completion Notes: paste grep proofs + UAT summary table.

## Tasks / Subtasks

- [x] **Popup scaling fixes** (AC: #2, #3) — highest user-visible impact
  - [x] Scale `ComboBox::kVerticalMargin_` through `PopupMenuPositioner`
  - [x] Scale `ScrollablePopupMenu::kMinThumbHeightPx_`
  - [x] Adopt ScaledDrawing for popup borders in `PopupMenuRenderer`

- [x] **ModulationBusHeader ↔ ModulationBusCell alignment** (AC: #3, #4)
  - [x] Add `ModulationBusHeaderDimensions` + Factory fill
  - [x] Inject into `ModulationBusHeader` ctor; remove local column constants
  - [x] Add `interControlGap` to `ModulationBusCellDimensions`; remove `kGap_=5`

- [x] **ScaledDrawing migration — atomic controls** (AC: #2)
  - [x] ComboBox, Toggle, NumberBox, PatchNameDisplay borders
  - [x] NumberBox: scale dot offset + text width cache with scaled font

- [x] **Display widgets** (AC: #3, #4)
  - [x] Pass display padding/hit constants via `DisplayBandDimensions` (or sub-struct)
  - [x] EnvelopeDisplay + TrackGeneratorDisplay: verify hit zones @ 50 % and 200 %

- [x] **Headers & separators** (AC: #3, #4)
  - [x] ModuleHeader: ScaledLayout in `resized()`; extend struct for title-only path
  - [x] SectionHeader, GroupLabel: optional ScaledDrawing for 1 px lines
  - [x] VerticalSeparator: move paddings into `SeparatorDimensions`

- [x] **Indicators** (AC: #3)
  - [x] ActivityLed / PeakIndicator: document or add dimension struct; optional `setSize` at ctor

- [x] **ParameterCell polish** (AC: #2)
  - [x] Rename or fix `getTotalHeight()` to avoid returning unscaled design px to callers

- [ ] **Manual UAT** (AC: #5)
  - [ ] Run seven-preset checklist per widget row in § Manual UAT
  - [ ] Capture screenshots @ 100 % and spot @ 50 % / 200 %

- [x] **Verify** (AC: #6)
  - [x] Build + tests
  - [x] Grep proofs in Completion Notes

## Manual UAT

Copy per widget type (or one consolidated sheet with widget column). Reference dimensions @ 100 %: [`u-0-zone-dimension-tables.md`](u-0-zone-dimension-tables.md) § Widgets transversaux.

| UI Scale | 50 % | 75 % | 100 % | 125 % | 150 % | 175 % | 200 % |
|----------|:----:|:----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| No clipped controls | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Gaps match Figma (±0 @ 100 %) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Text baseline / truncation OK | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Per-widget sign-off table (fill during UAT):**

| Widget | 50 | 75 | 100 | 125 | 150 | 175 | 200 | Notes |
|--------|:--:|:--:|:---:|:---:|:---:|:---:|:---:|-------|
| Button | | | | | | | | |
| Slider | | | | | | | | |
| ComboBox + popups | | | | | | | | |
| NumberBox | | | | | | | | |
| Toggle | | | | | | | | |
| Label | | | | | | | | |
| GroupLabel | | | | | | | | |
| ParameterCell | | | | | | | | |
| ModuleHeader | | | | | | | | |
| ModulationBusCell | | | | | | | | |
| ModulationBusHeader | | | | | | | | |
| SectionHeader | | | | | | | | |
| H/V Separator | | | | | | | | |
| ActivityLed | | | | | | | | |
| PeakIndicator | | | | | | | | |
| PatchNameDisplay | | | | | | | | |
| EnvelopeDisplay | | | | | | | | |
| TrackGeneratorDisplay | | | | | | | | |

## Dev Notes

### Depends on

| Story | Status | Requirement |
|-------|--------|-------------|
| **U-IDs** | done | Harmonized widget IDs |
| **U-0** | done | Design* SSOT + zone dimension tables |
| **U-0b** | done | Factory injection, zero Design in widgets/panels |
| **U-1** | backlog (parallel) | TestComponent helps isolation — not blocking |

### Recommended implementation order

1. **Popups** — fixes all ComboBox dropdowns globally.
2. **ModulationBusHeader** — unblocks U-7 Matrix Mod grid alignment.
3. **ScaledDrawing on atomics** — ComboBox, Toggle, NumberBox, PatchNameDisplay.
4. **Dimension struct expansion** — DimensionFactory + ctor wiring.
5. **Displays & headers** — lower coupling, can batch.
6. **UAT pass** — widget-by-widget sign-off.

### Current brownfield patterns (read before coding)

**Two scaling families coexist today:**

- **ScaledLayout** — only `ParameterCell`, `ModulationBusCell` in `resized()`.
- **ScaledDrawing** — `Button`, `Slider`, `ActivityLed`, `PeakIndicator` in `paint()`.
- **Manual** — most others: `std::max(1.0f, designConstant * uiScale_)`.

**Target for U-2:** unify on ScaledLayout for bounds math, ScaledDrawing for strokes — manual `* uiScale_` acceptable only for font heights and non-stroke fills until migrated.

**WidgetFactory** (`WidgetFactory.cpp`) creates Slider/ComboBox/Button with caller-supplied width/height. Composite widgets are built in panels with `DimensionFactory`-filled structs. Do **not** add Design includes to Factory beyond existing `DimensionFactory` / `WidgetDimensionRegistry`.

**Deferred from U-0b (do not fix here unless blocking U-2 UAT):**

- `WidgetFactory::createIntParameterSlider(parameterId, skin)` still reads `Design::Recipes::Slider::kStandard` — noted in U-0b review; full slider registry migration is a separate hardening pass.
- Suffix-based registry rules fragility — Path A accepted.

### Reference dimensions @ 100 % (U-0 tables)

| px @ 100 % | Design symbol | Widget |
|------------|---------------|--------|
| 92 × 16 | `Atoms::Widths::ParameterCell::kLabel` × `Atoms::Heights::kLabel` | ParameterCell label |
| 60 × 16 | `Atoms::Widths::ParameterCell::kControl` × `Atoms::Heights::kSlider` | ParameterCell control |
| 152 × 24 | `Recipes::ParameterCell::kWidth` × `kHeight` | ParameterCell row |
| 268 × 24 | `Recipes::ModulationBusCell::kWidth` × `kHeight` | ModulationBusCell |
| 20 × 20 | `Atoms::Widths::Button::kInit` × `Atoms::Heights::kButton` | Button |
| 8 | `Atoms::Heights::kHorizontalSeparator` | HorizontalSeparator |
| 4 / 8 / 12 | `Spacing::kStandard` / `kMedium` / `kLarge` | gaps |

Full table: [`u-0-zone-dimension-tables.md`](u-0-zone-dimension-tables.md).

### Key APIs

```cpp
// Layout — Source/GUI/Layout/ScaledLayout.h
ScaledLayout::scaledInt(designPx, uiScale);
ScaledLayout::distributeHeights(...);
ScaledLayout::distributeFixedDesignRowsWithRemainderOnLast(...);

// Drawing — Source/GUI/Layout/ScaledDrawing.h
ScaledDrawing::snappedStrokeThicknessFromDesign(
    designThickness, uiScale, systemDisplayScale, StrokeSnapPolicy::kRound);
ScaledDrawing::systemDisplayScaleForComponent(*this);
```

### Files to touch (expected)

| Area | Files |
|------|-------|
| Dimension structs | `WidgetDimensions.h`, `DimensionFactory.cpp` |
| Widgets | All files under `Source/GUI/Widgets/` in AC 3 table + popup files |
| Panel ctors | Call sites passing new structs: `MatrixModulationPanel`, `PatchEditDisplaysPanel`, `HeaderPanel`, module panels |
| Tests | Extend or add widget scaling unit tests if pure helpers extracted |
| CMake | Only if new `.cpp` test files |

### Out of scope

| Item | Story |
|------|-------|
| Panel-level placement / column gaps | U-5…U-9 |
| Header right cluster layout | U-3 |
| TestComponent pages for every widget | U-1 |
| Release sandbox removal | U-10 |
| `BodyPanel` originX float | U-5 |
| Interactive display APVTS refactor | Epic 10 |
| Factory slider default overload Design read | Future hardening (U-0b defer) |

### Testing standards

- Run unit tests: `Tests/Unit/` via CMake preset Standalone Debug.
- No new tests required unless extracting scalable helpers — prefer manual UAT for visual D-013 compliance.
- Do **not** add GUI dependencies to `Source/Core/`.

### Architecture compliance

- **Dependency direction:** GUI → Shared only; Core ↛ GUI.
- **No global AffineTransform** on prod UI tree.
- **Design SSOT:** only `DimensionFactory` / `WidgetDimensionRegistry` read Design* — widgets consume injected members.
- **English only** in source code comments/strings (display names stay in `PluginDisplayNames.h`).

### Previous story intelligence (U-0b)

- All panel/widget `resized()` paths migrated to injected member dimensions — **structural injection done**; U-2 is **paint/scale audit**, not another migration.
- `ModulationBusCellDimensions.h` duplicate removed — use unified `WidgetDimensions.h`.
- Registry guard test validates button widths — not widget internal geometry.
- Smoke @ 100 % passed post-U-0b — pixel-perfect sign-off explicitly deferred to U-2…U-9.

### Git intelligence (recent commits)

| Commit | Relevance |
|--------|-----------|
| `91f3e1b` Sync U-0 implementation artifacts | Design* SSOT landed — dimension tables authoritative |
| `2156709` Epic U planning for dimension registry | Confirms U-2 follows U-0b |
| `3b9d3d3` UI dimensions brainstorming | Factory-only Design consumer principle |

### Latest tech (JUCE 8.0.12)

- Use existing project helpers — no new scaling libraries.
- `juce::Component::setBufferedToImage` — do not add for scale fixes.
- Font scaling: `font.withHeight(designHeight * uiScale_)` remains valid; pair with ScaledDrawing for strokes.
- Retina: ScaledDrawing handles physical pixel snapping; ScaledLayout keeps **logical** bounds at uiScale only (see ScaledDrawing.h comment L57–60).

### Project context reference

- GUI scaling rules: `_bmad-output/project-context.md` § GUI & UI Scale Rules
- Detailed strategy: `Documentation/Development/GUI/GUI-Scaling-Strategy.md`
- Epic canonical spec: `planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` § Story U-2

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

### Completion Notes List

- Popup layout dimensions centralized in `PopupMenuLayoutDimensions` + `ComboBox::setPopupLayoutDimensions()` (wired in `PluginEditor`). `getScaledVerticalMargin()` fixes positioner gap; scrollbar min-thumb scales with `uiScale_`; `PopupMenuRenderer` borders use `ScaledDrawing`.
- `ModulationBusHeaderDimensions` injected from `DimensionFactory`; column widths aligned with Design atoms. `ModulationBusCellDimensions` gains `interControlGap` + `reorderDragThreshold`.
- Atomic controls (ComboBox, Toggle, NumberBox, PatchNameDisplay) borders migrated to `ScaledDrawing`. NumberBox text cache and dot offset scale with `uiScale_`.
- `DisplayBandDimensions` expanded; EnvelopeDisplay/TrackGeneratorDisplay consume injected padding/hit-zone fields.
- ModuleHeader TitleOnly path receives `ModuleHeaderDimensions`; button layout uses `ScaledLayout`. GroupLabel 1 px lines use `ScaledDrawing`. VerticalSeparator paddings in `SeparatorDimensions`.
- `IndicatorDimensions` + `ActivityLed`/`PeakIndicator` `setSize` at ctor. `getTotalHeight()` renamed `getDesignRowHeight()`.
- **AC6 grep proofs:** `#include.*Design` in Widgets → 0; `PluginDesignDimensions` → 0; `AffineTransform` → 0.
- **Build:** `Matrix-Control_Standalone` + `Matrix-Control_Tests` — BUILD SUCCEEDED, all tests pass.
- **Manual UAT (AC5):** Pending Guillaume sign-off @ 50–200 % (7 presets). Checklist in § Manual UAT ready to fill.

### File List

- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Layout/WidgetDimensions.h
- Source/GUI/Layout/PanelDimensions.h
- Source/GUI/Factories/DimensionFactory.cpp
- Source/GUI/Widgets/ComboBox.h
- Source/GUI/Widgets/ComboBox.cpp
- Source/GUI/Widgets/PopupMenuPositioner.cpp
- Source/GUI/Widgets/PopupMenuBase.h
- Source/GUI/Widgets/PopupMenuBase.cpp
- Source/GUI/Widgets/PopupMenuRenderer.h
- Source/GUI/Widgets/PopupMenuRenderer.cpp
- Source/GUI/Widgets/ScrollablePopupMenu.h
- Source/GUI/Widgets/ScrollablePopupMenu.cpp
- Source/GUI/Widgets/MultiColumnPopupMenu.cpp
- Source/GUI/Widgets/ModulationBusHeader.h
- Source/GUI/Widgets/ModulationBusHeader.cpp
- Source/GUI/Widgets/ModulationBusCell.h
- Source/GUI/Widgets/ModulationBusCell.cpp
- Source/GUI/Widgets/Toggle.cpp
- Source/GUI/Widgets/NumberBox.cpp
- Source/GUI/Widgets/PatchNameDisplay.cpp
- Source/GUI/Widgets/ModuleHeader.h
- Source/GUI/Widgets/ModuleHeader.cpp
- Source/GUI/Widgets/VerticalSeparator.h
- Source/GUI/Widgets/VerticalSeparator.cpp
- Source/GUI/Widgets/GroupLabel.cpp
- Source/GUI/Widgets/EnvelopeDisplay.h
- Source/GUI/Widgets/EnvelopeDisplay.cpp
- Source/GUI/Widgets/TrackGeneratorDisplay.h
- Source/GUI/Widgets/TrackGeneratorDisplay.cpp
- Source/GUI/Widgets/ActivityLed.cpp
- Source/GUI/Widgets/PeakIndicator.cpp
- Source/GUI/Widgets/ParameterCell.h
- Source/GUI/Widgets/ParameterCell.cpp
- Source/GUI/PluginEditor.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp

## Change Log

- 2026-06-09: Story created (create-story) — ready-for-dev
- 2026-06-09: U-2 code implementation — popup scaling, dimension structs, ScaledDrawing migration, display/header/separator fixes; build+tests green; Manual UAT pending Guillaume
