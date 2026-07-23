---
organization: Ten Square Software
project: Matrix-Control
title: Story U-5 — Body Shell Padding, Separators & Column Gaps
author: BMad Agent
status: done
baseline_commit: 42e49ddc23c3bfd382f460a1380293092d10a99f
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/epics.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-0b-factory-dimension-registry-and-descriptor-decoupling.md
  - implementation-artifacts/u-4-footer-panel-layout-audit.md
  - implementation-artifacts/spec-vertical-separator-full-height.md
  - Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp
  - Source/GUI/MainComponent.cpp
  - Source/GUI/Widgets/VerticalSeparator.cpp
  - Source/GUI/Layout/ScaledLayout.h
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/Design/DesignChecks.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Factories/DimensionFactory.cpp
  - _bmad-output/project-context.md
created: 2026-07-23
updated: 2026-07-23
---

# Story U.5: Body Shell — Padding, Separators & Column Gaps

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-5-body-shell-padding-separators-and-column-gaps
Unlocks U-6…U-9 (panel interiors). Header/Footer shell closed in U-3 / U-4.
-->

## Story

As a sound designer,
I want the three body columns and vertical separators positioned per Figma at every scale,
so that PATCH EDIT, shared column, and MASTER EDIT align as one grid (UX-DR8).

## Problem Statement / Errata

### What this story owns

The **Body shell** only: outer padding, the horizontal column chain (Patch Edit → separator → Shared → separator → Master Edit), vertical separator placement/height, and the SharedPanel **vertical** gap between Matrix Modulation and Patch Manager.

### Epic symbol errata (map before coding)

| Epic wording | Live Design* / injected field | Notes |
|--------------|-------------------------------|-------|
| `SharedColumn::kInterPanelGap` | **Does not exist** | Horizontal space between columns is the two `VerticalSeparator` widths (`Atoms::Widths::VerticalSeparator::kStandard` = **24**). |
| Shared stack gap | `SharedColumn::kVerticalStackGap` (= `Spacing::kLarge` = **12**) | Injected as `SharedPanelDimensions::verticalStackGap`. |
| “Body effective height” for separators | Content band = `Panels::Body::kEffectiveHeight` (**704**); separator height = `PanelWidgets::Heights::kVerticalSeparator` = `Panels::Body::kHeight` (**728**) | Full-height separators already shipped (`spec-vertical-separator-full-height`). **Do not regress to 704.** |
| Zone table row `24 × 704` | Stale vs DesignChecks / live code | When editing `u-0-zone-dimension-tables.md`, fix to **24 × 728**. |

### Critical brownfield debt — `originX` float

`BodyPanel::resized()` places later columns with:

```cpp
const float originX = static_cast<float>(bounds.getX() + padding);
// separator1X / sharedColumnX / separator2X / masterEditX =
//   roundToInt(originX + (sum of design widths) * sf);
```

Widths use `ScaledLayout::scaledInt` per token; X positions use **float design-sum × scale then round**. That dual path is the classic source of **1 px column drift at 150 % / 175 %** (epic UAT callout).

**Target:** successive integer placement via `Rectangle::removeFromLeft` (and/or equivalent) where **each** segment width comes from `ScaledLayout::scaledInt` — **no** float `originX` accumulation.

### U-0b already done (do not re-migrate)

`BodyPanelDimensions` / `SharedPanelDimensions` / separators are already Factory-injected. Panels must **not** re-`#include Design*`. This story is **layout algorithm + pixel UAT**, not a second dimension migration.

### Scope split vs U-8

| Gap | Owner |
|-----|-------|
| Matrix Modulation ↔ Patch Manager (**12** px `kVerticalStackGap`) | **U-5** (verify / harden shell) |
| Gaps **between** Patch Manager modules (`kModuleStackGap` = 8) | **U-8** — do not reopen |

### Functional / visual work already done (preserve)

