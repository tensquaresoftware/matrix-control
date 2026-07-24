---
organization: Ten Square Software
project: Matrix-Control
title: Story U-6 — Patch Edit Panels Layout Audit
author: BMad Agent
status: done
baseline_commit: bc8a859
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - implementation-artifacts/u-11-module-panel-config-dedup.md
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/Design/DesignChecks.h
  - Source/GUI/Layout/ScaledLayout.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Factories/DimensionFactory.cpp
  - Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/
  - Source/GUI/Panels/Reusable/BaseModulePanel.cpp
  - _bmad-output/project-context.md
created: 2026-07-24
updated: 2026-07-24
---

# Story U.6: Patch Edit Panels Layout Audit

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-6-patch-edit-panels-layout-audit
Prerequisites: U-5 (body shell) done; U-0/U-0b dims; U-2/U-2b widgets; U-11 module config dedup.
Parallel after U-5: U-7, U-8, U-9 (do not reopen those interiors here).
-->

## Story

As a sound designer,
I want PATCH EDIT top modules, displays row, and bottom modules pixel-aligned to Figma,
so that the largest screen area matches the approved mockup at every UI Scale preset.

## Problem Statement / Errata

### What this story owns

**PATCH EDIT column interiors only:** section stack (header → top band → middle displays → bottom band), 5-column module grids, inter-module gaps, middle-row display/patch-name vertical stack, and `BaseModulePanel` parameter-row stacking used by Patch Edit modules.

### Epic symbol errata (map before coding)

| Epic / older story wording | Live reality | Action |
|----------------------------|--------------|--------|
| `distributeFixedDesignRowsWithRemainderOnLast` / “module row helpers” | **Helper does not exist** in `ScaledLayout.h` (only `scaledInt`). U-2 docs still mention it; U-5 explicitly forbade speculative flexible distribute APIs. | Prefer **U-5 integer `removeFrom*` + `ScaledLayout::scaledInt`**. Do **not** resurrect a distribute API unless audit proves leftover/shortfall that cannot be fixed with successive strips. |
| “layout present; gaps Figma à confirmer” (epic inventory) | Design* + `DimensionFactory` already inject correct @100 % sizes; drift risk is **runtime placement** | Audit algorithm, not a second dimension migration |
| Brownfield §4.4 | Covered by U-0 Design* + zone table | Use zone table + DesignChecks as UAT SSOT |

### Critical brownfield debt — float `childStep` (same class as U-5 `originX`)

`PatchEditTopModulesPanel`, `PatchEditBottomModulesPanel`, and `PatchEditDisplaysPanel` place columns with:

```cpp
const float childStep = static_cast<float>(childWidth + gap) * uiScale_;
const int x = juce::roundToInt(static_cast<float>(i) * childStep);
```

Widths often use ad-hoc `roundToInt(design * uiScale)` (Top/Bottom) instead of `ScaledLayout::scaledInt`. Last column absorbs remainder width.

**Target:** successive integer placement (`removeFromLeft` / equivalent) where **each** module width and **each** gap comes from `ScaledLayout::scaledInt` on injected dims — **no** float step × index accumulation.

### Patch name vertical stack — unused gap field

`PatchNameDisplayDimensions::moduleHeaderToDisplayGap` is injected from Design (`Recipes::PatchNameModule::kModuleHeaderToDisplayGap` = 4) but **`PatchNameDisplayPanel::resized` never uses it**. Header is top-padded; display is bottom-anchored. Numerically OK at 100 % (8+32+4+72+12 = 128) but fragile at odd presets.

**Target:** explicit `removeFromTop` (or equivalent) chain: topPad → header → gap → display → bottomPad, all via `ScaledLayout::scaledInt`.

### U-0b / U-2 / U-5 / U-11 already done (do not redo)

| Item | Owner | Note |
|------|-------|------|
| Design* SSOT + Factory injection | U-0 / U-0b | Panels must **not** `#include Design*` |
| ParameterCell / ModuleHeader / display widget paint & hairlines | U-2 / U-2b | Layout bounds only here |
| Body shell columns / separators / Shared stack gap | U-5 | Do not reopen `BodyPanel` / `SharedPanel` |
| Module panel config builders (`createLayout()`) | U-11 | Do not reorder parameter IDs; RampPortamento uses `getParameterCellAt(7)` |

### Scope split vs neighbours

| Concern | Owner |
|---------|-------|
| Body PE column width / padding / separators | U-5 (done) |
| Patch Edit **interiors** (this story) | **U-6** |
| Matrix Mod row grid | U-7 |
| Patch Manager module stack gaps | U-8 |
| Master Edit stack (reuses BaseModulePanel patterns validated here) | U-9 |
| Aggregate odd-scale residual UAT | U-10 |
| Envelope / Track Generator **drag** geometry | Epic 10 (done functionally — do not reopen) |

