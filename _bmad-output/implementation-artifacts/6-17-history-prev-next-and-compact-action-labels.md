---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.17 — History Prev/Next and Compact Action Labels
author: BMad Agent
status: done
baseline_commit: c12c85752dc3c483019d8d7badf8bd7de61cc82a
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md
  - implementation-artifacts/6-15-history-combobox-popup-ux.md
  - implementation-artifacts/6-16-matrix-mod-recipe-toggle.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/6-9-delete-and-clear-history.md
  - implementation-artifacts/4-6-previous-and-next-file-navigation.md
  - implementation-artifacts/spec-patch-mutator-history-control-widths.md
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
  - Source/GUI/Layout/Design/DesignAtoms.h
  - Source/Shared/Definitions/PluginIDs.h
  - Source/Shared/Definitions/PluginDisplayNames.h
  - project-context.md
created: 2026-07-16
updated: 2026-07-16
---

# Story 6.17: History Prev/Next and Compact Action Labels

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a sound designer,
I want History `[<]` / `[>]` buttons beside the History combobox, plus compact `C` / `D` / `F` / `E` action labels and the restored 4 px label gaps,
so that I can circularly step mutations/retries for quick Compare without opening the combo, while matching the Figma Mutator chrome.

## Acceptance Criteria

1. **Given** hierarchical History (Stories **6-14** / **6-15**) with selection via `patchMutatorSelectedM` / `patchMutatorSelectedR` **When** this story lands **Then** two new History navigation buttons appear **immediately to the right of the History combobox** (before Compare) with face labels `"<"` and `">"` **And** clicking them advances selection **circularly** through the flat History traversal order defined in AC #2 **And** the existing audition path (debounce → `auditionSelectedHistoryEntry`) runs unchanged — panel remains selection-property writer only (AD-5).

2. **And** flat traversal order (normative) matches History display / sorted store order:
   - For each root `Mi` in `MutationHistoryStore::getSortedRootIndices()`:
     1. Visit **root-only** (`selectedR = kSelectedRetryRootOnly`)
     2. Visit each retry in `getSortedRetryIndices(Mi)` ascending
   - Example: `M00` → `M00-R00` → `M00-R01` → `M01` → `M01-R00` → … then wrap
   - `[>]` from last → first; `[<]` from first → last
   - **Do not** invent creation-time or unsorted order — sorted indices are SSOT

3. **And** enablement for `[<]` / `[>]`:
   - **Empty history** (`rootCount == 0` / History combo disabled): both **disabled** (same visual gray as History combo)
   - **Compare active** (`kCompareActive`): both **disabled/grayed** like the History combobox (6.8) — do **not** advance selection while locked
   - When History has **fewer than 2 flat entries** (root-only + retries in sorted order) or Compare is on: both **disabled** — a single entry makes circular step a no-op, so the buttons stay gray (PO 2026-07-16)
   - When History has **≥2 flat entries** and Compare is off: both **enabled**

4. **And** History-row action face labels densify to single letters at **20 px** width (behavior unchanged):

   | Face | Meaning | Wire ID (unchanged) | Previous face |
   |------|---------|---------------------|---------------|
   | `C` | Compare | `patchMutatorCompare` | `COMPARE` |
   | `D` | Delete | `patchMutatorDelete` | em dash `—` |
   | `F` | **Flush** (session clear) | `patchMutatorClear` | `CL` |
   | `E` | Export | `patchMutatorExport` | `EXPORT` |

   - **UI face = `F` = Flush**; product/docs/engine/API may still say **CLEAR** / `clearHistory` / `kClear` — **do not** rename wire IDs, Core methods, or non-Mutator-button CLEAR vocabulary in this story
   - Atom widths: `kPatchMutatorCompare` and `kPatchMutatorExport` → **20** (Delete/Clear already 20)
   - MM toggle stays **20** at row end; SharedColumn width stays **268**

5. **And** History row order left → right @ scale 1.0:
   `HISTORY` → combo → `[<]` → `[>]` → `C` → `D` → `F` → `E` → `MM`

6. **And** restore **4 px** gap (`interControlGap` / `Spacing::kStandard`) between labels **AMOUNT**, **RANDOM**, and **HISTORY** and the control immediately to their right:
   - Today `labelStep` omits the gap (flush — from `spec-patch-mutator-history-control-widths`)
   - Required: `labelStep = labelWidth + interControlGap` in both `layoutSliderLine` and `layoutHistoryLine`
   - **Amount/Random reclaim (normative):** shrink `kPatchMutatorMutate` and `kPatchMutatorRetry` from **48 → 44** so those rows stay within 268 after restoring the label gap (sliders/History combo stay **56** for vertical alignment)
   - History row budget with restored gap + nav + C/D/F/E@20 + MM closes exactly at **268** (see Dev Notes math)

