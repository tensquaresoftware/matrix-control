---
organization: Ten Square Software
project: Matrix-Control
title: Story U-8 — Patch Manager Panels Layout Audit
author: BMad Agent
status: done
baseline_commit: 606b242
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-0-figma-intake-and-design-reconciliation.md
  - implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md
  - implementation-artifacts/u-6-patch-edit-panels-layout-audit.md
  - implementation-artifacts/u-7-matrix-modulation-panel-layout-audit.md
  - implementation-artifacts/deferred-work.md
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/Design/DesignRecipes.h
  - Source/GUI/Layout/Design/DesignAtoms.h
  - Source/GUI/Layout/Design/DesignChecks.h
  - Source/GUI/Layout/ScaledLayout.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Factories/DimensionFactory.cpp
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/
  - _bmad-output/project-context.md
created: 2026-07-24
updated: 2026-07-24
---

# Story U.8: Patch Manager Panels Layout Audit

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-8-patch-manager-panels-layout-audit
Prerequisites: U-5 (body shell + Shared Matrix↔PM gap 12) done; U-0/U-0b dims; U-2 widget sizes; U-6/U-7 integer-layout patterns done.
Parallel after U-5: U-6 (done), U-7 (done), U-9 (do not reopen Master Edit here).
-->

## Story

As a sound designer,
I want Bank Utility, Internal Patches, Computer Patches, and Patch Mutator modules aligned to Figma,
so that the Patch Manager column matches mockup density at every UI Scale preset.

## Problem Statement / Errata

### What this story owns

**Patch Manager section interiors only:**

1. Vertical module stack in `PatchManagerPanel` (section header → Bank → gap → Internal → gap → Computer → gap → Mutator).
2. Inter-module gap **`kModuleStackGap` = 8** (between the four modules).
3. Interior control placement inside the four module panels (kill float `originX` / step × index drift).

### Epic symbol errata (map before coding)

