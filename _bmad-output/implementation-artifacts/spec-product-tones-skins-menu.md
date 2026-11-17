---
title: 'Ship product TONES skins and logo family submenus'
type: 'feature'
created: '2026-11-17'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '530f91bfac508a023c5a0549007fe64615ccd6a2'
context:
  - '{project-root}/Source/GUI/Skins/Skin.h'
  - '{project-root}/Source/GUI/Helpers/EditorChromeShortcuts.h'
  - '{project-root}/Source/GUI/Widgets/HeaderLogoPopupMenuItems.cpp'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Fifteen validated dark TONES skins live only in Trials, while the logo Skin menu is a flat list of six bases with no family structure or in-family keyboard browse.

**Approach:** Compile the fifteen dark TONES into product skins from Trials; regroup the logo Skin section into BLACK / CREAM / TONES family rows that open a side sub-popup; keep Cmd/Ctrl 1-6 on the six bases; add Cmd/Ctrl arrows (family) and Cmd/Ctrl+Shift arrows (global) cycling; extend prefs ids so Settings / Getting Started stay coherent.

## Boundaries & Constraints

**Always:**
- Trials SSOT (read once): Dropbox `Matrix-Control Skin Designer - Trials.json` schemaVersion 2 skins `dev:tones-*` named `TONES/CHERRY` … `TONES/FUCHSIA` (15).
- Display order TONES (color-dominant chain = Designer combo order): CHERRY, CORAL, SAFFRON, OCHRE, GOLD, OLIVE, LIME, EMERALD, KIGEE, PETROL, AZUR, MARINE, PURPLE, INDIGO, FUCHSIA.
- Logo Skin top-level families only: BLACK, CREAM, TONES (no GREEN / CHROMA / MINIMAL as top-level families).
- Family rows show a **drawn filled right-pointing triangle** at the right (same visual language as combo down-triangles / existing submenu chevron paint) — not a `>` or other Unicode triangle character in the label text. Labels stay ASCII (`BLACK`, `CREAM`, `TONES`, `TONES/EMERALD`, …).
- Hover/click on a family opens a **side sub-popup** beside the logo menu (not a replace-in-place of the Skin column). Sub-popup uses the **same background and border** treatment as the logo menu popup.
- BLACK = three dark neutrals; CREAM = three light neutrals; TONES V1 = fifteen dark tones flat in the TONES sub-popup.
- Prefs ids: keep 1-6 for existing bases; assign stable 7-21 to TONES in display order; invalid → BLACK/GREEN (`kBlack`).
- Cmd/Ctrl 1-6 unchanged (digit 1 = BLACK/GREEN reset). Cmd/Ctrl 0 remains UI Scale 100%.
- Arrow mapping: up/left = previous; down/right = next; cycle. Family cycle uses active skin's family.
- Global cycle (Cmd/Ctrl+Shift+arrow) order: BLACK/GREEN → BLACK/CHROMA → BLACK/MINIMAL → CREAM/GREEN → CREAM/CHROMA → CREAM/MINIMAL → then the fifteen TONES in display order above.
- Cmd/Ctrl(+Shift)+arrow skin shortcuts consume the key (no focused slider/combo nudge). Bare arrows / Shift-only unchanged.
- Display strings ASCII only.
- Structure must allow later TONES Dark/Light nesting without a fourth top-level family and without putting light tones in CREAM — do not implement Light or Dark/Light nesting now.

**Never:**
- Runtime dependency on the Dropbox Trials JSON.
- TONES Light variants or TONES Dark/Light submenus (post-V1).
- Remap Cmd/Ctrl 0 for skin reset; flatten all 21 skins into one logo column.
- Unicode triangle / chevron characters in `PluginDisplayNames` labels (draw geometry instead).
- Skin Designer Trials editing / Designer Product-combo redesign (product path + ids only unless a unit test inventory forces a minimal compiled hook).
- French in source; GUI deps in Core; broad refactors outside skin/menu/shortcuts.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Pick TONES/EMERALD from logo | TONES family → side popup → EMERALD | Look matches validated Trials; prefs id 14 | N/A |
| Open family row | Hover/click BLACK, CREAM, or TONES | Side sub-popup opens beside logo menu; same bg/border; filled right triangle on family row | N/A |
| Cmd/Ctrl 1-6 | Digits on six bases | Same skins as today (incl. digit 5 → CreamChroma id 6) | N/A |
| Cmd/Ctrl+Down in TONES | Active TONES/GOLD | Next tone (OLIVE); wraps FUCHSIA→CHERRY | N/A |
| Cmd/Ctrl+Shift+Down from CREAM/MINIMAL | Active CreamMinimal | First TONES (CHERRY) | N/A |
| Cmd/Ctrl+Shift+Down from BLACK/MINIMAL | Active BlackMinimal | CREAM/GREEN (digit order, not family-only) | N/A |
| Cmd/Ctrl 0 | Scale shortcut | UI Scale 100% only | N/A |
| Arrow without Cmd/Ctrl | Focused slider/combo | Existing widget nudge | N/A |
| Corrupt prefs skin id | e.g. 0 or 99 | Fall back to BLACK/GREEN | Normalize via `isValid` |
| Restore TONES id after relaunch | Saved id 7-21 | Matching TONES reapplies | Unknown → default |

