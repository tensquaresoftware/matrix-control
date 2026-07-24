---
organization: Ten Square Software
project: Matrix-Control
title: Story U-7 — Matrix Modulation Panel Layout Audit
author: BMad Agent
status: done
baseline_commit: 7b615000a1fc7b0fb90dafd4f3b2c279af3e956d
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md
  - implementation-artifacts/u-6-patch-edit-panels-layout-audit.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - implementation-artifacts/u-7b-matrix-mod-reorder-drag-colours.md
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/Design/DesignRecipes.h
  - Source/GUI/Layout/Design/DesignAtoms.h
  - Source/GUI/Layout/Design/DesignChecks.h
  - Source/GUI/Layout/ScaledLayout.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Layout/WidgetDimensions.h
  - Source/GUI/Factories/DimensionFactory.cpp
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/
  - Source/GUI/Widgets/ModulationBusCell.cpp
  - Source/GUI/Widgets/ModulationBusHeader.cpp
  - _bmad-output/project-context.md
created: 2026-07-24
updated: 2026-07-24
---

# Story U.7: Matrix Modulation Panel Layout Audit

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-7-matrix-modulation-panel-layout-audit
Prerequisites: U-5 (body shell) done; U-0/U-0b dims; U-2/U-2b ModulationBus* widgets; U-7b drag colours done.
Parallel after U-5: U-6 (done), U-8, U-9 (do not reopen those interiors here).
Note: U-7b shipped before U-7 — colour polish is closed; this story is layout only.
-->

## Story

As a sound designer,
I want the ten modulation bus rows and section header aligned to Figma,
so that Matrix Mod reads as a tight, scalable grid at every UI Scale preset.

## Problem Statement / Errata

### What this story owns

