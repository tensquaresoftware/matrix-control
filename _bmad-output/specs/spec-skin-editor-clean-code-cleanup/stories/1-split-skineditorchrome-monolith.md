---
title: 'Split SkinEditorChrome monolith'
type: 'refactor'
created: '2026-10-24'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '2c459c586f3b0b203eadeec7604d72c7c16a2021'
context:
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/slices.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/inventory.md'
  - '{project-root}/CONVENTIONS.md'
  - '{project-root}/.cursor/rules/clean-code.mdc'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Editor chrome lives in one large header that mixes colour/layout tokens, fonts/measure, panel silhouette paint, and table path/badge helpers, so maintainers cannot navigate or compile against a thin surface even though lint is green.

**Approach:** Move-first split into content-typed units under the same `SkinEditorChrome` namespace, keep a thin umbrella header so existing includes and call sites stay stable, and change neither paint math nor UI polish.

## Boundaries & Constraints

**Always:**
- Scope `Tools/SkinEditor/` only; touch `CMakeLists.txt` only to register new Skin Editor `.cpp` sources.
- Preserve exact paint/layout semantics (tokens, fonts, panel/table silhouettes, 45° tabs, focus stroke, swatches, badges).
- Keep umbrella `SkinEditorChrome.h` that re-exports the units so `#include "Tools/SkinEditor/SkinEditorChrome.h"` and `SkinEditorChrome::*` call sites remain valid.
- English source, Allman braces, project C++ standard; Boy Scout limited to Chrome units + include/CMake glue.

