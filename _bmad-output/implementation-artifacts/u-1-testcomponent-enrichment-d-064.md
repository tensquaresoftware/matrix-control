---
organization: Ten Square Software
project: Matrix-Control
title: Story U-1 — TestComponent Enrichment (D-064)
author: BMad Agent
status: review
baseline_commit: 5a4c988
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md (D-064, D-063)
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - Source/GUI/Tests/TestComponent.cpp
  - Source/GUI/Tests/TestButtons.cpp
  - Source/GUI/Tests/TestSliders.cpp
  - CONVENTIONS.md
created: 2026-06-10
updated: 2026-06-10
---

# Story U.1: TestComponent Enrichment (D-064)

Status: review

<!-- Debug sandbox (D-064). Parallel with U-2b — helps hairline UAT but does not block it. Release exclusion = U-10 (D-063). -->

## Story

As a developer,
I want TestComponent to showcase every custom widget at all seven UI scale presets side by side,
so that I can validate sizing and paint in isolation before prod panel sign-off (D-064).

## Acceptance Criteria

### AC 1 — Reference pattern (brownfield, do not reinvent)

**Given** existing `TestButtons` and `TestSliders` in `Source/GUI/Tests/`
**When** a new widget page is added
**Then** it follows the same layout model:

1. Outer `TestXxx` component owns **7 column panels** (`kColumnSpecs_`: 50 % … 200 %).
2. Each column is a private `XxxScalePanel` : label scale + representative widget instances.
3. Every custom widget receives `setUiScale(columnScale)` matching its column.
4. Design dimensions @ 100 % come from `DimensionFactory` / `WidgetFactory` where the prod widget needs injected structs — **not** from `Design*` headers in test code.
5. Horizontal row layout with gap `kGap_ * kPanelGapMultiplier_` (same as `TestButtons`).

### AC 2 — Widget selector wiring

**Given** `TestComponent::populateWidgetSelector()` already lists widget names
**When** the user picks a widget in the UI Elements sandbox
**Then** exactly one `TestXxx` child is visible; all others hidden (`updateVisibleTests()` pattern).
**And** selection persists via `PluginIDs::Settings::kTestWidget` (existing behaviour).

| Selector item | `TestWidgetType` id | Test page | Status @ story start |
|---------------|---------------------|-----------|----------------------|
| Button | 1 | `TestButtons` | **done** |
| Toggle | 2 | `TestToggles` | backlog |
| Slider | 3 | `TestSliders` | **done** |
| ComboBox | 4 | `TestComboBoxes` | backlog |
| NumberBox | 5 | `TestNumberBoxes` | backlog |
| Label | 6 | `TestLabels` | backlog |
| GroupLabel | 7 | `TestGroupLabels` | backlog |
| ModuleHeader | 8 | `TestModuleHeaders` | backlog |
| SectionHeader | 9 | `TestSectionHeaders` | backlog |
| EnvelopeDisplay | 10 | `TestEnvelopeDisplays` | backlog |
| TrackGeneratorDisplay | 11 | `TestTrackGeneratorDisplays` | backlog |
| PatchNameDisplay | 12 | `TestPatchNameDisplays` | backlog |

### AC 3 — Additional pages (epic minimum, not in selector yet)

Add selector entries + pages for:

| Widget | Suggested test class | Notes |
|--------|---------------------|-------|
| HorizontalSeparator | `TestHorizontalSeparators` | D-013 hairline validation — pairs with U-2b |
| VerticalSeparator | `TestVerticalSeparators` | inject `SeparatorDimensions` from Factory |
| ActivityLed | `TestActivityLeds` | on/off states per column |
| PeakIndicator | `TestPeakIndicators` | level steps per column |
| ParameterCell | `TestParameterCells` | slider + combobox rows; needs `WidgetFactory` + APVTS stub |
| ModulationBusCell | `TestModulationBusCells` | one bus row; Factory + APVTS |
| ModulationBusHeader | `TestModulationBusHeaders` | header band only |
| Popup menu sample | `TestPopupMenus` | open ComboBox popup @ each scale (manual check OK) |

