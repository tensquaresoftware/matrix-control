---
organization: Ten Square Software
project: Matrix-Control
title: Story U-1 — TestComponent Enrichment (D-064)
author: BMad Agent
status: done
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
updated: 2026-06-16
---

# Story U.1: TestComponent Enrichment (D-064)

Status: done

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

- [x] `cmake --build Builds/macOS` — BUILD SUCCEEDED (Debug)
- [x] Manual smoke: open UI Elements → each selector entry shows 7 columns without crash
- [x] **No** requirement to exclude Test sources from release in this story (U-10 / D-063)

### AC 7 — Out of scope (explicit)

| Item | Story |
|------|-------|
| Prod panel layout sign-off | U-3…U-9 |
| Release removal of TestComponent | U-10 |
| UI Scale presets > 200 % | D-065 study only — document in Dev Notes if experimented |
| Automated screenshot tests | manual UAT only |

## Tasks / Subtasks

### Phase A — Infrastructure

- [x] **Shared scale constants** (AC: #1) — `TestScaleColumns.h`
- [x] **Viewport** (AC: #4) — wrap test content area below widget selector
- [x] **TestComponent router** (AC: #2) — generalize `updateVisibleTests()` for all `TestXxx` children

### Phase B — Simple atomics (mirror Button/Slider effort)

- [x] `TestToggles` — INIT / on-off states
- [x] `TestLabels` — default + centred (`LabelStyle::Centered` sample)
- [x] `TestComboBoxes` — standard + button-like style if applicable
- [x] `TestNumberBoxes` — value + editable dot variant

### Phase C — Headers & separators

- [x] `TestGroupLabels`
- [x] `TestModuleHeaders` — blue + orange variants
- [x] `TestSectionHeaders` — blue + orange variants
- [x] `TestHorizontalSeparators` — **priority for U-2b UAT**
- [x] `TestVerticalSeparators`

### Phase D — Indicators & displays

- [x] `TestActivityLeds` / `TestPeakIndicators`
- [x] `TestPatchNameDisplays`
- [x] `TestEnvelopeDisplays` / `TestTrackGeneratorDisplays` — static preview values OK (no APVTS drag required)

### Phase E — Composites & popups

- [x] `TestParameterCells` — slider row + combobox row
- [x] `TestModulationBusHeaders` + `TestModulationBusCells`
- [x] `TestPopupMenus` — ComboBox with open popup per column
- [x] Extend `populateWidgetSelector()` for AC 3 widgets

### Phase F — Verify

- [x] Build + manual smoke (AC: #6)
- [x] Update `TestComponent` enum `TestWidgetType` ids consistently (no gaps if reordering)

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
- Manual UAT smoke (2026-06-16): all 20 selector pages, 7 columns each, skin toggle, viewport scroll — **OK** (Guillaume).
- Code review (2026-06-16): 14 patches applied across groups 1–4; group 5 clean.

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

#### Group 1 — Infrastructure (2026-06-16)

- [x] [Review][Patch] **Replace content-zoom AffineTransform with bounds-based rescaling** — Decision: **B** (keep Cmd++/−/0 zoom; remove `setTransform`; rescale `testContentHost_` size and child preferred dimensions instead). [`TestComponent.cpp:applyContentZoom`]

- [x] [Review][Patch] **Clamp `bandWidthForTestRow` to non-negative** [`TestScaleColumns.h:39`]
- [x] [Review][Patch] **Guard debug header row against horizontal overflow** [`TestComponent.cpp:layoutHeaderControls`]
- [x] [Review][Patch] **Remove redundant `resized()` after `setBounds()`** [`TestComponent.cpp:layoutTestContentHost`]
- [x] [Review][Patch] **Viewport: horizontal scroll only (`setScrollBarsShown(true, false)`)** [`TestComponent.cpp` ctor]
- [x] [Review][Patch] **Restore skin-aware label for widget selector header** [`TestComponent.cpp:createHeaderControls`]
- [x] [Review][Patch] **Propagate `setSkin()` to `TestColourPickerButton` debug controls** [`TestComponent.cpp:setSkin`]
- [x] [Review][Patch] **Derive `kScaleFactorSum` from `kSpecs` at compile time** [`TestScaleColumns.h`]
- [x] [Review][Patch] **Defer `layoutTestContentHost` until component has bounds** [`TestComponent.cpp` ctor — viewport height 0 on first pass]

- [x] [Review][Defer] **CMake `TIMESTAMP` build date frozen at configure time** [`CMakeLists.txt`] — deferred, bundled non-U-1 change (versioning story)
- [x] [Review][Defer] **`MATRIX_CONTROL_PRERELEASE_SUFFIX` defaults to `"alpha"`** [`CMakeLists.txt`] — deferred, bundled non-U-1 change
- [x] [Review][Defer] **`juce_gui_basics` added to unit test target** [`CMakeLists.txt`] — deferred, headless CI risk from bundled CMake changes
- [x] [Review][Defer] **`restoreSettingsPanelFromState` skips HW latency in standalone** [`PluginEditor.cpp`] — deferred, settings consolidation story scope
- [x] [Review][Defer] **`refreshAudioFromCombo` fallback without populated header** [`PluginEditor.cpp`] — deferred, header-panel story scope
- [x] [Review][Defer] **VST3 helper `-Wno-deprecated-declarations` removal** [`CMakeLists.txt`] — deferred, unrelated build hygiene
- [x] [Review][Defer] **`juce_gui_extra` linked without documented rationale** [`CMakeLists.txt`] — deferred, About/popup story scope
- [x] [Review][Defer] **`std::function` heap alloc on every `layoutTestContentHost` pass** [`TestComponent.cpp`] — deferred, perf optimization not blocking UAT

#### Group 2 — Atomics (2026-06-16)

- [x] [Review][Patch] **TestSliders — remove spurious `kPadding_` from preferred size and layout** [`TestSliders.cpp`] — `getPreferredWidth/Height` add `2*kPadding_` but reference `TestButtons` does not; `layoutColumnPanels` offsets columns at `(kPadding_, kPadding_)` vs `(0, 0)`
- [x] [Review][Patch] **TestComboBoxes — scale label/column width must use `kScaleLabelColumnDesignWidth`** [`TestComboBoxes.cpp`] — uses `comboWidth_` for scale label and `getScaledColumnWidth`, inconsistent with all other atomics
- [x] [Review][Patch] **TestButtons/TestSliders — migrate panel gap to `TestScaleColumns` constants** [`TestButtons.cpp`, `TestSliders.cpp`] — `panelGap` still uses private `kGap_ * kPanelGapMultiplier_` in width/layout queries

- [x] [Review][Defer] **TestLabels/TestComboBoxes dimensions via `getBaseWidth/Height()` not Factory** — AC 1.4 borderline; static widget accessors are acceptable when no injected struct is required
- [x] [Review][Defer] **AC 2 selector wiring** — verified in Group 1 (`TestComponent` router diff)
- [x] [Review][Defer] **Duplicated `layoutColumnPanels` across six files** — accepted brownfield pattern until shared helper is needed (YAGNI per story)
- [x] [Review][Defer] **`labelLook` not stored as member in Toggle/NumberBox panels** — minor future skin-refresh risk; `rebuildPanels()` recreates today

#### Group 3 — Headers & Separators (2026-06-16)

- [x] [Review][Patch] **TestHorizontalSeparators — column width must be at least `kScaleLabelColumnDesignWidth`** [`TestHorizontalSeparators.cpp`] — scale label and `getScaledColumnWidth` use `separatorWidth_` only; when width &lt; 56 the "50%" label clips at low scales (same class of bug fixed in Group 2 ComboBoxes)

- [x] [Review][Defer] **TestVerticalSeparators separator wider than column** — `verticalStandardWidth` (24) &lt; `kScaleLabelColumnDesignWidth` (56); no overflow with current Factory dims
- [x] [Review][Defer] **`scaleLabel_` missing `setUiScale(scale_)`** — cross-cutting pattern; reference `TestButtons`/`TestSliders` also omit; defer unless global scale-label pass is scheduled
- [x] [Review][Defer] **`columnPanels_.clear()` before `removeAllChildren()`** — matches established rebuild pattern across all `TestXxx` pages; `unique_ptr` destruction removes children safely

#### Group 4 — Indicators & Displays (2026-06-16)

- [x] [Review][Patch] **`TestActivityLeds::tickLedDecay()` zeros both LEDs — destroys on/off contrast** [`TestActivityLeds.cpp`] — timer sets `ledOn_` to 0 every non-pulse frame; violates Phase D « on/off states per column ». Fix: pulse only `ledOff_`; `tickLedDecay` restores `ledOff_=0`, `ledOn_=1`

- [x] [Review][Defer] **Selector label « Led » vs « ActivityLed »** [`TestComponent.cpp`] — advisory AC 2 naming; widget renamed in prod (`ActivityLed` → `Led`)
- [x] [Review][Defer] **`TestPeakIndicators` column width vs indicator width** — `indicatorWidth_` = `activityLed.size` (&lt; 56); no overflow with Factory dims
- [x] [Review][Defer] **Timer interval integer truncation (1000/30)** — cosmetic timing drift in demo animation
- [x] [Review][Defer] **Inner panel `setSkin()` not called from parent `setSkin()`** — `rebuildPanels()` recreates widgets; dead but harmless
- [x] [Review][Defer] **Zero-dimension constructor guards** — Factory always supplies positive dims

#### Group 5 — Composites & Popups (2026-06-16)

- [x] [Review][Defer] **Dead inner-panel `setSkin()` on ParameterCell/ModulationBusCell** — outer `rebuildPanels()` recreates; same pattern as Group 4
- [x] [Review][Defer] **All 7 ModulationBusCell columns bind to bus 0 APVTS** — intentional for scale UAT; one bus row at seven UI scales
- [x] [Review][Defer] **All 7 ParameterCell columns share Dco1 Frequency/WaveSelect** — intentional scale-comparison sandbox
- [x] [Review][Defer] **`TestPopupMenus` placeholder item strings** — sufficient for popup layout smoke per AC 3
- [x] [Review][Defer] **`layoutColumnPanels()` at end of `rebuildPanels()` before bounds** — harmless; `resized()` relayouts
