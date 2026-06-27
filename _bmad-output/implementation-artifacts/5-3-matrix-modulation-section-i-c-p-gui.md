---
organization: Ten Square Software
project: Matrix-Control
title: Story 5.3 — Matrix Modulation Section I/C/P GUI
author: BMad Agent
status: review
baseline_commit: 494bb5e2e63c57ddc0bb18d0ca44c6e9d8cb5d1
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/sprint-change-proposal-2026-06-18.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/5-1-clipboardservice-compatibility-matrix.md
  - implementation-artifacts/5-2-module-copy-paste-enable-and-gray-rules.md
  - project-context.md
created: 2026-06-18
updated: 2026-06-18
---

# Story 5.3: Matrix Modulation Section I/C/P GUI

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want Init/Copy/Paste buttons in the Matrix Modulation section header,
so that I can reset, copy, or paste the full 10-bus matrix between patches (FR-35, D-095).

## Acceptance Criteria

1. **Given** Stories 5.1–5.2 (Core clipboard + `matrixModulationPasteEnabled` mirror) and Epic 3.3 Matrix Mod Init **When** the Matrix Modulation panel is displayed **Then** the section header exposes an **I/C/P trio** (3 × 20 px) to the right of the `ModulationBusHeader` DESTINATION label.
2. **And** DESTINATION **header label** width is **68 px** with **zero gap** before the I button (`68 + 0 + 60 = 128` px action column preserved; P right edge aligns with per-bus Init buttons at all UI scales).
3. **And** bus-row **DESTINATION combo** width remains **104 px** unchanged (`Atoms::Widths::ComboBox::kMatrixModulationDestination`).
4. **And** `PluginDisplayNames` and `PluginDescriptors` register Copy/Paste standalone widgets **immediately after** the existing Init entry in `kStandaloneWidgets` (not appended at file end).
5. **And** Copy/Paste APVTS trigger properties use IDs already in `PluginIDs.h`: `matrixModulationCopy`, `matrixModulationPaste` (same timestamp-stamp pattern as `matrixModulationInit` and module headers).
6. **And** Copy button is always enabled; Paste button gray state listens to `matrixModulationPasteEnabled` via `apvts.state` (D-033 — GUI reads mirror; never `#include ClipboardService.h` in GUI).
7. **And** Copy click stamps `matrixModulationCopy` (Core handler from Story 5.2 already calls `clipboardService_->copyMatrixModulation`); Paste click stamps `matrixModulationPaste` only — **no Paste execution** in this story (Story 7.2).
8. **And** Init button behaviour unchanged (section init → `matrixModulationInit` → existing `handleMatrixModInitPropertyChange` path).
9. **And** buttons are created via `WidgetFactory::createStandaloneButton` with descriptor-backed labels (migrate away from raw `TSS::Button` + hardcoded `ShortLabels::kInit`).
10. **And** `Matrix-Control_Tests` passes; `WidgetDimensionRegistryTests` still resolves all Matrix Modulation section standalone buttons at 20 px width.

## Tasks / Subtasks