| Item | State |
|------|--------|
| Full-height vertical separators (Y = body top, H = 728 @ 100 %) | Done — join Header/Footer borders through padding gutters |
| Shared column **4 px shorter** than Patch/Master content (700 vs 704) | Intentional — `DesignChecks` asserts `704 − 700 == 4` |
| Header / Footer shell layout | U-3 / U-4 done — do not redesign `MainComponent` stack |
| Widget paint / hairlines | U-2 / U-2b done |

## Acceptance Criteria

### AC 1 — Column grid matches Design @ 100 % and scales without drift (primary)

**Given** U-0 body shell dimensions (`DesignPanels.h` / injected `BodyPanelDimensions`)  
**When** `BodyPanel` (and `MainComponent` body placement) lay out at each UI Scale preset  
**Then**

1. At **100 %**, horizontal identity holds:  
   `808 + 24 + 268 + 24 + 152 = 1276` (`GUI::kBodyInnerWidth`) inside `12` padding each side → `1300` (`GUI::kWidth`).
2. Column widths / separator widths / shared height / content heights come from **injected** dims via `ScaledLayout::scaledInt` (no magic numbers, no Design* reads in panels).
3. Layout uses **`removeFrom*` / successive integer segments** (or equivalent integer chain) — **no** float `originX` / design-sum × scale accumulation in `BodyPanel`.
4. Manual UAT at **50–200 %** (7 presets) with **no 1 px column drift** at **150 %** and **175 %** (common failure modes). Checklist below signed by Guillaume.

### AC 2 — Vertical separators track full body height

**Given** `spec-vertical-separator-full-height` and `PanelWidgets::Heights::kVerticalSeparator == Panels::Body::kHeight` (728)  
**When** body resizes at each preset  
**Then**

1. Both separators are placed at body **top** (`bounds.getY()` / local Y = 0), height = scaled `separators.verticalStandardHeight` (design **728**, not content-band **704**).
2. Columns (Patch Edit / Shared / Master Edit) remain inset by scaled body padding on top (and bottom for height), as today.
3. No paint regression on separator line (U-2b hairline / `ScaledDrawing` path untouched unless a layout-only bug forces a one-line fix).

### AC 3 — SharedPanel Matrix ↔ Patch Manager stack gap

**Given** `SharedColumn::kVerticalStackGap` = 12 injected as `verticalStackGap`  
**When** `SharedPanel::resized` runs at each preset  
**Then**

1. Gap between Matrix Modulation and Patch Manager matches design (scaled 12 @ 100 %).
2. Prefer the same integer / `removeFromTop` style as Body columns if the current `y += h + gap` path shows drift; otherwise **behaviour-preserving verify** is enough.
3. Do **not** change Patch Manager **inter-module** gaps (U-8).

### AC 4 — Preserve intentional invariants & contracts

1. Shared column height stays design **700** (not stretched to 704).
2. No global `AffineTransform` on the prod UI tree — scale via `uiScale_` + `ScaledLayout` only.
3. Do not reopen Patch Edit / Matrix Mod / Patch Manager / Master Edit **interiors** (U-6…U-9).
4. Do not change Header/Footer geometry, CompareLockBinder wiring, Core/SysEx, or APVTS.
5. Unrelated float `originX` in Patch Mutator / Bank Utility / Internal Patches stays **out of scope** (U-8) unless a shared helper is extracted **only** for Body shell reuse.

### AC 5 — Docs, build, sprint

1. Update `u-0-zone-dimension-tables.md` Shell section: separator row **24 × 728**; confirm Shared `kVerticalStackGap` = 12 ownership as U-5 (module gaps remain U-8).
2. macOS Debug build green; existing unit tests green (no new automated GUI pixel tests).
3. Sprint key unchanged; Status → `review` after implementation + UAT.

## Tasks / Subtasks

