---
organization: Ten Square Software
project: Matrix-Control
title: Story U-4 — Footer Panel Layout Audit
author: BMad Agent
status: done
baseline_commit: 3590229ba68068c812a0b6e6b28de388109b23dc
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/epics.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-0b-factory-dimension-registry-and-descriptor-decoupling.md
  - implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md
  - implementation-artifacts/7-8-header-footer-shell-and-persistence.md
  - implementation-artifacts/8-2-device-inquiry-and-footer-identity.md
  - implementation-artifacts/8-3-ui-lock-without-synth.md
  - Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.h
  - Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Factories/DimensionFactory.cpp
  - Source/GUI/MainComponent.cpp
  - _bmad-output/project-context.md
created: 2026-07-21
updated: 2026-07-22
---

# Story U.4: Footer Panel Layout Audit

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-4-footer-panel-layout-audit
Closes the footer half of the 7-8 pixel-layout deferral (header half closed in U-3).
-->

## Story

As a user,
I want footer zones, typography, and padding aligned to Figma at every UI scale,
so that messages and device identity read clearly (FR-53 layout only).

## Problem Statement / Errata

### Epic intent vs live UI

Epic U-4 ACs mention **left / centre actions (if present) / right** zones. Live `FooterPanel` is **two-zone only** (Story 7-8):

| Zone | Content |
|------|---------|
| **Left** | Severity icon + `uiMessageText` (info / success / warning / error) |
| **Right** | Device identity `{type} · v{version}` or `PluginDisplayNames::FooterPanel::kNoDevice` |

**There is no centre-actions UI.** Treat “centre actions (if present)” as **N/A** — do not invent a third zone unless Guillaume / Figma explicitly requires it.

### Functional work already done (do not reopen)

| Story | What stays frozen |
|-------|-------------------|
| 7-8 | Two-zone paint, APVTS listeners, severity skin colours |
| 8-2 | Device inquiry → APVTS → right-zone identity (FR-53 display) |
| 8-3 | Left-zone lock guidance via `GrayedControlHelper` / `CompareLockBinder` |

**Out of scope (epic):** `uiMessageText` routing, `ExceptionPropagator`, device inquiry strings / MIDI.

### U-0b debt (this story owns the footer half)

| Live fact | Value / state |
|-----------|----------------|
| Footer size @ 100 % | **1300 × 32** (`Panels::Footer::kWidth` × `kHeight`) |
| Identity min width | **160** in Design* — still read via `#include DesignPanels.h` in panel |
| Padding / icon | Private `kPadding_ = 8`, `kIconSize_ = 14` in `FooterPanel.h` |
| `FooterPanelDimensions` | Stub: `{ width, height }` only |
| Ctor | `FooterPanel(skin, int width, int height, apvts)` — not injected struct |
| Layout | Computed in **`paint()`**; `resized()` empty |
| Rounding mix | padding/icon: `roundToInt(design * uiScale_)`; identity: `ScaledLayout::scaledInt` |

U-3 completed header injection. Mirror that Clean Code path here **and** run the epic layout UAT (footer visuals were **not** owner-accepted as “freeze” the way header was).

### Icon size 14 vs ÷4 rule

`kIconSize_ = 14` is **not** divisible by 4 (Brief / DesignChecks culture). Default for this story:

1. **Promote 14 as-is** into Design* / Factory (document intentional exception).
2. Do **not** snap to 12 or 16 unless Figma evidence or Guillaume asks — changing icon slot width shifts text and risks a visual regression without an audit win.

### Optional owner gate (only if visuals already accepted)

If Guillaume already accepts footer look at all presets (U-3-style), narrow AC 1 to **behaviour freeze** and keep AC 2–5 (injection + UAT non-regression). Until that explicit decision, **full epic layout audit applies**.

## Acceptance Criteria

### AC 1 — Layout audit vs Figma / Design @ all presets (primary)

**Given** U-0 footer dimensions and the brownfield `FooterPanel` two-zone layout  
**When** the footer is shown at each UI Scale preset (50 / 75 / 100 / 125 / 150 / 175 / 200 %)  
**Then**

1. Left message zone and right device identity zone use **scaled** padding / gaps / identity min-width from **injected** panel dimensions (Design* values assembled in `DimensionFactory` at ctor time — not private `k*` or Design includes in the panel).
2. Text baselines and truncation (`drawFittedText`, 1 line) remain readable: long messages truncate in the left remainder; long identity truncates in the right slot; zones do not overlap (identity still capped to ≤ half of content width after padding, same rule as today).
3. No `AffineTransform` on the footer subtree (scale via existing `uiScale_` + `ScaledLayout` / `roundToInt` only).
4. Centre-actions zone: **N/A** (document in Completion Notes).
5. Manual UAT checklist below signed by Guillaume.