- [x] **Descriptors and display names** (AC: #4, #5)
  - [x] `PluginDisplayNames.h` — add `kCopy`, `kPaste` under `MatrixModulationSection::Header::StandaloneWidgets` (reuse `ShortLabels::kCopy` / `kPaste` like DCO1 module)
  - [x] `PluginDescriptorsMatrixModulation.cpp` — add Copy/Paste entries after Init in `MatrixModulationSection::kStandaloneWidgets`

- [x] **Layout atoms and dimensions** (AC: #2, #3)
  - [x] `DesignAtoms.h` — `ModulationBusHeader::kBusDestinationTextWidth`: **104 → 68** (header label only; do **not** change `ComboBox::kMatrixModulationDestination`)
  - [x] Optional `DesignChecks.h` static_assert: `68 + 0 + 3 * Button::kInit == 128` (action column invariant per D-095)
  - [x] `PanelDimensions.h` / `DimensionFactory.cpp` — replace single `initAllButtonWidth` with section action strip widths (`initWidth`, `copyWidth`, `pasteWidth` = 20 each, or `sectionActionStripWidth = 60`)

- [x] **MatrixModulationPanel GUI** (AC: #1, #5–#9)
  - [x] Store `WidgetFactory&` member (constructor already receives it but discards it today)
  - [x] Replace `createInitAllBussesButton` with `createSectionActionButtons` — three `WidgetFactory::createStandaloneButton` instances
  - [x] `layoutSectionActionButtons` — mirror `ModuleHeader::layoutInitCopyPasteButtons`: P rightmost, C, I from right; `initX = panelWidth - 60`, `y = sectionHeaderHeight`
  - [x] Timestamp `onClick` handlers for Init / Copy / Paste property IDs
  - [x] Nested `PasteEnabledPropertyListener` (copy `ModuleHeader.cpp` pattern) on `matrixModulationPasteEnabled`; resync on `valueTreeRedirected`
  - [x] Update `setSkin` / `setUiScale` / destructor for all three buttons + listener

- [x] **Regression guard** (AC: #10)
  - [x] Build `Matrix-Control_Tests` + `Matrix-Control_Standalone`; manual smoke: Copy matrix mod → only Matrix Mod Paste enabled; Init still resets all buses

- [x] **Self-review** — no Paste execution; no SysEx; no `ActionDispatcher`; no `ClipboardService` changes; no `ApvtsLayoutBuilder` changes (trigger properties are `ValueTree` properties, not `AudioProcessorParameter`s)

## Dev Notes

### What Story 5.3 IS — and what it is NOT

Story 5.3 completes the **Matrix Modulation section header GUI** for Init/Copy/Paste. It wires visible buttons and layout so users can exercise the clipboard gray rules delivered in 5.2. Core copy handling for `matrixModulationCopy` **already exists** in `PluginProcessor`.

It must **NOT** in this story:
- Execute Paste (`clipboardService_->pasteMatrixModulation` + APVTS push + SysEx burst) — Story **7.2**
- Add `matrixModulationPaste` handler in `PluginProcessor` — Story **7.2**
- Modify `ClipboardService` or `ClipboardPasteEnabledResolver` — Stories **5.1** / **5.2** (frozen)
- Add `ActionDispatcher` / `ModuleActionHandler` — Stories **7.1** / **7.2**
- Change per-bus Init buttons in `ModulationBusCell` — Epic 3.3 (complete)
- Change bus-row DESTINATION combo width (104 px) or `ModulationBusCell` row layout math

[Source: D-095; FR-35; `sprint-change-proposal-2026-06-18.md` §4.7; `5-2-module-copy-paste-enable-and-gray-rules.md`]

### Epic 5 cross-story map

| Story | Delivers | Relation to 5.3 |
|---|---|---|
| **5.1 (done)** | `ClipboardService` matrix-mod mode (bytes 104–133) | Copy handler consumes `copyMatrixModulation` |
| **5.2 (done)** | `matrixModulationPasteEnabled` + Copy handler + module gray wiring | 5.3 wires Paste gray listener; Copy handler already live |
| **5.3 (this)** | Section I/C/P GUI + descriptors + layout 68/0/60 | Completes Epic 5 GUI surface |
| **7.2** | Paste execution + SysEx for all I/C/P targets | Handles `matrixModulationPaste` stamp |

### Authoritative layout math (D-095)

**Bus row** (unchanged — `DesignRecipes::ModulationBusCell::kWidth == 268`):

```
8 (#) + 4 + 60 (SOURCE) + 4 + 60 (AMOUNT) + 4 + 104 (DEST combo) + 4 + 20 (Init) = 268
Per-bus Init right edge = panel width (268 at design scale)
```

**Section header action strip** (new):

```
DEST header label = 68 px
Gap label → I button = 0 px (exception to standard 4 px interControlGap)
I + C + P = 3 × 20 = 60 px
68 + 0 + 60 = 128 px (same action column as old 104 + 4 + 20)
P right edge at panelWidth - 0 = aligns with per-bus Init right edge
```

**Header text X positions** (`ModulationBusHeader::drawText` — only `busDestinationTextWidth` changes):

```
x after AMOUNT = 8 + 4 + 60 + 4 + 60 + 4 = 136
DESTINATION label occupies [136, 136+68) = [136, 204)
I button starts at panelWidth - 60 = 208 (at 268 panel width) — contiguous, zero gap
```

**I/C/P button X** (copy `ModuleHeader::layoutInitCopyPasteButtons`):

```cpp
const int pasteX = panelWidth - pasteWidth;           // 248
const int copyX  = panelWidth - pasteWidth - copyWidth; // 228
const int initX  = panelWidth - pasteWidth - copyWidth - initWidth; // 208
```

All three buttons share `y = sectionHeaderHeight` (overlay on section header band, same as current single Init button).

### Current state vs target (UPDATE files)

#### `MatrixModulationPanel.cpp` / `.h`

**Today:**
- Single `initAllBussesButton_` — raw `TSS::Button`, hardcoded `ShortLabels::kInit`
- `widgetFactory` parameter ignored
- Positioned at `(panelWidth - 20, sectionHeaderHeight)`
- Init stamps `matrixModulationInit` only

**Target:**
- `initButton_`, `copyButton_`, `pasteButton_` via `WidgetFactory`
- `layoutSectionActionButtons()` in `resized()`
- `PasteEnabledPropertyListener` on `matrixModulationPasteEnabled`
- Copy/Paste stamp properties; Paste `setEnabled` from listener

#### `ModulationBusHeader.cpp`

**Today:** `busDestinationTextWidth` from dimensions (currently 104 via `DesignAtoms`).

**Target:** Reads 68 from updated atom — **no code change** if dimensions pipeline updated in `DimensionFactory`.

#### `PluginDescriptorsMatrixModulation.cpp`

**Today:** Init-only `kStandaloneWidgets` vector (1 entry).

**Target:** 3 entries — Init, Copy, Paste (match DCO1 pattern in `PluginDescriptorsPatchEdit.cpp` lines 19–37).

#### `PluginIDs.h`

**Already done in 5.2** — no changes expected:

```410:413:Source/Shared/Definitions/PluginIDs.h
            constexpr const char* kMatrixModulationInit         = "matrixModulationInit";
            constexpr const char* kMatrixModulationCopy         = "matrixModulationCopy";
            constexpr const char* kMatrixModulationPaste        = "matrixModulationPaste";
            constexpr const char* kMatrixModulationPasteEnabled = "matrixModulationPasteEnabled";
```

### Core integration (already wired — do not duplicate)

**Copy handler** (`PluginProcessor.cpp`):

```1429:1434:Source/Core/PluginProcessor.cpp
    if (propertyId == MatrixMod::kMatrixModulationCopy)
    {
        apvtsPatchMapper_->apvtsToBuffer();
        clipboardService_->copyMatrixModulation(*patchModel_);
        refreshClipboardPasteEnabledProperties();
        return;
```

**Paste enabled mirror** (published; GUI consumer missing):

```1410:1410:Source/Core/PluginProcessor.cpp
    apvts.state.setProperty(MatrixMod::kMatrixModulationPasteEnabled, state.matrixModulation, nullptr);
```

**Init handler** — existing `handleMatrixModInitPropertyChange` for `kMatrixModulationInit` (Epic 3.3).

**NOT wired:** `matrixModulationPaste` — intentional; Story 7.2 adds handler calling `pasteMatrixModulation` + mapper + SysEx.

### GUI patterns to reuse (do not reinvent)

| Pattern | Reference | Apply in 5.3 |
|---|---|---|
| I/C/P layout from right | `ModuleHeader::layoutInitCopyPasteButtons` | `MatrixModulationPanel::layoutSectionActionButtons` |
| Timestamp APVTS trigger | `ModuleHeader::createCopyPasteButtons` onClick | Copy/Paste onClick |
| Paste gray listener | `ModuleHeader::PasteEnabledPropertyListener` | Same nested class or extracted shared helper |
| Factory button creation | `ModuleHeader` via `widgetFactory.createStandaloneButton` | Replace raw `TSS::Button` |
| Copy always enabled | FR-35 / Story 5.2 | Never disable Copy button |

### APVTS property contract

| Property | Type | Writer | Reader (after 5.3) |
|---|---|---|---|
| `matrixModulationInit` | timestamp | GUI stamp | `PluginProcessor` init handler |
| `matrixModulationCopy` | timestamp | GUI stamp | `PluginProcessor` copy handler (exists) |
| `matrixModulationPaste` | timestamp | GUI stamp | **None until 7.2** |
| `matrixModulationPasteEnabled` | bool | `PluginProcessor::refreshClipboardPasteEnabledProperties` | `MatrixModulationPanel` listener |

**Do not** add these to `ApvtsLayoutBuilder` — they are `ValueTree` properties on `apvts.state`, not `AudioProcessorParameter`s (same as module Init/Copy/Paste and 5.2 `*PasteEnabled` mirrors).

### Files to touch (expected)

| File | Action |
|---|---|
| `Source/Shared/Definitions/PluginDisplayNames.h` | **UPDATE** — `kCopy`, `kPaste` in section header StandaloneWidgets |
| `Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp` | **UPDATE** — Copy/Paste descriptors after Init |
| `Source/GUI/Layout/Design/DesignAtoms.h` | **UPDATE** — `kBusDestinationTextWidth` 104→68 |
| `Source/GUI/Layout/Design/DesignChecks.h` | **UPDATE** (optional) — action column static_assert |
| `Source/GUI/Layout/PanelDimensions.h` | **UPDATE** — section action button widths |
| `Source/GUI/Factories/DimensionFactory.cpp` | **UPDATE** — populate new width fields |
| `Source/GUI/Panels/.../MatrixModulationPanel.h` | **UPDATE** — three buttons, factory ref, listener |
| `Source/GUI/Panels/.../MatrixModulationPanel.cpp` | **UPDATE** — primary implementation |

**Do not modify:**
- `Source/Core/Services/ClipboardService.*`
- `Source/Core/PluginProcessor.cpp` (unless a trivial bugfix — handlers already complete for Copy)
- `Source/GUI/Widgets/ModulationBusCell.cpp` (per-bus layout unchanged)
- `Source/Core/Factories/ApvtsLayoutBuilder.cpp`

### Testing requirements

Per NFR-1 and architecture AD-8:

- **No new Core unit tests required** — clipboard + gray rules covered in 5.1/5.2
- **Regression:** full `Matrix-Control_Tests` suite
- `WidgetDimensionRegistryTests` already iterates `MatrixModulationSection::kStandaloneWidgets` — adding Copy/Paste must keep all buttons resolvable at 20 px (suffix rules in `WidgetDimensionRegistry.cpp` lines 67–72 already map `*Copy`/`*Paste`/`*Init`)
- **Manual smoke checklist:**
  1. Open plugin → Matrix Mod section shows I, C, P (not just I)
  2. P right edge lines up with per-bus Init buttons at 100% and 125% UI scale
  3. DESTINATION header label visually shorter; combo columns unchanged
  4. Copy matrix modulation → PATCH module Paste grayed; Matrix Mod Paste enabled
  5. Copy DCO1 → Matrix Mod Paste grayed
  6. Init all buses still resets to NONE/0%/NONE
  7. Paste click changes `matrixModulationPaste` timestamp but does not mutate buses (until 7.2)

### Architecture compliance

| Invariant | Application |
|---|---|
| `Core ↛ GUI` | Panel reads `matrixModulationPasteEnabled` only; no ClipboardService in GUI |
| D-033 | Core writes paste enabled; GUI grays Paste |
| Descriptor SSOT | Button labels/IDs from `PluginDescriptors` + `WidgetFactory` |
| No global AffineTransform | Per-control `setUiScale` + `ScaledLayout::scaledInt` (existing panel pattern) |
| No French in source | English identifiers and display names only |
| Brownfield | Extend existing panel; do not refactor `ModulationBusHeader` into `ModuleHeader` |

### Previous story intelligence

#### Story 5.2

| Learning | Application |
|---|---|
| `matrixModulationCopy` / `matrixModulationPaste` IDs pre-added | Use exact IDs; no PluginIDs changes |
| `matrixModulationPasteEnabled` published but no GUI consumer | **Primary 5.3 deliverable** |
| `PasteEnabledPropertyListener` + `valueTreeRedirected` resync required | Copy pattern to Matrix Mod panel (DAW state restore) |
| Copy handler for matrix mod already in processor | GUI only stamps property on Copy click |
| Review deferral: no Paste execution in 5.2 | Still true for 5.3 — stamp only |

#### Story 5.1

| Learning | Application |
|---|---|
| Matrix snapshot = bytes 104–133 (30 bytes) | User-facing Copy copies all 10 buses; GUI does not handle bytes |
| `canPasteMatrixModulation()` true only in MatrixModulation mode | Paste enabled only after matrix-mod Copy |

### Git intelligence

- `494bb5e` — **Story 5.2** — paste-enabled mirrors, module gray wiring, matrix mod Copy handler + IDs
- `53f70a2` — **Story 5.1** — `ClipboardService` Core service
- Matrix Mod Init pattern — `handleMatrixModInitPropertyChange` in processor (Epic 3.3); section Init button already stamps `matrixModulationInit`

### Latest tech information

- **JUCE 8.0.12** — `ValueTree::Listener` on `apvts.state`; `setProperty` with `nullptr` undo manager
- **C++17** — nested listener class pattern established in `ModuleHeader.cpp`
- No new dependencies

### Project context reference

- `_bmad-output/project-context.md` — Core/GUI boundary, descriptor SSOT, no French in code
- `CONVENTIONS.md` — widget factory, scaled layout, CMake registration
- `Documentation/Development/software-development-quality-principles.md`

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` § Story 5.3]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` D-033, D-095]
- [Source: `_bmad-output/planning-artifacts/sprint-change-proposal-2026-06-18.md` §4.7]
- [Source: `implementation-artifacts/5-2-module-copy-paste-enable-and-gray-rules.md`]
- [Source: `Source/GUI/Widgets/ModuleHeader.cpp` — I/C/P layout + PasteEnabledPropertyListener]
- [Source: `Source/GUI/Panels/.../MatrixModulationPanel.cpp` — current Init-only stub]
- [Source: `Source/GUI/Layout/Design/DesignRecipes.h` — ModulationBusCell width 268 invariant]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Build: `cmake --build Builds/macOS/ARM --target Matrix-Control_Tests Matrix-Control_Standalone` — success
- Tests: `Builds/macOS/ARM/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` — all pass (incl. WidgetDimensionRegistry)

### Completion Notes List

- Added Matrix Modulation section header I/C/P trio via `WidgetFactory::createStandaloneButton` (Init, Copy, Paste descriptors after Init in SSOT).
- Narrowed DESTINATION header label width 104→68 px; added D-095 static_assert (68 + 3×20 = 128); bus-row combo width unchanged at 104 px.
- Replaced raw `TSS::Button` + hardcoded label with factory-backed buttons; `PasteEnabledPropertyListener` on `matrixModulationPasteEnabled` (D-033).
- Copy/Paste stamp APVTS trigger properties only — no Paste execution (Story 7.2). Core Copy handler unchanged.
- Manual smoke checklist in Dev Notes remains for human UAT at 100%/125% scale.
- **Manual UAT (Guillaume, 2026-06-18):** all smoke checks passed.

### File List

- `Source/Shared/Definitions/PluginDisplayNames.h`
- `Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp`
- `Source/GUI/Layout/Design/DesignAtoms.h`
- `Source/GUI/Layout/Design/DesignChecks.h`
- `Source/GUI/Layout/PanelDimensions.h`
- `Source/GUI/Factories/DimensionFactory.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.h`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.cpp`

### Change Log

- 2026-06-18 — Story 5.3: Matrix Modulation section I/C/P GUI, layout D-095, paste-enabled gray wiring (implementation complete, ready for code review)

### Review Findings

- [x] [Review][Patch] Exclude `Documentation/Development/bmad-method.md` from the story 5.3 commit — out-of-scope BMad workflow doc (+43 lines); split into a separate commit
- [x] [Review][Patch] Reuse `pasteButtonWidth` in `pasteX` calculation [`MatrixModulationPanel.cpp:349`] — avoids redundant `scaledInt` call; no behaviour change today
- [x] [Review][Defer] `valueTreePropertyChanged` without `treeWhosePropertyHasChanged != state_` guard [`MatrixModulationPanel.cpp:35-41`] — deferred, pre-existing (same pattern as `ModuleHeader.cpp`)
- [x] [Review][Defer] Millisecond timestamp collision on rapid clicks [`MatrixModulationPanel.cpp:288-319`] — deferred, pre-existing (project-wide APVTS trigger pattern)
- [x] [Review][Defer] ±1 px button gap at non-integer UI scales [`MatrixModulationPanel.cpp:349-353`] — deferred, pre-existing (same `scaledInt` layout math as `ModuleHeader::layoutInitCopyPasteButtons`)
