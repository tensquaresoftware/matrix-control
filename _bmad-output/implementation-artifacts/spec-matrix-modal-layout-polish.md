---
title: 'Matrix modal layout polish'
type: 'feature'
created: '2026-09-30'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '70ca433022e861dc9a066e2781764f33a911c099'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-window-modal-look-strategy.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After the Matrix window/modal look restyle, smoke showed uneven modal layout (right-aligned buttons, tight body padding, mixed title casing) and a few copy/wrapping glitches; About body still uses PT Sans instead of Montserrat.

**Approach:** Centralize shared Matrix modal layout (title caps, centred button group, ~10% side inset with left-aligned body text, Don't ask again placement/font) via DialogMatrixHelpers; apply listed copy/wrapping fixes; switch About body to Montserrat while keeping brand/title fonts.

**Decisions locked (2026-09-30):**
- Shared layout for Matrix chrome text dialogs: title UPPERCASE; button group centred; body in an invisible centred block with ~10% left/right padding of modal width, text left-aligned inside; cited button names in body UPPERCASE to match labels; bottom button margin ≥ side/near-edge margin; airy, not sparse or cramped.
- Don't ask again: vertically centred between body and buttons; label Montserrat (keep `juce::ToggleButton` unless a tiny Look tweak is required for the font).
- About: Montserrat for body (tagline, grid, BMAD, links); keep current fonts for MATRIX-CONTROL brand and ABOUT title bar.
- Copy/wrapping: DEVICE SETUP — break before "A quick setup…"; break after "… when possible"; Load .m1km — do not clip "Cancel leaves…"; Flush — do not clip "The initial patch…"; Patch name mismatch — blank line between Internal name and Filename rows; vertically align values after ":"; Delete init — line break between the first two sentences.
- Punctuation: English rules already shipped — do not reintroduce spaces before `?` `!` `:`.
- Confirm semantics unchanged (codes 0/1/2; LTR Cancel → middle → primary; Enter/Escape).

## Boundaries & Constraints

**Always:**
- Prefer shared helpers in `DialogMatrixHelpers` over one-off layout tweaks that drift.
- Preserve modal return codes and LTR button order; only change placement (centred group).
- ASCII-only display strings; English UI via `PluginDisplayNames`.
- DEVICE SETUP: apply body copy aeration without breaking MIDI/EPROM row layout.
- Titles may stay PT Sans bold; body stays Montserrat via existing modal body font API.

**Never:**
- Audio From desync / Larsen / first-run None (chantier 3).
- Full Audio Settings Matrix rebuild (chantier 4).
- Native title-bar recolour; custom FileChooser; host plugin chrome.
- Re-open English punctuation pass except to fix a regression introduced here.
- Force ~10% text inset onto Settings form controls or Audio Settings device rows (confirm/text dialogs + About only for body inset rules).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Ordered confirm (2–3 buttons) | Any Matrix ordered confirm | Title caps; centred Cancel→[middle]→primary; body inset ~10%; Escape=0 Enter=1 | Unchanged codes |
| Mutator Delete + Don't ask again | Warning enabled | Toggle between body and buttons, Montserrat label; codes unchanged | N/A |
| Patch name mismatch | Internal ≠ filename | Two rows; values share a vertical column after ":" | N/A |
| DEVICE SETUP | Welcome + optional suggestion | Forced breaks per frozen copy; MIDI/EPROM rows intact | N/A |
| About | Open About | Body Montserrat; brand Orbitron; ABOUT title bar PT Sans bold | N/A |
| m1km / Flush / Delete init | Open dialog | Listed phrases fully visible (taller dialog and/or `\n` as needed) | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Dialogs/DialogMatrixHelpers.{h,cpp}` — extend with shared body inset (~10% of dialog width), centred button-row placer, title `toUpperCase` in `paintMatrixOverlayChrome` (or helper); reuse `scaledModalBodyFont` / `makeButton`.
- `Source/GUI/Dialogs/MatrixOrderedConfirmDialog.{h,cpp}` — ordered confirms + `MatrixMutatorDeleteConfirmDialog` (Don't ask again); adopt helpers; special paint/layout for patch-name mismatch value columns when message is reconciliation (or thin structured path from call site).
- `Source/GUI/Dialogs/MasterInitConfirmDialog.*`, `MutatorHistoryDefragConfirmDialog.*`, `MasterM1kmLoadChoiceDialog.*` — same overlay family; replace duplicated 12px/right-align layout with helpers; bump design height if copy needs it.
- `Source/GUI/Dialogs/EpromTypePromptDialog*` / `EpromTypePromptDialogLayout.cpp` — body copy breaks + shared button centring / title caps; keep `computeContentLayout` MIDI/EPROM rows.
- `Source/GUI/Dialogs/BankTransferProgressDialog.*` — centred Cancel + title caps via helpers where chrome still custom; do not redesign phase lanes.
- `Source/GUI/About/AboutPanel.cpp` — body paints/measurers → `getModalBodyFont()` / `scaledModalBodyFont`; leave `getBrandFontBold` and `AboutWindow` title bar alone.
- `Source/Shared/Definitions/PluginDisplayNames.h` — DEVICE SETUP / m1km / Flush / mismatch / Delete init copy + UPPERCASE button citations in body text; no French punctuation spaces.
- `Source/GUI/PluginEditorPatchBindings.cpp` — mismatch body assembly if structured layout needs separate label/value strings.
- Do not change: Core modal-gate semantics, FileChooser, AudioMidiSettings control rebuild, Settings form grid inset.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Dialogs/DialogMatrixHelpers.*` -- Add shared body-content rect (~10% side inset), centred button-group layout, and title uppercase in Matrix chrome paint -- one layout SSOT.
- [x] `Source/GUI/Dialogs/MatrixOrderedConfirmDialog.*` (+ Mutator delete) -- Adopt helpers; Montserrat + vertical placement for Don't ask again; patch-mismatch value column alignment -- covers most confirms.
- [x] `Source/GUI/Dialogs/MasterInitConfirmDialog.*`, `MutatorHistoryDefragConfirmDialog.*`, `MasterM1kmLoadChoiceDialog.*`, `EpromTypePromptDialog*`, `BankTransferProgressDialog.*` -- Adopt shared layout; keep DEVICE SETUP rows and bank phase UI -- no layout drift.
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` (+ mismatch binding if needed) -- Apply frozen copy/wrapping and UPPERCASE button citations -- smoke list.
- [x] `Source/GUI/About/AboutPanel.cpp` -- Montserrat body fonts in paint + layout measure -- About trial.
- [x] Build macOS Debug + `Scripts/quality/lint_touched.py` on touched C++ -- quality gate.
- [x] Update `guide-smoke-window-modal-look.md` polish checklist items when done; short Design Notes smoke list -- backlog sync.

**Acceptance Criteria:**
- Given a Matrix text confirm, when shown, then the title is uppercase, the button group is horizontally centred, and body text sits in a centred inset (~10% sides) left-aligned inside that block.
- Given Mutator Delete with warning, when shown, then Don't ask again sits between body and buttons (vertically centred in that band) with a Montserrat label, and Cancel/Delete codes stay 0/1.
- Given Patch name mismatch, when shown, then Internal name and Filename are on separate rows with values aligned on a shared column after ":".
- Given DEVICE SETUP / m1km / Flush / Delete init, when shown, then the frozen line breaks and unclipped phrases from the smoke list hold, without breaking DEVICE SETUP MIDI/EPROM rows.
- Given About, when opened, then body copy uses Montserrat while MATRIX-CONTROL and the ABOUT title bar keep their current fonts.
- Given Enter / Escape / outside-click cancel paths, when used, then semantic codes and LTR order remain unchanged.

## Implementation Notes

- Shared layout SSOT lives in `DialogMatrixHelpers`: `computeModalGeometry` / `computeTextModalLayout` (10% side inset, content-driven dialog height, band between body and buttons), `layoutCentredButtonRow`, `measureBodyHeight` / `paintBodyText` (no horizontal squeeze, so nothing clips), `ModalToggleLookAndFeel` (Montserrat label for Don't ask again).
- Dialog heights are now measured from the body text (old fixed design heights removed from ordered confirm, Mutator Delete, Master Init, Defrag, m1km, DEVICE SETUP); bank progress keeps its fixed lane heights.
- Patch name mismatch: `OrderedConfirmAlertOptions::valueRows` draws label and value columns with a blank line between rows.
- DEVICE SETUP copy: paragraph break after the first sentence; the firmware suggestion is its own paragraph (`kBodySuggestionSuffix`). Unit test expectation updated accordingly.
- Delete init: the two sentences were already separated by a line break in `PluginDisplayNames`; no text change beyond UPPERCASE `CANCEL`.
- Matrix audit: DEVICE SETUP body assembly covered by `DeviceSetupDeviceRowTests`; confirm semantic codes unchanged (existing Core gate tests + no gate wiring edits). Visual layout rows (inset, centred buttons, Don't ask again, mismatch columns, About fonts) are manual-smoke only — CONVENTIONS avoid GUI modal-loop unit tests.
- Review patches (2026-09-30): bottom margin >= side inset; centred button clamp; glyph-based `estimateButtonWidth`; scaled Don't-ask-again gap; mismatch value-column height + label clamp; bank progress shared bottom reserve; Defrag UPPERCASE citations; About uses `scaledModalBodyFont`; `MatrixModalCopyTests` locks frozen confirm bodies.
- Follow-up chrome (2026-09-30): black 20 px title band + white caps title; button gap / bottom margin fixed at 12 px; Settings/About/Audio/bank progress share `paintMatrixOverlayChrome` metrics; design rules in `guide-matrix-modal-design.md`.

## Spec Change Log

## Review Triage Log

- verdict: medium | evidence: Frozen intent requires bottom button margin >= side/near-edge margin; `kButtonBottomMargin` is 16 design px while ~10% side inset is ~46 px on a 460-wide dialog. Route: patch.
- verdict: false | evidence: Design Notes intentionally keep bank progress phase-lane paint with fixed pad; only chrome title caps + centred Cancel are in scope for that dialog.
- verdict: medium | evidence: `computeBodyLayout` measures row height from labels at full text width only; values use a narrower column and can wrap more — confirmed in `MatrixOrderedConfirmDialog.cpp`. Route: patch.
- verdict: low | evidence: `kToggleTextLeadPixels` is unscaled; Montserrat tick gap drifts at non-1.0 UI scale. Route: patch.
- verdict: medium | evidence: `estimateButtonWidth` uses char counts; m1km already documents under-sizing and keeps hand-tuned widths. Route: patch (glyph-based estimate).
- verdict: low | evidence: `AboutPanel::getScaledBodyFont` duplicates `scaledModalBodyFont` path. Route: patch.
- verdict: medium | evidence: No automated observer for shared layout helpers; CONVENTIONS + smoke own GUI paint. Route: defer (unverified coverage gap by policy).
- verdict: low | evidence: `MutatorHistoryDefrag::kBody` still ends with "Continue?" while primary is DEFRAG — missed UPPERCASE citation pass. Route: patch.
- verdict: false | evidence: paint/resized both call the same `computeBodyLayout` / `computeTextModalLayout`; no paint-vs-hit desync. Caching would add complexity without fixing a demonstrated bug.
- verdict: false | evidence: Guide marks items implemented with explicit "smoke visuel à refaire" — not claiming visual acceptance done.
- verdict: medium | evidence: `layoutCentredButtonRow` does not clamp when packWidth > row width; buttons can draw past chrome. Route: patch.
- verdict: medium | evidence: Same as mismatch value wrap under-measure (grouped). Route: patch.
- verdict: medium | evidence: `labelColumnWidth` uncapped vs textArea width can collapse value column. Route: patch.
- verdict: false | evidence: Current frozen bodies are far under `kMaxBodyLines` (40); no reachable overflow with listed copy.
- verdict: medium | evidence: Bank progress paint uses `round(40*uiScale)` while resized uses `round(24*s)+round(16*s)`; can diverge by 1 px at some scales. Route: patch (shared sum).
- verdict: medium | evidence: Same bottom-vs-side margin claim as first finding (grouped). Route: patch.
- verdict: medium | evidence: Verification-gap — shared layout SSOT unobserved by tests; disposition defer (CONVENTIONS). Route: defer.
- verdict: medium | evidence: Verification-gap — mismatch `valueRows` wiring unobserved; disposition defer. Route: defer.
- verdict: medium | evidence: Verification-gap — About Montserrat unobserved; disposition defer. Route: defer.
- verdict: medium | evidence: Verification-gap — frozen non–DEVICE SETUP copy lacks approved-string locks; disposition patch. Route: patch.

## Design Notes

- Agent choices (user would not notice): Settings / Audio Settings forms keep their own content grids (no 10% text-block inset). Bank progress keeps phase-lane paint; only shared chrome/button/title rules apply. Prefer measuring mismatch columns in the dialog (label widths + shared value X) over fragile tab characters. Bump dialog design height when `\n` alone cannot prevent clipping.
- Golden body inset: `sidePad = round(dialogWidth * 0.10f)` inside the chrome content area; buttons: pack widths + gaps, then centre the pack in the bottom row.
- Cited actions in body must match button labels (e.g. CONTINUE / CANCEL / DELETE), ASCII only.
- **Chrome metrics (2026-09-30 evening):** black title band **24 px** (= button height), title colour = idle button text (`kButtonTextOff`); gaps title→body, body→buttons, and buttons→bottom border = **24 px**; button gap = **12 px**. Paragraph copy uses `\n\n` between distinct sentences. SSOT: `guide-matrix-modal-design.md`.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: success
- `python3 Scripts/quality/lint_touched.py` on touched `Source/**/*.{h,cpp}` -- expected: pass

**Manual checks (if no CLI):**
- Smoke a few Matrix confirms (2-button + 3-button + Mutator Delete), DEVICE SETUP, m1km, Flush, mismatch, Delete init, and About — layout + copy + Montserrat body.