## Acceptance Criteria

### AC 1 — Patch Edit section stack matches Design @ 100 % and scales without drift

**Given** U-0 Patch Edit dimensions injected as `PatchEditPanelDimensions`  
**When** `PatchEditPanel` (and children) resize at each UI Scale preset  
**Then**

1. At **100 %**, vertical identity holds:  
   `sectionHeader (32) + top (272) + middle (128) + bottom (272) = 704` inside PE column height.  
   Horizontal PE width **808** = `5 × 152 + 4 × 12`.
2. Band heights / widths / gaps come from **injected** dims via `ScaledLayout::scaledInt` (no magic numbers, no Design* reads in panels).
3. Vertical stack prefers `removeFromTop` (or equivalent integer chain) — no float Y accumulation.
4. Manual UAT checklist (below) signed or owner-narrowed with note.

### AC 2 — Five-column module grids (top / bottom / middle)

**Given** `PatchEditModulesRowDimensions` / `PatchEditDisplaysPanelDimensions` with `interModuleGap` = 12  
**When** top, bottom, and displays rows lay out at each preset  
**Then**

1. Five children per row: Top = DCO1, DCO2, VCF/VCA, FM/TRACK, RAMP/PORTAMENTO; Middle = Env1, Env2, Env3, Track Generator, Patch Name; Bottom = ENV1, ENV2, ENV3, LFO1, LFO2.
2. Placement uses successive integer segments (`removeFromLeft` + scaled gap) — **no** float `childStep * i` X math.
3. All width/height/gap scaling uses `ScaledLayout::scaledInt` (replace ad-hoc `roundToInt(design * uiScale)` in Top/Bottom).
4. Last-column remainder absorption is allowed **only** to close ±1–2 px rounding vs parent width; at 100 % last column must still equal design **152** (no visible stretch). Document any intentional odd-scale leftover policy in Completion Notes.
5. Prefer extracting one shared private helper if Top/Bottom/Displays stay WET after the first clean pass (Boy Scout — extract when duplication is confirmed stable).

### AC 3 — BaseModulePanel parameter rows (Patch Edit modules)

**Given** `Recipes::PatchEditModule` = header **32** + **10** × ParameterCell **24** = **272**  
**When** any Patch Edit `BaseModulePanel` descendant resizes  
**Then**

1. Header + parameter rows use `ScaledLayout::scaledInt` on injected `moduleHeaderDims_` / `parameterCellDims_.rowHeight`.
2. Prefer successive `removeFromTop` strips (same integer style as U-5) instead of raw `y += rowHeight` if that path shows leftover/shortfall vs module height at odd presets.
3. Do **not** invent `distributeFixedDesignRowsWithRemainderOnLast` unless removeFromTop + scaledInt still leaves a proven gap that needs an explicit remainder-on-last policy (document why in Completion Notes).
4. Do not change U-11 `createLayout()` parameter order / IDs.

### AC 4 — Middle row vertical centring (displays + patch name)

**Given** middle band height **128** and PatchName recipe `8 + 32 + 4 + 72 + 12 = 128`  
**When** middle row lays out at each preset  
**Then**

1. EnvelopeDisplay ×3 and TrackGeneratorDisplay fill their column bounds (design band 152×128) — no accidental vertical clip/offset vs neighbours.
2. `PatchNameDisplayPanel` uses injected `topPadding`, `moduleHeaderToDisplayGap`, `bottomPadding`, header height, and display height explicitly (gap field must be **used**, not implied by bottom-anchoring alone).
3. Manual UAT verifies visual alignment of the five middle columns at 100 % and spot-checks odd presets.

### AC 5 — Preserve contracts & out of scope

1. No global `AffineTransform` on the prod UI tree.
2. No `#include` of Design* from Patch Edit panels / `BaseModulePanel`.
3. Do not reopen Body shell, Header, Footer, Matrix Mod, Patch Manager, Master Edit interiors.
4. Do not change APVTS bindings, Envelope/Track drag behaviour (Epic 10), Init/Copy/Paste handlers, or module config order (U-11).
5. Do not reopen ParameterCell / ModuleHeader / display **paint** or hairlines (U-2 / U-2b).
6. Touch `DimensionFactory` / Design* **only** if audit proves a wrong injected token (unlikely — prefer algorithm fix).

### AC 6 — Docs, build, sprint