**Matrix Modulation section interiors only:** vertical stack (section header → bus column header → 10× bus rows), horizontal bus-row control placement (bus# / source / amount / destination / Init), bus-header label column alignment with those controls, and section I/C/P button placement on the action column (D-095).

### Epic symbol errata (map before coding)

| Epic / older artifact wording | Live reality | Action |
|-------------------------------|--------------|--------|
| Zone table `Atoms::Widths::ModulationBusCell::kInterControlGap` | **Wrong namespace** — live atom is `Atoms::Widths::ModulationBus::kInterControlGap` (= 4). `ModulationBusCell` namespace only has `kReorderDragThreshold`. | Use **`ModulationBus::kInterControlGap`**. Optionally fix the zone-table symbol string when touching docs (cosmetic). |
| U-0 defer `kGap_ = 5` (ModulationBusCell) | **Already resolved in U-2** — cell uses injected `dimensions_.interControlGap` (= 4). No `kGap_` remains in the file. | Do **not** hunt for `kGap_ = 5` in Matrix Mod sources. |
| `distributeFixedDesignRowsWithRemainderOnLast` | **Does not exist** in `ScaledLayout.h` (only `scaledInt`). Forbidden by U-5/U-6. | Prefer integer `removeFrom*` + `ScaledLayout::scaledInt`. |
| “layout present; gaps Figma à confirmer” | Design* + `DimensionFactory` already inject correct @100 % sizes; drift risk is **runtime placement** | Audit algorithm, not a second dimension migration |

### Critical brownfield debt — float X accumulation (same class as U-5 `originX` / U-6 `childStep`)

`ModulationBusCell::layoutWidgetRow()` places controls with float scale then `roundToInt`:

```cpp
const float gap = static_cast<float>(dimensions_.interControlGap) * sf;
const float sourceX = (busNumberLabelWidth + interControlGap) * sf;
const float amountX = sourceX + sourceComboBoxWidth * sf + gap;
const float destX   = amountX + amountSliderWidth * sf + gap;
const float initX   = destX   + destinationComboBoxWidth * sf + gap;
// setBounds(roundToInt(…X), …)
```

`ModulationBusHeader::drawText()` mirrors the same float gap + width accumulation for column label X positions.

Widths/heights already use `ScaledLayout::scaledInt` in places; **X chain does not**.

**Target:** successive integer placement (`removeFromLeft` / equivalent) where **each** control width and **each** gap comes from `ScaledLayout::scaledInt` on injected dims — **no** float step accumulation across columns. Header paint X must stay aligned with cell bounds at every preset.

### Panel vertical stack — already healthy (verify, do not rewrite blindly)

`MatrixModulationPanel::resized()` already uses `removeFromTop` + `ScaledLayout::scaledInt` for section header, bus header, and ten bus rows. Primary work is **horizontal** cell/header alignment; vertical stack is a verify + light harden pass only if odd-scale leftover appears.

### U-0b / U-2 / U-5 / U-7b already done (do not redo)

| Item | Owner | Note |
|------|-------|------|
| Design* SSOT + Factory injection | U-0 / U-0b | Panels/widgets must **not** `#include Design*` |
| ModulationBusHeader / ModulationBusCell dimension structs + interControlGap = 4 | U-2 | Header column widths already injected — audit placement, do not re-hardcode |
| Hairlines / D-013 stroke snap | U-2b | Layout bounds only here |
| Body shell / Shared column Matrix↔PM gap **12** | U-5 | Do not reopen `BodyPanel` / `SharedPanel` |
| Reorder drag source gray / drop-target red alphas | **U-7b (done)** | Do not change `paint()` highlight constants or drag callbacks |
| Bus reorder UX + SysEx | 7.9 / 2.10 | Preserve state machine + `swapMatrixModBusContents` path |
| Init defaults SysEx | 3.3 | Out of scope |

### Scope split vs neighbours

| Concern | Owner |
|---------|-------|
| Shared column width / Matrix↔PM vertical gap 12 | U-5 (done) |
| Matrix Mod **interiors** (this story) | **U-7** |
| Matrix Mod reorder **colours** | U-7b (done) |
| Patch Edit interiors | U-6 (done) |
| Patch Manager module stack | U-8 |
| Master Edit stack | U-9 |
| Aggregate odd-scale residual UAT | U-10 |

## Acceptance Criteria

### AC 1 — Matrix Mod section stack matches Design @ 100 % and scales without drift

**Given** U-0 Matrix Mod dimensions injected as `MatrixModulationPanelDimensions`  
**When** `MatrixModulationPanel` resizes at each UI Scale preset  
**Then**

1. At **100 %**, vertical identity holds:  
   `sectionHeader (32) + busHeader (32) + 10 × busRow (24) = 304` inside Shared Matrix Mod height.  
   Horizontal width **268** = `Recipes::ModulationBusCell::kWidth`.
2. Band heights / widths come from **injected** dims via `ScaledLayout::scaledInt` (no magic numbers, no Design* reads in panel/widgets).
3. Vertical stack prefers `removeFromTop` (already present — keep / harden only if leftover proven).
4. Manual UAT checklist (below) signed or owner-narrowed with note.

### AC 2 — Bus row horizontal controls (gap 4, Init column align)

**Given** `ModulationBusCellDimensions` with `interControlGap` = 4 and recipe  
`8 + 60 + 60 + 104 + 20 + 4×4 = 268`  
**When** each `ModulationBusCell` lays out at each preset  
**Then**

1. Controls left→right: bus# label (8) → source combo (60) → amount slider (60) → destination combo (104) → Init button (20), separated by scaled **4** px gaps.
2. Placement uses successive integer segments (`removeFromLeft` + scaled gap) — **no** float `sourceX` / `amountX` / `destX` / `initX` accumulation.
3. All width/height/gap scaling uses `ScaledLayout::scaledInt`.
4. At **100 %**, Init button left edge is **248** (aligns with section Paste / D-095 action column). Last-control remainder absorption allowed **only** to close ±1–2 px rounding vs parent width; at 100 % Init must still sit on design X **248** with width **20** (no visible stretch of destination combo).
5. Prefer extracting one shared private helper for header+cell column X if WET is confirmed after the first clean pass (Boy Scout — mirror `PatchEditFiveColumnLayout.h` pattern).

### AC 3 — Bus header labels align with cell columns

**Given** `ModulationBusHeaderDimensions` (U-2 injected: bus# 8, source 60, amount 60, destination **text** 68, gap 4)  
**When** `ModulationBusHeader` paints at each preset  
**Then**

1. Label column origins for bus# / source / amount / destination match the corresponding cell control origins (destination **label** width stays **68**; cell destination **combo** stays **104** — do not conflate).
2. Header X math uses the same integer style as the cell (no divergent float chain that drifts at odd presets).
3. Local paint constants (`kTextAreaHeight`, `kLineThickness`, `kTextLeftPadding`) may stay file-local unless audit proves alignment drift that needs injection — prefer algorithm fix first.
4. Do not reopen D-013 hairline stroke policy (U-2b).

### AC 4 — Section I/C/P action column (D-095)

**Given** section Init / Copy / Paste buttons (20×20) right-aligned on the Matrix Mod panel  
**When** `layoutSectionActionButtons` runs at each preset  
**Then**

1. Paste right edge flush to panel width **268**; Copy and Init packed left of Paste with no extra gap (design: Init @ 208, Copy @ 228, Paste @ 248 @ 100 %).
2. Scaling uses `ScaledLayout::scaledInt` on injected `initWidth` / `copyWidth` / `pasteWidth` / `buttonHeight` / panel width (already largely true — verify no float drift vs bus Init column).
3. Paste X **248** remains aligned with per-bus Init buttons at 100 %.
4. Do **not** change APVTS property stamps, PasteEnabled gray behaviour, or button IDs.

### AC 5 — Preserve contracts & out of scope

1. No global `AffineTransform` on the prod UI tree.
2. No `#include` of Design* from `MatrixModulationPanel` / `ModulationBusCell` / `ModulationBusHeader`.
3. Do not reopen Body shell, SharedPanel vertical stack, Patch Edit, Patch Manager, Master Edit, Header, Footer.
4. Do not change bus reorder drag state machine, callbacks, SysEx (`MatrixModBusReorderService`), or U-7b highlight alphas / `paint()` overlays.
5. Do not change Init SysEx (3.3), section I/C/P **handlers**, per-bus Init handlers, Compare lock wiring, or descriptors/IDs.
6. Do not reopen ModulationBus* **widget paint** beyond what is required for header text X alignment (no colour/token work).
7. Touch `DimensionFactory` / Design* **only** if audit proves a wrong injected token (unlikely — prefer algorithm fix).

### AC 6 — Docs, build, sprint

1. Zone table Matrix Mod section remains authoritative; update only if a documented Figma delta appears. Optionally correct the stale `ModulationBusCell::kInterControlGap` symbol string → `ModulationBus::kInterControlGap` (doc-only).
2. macOS Debug build green; existing unit tests green (no new automated GUI pixel tests). Especially preserve green: `MatrixModBusReorderServiceTests`, `MatrixModBusParameterSysExDispatcherTests`, `MatrixModInitServiceTests`.
3. Sprint key `u-7-matrix-modulation-panel-layout-audit` → `review` after implementation + UAT (or owner-narrowed UAT with note).

## Tasks / Subtasks

- [x] **T1 — Baseline capture** (AC: #1, #2, #3, #5)
  - [x] Confirm float X chain in `ModulationBusCell::layoutWidgetRow`
  - [x] Confirm mirrored float X in `ModulationBusHeader::drawText`
  - [x] Confirm panel vertical stack already uses `removeFromTop` + `scaledInt`
  - [x] Confirm Init @ 248 / Paste @ 248 identity @ 100 %
  - [x] Optional: screenshots @ 100 % and @ 150 % / 175 % before edits

- [x] **T2 — Bus cell horizontal placement** (AC: #2, #5)
  - [x] Refactor `layoutWidgetRow` to integer `removeFromLeft` + scaled gaps
  - [x] Replace float `gap` / `sourceX` / `amountX` / `destX` / `initX` accumulation
  - [x] Grep proof: no float X accumulation remaining in `ModulationBusCell.cpp` layout path
  - [x] Preserve separator layout, reorder drag hit-testing, and paint overlays unchanged

- [x] **T3 — Bus header column alignment** (AC: #3, #5)
  - [x] Align header label X with cell integer columns (shared helper if WET confirmed)
  - [x] Keep destination **text** width 68 vs destination **combo** 104
  - [x] Grep proof: header/cell column origins match at 100 %; no Design* includes

- [x] **T4 — Section stack + I/C/P verify** (AC: #1, #4)
  - [x] Verify / lightly harden `MatrixModulationPanel::resized` vertical strips if needed
  - [x] Verify `layoutSectionActionButtons` stays D-095-aligned with bus Init after cell fix
  - [x] Smoke: drag reorder still works; Paste gray rules unchanged

- [x] **T5 — Docs + UAT + build** (AC: #1, #6)
  - [x] Complete manual UAT checklist (or owner narrowing note → U-10)
  - [x] Optional zone-table symbol typo fix (`ModulationBus::kInterControlGap`)
  - [x] Build + existing unit tests green
  - [x] Completion Notes: algorithm changes, any intentional deltas, grep proofs

## Manual UAT checklist (layout audit)

**Goal:** Matrix Mod reads as one Figma-aligned 10-row grid; header labels sit on cell columns; Init / Paste share X **248** @ 100 %; no 1 px column drift at odd presets.  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100 %

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|-----|------|------|------|------|------|
| Section stack heights (header / bus header / 10 rows) — no overlap/gap; total 304 @ 100 % | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Bus row controls: 8+60+60+104+20 with 4 px gaps; no 1 px drift | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Bus header labels align with cell columns (dest label ≠ combo width) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Section I/C/P right-aligned; Paste X = bus Init X @ 100 % (248) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| No clipped controls at Matrix Mod edges | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Reorder drag still works (source gray / drop red — U-7b colours) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013 — no paint regression) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured (preferred @ 100 + 150 + 175) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** Guillaume · 2026-07-24  
**UAT note:** Owner-narrowed smoke **passed** (2026-07-24). Full 7-preset matrix + 150 % / 175 % residual scrutiny deferred to **U-10**.

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Matrix Mod dims injected |
| U-2 / U-2b | done | ModulationBus* structs + hairlines; unblocks this grid audit |
| U-5 | done | Shared shell unlocked Matrix Mod interior audit |
| U-6 | done | Pattern twin — integer strips / shared helper precedent |
| U-7b | done | Drag colours closed — do not reopen |
| 7.9 / 2.10 | done | Reorder UX + SysEx — preserve |
| U-8 / U-9 | backlog | Parallel OK; do not edit their panels here |
| U-10 | backlog | Aggregate residual UAT |

### Recommended order

Baseline → bus cell integer X chain → bus header alignment (shared helper if WET) → panel stack / I/C/P verify → UAT → build/tests.

### Key files (Code Map)

| Path | Change |
|------|--------|
| `Source/GUI/Widgets/ModulationBusCell.cpp` | **Primary:** kill float X accumulation in `layoutWidgetRow` |
| `Source/GUI/Widgets/ModulationBusHeader.cpp` | Align label X with cell integer columns |
| `…/MatrixModulationPanel/MatrixModulationPanel.cpp` | Verify vertical stack + I/C/P D-095 after cell fix |
| Optional NEW: `…/MatrixModulationPanel/ModulationBusRowLayout.h` (or under Widgets/) | Shared integer column helper — only if header+cell stay WET (U-6 `PatchEditFiveColumnLayout.h` precedent) |
| `Source/GUI/Layout/ScaledLayout.h` | Use existing `scaledInt` only — **no** new distribute API |
| `DimensionFactory.cpp` / Design* / `WidgetDimensions.h` | Touch **only** if injected token wrong |
| `u-0-zone-dimension-tables.md` § Matrix Mod | Optional symbol typo fix only |

**Do not edit:** `SharedPanel.cpp`, Core Matrix Mod services, `PluginDescriptorsMatrixModulation.cpp`, reorder `paint()` alphas, Patch Manager / Patch Edit / Master Edit.

### Design SSOT @ 100 % (Matrix Mod)

| px | Symbol / meaning |
|----|------------------|
| 268 × 304 | `Panels::Body::MatrixModulationSection::kWidth` × `kHeight` |
| 32 | `Atoms::Heights::kSectionHeader` |
| 32 | `Atoms::Heights::kModulationBusHeader` |
| 24 | `Recipes::ModulationBusCell::kHeight` (16 core + 8 separator) |
| 268 × 24 | `Recipes::ModulationBusCell::kWidth` × `kHeight` |
| 4 | `Atoms::Widths::ModulationBus::kInterControlGap` |
| 8 / 60 / 60 / 104 / 20 | bus# / source / amount / dest combo / Init |
| 68 | `ModulationBusHeader::kBusDestinationTextWidth` (label only) |
| 248 | Init / Paste X (D-095) |
| 12 | Shared Matrix↔PM gap — **U-5, out of scope** |

Identities asserted in `DesignChecks.h` (268×24 cell, 304 section, D-095 `68 + 0 + 3×20 = 128`) — prefer not to change asserts unless a Figma delta is documented.

### Current cell layout (replace float X)

```291:318:Source/GUI/Widgets/ModulationBusCell.cpp
void ModulationBusCell::layoutWidgetRow()
{
    // widths via ScaledLayout::scaledInt ✓
    // gap / sourceX / amountX / destX / initX via float * sf then roundToInt ← REMOVE
}
```

### Target algorithm sketch (implement, do not copy blindly)

```cpp
auto area = getLocalBounds().withHeight(/* control row height */);
const float sf = uiScale_;
const int gap = ScaledLayout::scaledInt(static_cast<float>(dimensions_.interControlGap), sf);

auto place = [&](juce::Component* c, int designW, int designH)
{
    if (c == nullptr) return;
    const int w = ScaledLayout::scaledInt(static_cast<float>(designW), sf);
    const int h = ScaledLayout::scaledInt(static_cast<float>(designH), sf);
    c->setBounds(area.removeFromLeft(w).withHeight(h));
};

place(busNumberLabel_.get(), dimensions_.busNumberLabelWidth, dimensions_.busNumberLabelHeight);
area.removeFromLeft(gap);
place(sourceComboBox_.get(), dimensions_.sourceComboBoxWidth, dimensions_.sourceComboBoxHeight);
area.removeFromLeft(gap);
place(amountSlider_.get(), dimensions_.amountSliderWidth, dimensions_.amountSliderHeight);
area.removeFromLeft(gap);
place(destinationComboBox_.get(), dimensions_.destinationComboBoxWidth, dimensions_.destinationComboBoxHeight);
area.removeFromLeft(gap);
// Init: prefer fixed scaled width; remainder-on-last only if parent leftover is ±1–2 px class
place(initButton_.get(), dimensions_.initButtonWidth, dimensions_.initButtonHeight);
```

Mirror column origins for `ModulationBusHeader::drawText` (destination uses **text** width 68, not combo 104).

### Preserve — reorder / SysEx / colours

```
ModulationBusCell (bus# label hit)
  → MatrixModulationPanel begin/update/finishBusReorderDrag
  → busReorderHandler_ → … → PluginProcessor::swapMatrixModBusContents
  → MatrixModBusReorderService + SysEx
```

U-7b alphas in `ModulationBusCell.cpp` (`kReorderDragSourcePlaceholderAlpha`, `kReorderDropTargetRedAlpha`) — **do not touch**.

### Anti-patterns

- Leaving float X “because it looks fine at 100 %”
- Re-including Design* in panel/widgets
- Resurrecting flexible distribute APIs without a proven leftover problem
- Conflating destination **label** 68 with destination **combo** 104
- Reopening SharedPanel / Body shell / Patch Manager / Patch Edit / Master Edit
- Changing reorder drag colours, callbacks, or Core SysEx
- Changing I/C/P or per-bus Init **handlers** / APVTS stamps
- Global `AffineTransform` scaling
- Automated GUI pixel tests
- Stretching Init or destination at 100 % beyond design widths
- “Fixing” stale U-0 `kGap_=5` notes by inventing a new gap token

### Testing

- No new automated GUI pixel tests.
- Manual UAT is the gate — full 7 presets preferred; 150/175 scrutiny; owner narrowing allowed with note → residual to U-10.
- Unit tests: existing suite only (`cmake --build --preset macos-debug-arm64` + tests target). Reorder/Init/SysEx unit tests must stay green without code changes in Core.

### Previous story intelligence (U-6)

- Integer `removeFromLeft` / `removeFromTop` + `ScaledLayout::scaledInt` killed Patch Edit column drift.
- Shared helper (`PatchEditFiveColumnLayout.h`) extracted when Top/Displays/Bottom stayed WET — same option for header+cell here.
- Algorithm-only — no intentional Figma geometry deltas; Factory/Design untouched.
- UAT smoke accepted with residual odd-scale → U-10; same option available here.
- Every injected layout field used in `resized` (U-6 PatchName gap lesson) — if you add fields, use them.

### Previous story intelligence (U-5 / U-2 / U-7b)

- U-5: same float-accumulation bug class on Body columns — integer strips are the fix pattern.
- U-2: ModulationBusHeader injection explicitly unblocked this story — do not re-hardcode header column widths.
- U-7b: colour follow-up already shipped; layout story must not regress drag visuals.

### Git intelligence

Recent relevant commits:

- `7b61500` / `d8faa29` — U-6 Patch Edit integer layout strips (pattern to mirror)
- `bc8a859` — U-5 Body shell integer layout
- `aa72810` — Story 5.3 Matrix Mod section I/C/P GUI (D-095 — preserve behaviour)

### Project context reference

- Follow `_bmad-output/project-context.md`: GUI → Shared/Core direction; Design* SSOT at compile time; runtime dims via Factory injection; no Design* in panels/widgets; no global AffineTransform; English in source.
- JUCE 8.0.12; C++ standard from `CMakeLists.txt`; builds under `Builds/` via presets.

### References

- [Source: `_bmad-output/planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` — Story U-7]
- [Source: `_bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md` — § Matrix Mod]
- [Source: `_bmad-output/implementation-artifacts/u-6-patch-edit-panels-layout-audit.md` — layout-audit template]
- [Source: `_bmad-output/implementation-artifacts/u-7b-matrix-mod-reorder-drag-colours.md` — colours closed]
- [Source: `Source/GUI/Layout/Design/DesignChecks.h` — 268/304/D-095 asserts]
- [Source: `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditFiveColumnLayout.h` — shared integer helper precedent]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Grep: no float `sourceX` / `amountX` / `destX` / `initX` / `float gap` accumulation left in `ModulationBusCell.cpp` or `ModulationBusHeader.cpp` layout/paint X path
- Grep: no Design* includes in Matrix Mod widgets / new helper
- Math @100 %: dest X header=cell=140; Init X=Paste X=248; recipe 8+60+60+104+20+4×4=268
- Panel vertical stack already `removeFromTop` + `scaledInt` — no change required
- `layoutSectionActionButtons` already integer right-pack — Paste X 248 @100 % unchanged

### Completion Notes List

- Replaced float X accumulation in `ModulationBusCell::layoutWidgetRow` and `ModulationBusHeader::drawText` with shared integer helper `TSS::computeModulationBusColumnStrip` (`ModulationBusRowLayout.h`) — successive `ScaledLayout::scaledInt` widths + gaps (U-6 / U-5 pattern).
- Destination **label** width remains 68; destination **combo** remains 104; shared destination X origin when preceding columns match.
- Init uses fixed scaled width (no remainder absorption) so design X 248 @100 % is preserved and stays aligned with section Paste.
- `MatrixModulationPanel::resized` / `layoutSectionActionButtons` verified healthy — no algorithm change (vertical stack + D-095 right-pack already integer).
- Reorder drag paint alphas (U-7b), separators, hit-testing, I/C/P handlers, Core SysEx untouched.
- Zone table symbol typo fixed: `ModulationBusCell::kInterControlGap` → `ModulationBus::kInterControlGap`.
- Build: `macos-debug-arm64` green. Unit tests: `Matrix-Control_Tests` exit 0 (incl. MatrixModBusReorder / SysEx / Init).
- UAT: owner-narrowed smoke **passed** (Guillaume, 2026-07-24); residual odd-scale → U-10.
- Optional pre-edit screenshots skipped (algorithm-only; U-6 precedent).

### File List

- `Source/GUI/Widgets/ModulationBusRowLayout.h` (new)
- `Source/GUI/Widgets/ModulationBusCell.cpp`
- `Source/GUI/Widgets/ModulationBusHeader.cpp`
- `Source/GUI/Widgets/ModulationBusHeader.h`
- `_bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md`
- `_bmad-output/implementation-artifacts/u-7-matrix-modulation-panel-layout-audit.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Review Findings

- [x] [Review][Defer] Odd-scale Init vs Paste X can diverge without last-column remainder absorption [`ModulationBusRowLayout.h` / `layoutSectionActionButtons`] — deferred, intentional fixed-width strip protects Init @ 248 @ 100 %; all seven UI Scale presets keep Init X == Paste X and strip sum == panel width; non-preset scales (e.g. 110 %) can leave ±1–2 px leftover so bus Init and section Paste diverge — same residual class already owned by U-10 / owner-narrowed UAT note

## Change Log

| Date | Note |
|------|------|
| 2026-07-24 | Story context created (ready-for-dev) — ultimate context engine analysis |
| 2026-07-24 | Implemented integer Matrix Mod column strip (cell + header shared helper); zone-table symbol fix; build/tests green; status → review |
| 2026-07-24 | Owner smoke UAT passed; residual odd-scale scrutiny remains on U-10 |
| 2026-07-24 | Code review complete — 0 decision / 0 patch / 1 defer (U-10) / noise dismissed; status → done |