**Never:**
- Stories 2–6, plugin `Source/` / `Tests/` cleanup, chrome redesign, copy/feature work, or reopening closed polish review.
- Touch `Source/GUI/Skins/Dev/` unless a proven include/link block (document if forced).
- Remove the umbrella in this story; commit / push / PR without Guillaume’s explicit ask.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Panel paint | Existing panel bounds + title tab | Same folder-tab silhouette and body fill as pre-split | N/A |
| Mappings object focus | Focused object table section | Same Photocraft-style 1 px outside focus stroke | N/A |
| Swatch / badge | Colour with alpha; count ≥ 0 | Same checkerboard + colour; same badge size/text | Negative count clamped to 0 (unchanged) |
| Consumer include | `#include SkinEditorChrome.h` | All prior symbols resolve in `SkinEditorChrome` | Compile fails if umbrella/unit miss a symbol |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorChrome.h` (~532 lines) — **split source**; after change: thin umbrella `#include` of the four units only (no reimplementation).
- **New** `Tools/SkinEditor/SkinEditorChromeTokens.h` — colour helpers + layout `constexpr` (≈ lines 10–94 of current header).
- **New** `Tools/SkinEditor/SkinEditorChromeFonts.h` — typefaces, fonts, `preventTextSquish` / `stylePanelTitle`, measure + `titleTabWidth` (depends on Tokens for size constants; needs `PluginFontsData.h` + Label/Button for measure helpers).
- **New** `Tools/SkinEditor/SkinEditorChromePanel.h` (+ `.cpp`) — `PanelChrome`, `layoutPanel`, `paintPanelFrame`, `paintRoundedWell`. Heavy path body in `.cpp`.
- **New** `Tools/SkinEditor/SkinEditorChromeTable.h` (+ `.cpp`) — `TableRowFill` / `fillTableRow`, `TableSectionGeom` / `makeTableSectionGeom`, cubic corners, `makeObjectTableSilhouettePath`, `paintTableSectionHeader`, `strokeObjectTableFocusOutside`, checkerboard/swatch/badge helpers. Heavy path bodies in `.cpp`. Depends on Tokens + Fonts (measure/font for section tabs).
- `CMakeLists.txt` — `Matrix-Control_SkinEditor` `target_sources`: add Panel/Table `.cpp` if created.
- **Consumers (reuse umbrella — prefer no call-site edits):** `Main.cpp`, `SkinEditorMainComponent.{h,cpp,Actions,Session}`, `SkinEditorLookAndFeel.h` (nested `LookAndFeel` in namespace), `SkinEditorColourPicker.cpp`, `SkinEditorPalettePanel.cpp`, `SkinEditorMappingsPanel.cpp`, `SkinEditorMappingsPanelRows.cpp`, `SkinEditorUnboundPanel.cpp`.
- **Do not change:** paint formulas, colour literals, layout constants values, LookAndFeel drawing logic, Dev skin runtime, stories 2–6 files.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorChromeTokens.h` -- Create; move colour + layout constants verbatim into `namespace SkinEditorChrome`.
- [x] `Tools/SkinEditor/SkinEditorChromeFonts.h` -- Create; move typefaces/fonts/measure/title helpers; include Tokens.
- [x] `Tools/SkinEditor/SkinEditorChromePanel.{h,cpp}` -- Create; declare in header, move `layoutPanel` / `paintPanelFrame` / `paintRoundedWell` bodies to `.cpp` (move-only).
- [x] `Tools/SkinEditor/SkinEditorChromeTable.{h,cpp}` -- Create; declare table/geom/path/badge API in header; move heavy bodies to `.cpp` (move-only).
- [x] `Tools/SkinEditor/SkinEditorChrome.h` -- Replace monolith with umbrella includes of Tokens, Fonts, Panel, Table (order that avoids cycles).
- [x] `CMakeLists.txt` -- Register new Panel/Table `.cpp` under `Matrix-Control_SkinEditor` `target_sources`.
- [x] Consumers -- Fix includes only if a unit needs a direct include; default keep `SkinEditorChrome.h`.
- [x] Verify -- Build Skin Editor target; run `lint_touched.py` on diff; manual smoke listed below.

**Acceptance Criteria:**
- Given the post-polish Skin Editor baseline, when Chrome is split into Tokens/Fonts/Panel/Table + umbrella, then no single Chrome unit remains a kitchen-sink of tokens + path paint + badges.
- Given existing call sites that include `SkinEditorChrome.h`, when the project compiles, then all previous `SkinEditorChrome::*` symbols resolve with identical semantics.
- Given new Panel/Table translation units, when useful lines are counted, then each new `.cpp` stays ≤ ~400 useful lines.
- Given a Debug arm64 Skin Editor build, when the app is launched, then panel silhouettes, Mappings object tabs, focus outline, swatches, count badges, and title fonts are unchanged to the eye.
- Given touched Skin Editor / CMake files, when `lint_touched.py` runs on the diff, then it reports green for those paths.

## Implementation Notes

- 2026-10-24: Move-only split landed (Tokens / Fonts / Panel.cpp / Table.cpp + thin umbrella). Call sites unchanged. API symbol count 83→83. Build `Matrix-Control_SkinEditor` macos-debug-arm64 OK; `lint_touched.py --base baseline` OK. Manual on-screen smoke confirmed OK by Guillaume (2026-10-24).

## Spec Change Log

## Review Triage Log

- false — Blind: hide `makeTableSectionGeom` / cubic corners from Table header — monolith already exported them; move-only + umbrella require identical `SkinEditorChrome::*` surface; unused-by-callers is not a new defect.
- false — Blind: badges/swatches “misplaced” under Table without banner — approved file shape places those helpers in Table; naming was intentional.
- false — Blind: missing per-file purpose comments / umbrella facade note — documentation wish, not a demonstrated bad outcome.
- false — Blind: Tokens still flat — approved Tokens unit; regrouping was out of intent.
- false — Blind: Panel.h includes `juce_gui_basics` — same include breadth as prior monolith consumers; no behavioral harm shown.
- false — Blind: Table.h redundant Tokens include via Fonts — harmless with include guards; no compile/link failure.
- false — Blind: duplicated hunks in review diff artifact — tooling staging issue; on-disk tree not duplicated.
- false — Edge: `paintCheckerboard` cell <= 0 hang — real loop math if cell<=0, but only caller is `paintColourSwatch` with default cell=4; unreachable in current Skin Editor.
- false — Edge: `layoutPanel` jlimit when titleRow width < 1 — same pre-split code; window width is locked to `kWindowMaxWidth`, panels never get a zero-width title row in reachable layout.
- low / patch — Blind: French words in moved comments (`créneau`, `damier`) — violates English-in-source; fixed to `title tab` / `checkerboard` in Tokens and Table headers.
- defer — Blind: duplicated badge width/clamp math in `drawCountBadge` / `countBadgeWidth` — pre-existing in monolith; not introduced by the split; optional later DRY.

## Design Notes

**Include order (umbrella):** Tokens → Fonts → Panel → Table. Fonts includes Tokens; Panel includes Tokens; Table includes Tokens + Fonts.

**Bodies in `.cpp`:** Panel frame path and Table silhouette / header / focus stroke / badge measure paths — improves header skim and compile isolation without API change. Tokens and Fonts stay header-only (small inlines / static typeface caches).

**LookAndFeel:** Remains a separate type in `SkinEditorChrome` via `SkinEditorLookAndFeel.h` including the umbrella; do not fold LnF into this split.

**Golden parity:** Do not “simplify” 45° tab math, `kTabBevelStartEarly`, or the 0.5 outset focus stroke while moving code.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 && cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` -- expected: success
- `python3 Scripts/quality/lint_touched.py` -- expected: green on touched Skin Editor / related paths

**Manual checks (if no CLI):**
- Launch Skin Editor; confirm panel silhouettes (COLOR / PALETTE / MAPPINGS / UNBOUND), Mappings object folder-tabs, focused-object outline, colour swatches (alpha damier), Unbound count badge, panel/app title fonts match pre-split look.
