# Skin Editor structural inventory (2026-10-24)

Evidence: file read + useful-line counts + `python3 Scripts/quality/lint_touched.py --all`.
**Linter:** 0 findings under `Tools/SkinEditor/`. Plugin-tree findings exist elsewhere and are **out of scope**.

> **Reading order:** Sections from **Tree map** through **Gate nuance** are the **pre-cleanup baseline** (start of campaign). Hotspot disposition and the live tree after stories 1–5 are in **Close-out residual debt** at the bottom — that section is the CAP-7 SSOT for “open or closed.”

## Tree map (17 translation/header units) — baseline before slices

| File | Total lines | Useful ≈ | Role |
|---|---:|---:|---|
| `SkinEditorChrome.h` | 532 | 411 | Tokens, fonts, measure, panel/table paint (all inline) |
| `SkinEditorMappingsPanel.cpp` | 386 | 323 | Mappings layout, sticky legend, paint, bind |
| `Main.cpp` | 343 | 279 | JUCEApplication + nested `MainWindow` (placement/persist/zoom) |
| `SkinEditorMainComponentActions.cpp` | 333 | 303 | Skin CRUD, push/export/import, pastille, compare chrome |
| `SkinEditorMainComponent.cpp` | 318 | 286 | ctor/setup, paint, resized, title/skins/body layout |
| `SkinEditorMainComponentSession.cpp` | 311 | 273 | Dirty gate, refresh, combos, autosave/load, keybinds |
| `SkinEditorPalettePanel.cpp` | 280 | 238 | Pastille list + groups |
| `SkinEditorColourPicker.cpp` | 259 | 227 | HSV/hex/RGB editors |
| `SkinEditorLookAndFeel.cpp` | 162 | 144 | Control drawing for editor chrome |
| `SkinEditorMainComponent.h` | 138 | 116 | Orchestrator API + all child panels |
| `SkinEditorUnboundPanel.cpp` | 130 | 108 | Unbound keys list |
| `SkinEditorMappingsPanel.h` | 121 | 98 | Nested Row/ObjectTable/Content/PaintRowSpec |
| `SkinEditorMappingsPanelRows.cpp` | 117 | 103 | Row rebuild helpers |
| Remaining `.h` | ≤71 | ≤57 | Thin panel / LnF / picker headers |

**Totals:** ~3654 lines across Skin Editor sources.

## Dependency sketch

```
Main.cpp
  └─ SkinEditorMainComponent
        ├─ SkinEditorChrome (tokens + paint helpers)
        ├─ SkinEditorLookAndFeel
        ├─ SkinEditorColourPicker
        ├─ SkinEditorPalettePanel
        ├─ SkinEditorMappingsPanel
        ├─ SkinEditorUnboundPanel
        └─ TSS::DevSkin::{Layer, BridgeClient, EditorSessionState, …}
```

Panels call into `SkinEditorChrome::*` for colours, fonts, panel frame, table silhouette, badges, swatches.
`MainComponent` owns session dirty state and routes palette/mappings callbacks.

## Hotspots — baseline (why each slice existed)

Disposition after delivery: see **Close-out residual debt**. Do not read the tables below as still-open work.

### MUST-FIX

| ID | Area | Why (beyond green lint) | Suggested slice |
|---|---|---|---|
| H1 | `SkinEditorChrome.h` | ~411 useful lines in a **header**; file-size gate is `.cpp`-only so this escapes analyser size. Mixes colour tokens, layout constants, typefaces/fonts, text measure, panel silhouette paint, table geometry/path, focus stroke, checkerboard/swatch/badge. Hard to navigate; every panel includes the whole paint stack. | Slice 1 |
| H2 | `Main.cpp` nested `MainWindow` | App bootstrap + large nested window class (restore, constrainer, title-bar ensure, zoom, double-click, persist timer) in one TU. Methods individually under GUI nloc thresholds, but the file is a glue dump that hides ownership. Window geometry is behavior-critical. | Slice 2 |

### SHOULD-FIX

| ID | Area | Why | Suggested slice |
|---|---|---|---|
| H3 | `SkinEditorMainComponent*.{h,cpp}` | Already split (Actions / Session / layout). Header still pulls full Chrome + all panels; private surface is wide (setup/layout/action/session). Opportunity: narrower includes after Chrome split, clarify naming, extract tiny nested `BlinkTimer` if it aids reading — **no behavior change**. | Slice 3 |
| H4 | `SkinEditorMappingsPanel*.{h,cpp}` | Largest panel `.cpp` (~323 useful; near ~400). Header nests Row model, Content painter, PaintRowSpec. Paint + bind + rebuild already partially split to Rows — further clarity (named paint helpers, thinner header) without UX change. | Slice 4 |

### NICE / PASS-LAST

| ID | Area | Why | Suggested slice |
|---|---|---|---|
| H5 | `PalettePanel` / `UnboundPanel` / `ColourPicker` | Under size ceilings; check stable panel-setup duplication (title tab, chrome style) and weak names only if 2nd–3rd stable copy. | Slice 5 |
| H6 | `SkinEditorLookAndFeel.*` | Small; rename/clarity only if touched by earlier slices or obvious. | Slice 5 |

## Gate nuance (do not invent violations) — baseline notes

- Touched-mode + `--all`: **no** Skin Editor function-length / complexity / nesting / params findings at inventory time (reconfirmed at close-out).
- `.cpp` useful-line ceiling (~400) was **not** breached under `Tools/SkinEditor/` at inventory time (reconfirmed at close-out).
- Headers are analysed for function metrics but **not** for file-size — that is why baseline H1 (monolith header) escaped the analyser until Slice 1 split it.
- Nested classes inside baseline `Main.cpp` could under-report as “one big problem” in lizard; inventory treated the nested window as a readability hotspot regardless (closed by Slice 2).