7. **And** **tooltips are out of scope** for this story (PO 2026-07-16): do **not** add `juce::TooltipWindow`, tooltip SSOT strings, or `setTooltip` on Mutator chrome. Compact faces `C`/`D`/`F`/`E` and `[<]`/`[>]` ship without hover text. A future story may revisit tooltips if product wants them.

8. **And** **D-026 is superseded** by this story:
   - Append errata to `.decision-log.md` under D-026: History `[<]`/`[>]` **allowed** for circular flat traversal (2026-07-16)
   - Update `epics.md` Story 6-17 block + any FR-54 / UX-DR6 prose that still says “no `<` `>` nav” for Mutator
   - **Intentional supersession of Story 6-14 AC #5:** COMPARE-under-RETRY column alignment is obsolete once `[<]`/`[>]` sit between combo and `C`

9. **And** this story does **not**:
   - Change MUTATE / RETRY / Compare / Delete / Clear / Export **semantics** (only chrome, nav, layout)
   - Change hierarchical combobox popup UX (6-15)
   - Change MM recipe gate (6-16)
   - Wire Patch Name display (separate US — Epic 1)
   - Calibrate mutation algorithm playability (US 2 — brainstorm later)
   - Bump SharedColumn width or Patch Mutator module height
   - Rename Core `clearHistory` / wire id `patchMutatorClear` to Flush

## Tasks / Subtasks

