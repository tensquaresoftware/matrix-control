---
title: 'Skin Editor chrome — SESSION, SKINS panel, drop Unbound'
type: 'feature'
created: '2026-10-25'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'b2b720cc5575219c65cb541afe9800bc3d9e995d'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-ui-panels.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-hold-mapping-chrome.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Editor chrome still spends a full column on Unbound (redundant with yellow Unbound rows in Mappings) and packs skin CRUD plus session actions into one horizontal SKINS strip, so the window stays locked wide and mapping help competes with the logo title.

**Approach:** Drop the Unbound panel; put session actions in a right-side SESSION well on the logo bar; put mapping help in an OPERATION well under the logo (idle = empty dark pocket); add a left SKINS panel above COLOR; keep Mappings at the former Unbound half-share width (~385) and fit the window to the fixed columns (no stretch into the freed gap).

## Boundaries & Constraints

**Always:**
- Unbound panel gone from UI; bind unbound keys only via Mappings (yellow Unbound). Keep Export gated on `unboundCount == 0`.
- SESSION gestures: Undo, Redo | Compare, Push | Export, Import — identifiable well, right of the logo title — logo stays on dark app fill (outside the well).
- OPERATION well under the logo: same height language as SESSION (36 px well); title tab `OPERATION`; dark content pocket hosts the MAPPING witness when active; badge→label gap = `kChromeBadgeGap` (same as CONNECTION).
- Idle mapping mode: OPERATION pocket empty (no permanent greyed MAPPING badge); no second witness on the logo bar.
- SKINS panel (left, above COLOR, `kChromeStackGap` 12 between): Product combo + Duplicate on one row; separator; Editor combo + Duplicate on one row; New / Rename / Delete on one row under Editor (Delete width = Duplicate; New absorbs remainder). Label column width = max(Product:, Editor:). Column width = `kColorPanelWidth`.
- Stack gaps 12 px: SESSION↔OPERATION, OPERATION↔SKINS, SKINS↔COLOR; app left/right edge 12; Palette side gaps 12.
- COLOR / Palette / Mappings flush to the status bar (`bottomGutter = 0`); SKINS keeps its bottom gutter so the SKINS↔COLOR gap stays 12.
- Status bar height = SESSION / OPERATION well height (36). Panel title tabs use `textBrand`. Mappings table header fill/text slightly lightened. Held Product colour witness prefix: `Palette > Product colour:`.
- SESSION well: title tab `SESSION`; vertical separators between Undo/Redo, Compare/Push, Export/Import.
- Window width: `windowContentWidth()` = max(body columns, title+SESSION); min width = max width (fitted, not user-resizable horizontally).
- Product labels stay English ASCII (SKINS, SESSION, OPERATION, MAPPING, Product:, Editor:, …).