### AC 2 — U-0b injection (Clean Code)

**Given** U-0b: panels must not `#include Design*`  
**When** this story lands  
**Then**

1. Expand `FooterPanelDimensions` with every constant used by layout today: at least `width`, `height`, `padding`, `iconSize`, `identityMinWidth` (names may match project style; keep integers identical to current behaviour unless AC 1 Figma audit requires a documented change).
2. `DimensionFactory` fills those fields from Design* (promote `padding` / `iconSize` into `Design::Panels::Footer` or Spacing atoms — padding **8** already matches `Spacing::kMedium`).
3. `FooterPanel` takes `const FooterPanelDimensions&` (plus existing `ISkin&` / `APVTS&`); remove `#include "GUI/Layout/Design/DesignPanels.h"` from the FooterPanel folder.
4. `MainComponent` passes `layoutDimensions_.footer` like header after U-3.
5. Proof: `rg '#include.*Design' Source/GUI/Panels/MainComponent/FooterPanel/` → **zero** hits.
6. Prefer removing unused `width_` / `height_` members if still unread after injection (bounds come from `MainComponent::resized`).

### AC 3 — Preserve functional contracts

**Given** Stories 7-8 / 8-2 / 8-3  
**When** layout/injection changes  
**Then**

1. Still listen to `uiMessageText`, `uiMessageSeverity`, `deviceDetected`, `deviceType`, `deviceVersion`.
2. Identity format unchanged: `{type} · v{version}` or `kNoDevice`; empty type while detected → `kNoDevice`.
3. Severity colours remain skin tokens (`kFooterMessage*`, `kDarkPanelText`); severity icons unchanged (`ℹ` `✓` `⚠` `✗`).
4. No changes to `CompareLockBinder`, `GrayedControlHelper`, `MidiManager`, or inquiry wiring unless a layout-only bug forces a one-line fix (prefer avoid).
5. `PluginDisplayNames::FooterPanel` strings untouched unless typography audit discovers a display bug (unlikely).

### AC 4 — Rounding / structure (no speculative redesign)

1. Document and preserve the **current** scale/round order unless a Figma defect forces a change (then note the delta in Completion Notes).
2. Layout may stay paint-driven (brownfield) **or** move pure geometry helpers out of `paint` for readability — either is fine if on-screen result matches the audit. Do not introduce new layout frameworks.
3. No new abstractions beyond U-0b injection pattern already used by `HeaderPanel`.
4. Optional: add `DesignChecks.h` asserts for newly promoted ÷4 tokens; for `iconSize == 14`, either skip assert or assert the exception explicitly with a one-line comment. If DesignChecks deferred, add a `deferred-work.md` entry (U-3 precedent).

### AC 5 — Docs, build, sprint

1. Update `u-0-zone-dimension-tables.md` Footer section with any newly documented tokens (padding, identity min width, icon size). If editing that file, also fix stale shell height **792 → 800** (`Header 40 + Body 728 + Footer 32`) — leftover from pre–U-3 header height.
2. macOS Debug build green; existing unit tests green (no new automated pixel tests).
3. Sprint key unchanged; Status → `review` after implementation + UAT.

## Tasks / Subtasks

