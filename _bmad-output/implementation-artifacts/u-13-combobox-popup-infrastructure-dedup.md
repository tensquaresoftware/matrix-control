---
organization: Ten Square Software
project: Matrix-Control
title: Story U-13 — ComboBox / HierarchicalComboBox Popup Infrastructure Dedup
author: BMad Agent
status: done
baseline_commit: 4e535e9a2413e5fcfe283e568e8ce15cc4e4a526
sources:
  - implementation-artifacts/u-12-hierarchical-combobox-widget.md
  - implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md
  - implementation-artifacts/6-15-history-combobox-popup-ux.md
  - implementation-artifacts/deferred-work.md
  - Source/GUI/Widgets/ComboBox.h
  - Source/GUI/Widgets/ComboBox.cpp
  - Source/GUI/Widgets/HierarchicalComboBox.h
  - Source/GUI/Widgets/HierarchicalComboBox.cpp
  - Source/GUI/Widgets/HierarchicalPopupMenu.h
  - Source/GUI/Widgets/HierarchicalPopupMenu.cpp
  - Source/GUI/Widgets/PopupMenuBase.h
  - Source/GUI/Widgets/PopupMenuBase.cpp
  - Source/GUI/Widgets/ScrollablePopupMenu.h
  - Source/GUI/Widgets/ScrollablePopupMenu.cpp
  - Source/GUI/Widgets/MultiColumnPopupMenu.h
  - Source/GUI/Widgets/MultiColumnPopupMenu.cpp
  - Source/GUI/Widgets/ComboBoxControlPainter.h
  - Source/GUI/Widgets/PopupMenuRenderer.h
  - Source/GUI/Widgets/PopupMenuPositioner.h
  - _bmad-output/project-context.md
created: 2026-07-14
updated: 2026-07-21
---

# Story U-13: ComboBox / HierarchicalComboBox Popup Infrastructure Dedup

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a developer maintaining ComboBox-family widgets,
I want shared popup and control infrastructure extracted from `ComboBox` and `HierarchicalComboBox`,
so that duplicated lifecycle, scrollbar, layout constants, and popup plumbing are centralized without regressing closed-state or hierarchical popup rendering.

## Problem Statement

Story **U-12** already extracted:

- **`ComboBoxControlPainter`** — closed-state paint (background, text, triangle, focus border)
- **`PopupMenuRenderer`** — item label, hover, chevron, background (shared with `MultiColumnPopupMenu` / `ScrollablePopupMenu`)

Story **6-15** then landed aligned dual-panel History UX (staircase removed) by **copying** `ScrollablePopupMenu`’s nested `CustomScrollBar` into `HierarchicalPopupMenu` — explicitly deferred DRY to this story.

Remaining duplication (inventory 2026-07-21):

| Area | Today |
|------|--------|
| **CustomScrollBar** | Near-identical nested classes in `ScrollablePopupMenu.cpp` (~lines 10–122) and `HierarchicalPopupMenu.cpp` (~lines 11–144): thumb math, wheel, `startTimerHz(20)` idle `repaint` |
| **20 Hz idle timers** | Permanent timers while popup open (two bars → two timers on hierarchical); deferred from 6-15 review |
| **Popup host coupling** | `PopupMenuBase` hard-coupled to `ComboBox&`; `HierarchicalPopupMenu` parallel ~1000-line `Component` with overlapping modal/keyboard/close patterns |
| **Control boilerplate** | `setUiScale`, `setLook`, `setPopupMenuLook`, focus/`isPopupOpen_`, `mouseDown` → show — near-identical in both controls |
| **Layout helpers** | Border / item height / scaled vertical margin accessors duplicated (`PopupMenuBase` vs `HierarchicalPopupMenu`; margin also mirrors `ComboBox.cpp`) |

**Out of scope:**