## Explicit non-inventory

- Closed polish UI review findings — do not reopen unless a cleanup slice regresses them.
- Plugin `Source/` / `Tests/` lint debt — out of scope even when `--all` lists them.
- Dev skin runtime (`Source/GUI/Skins/Dev/`) — out of scope unless a **proven** compile/link coupling blocks a Tools-only fix (document as motivated exception).

## Close-out residual debt (2026-10-24) — CAP-7 / Slice 6

Evidence after stories 1–5 on `feature/skin-editor` (HEAD at close-out start: `e0da55f2`). Re-ran `python3 Scripts/quality/lint_touched.py --all`: **0** findings under `Tools/SkinEditor/` (plugin/Tests findings elsewhere remain out of scope). Manual smoke at close-out: **waived** (doc-only; no product C++ change) — stories 1–5 DoD / Guillaume UAT remain the behavior baseline.

### Hotspot disposition (H1–H6)

| ID | Severity | Status | Reason |
|---|---|---|---|
| H1 | MUST | closed | Chrome split into Tokens / Fonts / Panel / Table; `SkinEditorChrome.h` is a 6-line umbrella |
| H2 | MUST | closed | `SkinEditorMainWindow` extracted; `Main.cpp` is thin JUCEApplication bootstrap (~59 lines / ~49 useful) |
| H3 | SHOULD | closed | Slice 3 clarified MainComponent layout / actions / session boundaries |
| H4 | SHOULD | closed | Slice 4 clarified Mappings paint / rebuild / bind; `SkinEditorMappingsPanel.cpp` ~337 useful (414 total) under ~400 useful ceiling |
| H5 | NICE | closed | Slice 5 light rename/clarity on Palette / Unbound / ColourPicker |
| H6 | NICE | closed | Slice 5 thinned LookAndFeel includes off the Chrome umbrella |

Must-fix set is empty. No H1/H2 left open without defer.

### Non-hotspot leftovers from this campaign

| Item | Status | Reason |
|---|---|---|
| Count-badge width/clamp DRY (`drawCountBadge` / `countBadgeWidth`) | defer | Pre-existed in monolith; Story 1 was move-only. Optional future Boy Scout if those helpers are retouched — not a must-fix hotspot. |
| LookAndFeel → Chrome umbrella include (Story 3 defer) | closed | Closed by Story 5: `SkinEditorLookAndFeel.h` uses `juce_gui_basics` only; Fonts include moved to `.cpp`. |

Polish-era / packaging / automated-GUI verification gaps listed under other Skin Editor specs in `deferred-work.md` stay **out of this cleanup campaign** (not silent H* debt).

Campaign may be treated as **finished**; further Skin Editor structural cleanup only via a new campaign or an unlocked defer (badge DRY above).

### Post-campaign tree snapshot

Total lines unless noted. Useful ≈ is a blank/`//`-stripped proxy (same spirit as the baseline inventory).

| File | Total | Useful ≈ | Role |
|---|---:|---:|---|
| `SkinEditorMappingsPanel.cpp` | 414 | 337 | Mappings layout / sticky legend / paint / bind |
| `SkinEditorMainComponentActions.cpp` | 334 | — | Skin CRUD, push/export/import, pastille, compare |
| `SkinEditorMainComponent.cpp` | 318 | — | ctor/setup, paint, resized, title/skins/body layout |
| `SkinEditorMainComponentSession.cpp` | 309 | — | Dirty gate, refresh, combos, autosave/load, keybinds |
| `SkinEditorPalettePanel.cpp` | 279 | — | Pastille list + groups |
| `SkinEditorMainWindow.cpp` | 277 | — | Window placement / persist / zoom |
| `SkinEditorColourPicker.cpp` | 257 | — | HSV/hex/RGB editors |
| `SkinEditorChromeTable.cpp` | 214 | — | Table geom, silhouette, focus, swatch/badge |
| `SkinEditorLookAndFeel.cpp` | 164 | — | Control drawing |
| `SkinEditorMainComponent.h` | 135 | — | Orchestrator API |
| `SkinEditorMappingsPanel.h` | 134 | — | Nested row / content / paint types |
| `SkinEditorUnboundPanel.cpp` | 131 | — | Unbound keys list |
| `SkinEditorChromeFonts.h` | 119 | — | Typefaces + measure |
| `SkinEditorMappingsPanelRows.cpp` | 117 | — | Row rebuild helpers |
| `SkinEditorChromeTokens.h` | 92 | — | Colours + layout constants |
| `SkinEditorChromeTable.h` | 83 | — | Table paint declarations |
| `SkinEditorPalettePanel.h` | 73 | — | Palette API |
| `SkinEditorChromePanel.cpp` | 69 | — | Panel frame / wells |
| `Main.cpp` | 59 | 49 | App lifecycle + create window |
| `SkinEditorColourPicker.h` | 57 | — | Picker API |
| `SkinEditorLookAndFeel.h` | 49 | — | LnF API |
| `SkinEditorUnboundPanel.h` | 48 | — | Unbound API |
| `SkinEditorMainWindow.h` | 42 | — | Window API |
| `SkinEditorChromePanel.h` | 31 | — | Panel paint declarations |
| `SkinEditorChrome.h` | 6 | 5 | Thin umbrella re-export |

Companion note: `_bmad-output/implementation-artifacts/skin-editor-clean-code-cleanup-close-out.md`.
