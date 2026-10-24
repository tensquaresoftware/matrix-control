# Skin Editor structural inventory (2026-10-24)

Evidence: file read + useful-line counts + `python3 Scripts/quality/lint_touched.py --all`.
**Linter:** 0 findings under `Tools/SkinEditor/`. Plugin-tree findings exist elsewhere and are **out of scope**.

## Tree map (17 translation/header units)

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

## Hotspots

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

## Gate nuance (do not invent violations)

- Touched-mode + `--all`: **no** Skin Editor function-length / complexity / nesting / params findings today.
- `.cpp` useful-line ceiling (~400) is **not** currently breached under `Tools/SkinEditor/`.
- Headers are analysed for function metrics but **not** for file-size — H1 is structural debt the green linter does not surface.
- Nested classes inside `Main.cpp` may under-report as “one big problem” in lizard; inventory treats the nested window as a readability/maintenance hotspot regardless.

## Explicit non-inventory

- Closed polish UI review findings — do not reopen unless a cleanup slice regresses them.
- Plugin `Source/` / `Tests/` lint debt — out of scope even when `--all` lists them.
- Dev skin runtime (`Source/GUI/Skins/Dev/`) — out of scope unless a **proven** compile/link coupling blocks a Tools-only fix (document as motivated exception).