- [x] **T1 — Baseline capture** (AC: #1, #4)
  - [x] Note current `paint()` formulas (padding/icon `roundToInt`, identity `scaledInt`, half-width cap)
  - [ ] Optional: screenshots @ 100 % and one non-100 % preset before edits
  - [x] Confirm Figma / Design targets: height 32, width 1300, identity 160, padding 8; icon 14 as intentional exception unless owner says otherwise

- [x] **T2 — Expand `FooterPanelDimensions` + Design* + Factory** (AC: #2)
  - [x] Add fields; promote tokens into Design* with **identical** integers by default
  - [x] Fill in `DimensionFactory::build…` / layout dimensions assembly
  - [x] Optional DesignChecks for ÷4 tokens; document icon 14 exception

- [x] **T3 — Refactor `FooterPanel` + `MainComponent` wiring** (AC: #1, #2, #3)
  - [x] Ctor takes `const FooterPanelDimensions&`; drop Design* include
  - [x] Replace private `k*` / Design reads with `dimensions_` members
  - [x] Preserve APVTS / paint behaviour; apply any Figma padding/gap fixes found in audit
  - [x] Grep proof: zero Design includes under FooterPanel/

- [x] **T4 — Docs** (AC: #5)
  - [x] Zone table footer tokens (+ shell 800 if touching file)
  - [x] Completion Notes: centre zone N/A; icon 14 exception; any intentional geometry deltas

- [x] **T5 — Manual UAT + build** (AC: #1, #5)
  - [x] Checklist signed; Standalone Debug (+ VST3 if usual)
  - [x] Unit tests: existing suite only

## Manual UAT checklist (layout audit)

**Goal:** Footer matches Design / Figma craftsmanship at every preset; messages and identity remain clear.  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100 %

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|-----|------|------|------|------|------|
| No clipped icon / message / identity | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Padding / identity slot match Design (±0 @ 100 %) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Long message truncates cleanly (left) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Long identity truncates cleanly (right) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Severity colours readable | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| `kNoDevice` vs `type · vX` correct | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Lock guidance still appears when no synth (8-3) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured (optional but preferred) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** Guillaume (Standalone smoke) date 2026-07-22

**UAT decision:** Owner accepted Standalone smoke subset (U-3 precedent) — full 7-preset matrix not required for this injection-only change; recorded 2026-07-22.

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Complete footer injection debt here |
| U-2 / U-2b | done | Do not reopen widget paint / hairlines |
| U-3 | done | Mirror HeaderPanel injection + UAT pattern |
| 7-8 / 8-2 / 8-3 | done | Messaging + identity + lock guidance frozen |
| U-5…U-9 | backlog | Body / columns — out of scope |
| U-10 | backlog | Formal zone sheet aggregates this UAT |

### Recommended order

Baseline → Design* + `FooterPanelDimensions` + Factory → FooterPanel / MainComponent (math preserved unless audit delta) → docs → UAT.

### Key files

| Path | Change |
|------|--------|
| `Source/GUI/Layout/PanelDimensions.h` | Expand `FooterPanelDimensions` |
| `Source/GUI/Layout/Design/DesignPanels.h` | Promote padding / iconSize (and keep height / identity / width) |
| `Source/GUI/Layout/Design/DesignAtoms.h` | Optional: reuse `Spacing::kMedium` for padding |
| `Source/GUI/Layout/Design/DesignChecks.h` | Optional asserts / icon exception |
| `Source/GUI/Factories/DimensionFactory.cpp` | Fill footer fields |
| `Source/GUI/MainComponent.{h,cpp}` | Pass `layoutDimensions_.footer` struct |
| `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` | Inject; drop Design*; layout from dims |
| `u-0-zone-dimension-tables.md` | Document footer tokens; fix shell 792→800 if editing |

### Current `paint()` anchors (preserve unless audit requires change)

```34:76:Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp
// padding / iconSize: roundToInt(design * uiScale_), jmax(1, …)
// identityMinWidth: ScaledLayout::scaledInt(Design Footer::kIdentityMinWidth, uiScale_)
// bounds = getLocalBounds().reduced(padding)
// identity: removeFromRight(jmin(identityMinWidth, bounds.width/2)), centredRight
// message: optional icon (iconSize + padding), then drawFittedText centredLeft, 1 line
```

### MainComponent placement (do not redesign shell stack)

```39:57:Source/GUI/MainComponent.cpp
// footerHeight = ScaledLayout::scaledInt(footer.height, sf)
// footerY = headerHeight + bodyHeight
// footerPanel.setBounds(…); UI Elements mode hides body + footer
```

Ctor today passes raw ints — switch to struct like header:

```cpp
// Today:  footerPanel(skin, layoutDimensions_.footer.width, layoutDimensions_.footer.height, apvts)
// Target: footerPanel(skin, layoutDimensions_.footer, apvts)
```

### Anti-patterns

- Rewiring `uiMessageText` / inquiry / lock binder “while we’re here”
- Inventing a centre-actions cluster without Figma / owner ask
- Snapping icon 14 → 12/16 without evidence
- Restoring Design* includes in the panel after injection
- Changing `MainComponent` body/column layout (U-5)
- Forcing all geometry through `scaledInt` if that alters accepted pixels without an audit finding
- Adding automated GUI pixel tests
- Touching Core / MIDI for layout

### Testing

- No new automated GUI pixel tests.
- Manual UAT is the gate (epic template + footer-specific rows).
- Unit tests: existing suite only (`cmake --build --preset macos-debug-arm64` + tests target).

### Previous story intelligence (U-3)

- Owner reframed U-3 to behaviour-preserving Clean Code after accepting header visuals — **do not assume the same for footer** until Guillaume says so.
- Keep `explicit` on single-arg ctors; avoid dead helpers (`getGap`-style).
- Finish injection fully (U-3 left optional `logoPopupColumnWidth` half-done — for footer, inject all layout constants used in paint).
- Defer DesignChecks only with an explicit `deferred-work.md` note.

### Git intelligence

Recent related commits: U-3 header injection (`11fc4ad`, `3590229`); 7-8 shell (`d4e0ed8`); separator/header-footer frame (`754cb65`). Follow U-3’s Factory-injection style, not a greenfield rewrite.

### Latest tech notes

- JUCE 8.0.12: `Graphics::drawFittedText` / `Component::paint` unchanged for this work — no API migration needed.
- Scale policy remains project SSOT: Design @ 100 % × `uiScale_`; no global AffineTransform on prod UI.

### Project context reference

Follow `_bmad-output/project-context.md`: Factory-only Design* consumption, ÷4 design grid (with documented icon exception), English-only source, French chat only for humans.

### References

- [Source: epic-ui-scale-audit-pixel-perfect-layout.md — Story U-4]
- [Source: u-0-zone-dimension-tables.md — Footer 1300×32]
- [Source: u-0b-factory-dimension-registry-and-descriptor-decoupling.md]
- [Source: u-3-header-right-cluster-layout-and-sign-off.md — injection pattern]
- [Source: 7-8-header-footer-shell-and-persistence.md — pixel audit deferral to U-3/U-4]
- [Source: 8-2-device-inquiry-and-footer-identity.md — identity contract]
- [Source: 8-3-ui-lock-without-synth.md — left-zone guidance]
- [Source: FooterPanel.cpp `paint()`]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Baseline paint formulas preserved: padding/icon `roundToInt(design * uiScale_)` with `jmax(1,…)`; identity `ScaledLayout::scaledInt`; identity slot capped to `jmin(identityMinWidth, bounds.width/2)`.
- Design targets confirmed identical: 1300×32, padding 8, identity 160, icon 14 (intentional ÷4 exception).
- macOS Debug build green; `Matrix-Control_Tests` exit 0.
- Grep: zero `#include.*Design` under `Source/GUI/Panels/MainComponent/FooterPanel/`.

### Completion Notes List

- Centre-actions zone: **N/A** (two-zone footer only — left message, right identity).
- Icon size **14** promoted as intentional ÷4 exception (`DesignPanels` comment + `DesignChecks` assert `kIconSize == 14`).
- No intentional geometry deltas vs pre-U-4 paint math; injection-only Clean Code path mirroring HeaderPanel/U-3.
- Removed unread `width_` / `height_` members (bounds still owned by `MainComponent::resized`).
- Shell height table corrected **792 → 800** in `u-0-zone-dimension-tables.md`.
- Manual UAT: Guillaume signed Standalone smoke subset on 2026-07-22 (success); full 7-preset matrix deferred to formal zone sheet (U-10) per U-3 precedent.

### File List

- `Source/GUI/Layout/PanelDimensions.h`
- `Source/GUI/Layout/Design/DesignPanels.h`
- `Source/GUI/Layout/Design/DesignChecks.h`
- `Source/GUI/Factories/DimensionFactory.cpp`
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.h`
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp`
- `Source/GUI/MainComponent.cpp`
- `_bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md`
- `_bmad-output/implementation-artifacts/u-4-footer-panel-layout-audit.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Review Findings

- [x] [Review][Decision] Accept Standalone smoke as AC1 UAT gate, or require full 7-preset matrix? — **Resolved (2026-07-22): option 1** — Standalone smoke is sufficient to close U-4; full 7-preset matrix remains owned by U-10 (U-3 precedent). No code change.
- [x] [Review][Patch] Lock absolute Footer DesignChecks for padding and identity min-width [Source/GUI/Layout/Design/DesignChecks.h:19] — Applied 2026-07-22: `kPadding == 8` and `kIdentityMinWidth == 160` (kept ÷4 asserts).
- [x] [Review][Defer] Pathological footer paint bounds at extreme/invalid scales [FooterPanel.cpp:40-70] — deferred, pre-existing — identity min-width has no `jmax(1,…)` floor; huge padding can empty bounds; `iconSize + padding` can exceed leftover width. Unchanged math vs pre-U-4; normal 50–200 % presets unaffected.
- [x] [Review][Defer] Icon-to-message gap reuses chrome padding (no dedicated gap token) [FooterPanel.cpp:70] — deferred, pre-existing — Figma may separate inset vs icon gap; injection story preserved brownfield `iconSize + padding`.

## Change Log

- 2026-07-21: Story created (ready-for-dev) — footer layout audit + U-0b injection; centre zone N/A; icon 14 exception documented.
- 2026-07-21: Implemented FooterPanelDimensions injection + Design* promotion; build/tests green; awaiting Manual UAT.
- 2026-07-22: Standalone smoke UAT signed by Guillaume (success); status → review.
- 2026-07-22: Code review — decision: Standalone smoke closes AC1 (full matrix → U-10); patch: absolute Footer DesignChecks for padding/identity; 2 defer; status → done.
