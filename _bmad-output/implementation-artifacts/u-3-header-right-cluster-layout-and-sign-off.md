---
organization: Ten Square Software
project: Matrix-Control
title: Story U-3 — Header Panel Clean Code Refactor (Behaviour-Preserving)
author: BMad Agent
status: done
baseline_commit: 8e72fad869c8e9211b0c8197fb523b6eb620a7f2
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/epics.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-0b-factory-dimension-registry-and-descriptor-decoupling.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - implementation-artifacts/7-8-header-footer-shell-and-persistence.md
  - Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h
  - Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Layout/ScaledLayout.h
  - _bmad-output/project-context.md
created: 2026-07-21
updated: 2026-07-21
---

# Story U.3: Header Panel Clean Code Refactor (Behaviour-Preserving)

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key (unchanged): u-3-header-right-cluster-layout-and-sign-off
Owner decision 2026-07-21: visual header @ all UI Scale already accepted.
Scope = Clean Code / U-0b injection only — preserve on-screen behaviour.
-->

## Story

As a maintainer,
I want `HeaderPanel` (and related logo sizing) to follow Factory-injected dimensions and Clean Code structure,
so that the header matches the rest of the UI architecture **without changing** the layout or interactions Guillaume already accepts at every UI Scale.

## Owner decision (hard gate)

**Visual / interaction result is already accepted** at presets 50–200 %. This story is **not** a redesign and **not** a Figma re-audit.

| Allowed | Forbidden |
|---------|-----------|
| Move constants into `HeaderPanelDimensions` + Factory | Change packet order, visibility rules, or wiring |
| Remove Design* includes from HeaderPanel / Logo sizing | Re-add SCALE/SKIN/UI Elements on the bar |
| Same numeric formulas → same bounds | “Improve” rounding / vertical centre / logo offset |
| Docs + dead-token cleanup | Behaviour changes to `HeaderLogoPopupMenu` |
| Non-regression UAT (looks identical) | Pixel hunting against old epic “right cluster” ACs |

**Rounding rule (non-negotiable):** keep the **same** scale/round order as today’s `resized()` (mostly `roundToInt(design * sf)` for packet advance; `ScaledLayout::scaledInt` only where already used — logo W/H and `kContentVerticalOffset`). Do **not** “normalize everything to `scaledInt`” if that alters bounds. Preserve **unscaled** `kLogoVerticalOffset` (−1) as today.

## Problem Statement / Errata

Epic U still mentions a SCALE/SKIN/UI Elements **right cluster**. That UI is gone (logo popup + Shift+Ctrl UI Elements). Keep sprint key; ignore historical right-cluster geometry work.

| Live fact | Value |
|-----------|--------|
| Header height (Design) | **40** (`Panels::Header::kHeight`) — zone table synced to 40 |
| Bar contents | Logo + left routing packets only |
| Skin / UI Scale | `HeaderLogoPopupMenu` |
| UI Elements | Logo Shift+Ctrl (Debug) |
| Gaps | `kGap_=4`, `kPacketExternalGap_=16` |
| Dead tokens | `kHeaderPanelTheme` (68), `kSettingsButtonWidth` (72) |

## Acceptance Criteria

### AC 1 — Behaviour freeze (primary)

**Given** the current HeaderPanel layout and logo gestures are owner-accepted  
**When** this story merges  
**Then**

1. At **100 %** UI scale, control bounds (logo + each packet widget) match pre-refactor positions (±0 px). Prefer a short before/after note or screenshot pair in Completion Notes if practical.
2. At other presets, layout remains **visually identical** to pre-refactor (no intentional re-centring, gap, or width changes).
3. Gestures unchanged: logo click → popup; Shift → Settings; double-click → 100 % scale; Shift+Ctrl → UI Elements (Debug).
4. Standalone vs plugin packet visibility unchanged (audio/gain/peak standalone-only; Keyboard From HOST in plugin).
5. No functional change to MIDI/audio port lists, LEDs, or editor wiring.

### AC 2 — U-0b injection (Clean Code)

**Given** U-0b: panels must not `#include Design*`  
**When** refactor lands  
**Then**

1. Expand `HeaderPanelDimensions` with every constant currently used by HeaderPanel ctor / `resized()` (logo W/H, logo vertical offset, content vertical offset, logo gap after, left padding, control height, LED size, intra/inter-packet gaps, label/combo/slider/peak widths). Optionally include `logoPopupColumnWidth` if Logo popup injection is in the same pass.
2. `DimensionFactory` fills fields from Design* (and keep packet widths that today live only as `HeaderPanel` private `k*` — promote those into Design atoms **only if** values stay identical; otherwise inject the same integers via Factory without inventing new Figma values).
3. `HeaderPanel` takes `const HeaderPanelDimensions&` (or equivalent); remove Design* includes from HeaderPanel.
4. `Logo` no longer reads Design* for W/H — sizes come from injected dims / parent.
5. `grep '#include.*Design' Source/GUI/Panels/MainComponent/HeaderPanel/` → zero hits. Logo Design* include removed if sizing is injected.