</frozen-after-approval>

## Code Map

- Trials (read-only port source) — `…/Skins/Matrix-Control Skin Designer - Trials.json` (`dev:tones-*`, 21-stop ramps, 197 mappings; zones identical; PETROL differs on 5 step ranks).
- `Source/GUI/Skins/ColourChartTones.h` (new) — 15×21 ARGB ramps from Trials.
- `Source/GUI/Skins/SkinColoursTones*.h` (new) — shared zone order + per-tone step indices (PETROL overrides) resolving into `Skin` colours; do **not** use CreamChroma-style sparse overlays (TONES remaps all 197 zones).
- `Source/GUI/Skins/Skin.h` / `Skin.cpp` — extend `ColourVariant` (or equivalent tone family API), `SkinComboBoxItemId` 7-21, `create` / `initializeDefaultColours`.
- `Source/Shared/Definitions/PluginIDs.h` — `SkinVariants` constants + `isValid` through 21; default remains `kBlack`.
- `Source/Shared/Definitions/PluginDisplayNames.h` — `TONES/…` labels; logo family headers BLACK/CREAM/TONES; shortcut hint copy ASCII.
- `Source/GUI/Widgets/HeaderLogoPopupMenu*.{h,cpp}` — Skin column = family rows (BLACK/CREAM/TONES) + side sub-popup listing members; reuse filled right-triangle paint (`PopupMenuRenderer::drawSubMenuChevron` / combo triangle metrics); match logo popup bg/border; keep Scale column; wire selection to `onSkinSelected` → `applySkinFromItemId`.
- `Source/GUI/Widgets/PopupMenuRenderer.*` / `ComboBoxControlPainter.*` — reference for triangle geometry and chrome colours (reuse, do not invent a third glyph style).
- `Source/GUI/Helpers/EditorChromeShortcuts.h` (+ labels) — keep digit 1-6; add family/global arrow classifiers; Scale 0 untouched.
- `Source/GUI/PluginEditor.cpp` / `PluginEditorSkinScale.cpp` — handle new shortcuts; intercept before widget arrow nudge; apply via `applySkinFromItemId`.
- `Source/GUI/Settings/SettingsPanelSetup.cpp` + Getting Started skin combo — list all valid ids (flat `FAMILY/NAME` labels OK); share allowlist via `isValid`.
- DevSkin / CMake — only if product compile or existing inventory tests require minimal hooks; no Designer Trials work.
- Tests: `EditorChromeShortcutTests`, `GettingStartedSettingsContractTests`, skin colour / id contract tests — cover ids 7-21, family/global cycle order, Scale 0, no bare-arrow regression.
- Prior method reference: `spec-cream-chroma-product.md` (overlay pattern is **not** the TONES model).

## Tasks & Acceptance

**Execution:**
- [x] Generate tone ramps + zone/step tables from Trials into product headers — compile-time SSOT, no Dropbox runtime.
- [x] Extend Skin / PluginIDs / DisplayNames / applySkin path for ids 7-21 and fifteen TONES looks.
- [x] Logo menu: BLACK / CREAM / TONES family rows with drawn right triangles; side sub-popups (same bg/border); digits 1-6 hints on bases; optional prev/next hint strings.
- [x] Keyboard: family + global arrow cycling; consume Cmd/Ctrl(+Shift)+arrows; leave Scale 0 and bare arrows alone.
- [x] Settings + Getting Started + persistence allowlist via extended `isValid`.
- [x] Unit tests for ids, cycle order, families, Scale 0, classifier non-collision with bare arrows.
- [x] `lint_touched.py` + relevant tests/build green.

**Acceptance Criteria:**
- Given the plugin, when the user opens logo Skin, then they see BLACK / CREAM / TONES family rows with drawn right triangles (not a flat 21-row column, not `>` characters in labels).
- Given a family row, when opened, then a side sub-popup lists that family's skins using the same background and border as the logo menu.
- Given the TONES sub-popup, when any of the fifteen dark tones is chosen, then the UI matches the validated Trials look for that name.
- Given Cmd/Ctrl 1-6, when pressed, then the six base skins still apply exactly as today.
- Given an active TONES skin, when Cmd/Ctrl+arrow is used, then cycling stays inside TONES; with Cmd/Ctrl+Shift+arrow, cycling follows digit order 1-6 then the fifteen TONES.
- Given Cmd/Ctrl 0, when pressed, then UI Scale resets to 100% and no skin changes.
- Given a focused slider, when an unmodified arrow is pressed, then the control still nudges and skin does not change.
- Given prefs id 7-21, when the session reloads, then the matching TONES skin restores; invalid ids fall back to BLACK/GREEN.