**Never:**
- Change colour model / JSON schema / Push bridge semantics.
- Reintroduce a dedicated Unbound surface or invent an unbound count badge.
- Refactor Palette / ColourPicker beyond layout bounds needed for fixed COLOR height + flush + min-window.
- Leave MAPPING witness painted in the logo bar after OPERATION exists (no double witness).
- Commit / push / PR unless Guillaume asks after the build.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Idle chrome | MappingMode Idle | Logo + SESSION well; OPERATION well with empty pocket; SKINS+COLOR \| Palette \| Mappings; status bar | N/A |
| Hold mapping | HeldColour or ArmedRow | OPERATION pocket shows MAPPING + help (+ swatch); logo has no second witness | N/A |
| Held Product colour | Hold from Palette product colour | Prefix `Palette > Product colour:` (+ elide in pocket) | N/A |
| Bind unbound | Click yellow Unbound row in Mappings + pastille | Mapping writes; Export still disabled until unboundCount==0 | Existing bind error dialogs |
| Export gate | unboundCount > 0 | Export disabled; no Unbound panel required | Status/tooltip copy may still mention Unbound = 0 |
| Compare Product | Compare shows Product | Mappings read-only; no Unbound panel refresh path | N/A |
| Window width | Launch / normalize | Client width = `windowContentWidth()`; horizontal resize locked (min=max) | Constrainer |
| SKINS actions | Editor CRUD at `kColorPanelWidth` | New / Rename / Delete stay one row; Delete width = Duplicate | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMainComponent.cpp` / `.h` — chrome `resized`/`paint`, title/status layout, body columns; OPERATION well + witness; no Unbound column.
- `Tools/SkinEditor/SkinEditorMainComponentSkins.cpp` — SESSION + OPERATION setup/layout.
- `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` — `updateMappingWitness` / bind via Mappings; witness copy prefixes.
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` — Export `unboundCount == 0` gate; no Unbound panel refresh.
- `Tools/SkinEditor/SkinEditorSkinsPanel.*` — left SKINS CRUD panel.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` / `SkinEditorChromeFonts.h` — stack gaps, fitted width helpers, well heights.
- `Tools/SkinEditor/SkinEditorChromePanel.*` — `paintPanelFrame` / `paintRoundedWell` / `layoutPanel` (`bottomGutter` 0 when flush).
- `Tools/SkinEditor/SkinEditorMainWindow.cpp` — constrainer min=max width = `windowContentWidth()`; min height from SKINS+COLOR + chrome stack.
- `CMakeLists.txt` — SkinEditorSkinsPanel in; UnboundPanel out.

**Reuse:** `kChromeBadgeGap`; SESSION separators; Compare read-only on Mappings; Export gate.

**Do not change:** DevSkin layer / bridge / JSON export payload shape; Mappings yellow Unbound cell rendering; Palette colour model.

## Tasks & Acceptance

**Execution:**
- [x] SESSION well on logo right; OPERATION well under logo; SKINS panel above COLOR; Unbound removed.
- [x] Fitted window width (`windowContentWidth`); Mappings fixed ~385; gaps 12; status/OPERATION/SESSION 36; flush COLOR/Palette/Mappings.
- [x] Smoke polish: titles `textBrand`; Mappings header lighten; `Palette > Product colour:` prefix.
- [x] `Tests/Unit/SkinEditorChromeLayoutTests.cpp` + `lint_touched.py` on touched paths.

**Acceptance Criteria:**
- Given the editor open, when Idle, then no Unbound column, SESSION well on logo right, OPERATION pocket empty, SKINS above COLOR with Product/Editor CRUD on the smoke grid.
- Given Hold mapping active, when OPERATION paints, then MAPPING badge + help (+ swatch) appear only in the OPERATION pocket (not in the logo bar), gap matching CONNECTION.
- Given unbound keys remain, when Export is considered, then Export stays disabled until unboundCount == 0, with bind available from Mappings.
- Given launch / normalize, when width is applied, then client width equals `windowContentWidth()` (horizontal min=max).
- Given touched C++ under `Tools/SkinEditor/`, when `lint_touched.py` runs, then it passes.

## Implementation Notes

Agent decisions (planning — superseded by smoke where noted):
- Originally planned a nude mapping band and resizable min width; smoke renegotiated to OPERATION well + fitted width (see Spec Change Log).

Implementation (2026-10-25):
- Extracted `SkinEditorSkinsPanel`; Unbound binds via Mappings; chrome layout tests + DevSkin / MappingModePolicy coverage.
- Smoke: Mappings keeps former half-share (`kMappingsPanelWidth` = 385); window fits columns; OPERATION well; gaps 12; status 36; SKINS one-line New/Rename/Delete; flush bottom on COLOR/Palette/Mappings.
- Code-review patches (2026-10-25): frozen Intent/AC/Design aligned to smoke; retired `preferredHeightForMinWindow` + `kLogoChromeNudgeY`; chrome tests drive `SkinEditorSkinsPanel` + preferred-height stack.

## Spec Change Log

- 2026-10-25 (code review): Renegotiated frozen Intent / Always / I-O / AC / Design to match smoke (OPERATION well, gaps 12, fitted width, fixed Mappings, one-line SKINS CRUD, status 36, titles/`Palette >` polish). Decision: document smoke as truth (option 1).

### Review Findings

- [x] [Review][Patch] Document smoke as chrome truth — renegotiate frozen Intent/Always/I-O/AC/Design to match OPERATION well (idle empty pocket), `kChromeStackGap` 12, fitted min=max width, fixed Mappings ~385, one-line New/Rename/Delete, status 36, titles `textBrand`, Mappings header lighten, `Palette > Product colour:` (decision 2026-10-25: option 1). [`spec-skin-editor-chrome-session-skins.md`]
- [x] [Review][Patch] Retire dead `preferredHeightForMinWindow` alias — collapses to `preferredHeight()`; MainWindow still `jmax`es both. [`Tools/SkinEditor/SkinEditorSkinsPanel.cpp:108`]
- [x] [Review][Patch] Remove unused `kLogoChromeNudgeY` — defined after logo witness moved to OPERATION; no callers. [`Tools/SkinEditor/SkinEditorChromeTokens.h:115`]
- [x] [Review][Patch] Drive SKINS layout from production panel in chrome tests — `skinsPanelEditorActionsFitOneRow` recomputes widths without `SkinEditorSkinsPanel::resized` / helpers; a broken Delete=Duplicate row can stay green. [`Tests/Unit/SkinEditorChromeLayoutTests.cpp:87`]
- [x] [Review][Patch] Lock min-height stack helpers in chrome tests — no assert on `preferredHeight` + `preferredHeightForWidth` + chrome stack terms used by `skinEditorMinContentHeight`. [`Tests/Unit/SkinEditorChromeLayoutTests.cpp`]
- [x] [Review][Defer] Shell/body geometry not unit-tested beyond tokens — deferred: Skin Editor DocumentWindow / MainComponent layout stays manual smoke (prior triage + this review).
- [x] [Review][Defer] Orphan `drawCountBadge` / count-badge tokens after Unbound removal — deferred: optional Boy Scout; already tracked in clean-code close-out.
- [x] [Review][Defer] Tiny-display min height may exceed user area — deferred: needs live smoke on short displays; constrainer already clamps max to display but min is content-driven.

Rejected (this review):
- false — OPERATION well / Idle pocket / fixed Mappings 385 / fitted width / gaps 12 / one-line SKINS CRUD / textBrand titles / Mappings header colours / `Palette > Product colour:` — product smoke decisions; not defects.
- false — CMake membership missing from scoped review diff — `CMakeLists.txt` updated in `130d8c51` (SkinsPanel in, UnboundPanel out); path was outside the path-filtered diff only.
- false — ColourPicker `preferredHeightForWidth` — required for fixed COLOR height + flush-to-status + min-window contract; not a Palette model refactor.
- false — SKINS New collapse / selector overflow / inverted silhouette / body into status under normal constrainer — triggers not reachable at locked `kColorPanelWidth` + `skinEditorMinContentHeight`.
- rejected — Spec fiction (`kWindowMinWidth` resize AC, Notes 32/1210, Design nude ASCII) as standalone patches — fix would only edit this spec; folded into the Decision above.

## Review Triage Log

- false — Blind: fromUnbound state desync after bind — `bindUnbound` ends in `setMapping`; stale flag does not change write outcome.
- false — Edge: empty pastille string treated as unbound — `setMapping("")` erases; unboundKeys only checks missing keys.
- false — Edge: missing skin returns unbound=false blocks bind — `applyMapping` / layer already fail closed when skin is absent.
- false — Blind: Spec Verification should list unit suite — fix would edit this build's spec (rejected).
- low (rejected) — Blind: mapping band uses `kLogoChromeNudgeY` — optical only; everyday harm unlikely; new nudge token is extra surface.
- medium → patch (fixed) — SESSION trailing gap + dead if + overflow placement — measured well / stop-placing when row exhausted.
- medium → patch (fixed) — hardcoded 340/620 min floor vs measured title+SESSION — `windowMinWidth()` / `sessionWellPreferredWidth()` now drive constrainer.
- medium → patch (fixed) — mapping band text not elided — prefix/detail elide against band remainder.
- medium → patch (fixed) — SKINS wrap dual measure formulas — wrap uses `measureButtonWidth` only.
- medium → patch (fixed) — chrome layout tests used invented button widths / ignored measured SESSION — tests assert production measures.
- medium → defer — Verification Gap: MainWindow constrainer adoption not unit-tested — GUI shell outside Matrix-Control_Tests; manual resize remains.
- medium → defer — Verification Gap: band/body layout placement not unit-tested — Skin Editor GUI geometry stays manual smoke.
- medium (unverified) → defer — Blind: `kMinContentHeight` 480 may crush COLOR under taller SKINS stack — needs live min-height smoke to settle.
- low → defer — Edge: orphan `drawCountBadge` / count-badge tokens after Unbound removal — pre-existing cleanup inventory already tracks count-badge DRY.
- low → defer — Blind: companion clean-code inventory still lists UnboundPanel — docs outside this chrome ship.
- 2026-10-25 code review — decision: document smoke as truth; patches applied (spec + dead API + chrome tests).

## Design Notes

Target chrome (ASCII) — smoke:

```text
| logo title                         [SESSION well: Undo Redo | Compare Push | Export Import] |
| [OPERATION well: dark pocket — Idle empty | Hold: MAPPING  help… (+ swatch)]               |
| [SKINS]   [PALETTE]   [MAPPINGS (fixed ~385)]                                               |
| [COLOR]                                                                                     |
| [CONNECTION] …        [LAST PUSH] …                                       [APP] v…          |
```

Left stack width locked to `kColorPanelWidth`. SKINS / COLOR / Palette / Mappings use `paintPanelFrame`. Gaps 12 between chrome bands and SKINS↔COLOR. COLOR / Palette / Mappings flush to status.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` — expected: build success
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` then run `SkinEditorChromeLayout` — expected: pass
- `python3 Scripts/quality/lint_touched.py` — expected: clean on touched Skin Editor files

**Manual checks:**
- CRUD Product/Editor; Undo/Redo; Compare/Push; Export/Import
- Hold Mapping → OPERATION witness only; Esc → Idle clears pocket
- Export disabled while unbound > 0; no Unbound panel
- Window width stays fitted (no horizontal stretch)