| Epic / older artifact wording | Live reality | Action |
|-------------------------------|--------------|--------|
| Epic U-8 “Given … **84 px / 100+16** stacks” | Live Design: Bank/Internal/Computer **76**, Mutator **100**, section **384** | Use **zone table + DesignChecks**; ignore epic 84/+16 |
| Early U-0 section height **400** / Shared stack **704** / Matrix↔PM gap **24** | Live: section **384**, Shared **700**, Matrix↔PM gap **12** | Gap **12** is **U-5 (done)**; U-8 owns module gap **8** only |
| U-0 “runtime SharedPanel / PatchManagerPanel gaps → U-8” | SharedPanel Matrix↔PM **12** already hardened in U-5 | Do **not** reopen `SharedPanel` stack; harden **`PatchManagerPanel`** only |
| U-0 `kGap_ = 5` residual (Internal/Computer/Mutator) | **No `kGap_` under PatchManagerPanel/** — panels use injected `interControlGap` (= 4) | Do **not** invent private gap tokens |
| U-0 Completion Notes `PatchManagerModule::kRowGap` | Live symbol is **`Recipes::PatchManagerModule::kInterControlGap`** (= 4) | Use `kInterControlGap` |
| `distributeFixedDesignRowsWithRemainderOnLast` | **Does not exist** in `ScaledLayout.h` (only `scaledInt`). Forbidden by U-5/U-6/U-7 | Prefer integer `removeFrom*` + `ScaledLayout::scaledInt` |

### Critical brownfield debt — float placement (same class as U-5 `originX` / U-6 `childStep` / U-7 `sourceX`)

#### Panel stack — `y +=` instead of `removeFromTop`

`PatchManagerPanel::resized()` already uses `ScaledLayout::scaledInt` for heights/gaps, but places modules with mutable `y += height + gap` (drift cousin of pre-U-5 SharedPanel):

```63:98:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp
void PatchManagerPanel::resized()
{
    // … scaledInt on heights/gap …
    int y = bounds.getY() + sectionHeaderHeight;
    // bank → y += bankUtilityH + moduleStackGap;
    // internal → y += …; computer → y += …; mutator last
}
```

**Target:** successive `removeFromTop` (mirror `SharedPanel::resized` / `MatrixModulationPanel::resized`).

#### Module interiors — float origin + step × index

| Panel | Drift pattern |
|-------|---------------|
| `BankUtilityPanel` | `row1OriginX` / `row2OriginX` + `bankButtonStep * index` then `roundToInt` |
| `InternalPatchesPanel` | `navStep` / `memStep` float × index; bank NumberBox hide reflow must survive |
| `ComputerPatchesPanel` | `navStep` / `openStep` / `saveAsStep` float chains |
| `PatchMutatorPanel` | Explicit `originX` + `labelStep`/`sliderStep`/`cursorX` float accumulation in `layoutSliderLine` / `layoutHistoryLine`; also ad-hoc `roundToInt(design * sf)` instead of `ScaledLayout::scaledInt` |

**Target:** every width/height/gap via `ScaledLayout::scaledInt` on **injected** dims; successive integer `removeFromLeft` / `removeFromTop` (or equivalent integer cursor). Prefer extracting a shared private helper **only after** WET is confirmed (Boy Scout — mirror `PatchEditFiveColumnLayout.h` / `ModulationBusRowLayout.h`).

### U-0b / U-2 / U-5 / U-6 / U-7 already done (do not redo)

| Item | Owner | Note |
|------|-------|------|
| Design* SSOT + Factory injection | U-0 / U-0b | Panels must **not** `#include Design*` |
| Widget sizes / Factory maps for PM standalone buttons | U-2 / U-IDs | Place injected sizes; do not re-hardcode |
| Hairlines / D-013 stroke snap | U-2b | Layout bounds only here |
| Body shell + Shared Matrix↔PM gap **12** | U-5 | Do not reopen `BodyPanel` / `SharedPanel` vertical stack |
| Patch Edit integer strips | U-6 | Pattern twin — reuse algorithm style |
| Matrix Mod integer column strip | U-7 | Pattern twin — reuse algorithm style |
| Intentional Shared **700** vs PE/ME **704** (−4 from Mutator omitting trailing separator) | DesignChecks | Do **not** stretch Shared or change Mutator height to “align bottoms” |

### Scope split vs neighbours

| Concern | Owner |
|---------|-------|
| Shared column width / Matrix↔PM vertical gap **12** | U-5 (done) |
| Patch Manager **inter-module** gap **8** + module interiors | **U-8** |
| Master Edit stack | U-9 |
| Aggregate odd-scale residual UAT | U-10 |
| BankUtility alpha-gray → shared “visually disabled but clickable” API | deferred-work candidate — **out of scope** unless Guillaume explicitly expands (Ask First) |

## Acceptance Criteria

### AC 1 — Patch Manager section stack matches Design @ 100 % and scales without drift

**Given** U-0 Patch Manager dimensions injected as `PatchManagerPanelDimensions`  
**When** `PatchManagerPanel` resizes at each UI Scale preset  
**Then**

1. At **100 %**, vertical identity holds:  
   `sectionHeader (32) + 76 + 8 + 76 + 8 + 76 + 8 + 100 = 384`  
   (= `kSectionHeader + kModulesStackHeight`). Width **268**.
2. Band heights / `moduleStackGap` come from **injected** dims via `ScaledLayout::scaledInt` (no magic numbers, no Design* reads in panels).
3. Vertical stack uses successive `removeFromTop` (replace `y +=` placement).
4. Manual UAT checklist (below) signed or owner-narrowed with note → U-10.

### AC 2 — Four module interiors use integer placement (no float origin/step drift)

**Given** each module’s Factory-injected dims (`contentRowHeight` 20, `interControlGap` 4, `columnGap` 8, widget widths from U-2)  
**When** `BankUtilityPanel` / `InternalPatchesPanel` / `ComputerPatchesPanel` / `PatchMutatorPanel` lay out at each preset  
**Then**

1. All control X/Y use successive integer segments (`removeFromLeft` / `removeFromTop` or integer cursor) — **no** float `originX`, `navStep * i`, `bankButtonStep * index`, or `cursorX += floatStep` accumulation.
2. All width/height/gap scaling uses `ScaledLayout::scaledInt` (replace ad-hoc `roundToInt(design * sf)` in Mutator helpers and siblings).
3. At **100 %**, module heights remain **76 / 76 / 76 / 100**; interior rows match DesignRecipes formulas (see Dev Notes).
4. Prefer extracting one shared private helper under `PatchManagerPanel/` if WET across ≥2 modules after the first clean pass (Boy Scout). Do **not** fold these panels into `BaseModulePanel` (U-11: custom layouts).

### AC 3 — Preserve device-aware reflow & visual contracts inside modules

**Given** Internal Patches can hide the bank NumberBox when not applicable  
**When** layout is reflowed after the integer conversion  
**Then**

1. Hidden bank NumberBox still shifts patch NumberBox left (`bankNumberVisible_` behaviour preserved).
2. Computer Patches empty/select sentinels and nav enable rules unchanged (layout only).
3. Bank Utility grayed state (`setBankUtilityGrayed`: stay **enabled** + grayed look so clicks still reach footer) **semantics preserved** — do not convert to `setEnabled(false)` as part of this story.
4. Compare lock / ROM paste-store inactive appearance / STORE-INIT gating remain behaviourally unchanged.

### AC 4 — Preserve contracts & out of scope

1. No global `AffineTransform` on the prod UI tree.
2. No `#include` of Design* from Patch Manager panels.
3. Do not reopen Body shell, SharedPanel Matrix↔PM gap, Patch Edit, Matrix Mod, Master Edit, Header, Footer.
4. Do **not** change ActionDispatcher / APVTS property stamps, folder scan (`PatchFileService`), mutation / Compare / History logic, bank unlock handlers, SysEx paths, or descriptors/IDs.
5. Do not reopen widget **paint** / skin tokens / D-013 hairline policy.
6. Touch `DimensionFactory` / Design* **only** if audit proves a wrong injected token (unlikely — prefer algorithm fix). Do not change intentional Mutator **100** (no trailing separator) or Shared **700**.
7. BankUtility alpha-gray hygiene API is **out of scope** (deferred-work candidate) unless Guillaume explicitly expands scope.
8. No new automated GUI pixel tests.

### AC 5 — Docs, build, sprint

1. Zone table Patch Manager section remains authoritative; update only if a documented Figma delta appears.
2. macOS Debug build green; existing unit tests green (`cmake --build --preset macos-debug-arm64` + `Matrix-Control_Tests`).
3. Sprint key `u-8-patch-manager-panels-layout-audit` → `review` after implementation + UAT (or owner-narrowed UAT with note).
4. Completion Notes must include grep proofs: no `originX` / float step × index left in Patch Manager layout paths.

## Tasks / Subtasks

- [x] **T1 — Baseline capture** (AC: #1, #2, #4)
  - [x] Confirm DesignChecks identities: 76/76/76/100, gap 8, section 384, Shared 700
  - [x] Confirm `PatchManagerPanel` `y +=` stack vs SharedPanel `removeFromTop` precedent
  - [x] Confirm float chains in Bank / Internal / Computer / Mutator (`originX`, `navStep`, `memStep`, `cursorX`)
  - [x] Confirm no live `kGap_` under PatchManagerPanel/
  - [x] Optional: screenshots @ 100 % and @ 150 % / 175 % before edits

- [x] **T2 — PatchManagerPanel vertical stack** (AC: #1, #4)
  - [x] Refactor `resized` to `removeFromTop` + scaled gaps (mirror SharedPanel)
  - [x] Grep proof: no mutable `y += height + gap` placement remaining
  - [x] Preserve `setUiScale` propagation to children (parent drives child `resized` via `setBounds`)

- [x] **T3 — BankUtilityPanel integer placement** (AC: #2, #3, #4)
  - [x] Replace float `row1OriginX` / `row2OriginX` / `bankButtonStep * index`
  - [x] Prefer `ScaledLayout::scaledInt` for all lengths
  - [x] Preserve `setBankUtilityGrayed` clickable-gray semantics

- [x] **T4 — InternalPatchesPanel + ComputerPatchesPanel** (AC: #2, #3, #4)
  - [x] Replace float `navStep` / `memStep` / `openStep` chains with integer strips
  - [x] Preserve Internal bank NumberBox hide reflow
  - [x] Preserve Computer scan/nav enable wiring (layout only)
  - [x] Document per-row remainder policy in Completion Notes (fixed-width vs last-control absorb — U-7 Init fixed-width precedent when protecting a design X)

- [x] **T5 — PatchMutatorPanel integer placement** (AC: #2, #4)
  - [x] Refactor `layoutSliderLine` / `layoutHistoryLine` off `originX` / float steps / float `cursorX`
  - [x] Replace ad-hoc `roundToInt(design * sf)` with `ScaledLayout::scaledInt`
  - [x] Prefer shared helper if WET with Bank/Internal/Computer after clean pass
  - [x] Grep proof: no `originX` / float step accumulation in Mutator layout path

- [x] **T6 — Docs + UAT + build** (AC: #1, #5)
  - [x] Complete manual UAT checklist (or owner narrowing note → U-10)
  - [x] Build + existing unit tests green
  - [x] Completion Notes: algorithm changes, remainder policy, grep proofs, any intentional deltas (should be none)

## Manual UAT checklist (layout audit)

**Goal:** Patch Manager reads as one Figma-aligned 4-module stack; inter-module gap 8; no 1 px drift at odd presets; bank gray clicks and Internal bank-hide reflow still work.  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100 %

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|-----|------|------|------|------|------|
| Section stack: header + 76/76/76/100 + 3×8 = 384 @ 100 %; no overlap/gap | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Bank Utility rows: select label + banks 0–4 / unlock + banks 5–9 — no 1 px drift | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Internal Patches: browser + memory columns; bank NumberBox hide reflow OK | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Computer Patches: browser + storage columns; combo/nav not clipped | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Patch Mutator: 3 content rows; sliders/toggles/history actions aligned | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| No clipped controls at Patch Manager edges (width 268) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Grayed Bank Utility still clickable (footer message reachable) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013 — no paint regression) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured (preferred @ 100 + 150 + 175) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** owner smoke OK (2026-07-24) — full 7-preset matrix deferred to U-10  
**UAT note:** Owner smoke passed. Full 7-preset matrix residual → **U-10** (U-5/U-6/U-7 precedent).

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Patch Manager dims injected |
| U-2 / U-2b | done | Widget sizes + hairlines |
| U-5 | done | Shared shell unlocked PM interior audit; Matrix↔PM gap 12 closed |
| U-6 | done | Integer strip / shared helper precedent |
| U-7 | done | Integer column strip precedent |
| U-9 | backlog | Parallel OK; do not edit Master Edit |
| U-10 | backlog | Aggregate residual UAT |

### Recommended order

Baseline → `PatchManagerPanel` `removeFromTop` stack → BankUtility → Internal + Computer → PatchMutator (heaviest float debt) → optional shared helper if WET → UAT → build/tests.

### Key files (Code Map)

| Path | Change |
|------|--------|
| `…/PatchManagerPanel/PatchManagerPanel.cpp` | **Primary stack:** kill `y +=`; use `removeFromTop` |
| `…/Modules/BankUtilityPanel.cpp` | Kill float bank-button step placement |
| `…/Modules/InternalPatchesPanel.cpp` | Kill float nav/mem steps; preserve bank-hide reflow |
| `…/Modules/ComputerPatchesPanel.cpp` | Kill float nav/storage steps |
| `…/Modules/PatchMutatorPanel.cpp` | Kill `originX` / float `cursorX`; use `scaledInt` |
| Optional NEW: `…/PatchManagerPanel/*Layout.h` | Shared integer helper — only if WET confirmed |
| `Source/GUI/Layout/ScaledLayout.h` | Use existing `scaledInt` only — **no** new distribute API |
| `DimensionFactory.cpp` / Design* / `PanelDimensions.h` | Touch **only** if injected token wrong |
| `u-0-zone-dimension-tables.md` § Patch Manager | Update only on documented Figma delta |

**Do not edit:** `SharedPanel.cpp` (Matrix↔PM gap), `BodyPanel.cpp`, Matrix Mod / Patch Edit / Master Edit panels, Core services (ActionDispatcher, PatchFileService, mutator/Compare), widget paint/skins, bank gray semantics (unless scope expanded).

### Suggested Review Order

1. `PatchManagerPanel::resized` — stack identity + `removeFromTop`
2. `BankUtilityPanel::resized` — bank grid integer X
3. `InternalPatchesPanel::resized` — nav/memory + bank-hide reflow
4. `ComputerPatchesPanel::resized` — browser/storage strips
5. `PatchMutatorPanel` helpers — heaviest float debt
6. Completion Notes grep proofs + UAT note

### Design SSOT @ 100 % (Patch Manager)

| px | Symbol / meaning |
|----|------------------|
| 268 × 384 | `Panels::Body::PatchManagerSection::kWidth` × `kHeight` |
| 32 | `Atoms::Heights::kSectionHeader` |
| 76 | `Recipes::BankUtilityModule::kHeight` = `32 + 2×20 + 1×4` |
| 76 | `Recipes::InternalPatchesModule::kHeight` = `32 + 24 + 20` |
| 76 | `Recipes::ComputerPatchesModule::kHeight` (same as Internal) |
| 100 | `Recipes::PatchMutatorModule::kHeight` = `32 + 3×20 + 2×4` (**no** trailing HorizontalSeparator — intentional) |
| 8 | `PatchManagerSection::kModuleStackGap` (= `Spacing::kMedium`) |
| 352 | `kModulesStackHeight` = `76+76+76+100 + 3×8` |
| 20 / 4 / 8 | `PatchManagerModule::kContentRowHeight` / `kInterControlGap` / `kColumnGap` |
| 2 | `kShortControlVerticalInset` = `(20−16)/2` — asserted; optional inject vs local `(rowH−labelH)/2` |
| 12 | Shared Matrix↔PM gap — **U-5, out of scope** |
| 268 × 700 | Shared column — Matrix 304 + 12 + PM 384 |

Identities asserted in `DesignChecks.h` — prefer not to change asserts unless a Figma delta is documented.

### Current panel stack (replace `y +=`)

```63:98:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp
// scaledInt heights/gap already present — placement still uses mutable y
```

### Current BankUtility float step (replace)

```197:207:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
const float row1OriginX = static_cast<float>(dims_.bankSelectorLabel.patchManagerSelectBankWidth) * sf
    + rowGapDesign * sf;
const float bankButtonStep = static_cast<float>(dims_.buttons.patchManagerBankSelectWidth) * sf
    + rowGapDesign * sf;
// … roundToInt(row1OriginX + index * bankButtonStep)
```

### Current Mutator float origin (replace)

```971:986:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
const float originX = static_cast<float>(x);
// … originX + labelStep + sliderStep …; toggleOriginX + i * toggleStep
```

### Architecture compliance

- **UI Scale:** design @ 100 % × preset via `ScaledLayout::scaledInt` only — no global `AffineTransform` ([project-context.md](../project-context.md) § GUI & UI Scale Rules).
- **U-0b:** injected dims only; no Design* includes in panels.
- **Clean Architecture:** layout-only in `Source/GUI/`; no Core / SysEx / APVTS behaviour changes.
- **JUCE 8:** prefer `Rectangle::removeFromTop` / `removeFromLeft` integer slicing for successive strips (JUCE advanced layout tutorial pattern already used in U-5/U-6/U-7).
- **CMake:** sources already registered (`CMakeLists.txt` PatchManagerPanel + four modules). Register a new `.cpp` only if a shared helper becomes a translation unit (header-only preferred like U-6/U-7).

### Previous story intelligence

| Lesson | Source | Apply here |
|--------|--------|------------|
| Float accumulation “fine at 100 %” still drifts at 150/175 | U-5/U-6/U-7 | Kill all float X/Y chains in PM |
| Do not invent distribute / remainder APIs | U-5/U-6 | `scaledInt` only |
| Shared gap 12 ≠ module gap 8 | U-5 scope split | Do not reopen SharedPanel |
| Epic stale dims are normal — errata-map in story | U-5/U-6/U-7 | 76/100/384 not 84/+16 |
| Owner-narrowed UAT + U-10 residual is acceptable | U-5/U-6/U-7 | Document in Completion Notes |
| Extract shared helper after WET, not before | U-6/U-7 | Optional `*Layout.h` |
| Grep proofs required in Completion Notes | U-5/U-6/U-7 | `originX`, float steps |
| Do not bundle unrelated hygiene “while here” | U-7 | Skip BankUtility alpha API unless asked |

### Git intelligence (recent)

- `606b242` — Close U-7 after Matrix Mod integer column strip
- `7b61500` / `d8faa29` — Close U-6; Patch Edit float → integer strips
- `bc8a859` — Close U-5 Body shell integer layout
- Earlier PM feature commits (bank wiring, Compare, History) are **behaviour** — preserve; this story is placement only

### Latest tech notes

- Project JUCE **8.0.12**; layout pattern remains `getLocalBounds()` + successive `removeFrom*` with integer amounts from `ScaledLayout::scaledInt`.
- No new third-party deps. No widget API version chase required for this audit.

### Project context reference

Follow `_bmad-output/project-context.md` and `CONVENTIONS.md`: French chat / English artifacts; no French in source; no Core←GUI deps; UI Scale via `ScaledLayout`; Ask First before expanding beyond layout ACs.

### Testing requirements

- Manual UAT table above (or owner-narrowed smoke + U-10 note).
- Build: `cmake --build --preset macos-debug-arm64`.
- Run existing `Matrix-Control_Tests` — no new GUI pixel tests.
- Smoke: grayed bank click still shows Matrix-1000-only footer; Internal bank NumberBox hide still reflows; Compare/History/Mutate still clickable (no logic regressions).

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

- Build: `cmake --build --preset macos-debug-arm64` green
- Tests: `Matrix-Control_Tests` exit 0 (`-DMATRIX_BUILD_TESTS=ON`)

### Completion Notes List

- **Algorithm:** Patch Manager section stack uses successive `removeFromTop` + scaled `moduleStackGap` (mirrors SharedPanel). Module interiors use `ScaledLayout::scaledInt` + integer `removeFromLeft` / integer cursor; equal-width bank/memory/toggle runs via shared `placeEqualWidthStrip`.
- **Remainder policy:** Fixed-width strips only (U-7 Init precedent) — no last-control remainder absorption. Protects design X at 100 % for bank buttons, nav/memory/storage strips, and Mutator slider/history rows.
- **Behaviour preserved:** `setBankUtilityGrayed` clickable-gray unchanged; Internal `bankNumberVisible_` hide reflow still shifts patch NumberBox left; Computer scan/nav wiring untouched (layout only).
- **Grep proofs** under `PatchManagerPanel/` (2026-07-24): no `originX` / `navStep` / `memStep` / `bankButtonStep` / `openStep` / float `cursorX`; no `y +=` stack; no `roundToInt(design * sf)`; no `kGap_`; no Design* includes. Remaining `int cursorX` in Mutator is intentional integer strip cursor.
- **Intentional deltas:** none (Design* / DimensionFactory / SharedPanel / Mutator height 100 untouched).
- **UAT:** owner smoke passed (2026-07-24); full 7-preset matrix → U-10.
- Optional pre-edit screenshots skipped (same class as U-5/U-6/U-7).

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerEqualWidthStrip.h` (new)
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp`
- `_bmad-output/implementation-artifacts/u-8-patch-manager-panels-layout-audit.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-07-24 — Implemented U-8 Patch Manager integer layout audit: section `removeFromTop` stack; Bank / Internal / Computer / Mutator float step → integer strips; shared equal-width helper; build + unit tests green; status → review.
- 2026-07-24 — Code review clean (0 decision / 0 patch); one U-10-class defer on undersized strip clamp; status → done.

### Review Findings

- [x] [Review][Defer] Undersized Internal/Computer browser strips can shrink via `removeFromLeft` when panel width is below design sum [InternalPatchesPanel.cpp / ComputerPatchesPanel.cpp] — deferred, pre-existing class (parent always sizes to design width; same residual family as U-5 Shared overhang → U-10)

## Story completion status

- Story status: **done**
- Context engine: comprehensive developer guide created (2026-07-24)
- Sprint key: `u-8-patch-manager-panels-layout-audit`
- Implementation completed 2026-07-24
- Code review completed 2026-07-24