Update `kLastComboItemId_` in `TestComponent.cpp` when selector grows.

### AC 4 — Scroll / viewport

**Given** 7 columns × widest widget (e.g. ModulationBusCell 268 px design @ 200 %)
**When** sandbox area is narrower than total content width
**Then** test area scrolls horizontally (reuse `juce::Viewport` or equivalent — avoid clipping columns).

### AC 5 — Skin propagation

**When** user changes Skin in settings
**Then** visible `TestXxx` page refreshes looks (`setSkin` / reconstruct looks from `ISkin`) without restart.

### AC 6 — Build & debug scope

- [ ] `cmake --build Builds/macOS` — BUILD SUCCEEDED (Debug)
- [ ] Manual smoke: open UI Elements → each selector entry shows 7 columns without crash
- [ ] **No** requirement to exclude Test sources from release in this story (U-10 / D-063)

### AC 7 — Out of scope (explicit)

| Item | Story |
|------|-------|
| Prod panel layout sign-off | U-3…U-9 |
| Release removal of TestComponent | U-10 |
| UI Scale presets > 200 % | D-065 study only — document in Dev Notes if experimented |
| Automated screenshot tests | manual UAT only |

## Tasks / Subtasks

### Phase A — Infrastructure

- [ ] **Shared scale constants** (AC: #1) — optional `TestScaleColumns.h` with `kColumnSpecs_` if duplication across 3+ files becomes painful (YAGNI until then)
- [ ] **Viewport** (AC: #4) — wrap test content area below widget selector
- [ ] **TestComponent router** (AC: #2) — generalize `updateVisibleTests()` for all `TestXxx` children

### Phase B — Simple atomics (mirror Button/Slider effort)

- [ ] `TestToggles` — INIT / on-off states
- [ ] `TestLabels` — default + centred (`LabelStyle::Centered` sample)
- [ ] `TestComboBoxes` — standard + button-like style if applicable
- [ ] `TestNumberBoxes` — value + editable dot variant

### Phase C — Headers & separators

- [ ] `TestGroupLabels`
- [ ] `TestModuleHeaders` — blue + orange variants
- [ ] `TestSectionHeaders` — blue + orange variants
- [ ] `TestHorizontalSeparators` — **priority for U-2b UAT**
- [ ] `TestVerticalSeparators`

### Phase D — Indicators & displays

- [ ] `TestActivityLeds` / `TestPeakIndicators`
- [ ] `TestPatchNameDisplays`
- [ ] `TestEnvelopeDisplays` / `TestTrackGeneratorDisplays` — static preview values OK (no APVTS drag required)

### Phase E — Composites & popups

- [ ] `TestParameterCells` — slider row + combobox row
- [ ] `TestModulationBusHeaders` + `TestModulationBusCells`
- [ ] `TestPopupMenus` — ComboBox with open popup per column
- [ ] Extend `populateWidgetSelector()` for AC 3 widgets

### Phase F — Verify

- [ ] Build + manual smoke (AC: #6)
- [ ] Update `TestComponent` enum `TestWidgetType` ids consistently (no gaps if reordering)

## Dev Notes

### Depends on

| Story | Status | Requirement |
|-------|--------|-------------|
| **U-0b** | done | `DimensionFactory` / `WidgetFactory` for injected dimensions |
| **U-2** | done | Widget `setUiScale` / layout patterns stable |
| **U-2b** | ready-for-dev | Hairline pages benefit from `TestHorizontalSeparators` — can land in either order |

### Brownfield files (read first)

| File | Role |
|------|------|
| `Source/GUI/Tests/TestComponent.{h,cpp}` | Selector + visibility router |
| `Source/GUI/Tests/TestButtons.{h,cpp}` | **Reference** 7-column pattern |
| `Source/GUI/Tests/TestSliders.{h,cpp}` | **Reference** 7-column pattern |
| `Source/GUI/PluginEditor.cpp` | `layoutUiElementsTestComponent()` bounds |
| `Source/GUI/Factories/DimensionFactory.cpp` | Dimension structs for composites |

### APVTS stub for composites

`TestModulationBusCells` / `TestParameterCells` need a live `juce::AudioProcessorValueTreeState`. Reuse the editor's APVTS (`PluginEditor` already passes `settingsState` — use `pluginProcessor.getApvts()` via ctor injection if needed). Do **not** duplicate descriptor data.

### Suggested implementation order

1. Viewport + router (unblocks wide tables)
2. Simple atomics (Toggle → Label → ComboBox → NumberBox)
3. Separators (supports U-2b visual checks)
4. Headers → displays → composites

### Architecture compliance

- **CONVENTIONS.md** — methods ≤ 15 lines, English source, no magic numbers (use named constants like `TestButtons`)
- **No `Design*` includes** in `Source/GUI/Tests/`
- **No AffineTransform** scaling on test subtree
- Register new `.cpp` in `CMakeLists.txt` under GUI sources

### Manual UAT checklist (per page)

| Page | 50 | 75 | 100 | 125 | 150 | 175 | 200 | Notes |
|------|:--:|:--:|:---:|:---:|:---:|:---:|:---:|-------|
| (fill during implementation) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | |

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking (Cursor Agent)

### Completion Notes List

- Added `TestScaleColumns.h` shared helpers (7 column specs, `scaledSize`, row width).
- `TestComponent` now wraps test pages in a horizontal `juce::Viewport`, routes all 20 selector entries, and propagates skin via `setSkin()` (wired from `PluginEditor::updateSkin()`).
- Created 18 new `TestXxx` pages following the `TestButtons`/`TestSliders` 7-column pattern; dimensions from `DimensionFactory` / `WidgetFactory` (no `Design*` in test code).
- Composites (`TestParameterCells`, `TestModulationBusCells`) use editor APVTS + `WidgetFactory`.
- `cmake --build Builds/macOS` — BUILD SUCCEEDED (Debug, after reconfigure).

### File List

- `Source/GUI/Tests/TestScaleColumns.h` (new)
- `Source/GUI/Tests/TestActivityLeds.{h,cpp}` (new)
- `Source/GUI/Tests/TestComboBoxes.{h,cpp}` (new)
- `Source/GUI/Tests/TestEnvelopeDisplays.{h,cpp}` (new)
- `Source/GUI/Tests/TestGroupLabels.{h,cpp}` (new)
- `Source/GUI/Tests/TestHorizontalSeparators.{h,cpp}` (new)
- `Source/GUI/Tests/TestLabels.{h,cpp}` (new)
- `Source/GUI/Tests/TestModulationBusCells.{h,cpp}` (new)
- `Source/GUI/Tests/TestModulationBusHeaders.{h,cpp}` (new)
- `Source/GUI/Tests/TestModuleHeaders.{h,cpp}` (new)
- `Source/GUI/Tests/TestNumberBoxes.{h,cpp}` (new)
- `Source/GUI/Tests/TestParameterCells.{h,cpp}` (new)
- `Source/GUI/Tests/TestPatchNameDisplays.{h,cpp}` (new)
- `Source/GUI/Tests/TestPeakIndicators.{h,cpp}` (new)
- `Source/GUI/Tests/TestPopupMenus.{h,cpp}` (new)
- `Source/GUI/Tests/TestSectionHeaders.{h,cpp}` (new)
- `Source/GUI/Tests/TestToggles.{h,cpp}` (new)
- `Source/GUI/Tests/TestTrackGeneratorDisplays.{h,cpp}` (new)
- `Source/GUI/Tests/TestVerticalSeparators.{h,cpp}` (new)
- `Source/GUI/Tests/TestButtons.{h,cpp}` (updated: `setSkin`, preferred size, shared columns)
- `Source/GUI/Tests/TestSliders.{h,cpp}` (updated)
- `Source/GUI/Tests/TestComponent.{h,cpp}` (updated: viewport, router, 20 pages)
- `Source/GUI/PluginEditor.cpp` (updated: APVTS injection, `setSkin`)
- `CMakeLists.txt` (updated: 18 new test sources)

### Review Findings

(pending)