## Implementation Notes

- Ported Trials ramps into `ColourChartTones.h` + shared zone steps in `SkinColoursTones.h` (5 PETROL overrides).
- `SkinCatalog` owns family/global cycle order and flat Settings/Getting Started population.
- Logo menu split across Items/Layout/Painting/Flyout; family rows use `drawSubMenuChevron`; flyout reuses popup bg/border paint.
- Arrow skin cycling handled in `PluginEditor::tryHandleEditorChromeKey` before other chrome shortcuts.
- Parent verification added catalog family-membership + `populateFlatSkinCombo` + `SkinVariants::normalize` tests (visual flyout remains manual smoke).
- Review patches: Slider yields Cmd/Ctrl+arrows; flyout Y clamp; ColourChart include; family sizeof; static_asserts; skin help copy.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Cmd/Ctrl(+Shift)+arrow still nudges focused Slider; skin cycle never runs | high | Verified: `Slider::keyPressed` handles arrows without checking command modifier and returns true, so editor KeyListener never sees the key. route: patch |
| Claim duplicate: Slider consume vs skin cycle (edge claim) | high | Same root as row above. route: patch (grouped) |
| getSkinVariantId clamp / TONES restore never exercised in tests | medium | VG pre-verified: only `isValid` constants tested; getter uncalled. route: patch (`SkinVariants::normalize` + tests) |
| populateFlatSkinCombo never called in tests | medium | VG pre-verified: Settings/GS can drop TONES while cycle tests stay green. route: patch |
| TONES flyout can extend past screen / parent edge | medium | Verified: `layoutFamilyFlyout` expands size and clamps Y inside parent only; parent position not re-clamped after growth. route: patch |
| ColourChart symbols used in Layout.cpp without direct include | low | Compiles today via fragile transitive visibility; Painting.cpp includes explicitly. route: patch |
| BLACK/CREAM family counts hardcoded as `3` in membersForFamily | low | Desync risk vs catalog arrays. route: patch |
| No static_assert aligning tone id ranges / counts | low | Parallel tables can drift. route: patch |
| Contextual help `kSkin` still old one-liner | low | Spec asked for help/hints; still "Chooses the visual skin…". route: patch |
| `skinGlobalCycle()` label never shown in menu | low | Spec marks prev/next hints optional; family-cycle hint already on Skin section. Reject everyday: optional. route: false |
| Settings populateFlat without clear duplicates list | false | `populateAppearanceComboItems` only called once from Settings ctor. |
| setSkinVariantId writes invalid ints | false | Contract is normalize on read (`getSkinVariantId`); matches existing prefs pattern. |
| Hover Scale/Settings closes family flyout | false | Intentional family switch / non-family highlight closes flyout; show() still opens current family. |
| Logo menu selection stale while Cmd+arrow cycles | false | Logo menu is modal and only handles Escape; chrome arrow shortcuts do not reach the editor while it is up. |
| Flyout text may clip without ellipsis helper | low | Reject: unlikely everyday for current `TONES/NAME` lengths + digit hints; fix would add paint complexity. |
| familyMemberListsMatchLogoFlyout does not mount logo menu | medium | VG: test name overclaims; catalog arrays only. route: defer (rename in patch) + defer GUI mount |
| Spec triage/change logs empty / notes claim green | false | Reject: finding asks to edit this build's spec process artifact. |
| Redundant Settings re-normalize after getter | low | Reject: harmless; no user-facing defect. |

## Design Notes

**Port model:** TONES are full monochrome remaps of `compiled:black` (197 zones, 21 stops), not CreamChroma accent overlays. Prefer shared zone list + per-tone step indices + 15 ramps (PETROL has five intentional rank differences). Avoid fifteen `ColourElement` columns and avoid fifteen sparse overlay headers.

**Logo families UX:** Side sub-popup (flyout), not in-column replace. Drawn filled right triangles on family rows (combo down-triangle language / `drawSubMenuChevron`). Sub-popup chrome matches the logo menu popup.

**Global keyboard order:** Same as shortcuts 1-6 for the six bases, then TONES in color-dominant Designer order (listed in Always).

**Code shape for post-V1:** Keep a `SkinFamily` (or equivalent) of BLACK | CREAM | TONES so TONES can later split Dark/Light without a fourth top-level logo family and without putting light tones in CREAM.

**Catalog surfaces:** Settings / Getting Started stay flat combos listing all valid skins with existing `FAMILY/NAME` labels; only the logo menu uses hierarchical families + side popup.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under Source/Tests
- Preset build + `Matrix-Control_Tests` (skin/shortcut/settings contract filters) -- expected: 0 failures

**Manual checks:**
- Logo: family right-triangles + side sub-popup (same bg/border); spot-check EMERALD and PETROL vs Trials.
- Keyboard: family cycle, global cycle (1-6 then TONES), digits 1-6, Scale 0, bare arrows on a slider.