### AC 3 — Structure / readability (Clean Code, no behaviour)

1. Prefer named injected fields over private `inline constexpr static int k*_` duplicated in the panel (or keep thin aliases that read from `dimensions_` — no second source of truth).
2. `resized()` may be clarified (helpers already present) but must call the **same** placement math.
3. No `AffineTransform` for scale. No new abstractions beyond what U-0b already uses elsewhere.
4. Respect project Clean Code limits where practical; do not explode the class for speculative DRY.

### AC 4 — Docs & dead tokens (low risk, same story)

1. Update `u-0-zone-dimension-tables.md` Header height **32 → 40**; remove or annotate theme-button row per token decision.
2. Remove unused `kHeaderPanelTheme` / `kSettingsButtonWidth` **or** leave with a one-line “reserved / unused” comment — prefer **remove** if no Figma reservation.
3. Fix stale `project-context.md` “UI Elements button … right side” → logo Shift+Ctrl Debug gesture.
4. Short “FR-41 layout slots” note: logo anchor + popup columns already implemented in 7.8/7.10 — document only.

### AC 5 — Non-regression UAT & build

1. Guillaume runs the checklist below (non-regression, not Figma hunt).
2. macOS Debug: Standalone (+ VST3 if usual) and unit tests green.
3. Sprint key unchanged; Status → `review` after implementation.

## Tasks / Subtasks