- [x] **Supersede D-026 + tracking** (AC: #8)
  - [x] Decision-log errata under D-026
  - [x] Confirm `epics.md` Story 6-17 + sprint-status entry

- [x] **IDs / display names / descriptors** (AC: #1, #4)
  - [x] `PluginIDs.h` — `kHistoryPrevious` / `kHistoryNext` (suggested wire ids: `patchMutatorHistoryPrevious` / `patchMutatorHistoryNext`)
  - [x] `PluginDisplayNames.h` — face `"<"`, `">"`, `"C"`, `"D"`, `"F"`, `"E"` (no tooltip constants — AC #7 out of scope)
  - [x] `PluginDescriptorsPatchManager.cpp` — register two new standalone buttons
  - [x] Keep Clear wire id; face becomes `F`

- [x] **Design atoms + dimension pipeline** (AC: #4, #6)
  - [x] `DesignAtoms.h` — Compare/Export → 20; Mutate/Retry → 44; nav width = 20 (reuse `Button::kInit` or add `kPatchMutatorHistoryNav`)
  - [x] `DimensionFactory` / `WidgetDimensions` / `WidgetDimensionRegistry` (+ tests) pick up new widths and new widget IDs

- [x] **Core / handler navigation** (AC: #1, #2, #3)
  - [x] Prefer `PatchMutatorEngine::advanceHistorySelection(bool isNext)` building flat list from sorted store + writing selection props (or equivalent helper used only by handler)
  - [x] `MutatorActionHandler::handleAction` routes new button IDs → advance (mirror Computer Patches 4.6 property stamp → handler)
  - [x] After selection change: existing `onHistorySelectionChanged` / debounce / audition path
  - [x] Unit tests: empty no-op; single entry wrap; multi-root/retry order; wrap ends

- [x] **Panel layout + Compare lock** (AC: #3–#6)
  - [x] `PatchMutatorPanel` members for prev/next; `setupHistoryLine` + `layoutHistoryLine` new order
  - [x] Restore `labelStep += interControlGap` on Amount/Random/History
  - [x] Extend `refreshCompareUiState` / lock helpers so prev/next gray with History combo
  - [x] Manual geometry spot-check @ 100% and 150%: row right edge 268

- [x] **Tooltips** (AC: #7) — intentionally **not** implemented (PO: remove / do not ship hover tooltips)
  - [x] No `TooltipWindow` / `setTooltip` on Mutator chrome
  - [x] Story AC #7 + epics prose updated to “out of scope”

- [x] **Self-review**
  - [x] Grep: no leftover Mutator face `"COMPARE"` / `"EXPORT"` / `"CL"` / em-dash Delete on button faces
  - [x] Grep: D-026 “no `<` `>`” superseded in decision log
  - [x] SharedColumn still 268; no Core→GUI includes
  - [x] Build + run Mutator / WidgetDimensionRegistry unit tests

## Dev Notes

### Product intent (US 3 / Epic 6)

Planned as **US 3** in the 2026-07-16 Mutator follow-up triad (US 1 = Patch Name / Epic 1; US 2 = calibration / Epic 6 after brainstorm). This story is **Epic 6 story 6-17** — not a rewrite of done story **6-3** (MutationNaming).

Figma reference (attached to create-story): History row shows combo then `[<]` `[>]` then short letter buttons then `MM`; Amount/Random/History labels have a visible gap before controls.

### Width budget (do not re-derive ad hoc)

SharedColumn / Patch Mutator content width = **268**.

**Current History (flush label, no nav):**
```
40 + (56+4) + (48+4) + (20+4) + (20+4) + (44+4) + 20 = 268
```

**Target History (label gap + nav + C/D/F/E@20 + MM):**
```
(40+4) + (56+4) + 6×(20+4) + 20 = 44 + 60 + 144 + 20 = 268
```

| Change | Δpx |
|--------|-----|
| Restore label→control gap | +4 |
| Add `[<]` + `[>]` (+ gaps) | +48 |
| COMPARE 48→20 | −28 |
| EXPORT 44→20 | −24 |
| Net | **0** |

**Amount/Random with restored label gap:** reclaim **4 px** via MUTATE/RETRY **48→44** (keep slider **56** aligned with History combo).

### Architecture compliance

| Rule | Application |
|------|-------------|
| AD-5 thin panel | Panel stamps button timestamps / selection props only — no engine calls from GUI |
| Core ↛ GUI | `advanceHistorySelection` lives in Core (engine or store helper); handler mediates |
| Display SSOT | Faces in `PluginDisplayNames.h` only (tooltips out of scope) |
| Dimension SSOT | Widths in `DesignAtoms.h` → Factory → registry → panel dims |
| Threading | Selection audition stays on existing debounce / message-thread path |

### Navigation pattern to copy (4.6)

`PatchManagerActionHandler::advanceComputerPatchesSelection` (~446–464): circular wrap, then `setProperty` so existing load/selection listeners fire. Mutator analog: stamp prev/next action → handler → write `kSelectedMutateRootIndex` / `kSelectedRetryIndex` → existing `onHistorySelectionChanged`.

### Current layout hotspots (UPDATE files)

| File | Role today |
|------|------------|
| `PatchMutatorPanel.cpp` `layoutSliderLine` ~922 | `labelStep` = label width only (flush) |
| `PatchMutatorPanel.cpp` `layoutHistoryLine` ~950–993 | Order: combo → COMPARE → DELETE → CLEAR → EXPORT → MM; no nav |
| `DesignAtoms.h` ~67–72 | Compare=48, Export=44, Mutate/Retry=48, Delete/Clear=20 |
| `MutatorActionHandler.cpp` ~24–40 | Six action IDs only — extend for prev/next |
| `PluginDisplayNames.h` ~853–856 | COMPARE / em dash / CL / EXPORT |
| `PluginEditor` | No `TooltipWindow` — intentional (AC #7 out of scope) |

### Flush vs CLEAR naming policy

| Layer | Name |
|-------|------|
| Button face | `F` |
| Wire ID / Core / FR-58 prose | Keep **Clear** / `clearHistory` / `kClear` |
| Future vocabulary pass | Out of scope (optional later) |

Compare footer may keep the word **COMPARE** (“Click COMPARE again…”) — full word in messages is fine; only the button face is `C`.

### Anti-patterns (do not)

- Reintroduce dual M/R comboboxes
- SysEx or `auditionSelectedHistoryEntry` calls from the panel
- Hard-code button X positions bypassing atom/step math
- Widen SharedColumn past 268
- Rename `patchMutatorClear` → `patchMutatorFlush`
- Leave D-026 unamended (Acceptance Auditor will flag)

### Previous story intelligence

| Story | Carry forward |
|-------|---------------|
| **6-14** | Hierarchical combo + AD-5 selection; **explicitly forbade** Mutator `<`/`>` (D-026) — this story supersedes that |
| **6-15** | Popup UX untouched; selection props unchanged |
| **6-16** | MM at row end; CLEAR already densified to CL@20 — further densify to F; co-mingled width oneshot removed label gaps — **restore gaps here** |
| **spec-history-control-widths** | Done oneshot that **removed** 4 px label gaps — reverse that part only |
| **6-8** | Compare lock grays History; COMPARE stays clickable and blinks — prev/next follow History (disabled), not COMPARE |
| **6-9** | Clear semantics unchanged; face-only rename to F |
| **4-6** | Circular wrap + property-driven advance pattern |

### Git intelligence

Recent commits on branch focus on Mutator synth-load / Compare lock / export folders / BMad tooling — not History chrome. Implement on current tree; baseline `c12c857`.

### Testing requirements

- **Unit:** flat-order advance + wrap; empty history no-op; atom/registry widths for Compare/Export/Mutate/Retry/nav IDs
- **Manual UAT:** populate History with ≥2 roots and retries; step `[>]`/`[<]` and confirm combo label + audition match; Compare ON disables nav; with a single History entry nav stays gray; visual check 4 px label gaps and 268-wide rows @ 100% and 150% (no tooltip hover check — AC #7 out of scope)
- Build: `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests`

### Project context reference

Follow `_bmad-output/project-context.md`: English in source/artifacts; French only in chat; no GUI deps in Core; PluginIDs / PluginDisplayNames / DesignAtoms as SSOTs.

### References

- [Source: planning-artifacts/prds/.../.decision-log.md#D-026]
- [Source: implementation-artifacts/6-14-…md AC #5 — COMPARE under RETRY (superseded)]
- [Source: implementation-artifacts/spec-patch-mutator-history-control-widths.md — flush labels]
- [Source: Source/GUI/.../PatchMutatorPanel.cpp#layoutHistoryLine]
- [Source: Source/Core/Actions/PatchManagerActionHandler.cpp#advanceComputerPatchesSelection]
- [Source: Source/Core/Services/PatchMutator/MutationHistoryStore.h#getSortedRootIndices]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

### Completion Notes List

- Implemented History `[<]`/`[>]` via `PatchMutatorEngine::advanceHistorySelection` (sorted flat order) + `MutatorActionHandler` routing; selection writes reuse the existing debounce/audition path.
- Densified History-row faces to `C`/`D`/`F`/`E` @ 20 px; restored 4 px label→control gaps; Mutate/Retry widths 48→44; SharedColumn budget stays 268.
- Tooltips intentionally omitted (PO) — AC #7 marked out of scope; no `TooltipWindow`.
- Prev/next enabled only with ≥2 flat History entries (PO); single-entry circular step stays a no-op and buttons stay gray.
- Superseded D-026 in decision log, FR-54 / UX-DR6 / addendum prose updated.
- Unit tests: advance empty/single/multi/wrap/compare/unknown-selection; handler prev/next; registry nav widths.

### File List

- Source/Shared/Definitions/PluginIDs.h
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp
- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Layout/WidgetDimensions.h
- Source/GUI/Layout/WidgetDimensionRegistry.cpp
- Source/GUI/Factories/DimensionFactory.cpp
- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Source/Core/Actions/MutatorActionHandler.h
- Source/Core/Actions/MutatorActionHandler.cpp
- Source/Core/Actions/ActionPropertyRegistry.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
- Source/GUI/PluginEditor.h
- Tests/Unit/PatchMutatorEngineTests.cpp
- Tests/Unit/MutatorActionHandlerTests.cpp
- Tests/Unit/WidgetDimensionRegistryTests.cpp
- _bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
- _bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
- _bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
- _bmad-output/planning-artifacts/epics.md
- _bmad-output/implementation-artifacts/sprint-status.yaml
- _bmad-output/implementation-artifacts/6-17-history-prev-next-and-compact-action-labels.md

### Change Log

- 2026-07-16: Story 6-17 implemented — History prev/next, compact C/D/F/E faces, restored label gaps, D-026 supersession.
- 2026-07-16: Code review — AC #3 enablement ≥2; AC #7 tooltips out of scope (PO); unmatched selection advance no-op.

## Suggested Review Order

1. Decision-log D-026 errata + display-name faces (product chrome contract)
2. `DesignAtoms` + History/Amount/Random layout math → 268
3. Engine/handler `advanceHistorySelection` order + empty/Compare/unknown-selection guards
4. Panel setup/layout + Compare lock + ≥2 flat-entry enablement for prev/next
5. Unit tests for wrap order, unknown selection, and registry widths

### Review Findings

- [x] [Review][Decision] Prev/next enablement ≥2 vs AC #3 ≥1 — resolved 2026-07-16 (PO): keep code (≥2 flat entries); amend AC #3 / story to match. Also keep History-combo alignment as follow-up only if needed.
- [x] [Review][Patch] Amend AC #3 (and related story prose) to require ≥2 flat History entries for `[<]`/`[>]` enablement — matches intentional PO end-of-dev change [`6-17-…md` AC #3]
- [x] [Review][Patch] Amend AC #7 / tasks / completion notes — tooltips intentionally removed per PO (ugly; not requested); document as out of scope / not delivered [`6-17-…md` AC #7]
- [x] [Review][Patch] `advanceHistorySelection` treats unmatched selection as index 0 then steps — if current selection is not in the flat list after `applySelectionFromApvts`, first click jumps instead of no-op [`PatchMutatorEngine.cpp:668`]
- [x] [Review][Defer] No end-to-end test for APVTS stamp → handler → selection → audition chain — deferred, pre-existing test-gap class; unit tests cover engine advance + handler mock only
- [x] [Review][Defer] Panel `countFlatHistoryEntries` duplicates engine flat-order logic via APVTS mirrors — deferred, architectural drift risk; normal sync path keeps mirrors aligned
- [x] [Review][Dismiss] Mandatory tooltips “missing” as code defect — dismissed: PO asked to remove tooltips at end of dev-story; not a delivery gap to re-implement