- [x] **T1 — Baseline capture** (AC: #1, #2, #4)
  - [x] Confirm live `BodyPanel::resized` float `originX` path and current setBounds order
  - [x] Confirm separator full-height placement (Y = 0, H = scaled 728)
  - [x] Confirm Shared 700 vs Patch/Master 704 invariant
  - [ ] Optional: screenshots @ 100 % and @ 150 % / 175 % before edits

- [x] **T2 — Replace BodyPanel column placement** (AC: #1, #2, #4)
  - [x] Refactor `BodyPanel::resized` to successive `removeFromLeft` (after L/R padding) with each width from `ScaledLayout::scaledInt`
  - [x] Place separators on full remaining body height; place columns at `topY = padding` with content heights from dims
  - [x] Delete float `originX` / design-sum × scale X math
  - [x] Grep proof: no `originX` in `BodyPanel.cpp`

- [x] **T3 — SharedPanel gap verify / harden** (AC: #3)
  - [x] Verify Matrix ↔ Patch Manager gap @ 100 % and odd presets
  - [x] Only refactor to `removeFromTop` if needed for consistency / drift

- [x] **T4 — VerticalSeparator + MainComponent audit** (AC: #2, #4)
  - [x] Confirm `VerticalSeparator` needs no logic change when parent sets correct bounds
  - [x] Confirm `MainComponent` body stack (header → body → footer) unchanged except if a body-height bug is found

- [x] **T5 — Docs** (AC: #5)
  - [x] Zone table: separator **24 × 728**; note shell gap ownership
  - [x] Completion Notes: any intentional geometry deltas; UAT result

- [x] **T6 — Manual UAT + build** (AC: #1, #5)
  - [x] Full 7-preset checklist (U-5 is **not** an injection-only smoke story — epic calls out 150/175 drift)
  - [x] Build + existing unit tests green

## Manual UAT checklist (layout audit)

**Goal:** Body columns + separators form one stable grid at every preset; no 1 px drift at odd scales.  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100 %

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|-----|------|------|------|------|------|
| Column widths align (no 1 px gap/overlap between PE / sep / Shared / sep / ME) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Body padding 12 @ 100 % (L/R/T); gutters look even | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Separators full body height (meet Header/Footer borders) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Shared column ~4 px shorter than PE/ME content band (not stretched) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Matrix Mod ↔ Patch Manager gap matches Design (12 @ 100 %) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| No clipped controls at shell edges | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013 — no paint regression) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured (preferred @ 100 + 150 + 175) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** Guillaume · date 2026-07-23  
**Owner narrowing:** Full 7-preset matrix deferred; Guillaume accepted **smoke test success** as UAT gate for this story (story allows owner narrowing). Residual scale scrutiny remains available under U-10 aggregate zone UAT.

**UAT note:** Unlike U-3/U-4 injection-only smokes, **require the full 7-preset matrix** (or at minimum document owner narrowing). Focus scrutiny on **150 %** and **175 %**.

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Dims injected; originX deferred here |
| U-2 / U-2b | done | Do not reopen widget paint / hairlines |
| U-3 / U-4 | done | Header/Footer closed; body left alone by design |
| `spec-vertical-separator-full-height` | done | Preserve 728 @ Y = 0 |
| U-6…U-9 | backlog | **Blocked on this story** for parallel panel audits |
| U-10 | backlog | Formal aggregate zone UAT |

### Recommended order

Baseline → `BodyPanel::resized` integer `removeFromLeft` chain → SharedPanel gap verify → separator/MainComponent smoke → zone table → **full** UAT (esp. 150/175) → build/tests.

### Key files

| Path | Change |
|------|--------|
| `Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp` | **Primary:** remove `originX`; integer column chain |
| `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp` | Verify / optional `removeFromTop` harden |
| `Source/GUI/Widgets/VerticalSeparator.cpp` | Likely no-op if bounds correct |
| `Source/GUI/MainComponent.cpp` | Audit only — do not redesign header/footer stack |
| `Source/GUI/Layout/ScaledLayout.h` | Use existing `scaledInt` only — **no** new flexible distribute API required (`ScaledLayout` comment: fixed Figma dims, no flexible distribution) |
| `u-0-zone-dimension-tables.md` | Separator height 728; shell ownership notes |
| `DimensionFactory.cpp` / `DesignPanels.h` | Touch **only** if a missing/wrong shell token is proven |

### Current BodyPanel layout (replace)

```65:96:Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp
void BodyPanel::resized()
{
    // … scaledInt widths/heights …
    const float originX = static_cast<float>(bounds.getX() + padding);
    // X positions = roundToInt(originX + designSum * sf)  ← REMOVE
    // setBounds: PE / sep1 (full H) / Shared / sep2 (full H) / ME
}
```

### Target algorithm sketch (implement, do not copy blindly)

```cpp
auto area = getLocalBounds();
const int pad = ScaledLayout::scaledInt(static_cast<float>(dims_.padding), sf);
area.removeFromLeft(pad);
area.removeFromRight(pad);

const int topY = pad; // local origin
const int peW = ScaledLayout::scaledInt(…);
const int sepW = ScaledLayout::scaledInt(…);
// …

auto pe = area.removeFromLeft(peW);
patchEditPanel_->setBounds(pe.getX(), topY, pe.getWidth(), peH);

auto sep1 = area.removeFromLeft(sepW);
verticalSeparator1_->setBounds(sep1.getX(), 0, sep1.getWidth(), sepH); // full body H

// shared, sep2, master — same pattern; shared uses sharedColumnHeight (700)
```

Ensure final `area` width after Master Edit is ~0 (within rounding). Prefer consuming exact `scaledInt` widths so remainder is not silently absorbed into the last column unless Design requires it (it should not — identity is exact at 100 %).

### SharedPanel today (already close)

```45:59:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp
// matrixH / stackGap / patchManagerH via ScaledLayout::scaledInt
// y += matrixH + stackGap
```

### MainComponent body placement (do not redesign)

```34:57:Source/GUI/MainComponent.cpp
// headerHeight / bodyHeight / footerHeight via ScaledLayout::scaledInt
// bodyPanel.setBounds(…, headerHeight, …, bodyHeight)
```

### Design SSOT @ 100 % (shell)

| px | Symbol / meaning |
|----|------------------|
| 1300 × 800 | Full GUI |
| 1300 × 728 | Body panel |
| 1276 | Inner width (columns + seps) |
| 12 | Body padding |
| 808 / 268 / 152 | PE / Shared / ME widths |
| 24 | Vertical separator width ×2 |
| 704 | PE / ME content height |
| 700 | Shared column height |
| 12 | Matrix ↔ Patch Manager stack gap |
| 728 | Separator height (full body) |

Identities asserted in `DesignChecks.h` — prefer not to change asserts unless a Figma delta is documented.

### Anti-patterns

- Leaving float `originX` “because widths look fine at 100 %”
- Stretching Shared column to 704 to “align bottoms”
- Shrinking separators back to 704 / content-band height
- Reopening Patch Edit / Matrix / Patch Manager / Master **module** layouts
- Changing Patch Manager **module** gaps (U-8)
- Adding flexible remainder distribution APIs to `ScaledLayout` without need
- Global `AffineTransform` scaling
- Automated GUI pixel tests
- Touching Core / MIDI / APVTS for layout
- “Fixing” unrelated Mutator/BankUtility `originX` while in the area

### Testing

- No new automated GUI pixel tests.
- Manual UAT is the gate — **full 7 presets**, with 150/175 as must-pass.
- Unit tests: existing suite only (`cmake --build --preset macos-debug-arm64` + tests target).

### Previous story intelligence (U-4)

- Prefer behaviour-preserving geometry unless Figma audit forces a **documented** delta.
- U-3/U-4 allowed Standalone smoke for **injection-only** work — **U-5 is layout-critical**; do not inherit that shortcut without Guillaume’s explicit narrowing.
- Finish Clean Code leftovers if any Design* / private `k*` still appear in shell files (unlikely after U-0b).
- If DesignChecks updates are deferred, log in `deferred-work.md`.
- U-4 forbade changing `MainComponent` body/column layout — **that work is this story**.

### Git intelligence

Recent related: U-4 footer close (`42e49dd`); U-3 header injection (`11fc4ad`, `3590229`). Follow integer `ScaledLayout` + Factory dims style; do not invent a new layout framework.

### Latest tech notes

- JUCE 8.0.12: `Rectangle::removeFromLeft` / `removeFromRight` / `setBounds` — stable; no API migration.
- Scale policy SSOT: Design @ 100 % × `uiScale_`; no global AffineTransform on prod UI (`project-context.md`).

### Project context reference

- GUI & UI Scale Rules: `ScaledLayout::scaledInt`; ÷4 design grid; separate stroke thickness (`ScaledDrawing`).
- Agents must not use AffineTransform for main UI scaling.
- Key refs: `ScaledLayout.h`, `WidgetFactory.h` / `DimensionFactory`, `CONVENTIONS.md`.

## Dev Agent Record

### Agent Model Used

Composer (Cursor Agent)

### Debug Log References

- Baseline: `BodyPanel::resized` used float `originX` + design-sum × scale `roundToInt` for column X (classic 150/175 drift path). Separators already Y=0 / H=scaled 728. DesignChecks: Shared 700 vs PE/ME 704 (−4).
- Grep: no `originX` remaining in `BodyPanel.cpp` after T2.
- `VerticalSeparator` paint untouched; bounds set by parent. `MainComponent` header→body→footer stack unchanged.
- Build: `cmake --build --preset macos-debug-arm64` exit 0. Unit tests: `Matrix-Control_Tests` exit 0.

### Completion Notes List

- Replaced Body column placement with successive `removeFromLeft` after L/R padding; each segment width from `ScaledLayout::scaledInt` on injected dims. Separators full body height (Y=0); columns inset by padding with content heights (Shared stays 700).
- Hardened SharedPanel Matrix ↔ Patch Manager stack to `removeFromTop` + scaled `verticalStackGap` (same integer style as Body).
- No intentional Figma geometry deltas — algorithm-only fix. Zone table: separator row **24 × 728**; `kVerticalStackGap` ownership → U-5 (module gaps remain U-8).
- UAT: Guillaume smoke test **success** (2026-07-23); owner narrowed away from full 7-preset matrix (allowed by story). Residual odd-scale scrutiny can ride U-10.

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp`
- `_bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`
- `_bmad-output/implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md`

## Change Log

- 2026-07-23: Story context created (ready-for-dev) — Body shell originX removal + column/separator/gap UAT.
- 2026-07-23: Implemented integer `removeFromLeft` / `removeFromTop` shell layout; docs + build/tests green; awaiting manual UAT.
- 2026-07-23: Owner smoke UAT success; Status → review.
- 2026-07-23: Code review — decision: smoke closes AC1 UAT (odd-scale / full matrix → U-10); 0 patch; 2 defer; status → done.

### Review Findings

- [x] [Review][Decision] Accept U-5 on smoke UAT only, or re-check 150%/175% before closing? — **Resolved (2026-07-23): option 1** — smoke UAT closes U-5; residual 150%/175% / full 7-preset matrix remains owned by U-10 (same precedent as U-4). No code change.
- [x] [Review][Defer] SharedPanel child width still from scaled `dims_.width`, not removeFrom* strip width [SharedPanel.cpp:49-60] — deferred, pre-existing dual path; fine while BodyPanel sizes Shared to matching design width; only matters if Shared is truncated
- [x] [Review][Defer] No debug assert that leftover width after column chain is ~0 [BodyPanel.cpp:98-99] — deferred, pre-existing lack of runtime identity guard; DesignChecks cover compile-time constants; optional for U-10 aggregate UAT