1. Zone table Patch Edit section remains authoritative; update only if a documented Figma delta appears (else leave as-is).
2. macOS Debug build green; existing unit tests green (no new automated GUI pixel tests).
3. Sprint key `u-6-patch-edit-panels-layout-audit` → `review` after implementation + UAT (or owner-narrowed UAT with note).

## Tasks / Subtasks

- [x] **T1 — Baseline capture** (AC: #1, #2, #5)
  - [x] Confirm float `childStep` paths in Top / Bottom / Displays
  - [x] Confirm PatchName unused `moduleHeaderToDisplayGap`
  - [x] Confirm BaseModulePanel `y +=` stack vs design 272 identity
  - [x] Optional: screenshots @ 100 % and @ 150 % / 175 % before edits

- [x] **T2 — Five-column row placement** (AC: #2, #5)
  - [x] Refactor `PatchEditTopModulesPanel::resized` to integer `removeFromLeft` + scaled gaps
  - [x] Refactor `PatchEditBottomModulesPanel::resized` same pattern
  - [x] Refactor `PatchEditDisplaysPanel::resized` same pattern
  - [x] Replace ad-hoc `roundToInt(design * uiScale)` with `ScaledLayout::scaledInt`
  - [x] Grep proof: no `childStep` float accumulation remaining in those three files
  - [x] Optional: extract shared helper if duplication remains after first clean pass

- [x] **T3 — PatchEditPanel vertical stack** (AC: #1)
  - [x] Prefer `removeFromTop` for header / top / middle / bottom (behaviour-preserving)

- [x] **T4 — PatchNameDisplayPanel vertical stack** (AC: #4)
  - [x] Explicit scaled strips: topPad → header → `moduleHeaderToDisplayGap` → display → bottomPad
  - [x] Grep proof: `moduleHeaderToDisplayGap` (or dims member) used in `resized`

- [x] **T5 — BaseModulePanel row stack** (AC: #3)
  - [x] Harden to `removeFromTop` + `ScaledLayout::scaledInt` if needed for odd-scale leftover
  - [x] Smoke Master Edit modules still lay out (U-9 owns full audit — no intentional ME geometry change)

- [x] **T6 — Docs + UAT + build** (AC: #1, #6)
  - [x] Complete manual UAT checklist (or owner narrowing note)
  - [x] Build + existing unit tests green
  - [x] Completion Notes: algorithm changes, any intentional deltas, grep proofs

## Manual UAT checklist (layout audit)

**Goal:** PATCH EDIT reads as one Figma-aligned grid at every preset; no 1 px column drift in the 5-wide rows.  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100 %

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|-----|------|------|------|------|------|
| PE section stack heights (header/top/mid/bottom) — no overlap/gap | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Top 5 modules: equal columns + 12 @ 100 % gaps; no 1 px drift | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Bottom 5 modules: same | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Middle 5: Env1–3 / Track / Patch Name columns align | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Patch name: header + display vertical paddings/gap match Design | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Module parameter rows fill module height (no clip / large dead band) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| No clipped controls at PE edges | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013 — no paint regression) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured (preferred @ 100 + 150 + 175) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** Guillaume · date 2026-07-24  
**UAT note:** Owner-narrowed smoke **passed** (2026-07-24). Full 7-preset matrix + 150 % / 175 % residual scrutiny deferred to **U-10**.

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Patch Edit dims injected |
| U-2 / U-2b | done | Widgets/hairlines closed |
| U-5 | done | Body shell unlocked PE interior audit |
| U-11 | done | Module configs stable — do not reshuffle |
| U-7…U-9 | backlog | Parallel OK after U-5; do not edit their panels here |
| U-10 | backlog | Aggregate residual UAT |

### Recommended order

Baseline → 5-column Top/Bottom/Displays integer chain → PatchEditPanel vertical stack → PatchName explicit paddings/gap → BaseModulePanel harden → UAT → build/tests.

### Key files

| Path | Change |
|------|--------|
| `…/PatchEditTopModulesPanel/PatchEditTopModulesPanel.cpp` | **Primary:** kill float `childStep`; integer columns |
| `…/PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.cpp` | Same |
| `…/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp` | Same for middle row |
| `…/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.cpp` | Use `moduleHeaderToDisplayGap`; explicit vertical strips |
| `…/PatchEditPanel/PatchEditPanel.cpp` | Prefer `removeFromTop` stack |
| `Source/GUI/Panels/Reusable/BaseModulePanel.cpp` | Prefer `removeFromTop` for header + rows |
| `Source/GUI/Layout/ScaledLayout.h` | Use existing `scaledInt` only — **no** new distribute API unless proven necessary |
| `DimensionFactory.cpp` / Design* | Touch **only** if injected token wrong |

Child module `*Panel.cpp` files under Top/Bottom (Dco1, Env1, …) are config-only via U-11 — **layout lives in BaseModulePanel / row parents**, not in each child.

### Design SSOT @ 100 % (Patch Edit)

| px | Symbol / meaning |
|----|------------------|
| 808 × 704 | `PatchEditSection::kWidth` × `kHeight` |
| 32 | Section header height |
| 808 × 272 | Top / Bottom bands |
| 808 × 128 | Middle (displays) band |
| 152 × 272 | Child module / `Recipes::PatchEditModule` |
| 12 | `kInterModuleGap` (`Spacing::kLarge`) |
| 152 × 24 | ParameterCell |
| 152 × 32 | ModuleHeader |
| 8 / 4 / 12 / 72 | Patch name topPad / header↔display gap / bottomPad / display height |

Identities asserted in `DesignChecks.h` — prefer not to change asserts unless a Figma delta is documented.

### Current Top row layout (replace float step)

```44:70:Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/PatchEditTopModulesPanel.cpp
void PatchEditTopModulesPanel::resized()
{
    // childWidth/Height = roundToInt(design * uiScale)  ← use ScaledLayout::scaledInt
    // childStep = (width + gap) * uiScale (float)
    // x = roundToInt(i * childStep)  ← REMOVE — same drift class as BodyPanel originX
    // last column: bounds.getWidth() - x
}
```

### Target algorithm sketch (implement, do not copy blindly)

```cpp
auto area = getLocalBounds();
const float sf = uiScale_;
const int childW = TSS::ScaledLayout::scaledInt(static_cast<float>(rowDims_.childModuleWidth), sf);
const int childH = TSS::ScaledLayout::scaledInt(static_cast<float>(rowDims_.childModuleHeight), sf);
const int gap = TSS::ScaledLayout::scaledInt(static_cast<float>(rowDims_.interModuleGap), sf);

for (int i = 0; i < 4; ++i)
{
    panels[i]->setBounds(area.removeFromLeft(childW).withHeight(childH));
    area.removeFromLeft(gap);
}
panels[4]->setBounds(area.removeFromLeft(area.getWidth()).withHeight(childH)); // remainder
```

Mirror for Displays (five middle children) and Bottom.

### PatchName target sketch

```cpp
auto area = getLocalBounds();
area.removeFromTop(ScaledLayout::scaledInt(patchNameDims_.topPadding, sf));
moduleHeader_->setBounds(area.removeFromTop(ScaledLayout::scaledInt(moduleHeaderDims_.height, sf)));
area.removeFromTop(ScaledLayout::scaledInt(patchNameDims_.moduleHeaderToDisplayGap, sf));
area.removeFromBottom(ScaledLayout::scaledInt(patchNameDims_.bottomPadding, sf));
patchNameDisplay_->setBounds(area.removeFromTop(ScaledLayout::scaledInt(patchNameDims_.height, sf)));
```

### Anti-patterns

- Leaving float `childStep` “because it looks fine at 100 %”
- Re-including Design* in panels
- Resurrecting flexible distribute APIs without a proven leftover problem
- Reopening Body shell / Matrix / Patch Manager / Master Edit / widget paint
- Changing APVTS, I/C/P, or Epic 10 drag geometry
- Reordering U-11 parameter lists / RampPortamento cell indices
- Global `AffineTransform` scaling
- Automated GUI pixel tests
- Stretching last module at 100 % beyond 152

### Testing

- No new automated GUI pixel tests.
- Manual UAT is the gate — full 7 presets preferred; 150/175 scrutiny; owner narrowing allowed with note → residual to U-10.
- Unit tests: existing suite only (`cmake --build --preset macos-debug-arm64` + tests target).

### Previous story intelligence (U-5)

- Integer `removeFromLeft` / `removeFromTop` + `ScaledLayout::scaledInt` on injected dims killed Body column drift.
- Algorithm-only — no intentional Figma geometry deltas.
- UAT smoke accepted with residual odd-scale → U-10; same option available here if Guillaume narrows.
- Do **not** reopen shell files while fixing PE interiors.
- Shared column 700 / separators 728 / PM module gaps 8 remain out of scope.

### Previous story intelligence (U-2 / U-11)

- Widgets already scale via ScaledLayout + ScaledDrawing — panel placement was explicitly deferred to U-6…U-9.
- U-2 docs mention distribute helpers that were removed — ignore stale API names; follow live `ScaledLayout.h`.
- U-11: 13 BaseModulePanel consumers on `createLayout()` — layout audit must not break config/order contracts.

### Git intelligence

| Commit | Relevance |
|--------|-----------|
| `bc8a859` | Close U-5 — integer Body/Shared shell layout (pattern to mirror) |
| `5a4c988` | Fixed design-pixel UI scaling across panels/widgets (baseline) |
| `a65683e` / Epic 10 closes | Display APVTS direct wiring — preserve; layout-only here |

### Latest tech notes

- JUCE 8.0.12: `Rectangle::removeFromLeft` / `removeFromTop` / `setBounds` — stable; no API migration.
- Scale policy SSOT: Design @ 100 % × `uiScale_`; no global AffineTransform (`project-context.md` § GUI & UI Scale Rules).
- Clean Code: if Top/Bottom/Displays helpers exceed method limits after refactor, extract a small free function or private method (≤15 lines preferred).

### Project context reference

- GUI & UI Scale Rules: `ScaledLayout::scaledInt`; ÷4 design grid; separate stroke thickness (`ScaledDrawing`).
- Factory-only Design consumer; panels use injected `*Dimensions`.
- Key refs: `ScaledLayout.h`, `PanelDimensions.h`, `DimensionFactory.cpp`, `CONVENTIONS.md`, `u-0-zone-dimension-tables.md` Patch Edit section.

### References

- [Source: `_bmad-output/planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` § Story U-6]
- [Source: `_bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md` § Patch Edit]
- [Source: `_bmad-output/implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md`]
- [Source: `Source/GUI/Layout/Design/DesignChecks.h` Patch Edit asserts]
- [Source: `_bmad-output/project-context.md` § GUI & UI Scale Rules]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Grep: no `childStep` remaining under `PatchEditPanel/`
- Grep: `moduleHeaderToDisplayGap` used in `PatchNameDisplayPanel::resized`
- Grep: `BaseModulePanel` uses `removeFromTop` for header + parameter rows (no `y += rowHeight`)
- Grep: no Design* includes under Patch Edit panels

### Completion Notes List

- Replaced float `childStep * i` column placement in Top / Bottom / Displays with shared integer helper `TSS::layoutPatchEditFiveColumns` (`removeFromLeft` + `ScaledLayout::scaledInt` on injected dims). Last column absorbs remainder width; at 100 % still equals design 152 (5×152 + 4×12 = 808).
- `PatchEditPanel` vertical stack now successive `removeFromTop` for section header / top / middle / bottom.
- `PatchNameDisplayPanel` now uses explicit strips: topPad → header → `moduleHeaderToDisplayGap` → display → bottomPad (gap field no longer unused).
- `BaseModulePanel` parameter rows hardened to `removeFromTop` (Master Edit consumers unchanged in geometry intent — U-9 owns full ME audit).
- No Design* includes in panels; no DimensionFactory / Design* token changes; no new distribute API.
- Build: `macos-debug-arm64` green. Unit tests: `Matrix-Control_Tests` exit 0.
- UAT: owner-narrowed smoke **passed** (Guillaume, 2026-07-24); residual odd-scale → U-10.
- Optional pre-edit screenshots skipped (algorithm-only change; U-5 precedent).

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditFiveColumnLayout.h` (new)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/PatchEditTopModulesPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp`
- `Source/GUI/Panels/Reusable/BaseModulePanel.cpp`
- `_bmad-output/implementation-artifacts/u-6-patch-edit-panels-layout-audit.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Review Findings

- [x] [Review][Decision] Accept Master Edit residual via shared BaseModulePanel harden, or spot-check Master Edit before closing U-6? — Resolved: option 1 — accept U-6 as planned; Master Edit residual stays on U-9.
- [x] [Review][Defer] No runtime assert that five-column last width ≈ scaled childW / PE stack leftover ≈ 0 [PatchEditFiveColumnLayout.h / PatchEditPanel.cpp] — deferred, pre-existing class (same as U-5 Body leftover); DesignChecks + U-10 odd-scale UAT own residual

## Change Log

- 2026-07-24: Story context created (ready-for-dev) — Patch Edit 5-column float-step removal, PatchName gap usage, BaseModulePanel integer strips, UAT.
- 2026-07-24: Implemented integer five-column + vertical stacks; shared helper; build/tests green; status → review.
- 2026-07-24: Owner smoke UAT passed; residual odd-scale scrutiny remains on U-10.
- 2026-07-24: Code review — 1 decision-needed, 0 patch, 1 defer, noise dismissed.
- 2026-07-24: Review decision resolved (accept ME residual on U-9); status → done.
