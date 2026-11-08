# Build slices (behavior-preserving)

Each slice = one `/bmad-build` in a **fresh** chat. Do not stack all slices in one conversation.
Suggested review order = list order (dirtiest / riskiest first → cosmetic last).

## Shared DoD (every slice)

1. Compile: `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` (configure with project preset if needed).
2. `python3 Scripts/quality/lint_touched.py` green on the diff (Skin Editor paths in gate).
3. Manual smoke (minimum): launch Skin Editor → Product/Editor combos → Palette select → Mappings bind/unmap critical path → Unbound visible if applicable → quit dirty-gate if touched. Extra when listed below.
4. No intentional UI/copy/chrome redesign; pixel/behavior parity with post-polish baseline.
5. CMake: register any new `.cpp` under `Matrix-Control_SkinEditor` `target_sources`.
6. Boy Scout **only** inside the slice’s owned files; no drive-by on other hotspots.

## Slice 1 — Chrome split (H1) — MUST

**Owns:** `SkinEditorChrome.h` (+ new sibling headers/sources as needed); include fixes in Skin Editor consumers; CMake if new `.cpp`.

**Intent:** Replace the monolith header with content-typed units so tokens/fonts/measure are separable from panel/table paint, without changing paint/layout math.

**File shape (validated by Guillaume 2026-10-24):**

- `SkinEditorChromeTokens.h` — colours + layout constants
- `SkinEditorChromeFonts.h` — typefaces + font helpers + text measure / title width
- `SkinEditorChromePanel.h` (+ optional `.cpp`) — `layoutPanel`, `paintPanelFrame`, wells
- `SkinEditorChromeTable.h` (+ optional `.cpp`) — table geom, silhouette path, section header, focus stroke, row fill, checkerboard/swatch/badge
- `SkinEditorChrome.h` — thin umbrella re-export (keep call sites stable; umbrella may be removed in a later slice only if desired)

**AC**

- No single Chrome unit remains a kitchen-sink of tokens + path paint + badges.
- Useful lines per new `.cpp` ≤ ~400; headers lean (prefer declarations in `.h`, bodies in `.cpp` for large path builders when that improves compile/readability).
- All existing `SkinEditorChrome::*` call semantics preserved (colours, constants, paint outputs).
- Smoke: panel silhouettes, mappings object tabs, focus outline, swatches, count badges, title fonts unchanged to the eye.

**Risks:** Visual regression on 45° tab / focus stroke; include cycles. Mitigate with umbrella + careful move-only diffs.

## Slice 2 — Main shell / MainWindow (H2) — MUST

**Owns:** `Main.cpp` and new files under `Tools/SkinEditor/` for window shell (e.g. `SkinEditorMainWindow.*` or equivalent).

**Intent:** Lift nested `MainWindow` (and related free helpers) out of the application TU so bootstrap vs window placement/persist/zoom are readable.

**AC**

- `Main.cpp` is thin: application lifecycle + create window.
- Window restore, constrainer width lock, title-bar on-screen ensure, zoom/maximise, double-click hook, persist debounce stay behavior-identical.
- Smoke: open → resize height → move → quit/relaunch restores state; maximise / title-bar double-click zoom; dirty quit still gated via content component.

**Risks:** Window geometry edge cases on multi-display. Extra smoke on restore/zoom mandatory.

## Slice 3 — MainComponent clarity (H3) — SHOULD

**Owns:** `SkinEditorMainComponent.{h,cpp}`, `*Actions.cpp`, `*Session.cpp` only as needed for clarity (no feature work).

**Intent:** After Chrome split, tighten includes, keep session vs actions vs layout boundaries clear, rename for intent, extract only helpers that reduce real glue density.

**AC**

- Public API remains the small surface needed by `MainWindow` (quit gate, dirty, paint/resized/keys).
- No merge of Actions/Session back into one file; no new UX.
- Smoke: New/Duplicate/Rename/Delete, Compare blink, Push, Export/Import, Undo/Redo, pastille add/delete, colour change → mappings refresh.

## Slice 4 — MappingsPanel structure (H4) — SHOULD

**Owns:** `SkinEditorMappingsPanel.{h,cpp}`, `SkinEditorMappingsPanelRows.cpp` (+ optional new helper TU).

**Intent:** Keep `.cpp` comfortably under ~400 useful lines with clearer paint/rebuild/bind boundaries; thin nested types in the header if they obscure reading.

**AC**

- Bind / unbind / sticky legend / object combo / focus silhouette behavior unchanged.
- Extract named helpers only where length/complexity/nesting or duplicated paint blocks warrant it.
- Smoke: scroll tables, sticky legend, object jump combo, bind with/without pastille, focus outline on object tables.

## Slice 5 — Remaining panels + LookAndFeel (H5–H6) — NICE

**Owns:** `SkinEditorPalettePanel.*`, `SkinEditorUnboundPanel.*`, `SkinEditorColourPicker.*`, `SkinEditorLookAndFeel.*`.

**Intent:** Light pass — intentful names, stable DRY of panel chrome setup if 2nd–3rd copy is real, no abstraction for its own sake.

**AC**

- Each touched `.cpp` stays under ~400 useful; methods under GUI thresholds.
- Smoke: palette groups/selection, unbound list, colour picker HSV/hex/RGB sync, control hover/down look.

## Slice 6 — Close-out (CAP-7)

**Owns:** inventory residual list only (doc update in this spec folder via follow-up `/bmad-spec` or short note in implementation artifacts); code only if a must-fix leftover was missed.

**Intent:** Confirm must-fix hotspots closed; record **wontfix / defer** with reason for anything left; confirm `lint_touched.py --all` still shows 0 Skin Editor findings.

**AC**

- Written residual debt table (empty must-fix, or deferred with reason).
- No open H1/H2 without explicit defer approved by Guillaume.