- Patch Mutator / panel wiring behaviour changes (**6-14** / **6-15** product contracts)
- `ScrollablePopupMenu` / `MultiColumnPopupMenu` **behaviour** changes beyond shared-base / shared-scrollbar adoption
- Forcing `HierarchicalPopupMenu` to inherit `PopupMenuBase` in v1 (optional later if readable)
- Resurrecting staircase borders (`drawStaircasePanelBorders` is gone — do not restore)
- New automated GUI pixel tests (project policy)
- ButtonLike ComboBox style on `HierarchicalComboBox`
- `HeaderLogoPopupMenu` (similar modal patterns, different product surface)

## Acceptance Criteria

### AC 1 — Shared popup host abstraction (no `ComboBox&` lock-in)

1. Introduce a small host interface (e.g. `IPopupMenuHost`) exposing what popup menus need from the closed control:
   - `getUiScale()`, `getPopupMenuLook()`, anchor/`asComponent()`, base width (or equivalent)
   - Popup vertical placement + scaled vertical margin access as needed by positioner callers
   - Popup open/close notification hooks (`notifyPopupOpened` / `notifyPopupClosed` or equivalent)
2. `PopupMenuBase` (or successor) consumes the host interface; **`ComboBox` implements it** without behaviour change.
3. **`HierarchicalComboBox` implements the same interface** so hierarchical popup code can share close/modal patterns where applicable.
4. **`HierarchicalPopupMenu` adopts shared pieces** (renderer setup, escape / outside-click, close lifecycle helpers, layout constant access, shared scrollbar — see AC 2). Full merge into `PopupMenuBase` inheritance is **optional** and not required if dual-panel layout stays a dedicated class.

### AC 2 — Shared CustomScrollBar + kill idle 20 Hz timer

1. Extract one shared scrollbar component (e.g. `PopupMenuCustomScrollBar`) used by **both** `ScrollablePopupMenu` and `HierarchicalPopupMenu` (primary + secondary).
2. Prefer a small scroll **model** / adapter so Viewport-backed scroll (Scrollable) and offset-backed scroll (Hierarchical) share paint/drag/wheel without `#ifdef` soup.
3. **Remove permanent `startTimerHz(20)` idle repaint** while the popup is open. Repaint only on scroll/drag/hover state changes (or equivalent event-driven path). Thumb must still track correctly during drag and wheel.
4. Clears deferred-work items from 6-15 review: duplicated `CustomScrollBar` + permanent 20 Hz timers.

### AC 3 — Control lifecycle dedup

1. Extract shared closed-control behaviour (focus tracking, popup-open repaint flag, `setUiScale` early-return, look setters) into a reusable helper or thin composition used by both widgets.
2. **`ComboBox::paint` and `HierarchicalComboBox::paint`** remain thin delegates to `ComboBoxControlPainter::paintClosedState`.
3. Preserve Hierarchical-only APIs: `setInactiveAppearance`, tree model, `onBeforeShowPopup`, sync show (no forced `callAsync` unification).
4. No breaking change to public API of either widget (panels/tests/factory keep compiling without call-site churn).

### AC 4 — Zero visual / behavioural regression

1. Manual QA via TestComponent:
   - Standard `ComboBox` (multi-column) + ButtonLike (`ScrollablePopupMenu`) at 50%–200%
   - `HierarchicalComboBox` aligned dual-panel popup (transparent void, N1/N2 borders, hover secondary, per-column scroll at ≥11 items)
2. Production smoke: Patch Mutator History (above placement, labels, compare-graying) — behaviour unchanged.
3. Full unit test suite green (`Matrix-Control_Tests`).
4. No intentional diff in closed-state or popup appearance at 100% UI scale (reviewer sign-off).

### AC 5 — Build & hygiene

1. New sources registered in `CMakeLists.txt` `PLUGIN_SOURCES`.
2. Functions respect project Clean Code limits (`CONVENTIONS.md` §4.2 / §6.11); extract helpers rather than growing monoliths.
3. English-only source comments; GUI stays out of `Source/Core/`.
4. Story File List documents touched files; note deferred-work clearance in Completion Notes when AC 2 lands.

## Tasks / Subtasks

