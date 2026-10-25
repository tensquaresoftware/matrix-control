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

**Approach:** Drop the Unbound panel; put session actions in a right-side SESSION well on the logo bar; replace the old SKINS strip with a nude mapping-help band; add a left SKINS panel above COLOR; let Palette + Mappings share the freed width so the window can be meaningfully narrower.

## Boundaries & Constraints

**Always:**
- Unbound panel gone from UI; bind unbound keys only via Mappings (yellow Unbound). Keep Export gated on `unboundCount == 0`.
- SESSION gestures: Undo, Redo | Compare, Push | Export, Import — identifiable well (same look language as today's SKINS strip), right of the logo title — not orphan buttons on bare logo grey.
- Mapping help band under the logo: nude fill (status-bar spirit, no SKINS well); left-aligned `MAPPING` badge + text (+ swatch when needed); badge→label gap = same constant as CONNECTION→label in the status bar (`kGap` 8 today — promote to a shared chrome token).
- Idle mapping mode: band empty (no permanent greyed MAPPING badge) — same rule as current logo witness.
- SKINS panel (left, above COLOR, `kGutter` between): Product combo + Duplicate Product; separator; Editor combo + Duplicate/New/Rename/Delete (Rename+Delete wrap to a 5th row if needed). Label column width = max(Product:, Editor:) so combo right edges align. Column width = `kColorPanelWidth`.
- SESSION well: title tab `SESSION`; vertical separators between Undo/Redo, Compare/Push, Export/Import.
- Product labels stay English ASCII (SKINS, SESSION, MAPPING, Product:, Editor:, …).

**Never:**
- Change colour model / JSON schema / Push bridge semantics.
- Reintroduce a dedicated Unbound surface or invent an unbound count badge.
- Refactor Palette / ColourPicker beyond layout bounds.
- Leave MAPPING witness painted in the logo bar after the new band exists (no double witness).
- Commit / push / PR unless Guillaume asks after the build.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Idle chrome | MappingMode Idle | Logo + SESSION well; mapping band empty; SKINS+COLOR | Palette | Mappings; status bar unchanged | N/A |
| Hold mapping | HeldColour or ArmedRow | Mapping band shows MAPPING + help (+ swatch); logo has no second witness | N/A |
| Bind unbound | Click yellow Unbound row in Mappings + pastille | Mapping writes; Export still disabled until unboundCount==0 | Existing bind error dialogs |
| Export gate | unboundCount > 0 | Export disabled; no Unbound panel required | Status/tooltip copy may still mention Unbound = 0 |
| Compare Product | Compare shows Product | Mappings read-only; no Unbound panel refresh path | N/A |
| Narrow window | User shrinks width to new min | SKINS/COLOR stack + Palette + Mappings still usable; SESSION well stays in logo | Clamp via window constrainer |
| SKINS row wrap | Editor action buttons overflow one row | Rename + Delete move to line 5 inside SKINS panel | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMainComponent.cpp` / `.h` — chrome `resized`/`paint`, title/status layout, body columns; today Unbound is 50/50 with Mappings (`layoutBodyPanels`); MAPPING witness lives in logo via `layoutMappingWitness` + `mappingsPanelLeftX()` — relocate witness to new band; remove `unboundPanel_` member and setup.
- `Tools/SkinEditor/SkinEditorMainComponentSkins.cpp` — current horizontal SKINS strip (`layoutSkinsBar` / selectors / action buttons / separators). Split: SESSION actions → logo well; CRUD → new SKINS panel layout.
- `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` — `updateMappingWitness` / bind path; drop Unbound-panel entry (`fromUnbound` only if still needed internally; panel callbacks go away).
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` — `unboundPanel_.setEmptyForCompareProduct` / refresh — remove; keep Export `unboundCount == 0` gate (~line 37).
- `Tools/SkinEditor/SkinEditorMainComponentActions.cpp` — button handlers stay; rewire only if ownership moves to a child panel.
- `Tools/SkinEditor/SkinEditorUnboundPanel.*` — delete sources + CMake entry once unreferenced.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` — update comments (`Mappings / Unbound` leftover); add shared badge gap; add mapping-band height token; add `kWindowMinWidth` (formula below); keep `kColorPanelWidth` / `kGutter`.
- `Tools/SkinEditor/SkinEditorChromePanel.*` — reuse `paintPanelFrame` / `paintRoundedWell` / `layoutPanel` for SKINS panel + SESSION well.
- `Tools/SkinEditor/SkinEditorChromeTable.h` — `drawChromeBadge` / `mappingModeBadgeWidth` — reuse for band.
- `Tools/SkinEditor/SkinEditorMainWindow.cpp` — today min=max=`kWindowMaxWidth` (1210); unlock min to `kWindowMinWidth`, keep max; default/centre/zoom still use max unless restored state is valid.
- `CMakeLists.txt` (~1102–1111) — drop UnboundPanel; add any new `.cpp` (e.g. extracted Skins panel).
- Prefer extract `SkinEditorSkinsPanel` (or Session bar helper) if MainComponent\* would approach the ~400-line useful-file gate (`MainComponent.cpp` already ~359).

**Reuse:** status-bar `kGap = 8` pattern; SKINS separator painting; Compare read-only on Mappings; Export gate.

**Do not change:** DevSkin layer / bridge / JSON export payload shape; Mappings yellow Unbound cell rendering; Palette internals.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorChromeTokens.h` — add `kChromeBadgeGap` (8), mapping-band height (= `kStatusBarHeight`), `kWindowMinWidth` from layout formula; fix leftover Unbound width comments.
- [x] `Tools/SkinEditor/` SESSION well — layout/paint in logo bar right (tab `SESSION`, Undo/Redo | Compare/Push | Export/Import); move those buttons out of the old strip.
- [x] `Tools/SkinEditor/` mapping help band — nude band under logo; relocate MAPPING witness layout/paint here; left align with CONNECTION gap; Idle = empty; remove logo-bar witness placement.
- [x] `Tools/SkinEditor/` SKINS panel — new left panel above COLOR (`kGutter`), validated grid (Product/Editor rows + CRUD); extract file if needed for lint gate.
- [x] `Tools/SkinEditor/SkinEditorMainComponent*.{h,cpp}` — body: Color under SKINS stack width `kColorPanelWidth`; Palette fixed; Mappings takes remaining; remove Unbound member/wiring/`setEmptyForCompareProduct`.
- [x] `Tools/SkinEditor/SkinEditorUnboundPanel.*` + `CMakeLists.txt` — delete orphaned panel sources and target entry.
- [x] `Tools/SkinEditor/SkinEditorMainWindow.cpp` — constrainer min width = `kWindowMinWidth`, max stays `kWindowMaxWidth`; normalize/zoom respect new min.
- [x] Smoke + `python3 Scripts/quality/lint_touched.py` on touched Skin Editor / Tests paths.

**Acceptance Criteria:**
- Given the editor open, when Idle, then no Unbound column, SESSION well visible on logo right, mapping band empty, SKINS panel above COLOR with Product/Editor CRUD.
- Given Hold mapping active, when the band paints, then MAPPING badge + help (+ swatch) appear only in the nude band (not in the logo bar), gap matching CONNECTION.
- Given unbound keys remain, when Export is considered, then Export stays disabled until unboundCount == 0, with bind available from Mappings.
- Given the window is resized toward min width, when at `kWindowMinWidth`, then all chrome regions remain usable without clipping SESSION into the title.
- Given touched C++ under `Tools/SkinEditor/`, when `lint_touched.py` runs, then it passes.

## Implementation Notes

Agent decisions (planning):
- Mapping band height = `kStatusBarHeight` (32); fill = window/app surface (no rounded SKINS well).
- `kWindowMinWidth` = left gutters + `kColorPanelWidth` + gutter + `kPalettePanelWidth` + gutter + minimum Mappings width (choose a practical floor ~320 so tables remain readable) + right gutter + horizontal panel pads as needed; keep `kWindowMaxWidth` = 1210; default launch still centres at max unless restored state fits.
- Delete Unbound panel sources entirely once unreferenced (no stub).
- Promote badge gap to `kChromeBadgeGap` used by status + mapping band.
- If Editor action buttons do not fit one row at `kColorPanelWidth` content width, wrap Rename+Delete to line 5 (per frozen layout).

Implementation (2026-10-25):
- Extracted `SkinEditorSkinsPanel` with frozen grid (Product combo → Duplicate Product → sep → Editor combo → Dup/New/Rename/Delete wrap).
- First SkinsPanel draft put Duplicate on the combo row — corrected to match frozen lines 1–2.
- Unbound binds via `isKeyUnboundOnEditor` → `bindUnbound` from Mappings rows.
- Added `Tests/Unit/SkinEditorChromeLayoutTests.cpp` for token/matrix locks; existing DevSkin + MappingModePolicy tests cover bind/export/witness/compare session.
- Build Skin Editor OK; lint_touched OK; SkinEditorChromeLayout / MappingModePolicy / DevSkinLayerJson tests passed.
- Smoke fix: Mappings keeps former half-share width (`kMappingsPanelWidth` = 385 at legacy 1210); window width fits columns via `windowContentWidth()` instead of stretching Mappings into the Unbound gap.

## Spec Change Log

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

## Design Notes

Target chrome (ASCII):

```text
| logo title                         [SESSION well: Undo Redo | Compare Push | Export Import] |
| [nude mapping band: MAPPING  help… (+ swatch)]                                              |
| [SKINS]   [PALETTE]   [MAPPINGS ........................ ]                                  |
| [COLOR]                                                                                     |
| [CONNECTION] …        [LAST PUSH] …                                       [APP] v…          |
```

Left stack width locked to `kColorPanelWidth`. SKINS créneau uses existing `paintPanelFrame` language like COLOR/Palette/Mappings.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` — expected: build success
- `python3 Scripts/quality/lint_touched.py` — expected: clean on touched Skin Editor files

**Manual checks:**
- CRUD Product/Editor; Undo/Redo; Compare/Push; Export/Import
- Hold Mapping → band witness only; Esc → Idle clears band
- Export disabled while unbound > 0; no Unbound panel
- Narrow resize to min width still usable