- [x] **T1 — Capture baseline** (AC: #1)
  - [x] Note current `resized()` formulas (especially logo offset unscaled −1)
  - [x] Optional: screenshots @ 100 % and one non-100 % preset before edits

- [x] **T2 — Expand `HeaderPanelDimensions` + Factory** (AC: #2)
  - [x] Add fields; fill in `DimensionFactory` with **identical** integers
  - [x] Wire `MainComponent` → `HeaderPanel(skin, dimensions)`

- [x] **T3 — Refactor HeaderPanel / Logo without math changes** (AC: #1, #2, #3)
  - [x] Replace private `k*` / Design reads with `dimensions_` members
  - [x] Preserve round/scale order byte-for-byte where it matters
  - [x] Inject Logo W/H; remove Design* from HeaderPanel (+ Logo if applicable)

- [x] **T4 — Docs + dead tokens** (AC: #4)
  - [x] Zone table, project-context, token remove-or-comment, FR-41 slot blurb

- [x] **T5 — Non-regression UAT + proofs** (AC: #5)
  - [x] Checklist signed; paste Design-include grep proof

## Manual UAT checklist (non-regression)

**Goal:** “Looks and behaves the same as before.”  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100%

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|-----|------|------|------|------|------|
| Same as pre-refactor (no new clip / jump) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Logo + packets alignment unchanged | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Logo popup / scale / skin still work | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Standalone audio packet OK | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Plugin HOST keyboard / no audio packet | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** Guillaume — Standalone smoke test OK (2026-07-21). Full scale-preset checklist still optional if desired.

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Complete header injection debt here |
| U-2 / U-2b | done | Do not reopen widget paint |
| 2.11 | done | Left cluster frozen |
| 7.8 / 7.10 | done | Popup behaviour out of scope |
| U-4…U-9 | backlog | Other zones; unrelated |
| U-10 | backlog | Formal zone sheet: this UAT counts as header sign-off |

### Recommended order

Baseline → Dimensions + Factory → HeaderPanel/Logo wiring (math preserved) → docs/tokens → UAT.

### Key files

| Path | Change |
|------|--------|
| `Source/GUI/Layout/PanelDimensions.h` | Expand `HeaderPanelDimensions` |
| `Source/GUI/Factories/DimensionFactory.cpp` | Fill header fields |
| `Source/GUI/MainComponent.{h,cpp}` | Pass dimensions struct |
| `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.{h,cpp}` | Inject; drop Design* |
| `Source/GUI/Widgets/Logo.{h,cpp}` | Ctor W/H injection |
| `Source/GUI/Widgets/HeaderLogoPopupMenu.*` | Touch **only** if injecting column width with zero behaviour change |
| `u-0-zone-dimension-tables.md` / `project-context.md` | Doc sync |

### Current `resized()` anchors (preserve)

```148:182:Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
// controlY / ledY: centre in panelHeight + scaledInt(kContentVerticalOffset)
// logo W/H: ScaledLayout::scaledInt
// logo Y: controlY + kLogoVerticalOffset  // UNSCALED −1 — keep
// packet x advance: roundToInt(x), widths via roundToInt(design * sf)
```

### Anti-patterns

- Redesigning gaps/centring “while we’re here”
- Forcing all geometry through `scaledInt` when current code uses `roundToInt(f * sf)`
- Scaling `kLogoVerticalOffset` without owner ask
- Restoring on-bar SCALE/SKIN/UI Elements
- Footer / body work (U-4 / U-5)

### Testing

- No new automated GUI pixel tests.
- Non-regression UAT is the gate.
- Unit tests: existing suite only.

### References

- [Source: epic-ui-scale-audit-pixel-perfect-layout.md — Story U-3 historical ACs — superseded by owner decision above]
- [Source: u-0b-factory-dimension-registry-and-descriptor-decoupling.md]
- [Source: 7-8-header-footer-shell-and-persistence.md — pixel audit deferral; functional popup done]
- [Source: HeaderPanel.cpp `resized()`]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Baseline `resized()` (pre-refactor): packet advance via `roundToInt(design * sf)`; logo W/H + `contentVerticalOffset` via `ScaledLayout::scaledInt`; logo Y uses unscaled `logoVerticalOffset` (−1).
- Build: `cmake --build --preset macos-debug-arm64` — success (Standalone + VST3 + AU + tests target).
- Unit tests: `Matrix-Control_Tests` exit 0.

### Completion Notes List

- Behaviour-preserving Clean Code / U-0b injection only — no intentional layout or gesture changes.
- `HeaderPanelDimensions` expanded; `DimensionFactory` fills identical integers from Design* (packet widths promoted into `Design::Panels::Header`).
- `HeaderPanel` takes `const HeaderPanelDimensions&`; private `k*` removed; Design* includes removed from HeaderPanel folder.
- `Logo` ctor takes injected W/H; Design* include removed from Logo.
- Dead tokens removed: `kHeaderPanelTheme`, `kSettingsButtonWidth`.
- Docs: zone table header height 32→40; FR-41 slot blurb; project-context UI Elements → logo Shift+Ctrl.
- Design-include grep proof: `rg '#include.*Design' Source/GUI/Panels/MainComponent/HeaderPanel/` → zero hits; Logo has no Design includes.
- Manual non-regression: Guillaume confirmed Standalone smoke test success (2026-07-21). Full multi-preset table left optional.
- `HeaderLogoPopupMenu` left on Design `kLogoPopupColumnWidth` (optional injection deferred — field present on dimensions struct for future use).

### File List

- Source/GUI/Layout/PanelDimensions.h
- Source/GUI/Layout/Design/DesignPanels.h
- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Factories/DimensionFactory.cpp
- Source/GUI/MainComponent.cpp
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
- Source/GUI/Widgets/Logo.h
- Source/GUI/Widgets/Logo.cpp
- _bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md
- _bmad-output/implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md
- _bmad-output/implementation-artifacts/sprint-status.yaml
- _bmad-output/project-context.md

## Change Log

- 2026-07-21: Story created (ready-for-dev) with full-bar audit framing.
- 2026-07-21: **Scope reduced per owner:** behaviour-preserving Clean Code / U-0b injection only; visual Figma re-audit dropped; rounding and logo offset must stay identical.
- 2026-07-21: Implemented U-0b header injection + docs/token cleanup; status → review.
- 2026-07-21: Code review complete — smoke UAT accepted; patches applied (remove getGap, restore Logo explicit, fix Errata); status → done.

### Review Findings

- [x] [Review][Decision] Is Standalone smoke UAT enough to close U-3, or must the full multi-preset scale checklist be completed first? — **Resolved (owner 2026-07-21): option 1 — Standalone smoke test = success; full multi-preset matrix not required to close.**
- [x] [Review][Patch] Remove unused `HeaderPanel::getGap()` dead API [`Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h:79`] — applied
- [x] [Review][Patch] Restore `explicit` on `Logo` constructor [`Source/GUI/Widgets/Logo.h:15`] — applied
- [x] [Review][Patch] Fix stale story Errata claiming zone table still says Header height 32 [`_bmad-output/implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md` Problem Statement] — applied (Errata now notes zone table synced to 40)
- [x] [Review][Defer] Add DesignChecks for newly promoted Header packet tokens [`Source/GUI/Layout/Design/DesignChecks.h`] — deferred, pre-existing (private panel constants also lacked ÷4 asserts)
- [x] [Review][Defer] Propagate Header height 40 into other planning artifacts still citing 32 — deferred, pre-existing / out of AC4 file list
- [x] [Review][Defer] Wire `logoPopupColumnWidth` into `HeaderLogoPopupMenu` (still reads Design*) [`Source/GUI/Widgets/HeaderLogoPopupMenu.h`] — deferred, AC2 optional / documented deferral