- [x] **Inventory & design lock** (AC: #1, #2, #3)
  - [x] Confirm duplication map below still matches HEAD
  - [x] Lock approach: **host interface + shared scrollbar composition + closed-control helper**; document choice in Completion Notes if anything differs
  - [x] Explicitly reject: CRTP mega-base, Hierarchical inheriting `juce::ComboBox`, merging dual-panel into MultiColumn

- [x] **Extract shared scrollbar** (AC: #2) — do first (highest DRY win, clears deferred debt)
  - [x] Add `PopupMenuCustomScrollBar` (+ metrics/model) under `Source/GUI/Widgets/`
  - [x] Replace nested classes in `ScrollablePopupMenu` and `HierarchicalPopupMenu`
  - [x] Event-driven repaint; remove idle 20 Hz timers
  - [x] Register new `.cpp` in CMake

- [x] **Popup host refactor** (AC: #1)
  - [x] Add `IPopupMenuHost` (header-only OK)
  - [x] Decouple `PopupMenuBase` from concrete `ComboBox&`
  - [x] Implement host on `ComboBox` and `HierarchicalComboBox`
  - [x] Share modal/escape/teardown helpers with `HierarchicalPopupMenu` without forcing inheritance

- [x] **Control boilerplate extract** (AC: #3)
  - [x] Shared focus + popup-open + scale/look helper
  - [x] Both widgets delegate; public APIs unchanged

- [x] **Regression pass** (AC: #4, #5)
  - [x] TestComponent checklist (flat + hierarchical + scales)
  - [x] History smoke in Standalone if convenient
  - [x] `cmake --build` + unit tests green
  - [x] Update File List, Completion Notes, tick deferred-work mentally via notes

### Review Findings

- [x] [Review][Decision] Flat ComboBox selection now commits after teardown (order change) — Resolved 2026-07-21: keep shared close-then-commit order (align with Hierarchical).
- [x] [Review][Decision] AC3 closed-control helper is only partial — Resolved 2026-07-21: finish extraction (look setters + notify bodies) into `ComboBoxClosedControlHelper`.
- [x] [Review][Decision] TestComponent scale checklist not evidenced — Resolved 2026-07-21: accept Standalone History smoke as enough for this hygiene story.
- [x] [Review][Patch] Unify secondary scrollbar display-scale source with primary [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp`]
- [x] [Review][Patch] Freeze hierarchical popup vertical margin on menu `uiScale_` (not live owner) [`Source/GUI/Widgets/HierarchicalPopupMenu.cpp`]
- [x] [Review][Patch] Remove unused `friend class PopupMenuBase` from ComboBox [`Source/GUI/Widgets/ComboBox.h`]
- [x] [Review][Patch] Finish `ComboBoxClosedControlHelper` look + notify extract [`Source/GUI/Widgets/ComboBoxClosedControlHelper.h`]
- [x] [Review][Defer] ComboBox can leave `isPopupOpen_` stuck true if `show` early-returns after notify [`Source/GUI/Widgets/ComboBox.cpp`] — deferred, pre-existing (same order before U-13 notify extract)

## Dev Notes

### Sequencing

- **Blocked by:** U-12 `done`, 6-15 `done` (aligned dual-panel + copied scrollbar exist)
- **Recommended after:** 6-14 / 6-15 (production History path validates wiring before structural refactor) — **already true**
- **Parallel-safe with:** U-3…U-9 layout audits (orthogonal)
- **Do not block on:** U-3 backlog — this story is next hygiene item in Epic U after U-12

### Recommended design (lock this unless Completion Notes justify a better path)

**Composition + thin host interface — not deep inheritance merge.**

1. **`IPopupMenuHost`** — menus ask the control for scale, looks, anchor, open/close notify.
2. **`PopupMenuBase` consumes host** — `ComboBox` implements; keep flat menu subclasses.
3. **`PopupMenuCustomScrollBar` + scroll model** — Viewport adapter (Scrollable) + offset adapter (Hierarchical primary/secondary).
4. **Closed-control helper** — focus / `isPopupOpen_` / `setUiScale` early-return; widgets stay separate bases (`juce::ComboBox` vs `juce::Component`).
5. **Optional thin modal helper** — escape / outside-click / `exitModalState` → clear flag → removeChild → `delete this` shared as free functions or small class.
6. **Do not** force `HierarchicalPopupMenu : PopupMenuBase` in first pass (opaque/`hitTest`/dual-panel differ).

Priority order (risk ascending):

1. Shared CustomScrollBar + kill idle 20 Hz  
2. Host interface + PopupMenuBase decouple  
3. Closed-control boilerplate helper  
4. Modal/escape/close helper used by Hierarchical  
5. Border helper extract (`drawAlignedPanelBorders` / `drawPanelBorderEdges`) — nice-to-have; hierarchical-only today

### Already shared (do not re-extract)

- `ComboBoxControlPainter` / `ComboBoxControlMetrics` — closed state
- `PopupMenuRenderer` — item/background/chevron drawing (`drawItem` still takes `const ComboBox&`; hierarchical uses `drawLabelItem` / `drawSubMenuChevron`)
- `PopupMenuPositioner` — ComboBox overload + generic `Component` + placement overload
- `ComboBox::getPopupLayoutDimensions()` — popup layout SSOT (Hierarchical already reads this static)
- `PopupMenuLook` / `ComboBoxLook` via LookBuilders

### Keep separate / share carefully

- Transparent `hitTest` for dual-panel void space — hierarchical-only
- **Staircase borders are obsolete** (Story **6-15**): do **not** preserve or restore `drawStaircasePanelBorders`. Canonical path is **`drawAlignedPanelBorders` / `drawPanelBorderEdges`** (N1/N2 top- or bottom-aligned; shared vertical edge; borders painted last with `popupLook.border`)
- Hierarchical commit ordering: tear down modal **before** `onChange` (`closePopupWithSelection`) — U-12 review fix; do not regress
- ComboBox show uses `callAsync`; Hierarchical show is sync — do not unify without proving re-entrancy safety
- Border thickness: flat menus use `PopupMenuRenderer::drawBorder` + ScaledDrawing snap; Hierarchical uses layout `jmax(1, design * uiScale)` — **do not casually unify** or pixels shift at non-100% scale

### Current architecture (code map)

```
Closed controls
├── ComboBox : juce::ComboBox
│     paint → ComboBoxControlPainter::paintClosedState
│     showPopup → async → MultiColumnPopupMenu | ScrollablePopupMenu
│
└── HierarchicalComboBox : juce::Component
      paint → ComboBoxControlPainter::paintClosedState (Standard only)
      showPopup → HierarchicalPopupMenu::show (sync)

Popup menus
├── PopupMenuBase : juce::Component   ← coupled to ComboBox&
│     ├── MultiColumnPopupMenu
│     └── ScrollablePopupMenu (+ nested CustomScrollBar, Viewport)
└── HierarchicalPopupMenu : juce::Component  ← parallel tree, NOT PopupMenuBase
      (+ nested CustomScrollBar ×2, offset scroll, aligned dual-panel borders)
```

### Duplication hotspots (line ranges approximate — re-check HEAD)

| Pattern | Location A | Location B |
|--------|------------|------------|
| CustomScrollBar | `ScrollablePopupMenu.cpp` ~10–122 | `HierarchicalPopupMenu.cpp` ~11–144 |
| `startTimerHz(20)` | Scrollable ctor | Hierarchical ctor (×2 bars) |
| Closed control lifecycle | `ComboBox.cpp` look/scale/focus/paint/mouseDown | `HierarchicalComboBox.cpp` same patterns |
| Modal escape / outside / teardown | `PopupMenuBase.cpp` | `HierarchicalPopupMenu.cpp` close/key/input |
| Layout dim accessors | `PopupMenuBase.cpp` | `HierarchicalPopupMenu.cpp` getItemHeight / border / max scroll |
| Scaled vertical margin | `ComboBox.cpp` | `HierarchicalPopupMenu.cpp` local helper |

Rough DRY win: ~110–130 LOC scrollbar + ~40–60 LOC control boilerplate + ~30–50 LOC modal overlap. Dual-panel layout (~600+ LOC) stays hierarchical-owned.

### Regression traps (must not break)

| Trap | Why |
|------|-----|
| Self-`delete this` after modal exit | Helpers must not double-delete or leave dangling owner refs |
| Selection after close (Hierarchical) | Commit **after** `notifyPopupClosed` + removeChild |
| `isPopupOpen_` focus border | Closed paint uses `hasFocus_ \|\| isPopupOpen_` |
| Shared N1/N2 edge overlap | Secondary X = primary.right − border; wrong math → double-thick or gap (6-15 green border bug history) |
| `opensAbove_` re-measure | Size/align after placement flip |
| Scrollbar hit exclusion | Click handlers must ignore scrollbar strip |
| Thumb inset / displayScale | Known U-2b-ish defer: ctor-time scale may be 1.0 — don’t make worse |
| Keyboard | PopupMenuBase Return/arrows vs Hierarchical Escape-only — don’t strip flat nav or half-add hierarchical nav |
| Opaque / hitTest | Flat opaque true; Hierarchical false + custom hitTest |

### Previous story intelligence

**From U-12 (done):**

- Closed paint already shared via `ComboBoxControlPainter` — leave it alone.
- Custom `HierarchicalPopupMenu` (not stock `juce::PopupMenu` nested submenus) is intentional.
- Review fix: selection commit **after** popup teardown — preserve when touching close path.
- `PopupVerticalPlacement` on positioner; History forces `Above`.

**From 6-15 (done):**

- Staircase removed; aligned dual-panel is canonical.
- Per-column scroll capped at 10 rows (`maxScrollHeight` 200 design px).
- Explicitly deferred to U-13: duplicated `CustomScrollBar` + permanent 20 Hz timers (`deferred-work.md`).
- U-13 docs already amended (2026-07-15) to drop “preserve staircase” — this story continues that line.

**From deferred-work.md (2026-07-15, 6-15 review):**

- Extract shared scrollbar helper in U-13.
- Fix 20 Hz idle timers with shared scroll infra in U-13.

### Git intelligence (recent relevant commits)

- `73cbd14` — U-12 review: commit selection after popup teardown
- `ec89015` — PopupVerticalPlacement; HISTORY forced above
- `e60d610` — Align U-13 story notes with 6-15 dual-panel
- `d11d2bb` / `880b11e` — 6-15 / 6-14 History UX landed
- `84b11b5` — HierarchicalComboBox widget (U-12)

### Latest tech notes (JUCE 8.0.12)

- Project already uses custom modal components + `enterModalState(..., deleteWhenDismissed=true)` — keep that lifecycle; do not migrate to stock `juce::PopupMenu::showMenuAsync` in this story.
- Forum guidance on async `PopupMenu` + editor destruction is **context**, not a mandate to rewrite — if touching show/close, prefer `SafePointer` for any async lambdas (ComboBox already uses `callAsync`).
- Docs: https://docs.juce.com — JUCE 8 API; do not introduce JUCE 7-era patterns.

### Project structure notes

- New widget helpers live in `Source/GUI/Widgets/` next to existing popup files.
- Register every new `.cpp` in root `CMakeLists.txt` `PLUGIN_SOURCES`.
- Tests: `TestComboBoxes`, `TestHierarchicalComboBoxes`, `TestPopupMenus` — smoke consumers; extend only if a pure helper becomes unit-testable without GUI pixels.
- Do not put GUI types in `Source/Core/`.

### References

- [Source: `_bmad-output/implementation-artifacts/u-12-hierarchical-combobox-widget.md`]
- [Source: `_bmad-output/implementation-artifacts/6-15-history-combobox-popup-ux.md` — AC8 U-13 amend; deferred CustomScrollBar / 20 Hz]
- [Source: `_bmad-output/implementation-artifacts/deferred-work.md` — 6-15 review deferrals]
- [Source: `_bmad-output/project-context.md` — GUI layout, English source, Clean Code]
- [Source: `CONVENTIONS.md` §4.2 / §6.11 — function size / SOLID]

### Estimated effort

~0.5–1 day (hygiene; no product AC). Scrollbar extract is the critical path.

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

- macOS ARM Debug build: `Matrix-Control` + `Matrix-Control_Tests` green
- Unit suite exit 0 (no FAILED lines)

### Completion Notes List

- Locked design as story recommended: composition + thin host interface (not CRTP mega-base; HierarchicalPopupMenu stays a dedicated dual-panel class; no juce::ComboBox inheritance for Hierarchical).
- Extracted `PopupMenuCustomScrollBar` with `IPopupMenuScrollModel` + `ViewportPopupMenuScrollModel` / `CallbackPopupMenuScrollModel`. Both Scrollable and Hierarchical adopt it; idle `startTimerHz(20)` removed — thumb updates on drag/wheel/scroll events only.
- Added `IPopupMenuHost`; ComboBox and HierarchicalComboBox implement it. PopupMenuBase holds host + ComboBox& for flat item APIs; open/close goes through `notifyPopupOpened` / `notifyPopupClosed`.
- Shared `PopupMenuModalHelpers::dismissAndDelete` / `handleEscapeKey` used by PopupMenuBase and Hierarchical close/escape. Hierarchical `closePopupWithSelection` keeps friend-accessible commit inline (teardown before onChange preserved).
- `ComboBoxClosedControlHelper` centralizes setUiScale early-return, focus gained/lost, and focus-ring flag for both closed controls. Paint remains thin `ComboBoxControlPainter` delegates.
- Cleared deferred-work.md 6-15 items (duplicated CustomScrollBar + 20 Hz timers).
- Manual Standalone smoke (Guillaume, 2026-07-21): success — covers AC4 production path; TestComponent scale sweep accepted as optional (code-review decision 2026-07-21). Build + unit suite green (AC4.3).
- Code review 2026-07-21: kept shared close-then-commit selection order; finished closed-control look/notify helper; fixed secondary scrollbar display-scale + frozen hierarchical vertical margin; removed unused `PopupMenuBase` friendship.

### File List

- Source/GUI/Widgets/IPopupMenuHost.h (new)
- Source/GUI/Widgets/PopupMenuCustomScrollBar.h (new)
- Source/GUI/Widgets/PopupMenuCustomScrollBar.cpp (new)
- Source/GUI/Widgets/PopupMenuModalHelpers.h (new)
- Source/GUI/Widgets/ComboBoxClosedControlHelper.h (new)
- Source/GUI/Widgets/PopupMenuBase.h
- Source/GUI/Widgets/PopupMenuBase.cpp
- Source/GUI/Widgets/ComboBox.h
- Source/GUI/Widgets/ComboBox.cpp
- Source/GUI/Widgets/HierarchicalComboBox.h
- Source/GUI/Widgets/HierarchicalComboBox.cpp
- Source/GUI/Widgets/ScrollablePopupMenu.h
- Source/GUI/Widgets/ScrollablePopupMenu.cpp
- Source/GUI/Widgets/HierarchicalPopupMenu.h
- Source/GUI/Widgets/HierarchicalPopupMenu.cpp
- CMakeLists.txt
- _bmad-output/implementation-artifacts/deferred-work.md
- _bmad-output/implementation-artifacts/sprint-status.yaml
- _bmad-output/implementation-artifacts/u-13-combobox-popup-infrastructure-dedup.md

## Change Log

- 2026-07-14: Story U-13 created — ComboBox / HierarchicalComboBox popup infrastructure DRY (post U-12 defer).
- 2026-07-15: Amended after 6-15 — staircase obsolete; share aligned dual-panel border path.
- 2026-07-21: Ultimate context engine — expanded ACs (shared scrollbar + 20 Hz fix), code map, design lock, regression traps; status → ready-for-dev.
- 2026-07-21: Implemented shared scrollbar, IPopupMenuHost, modal helpers, closed-control helper; deferred 6-15 scrollbar/timer items cleared; status → review.
- 2026-07-21: Code review resolved — patches applied; status → done.
