---
organization: Ten Square Software
project: Matrix-Control
title: Matrix modal design rules
author: BMad Agent
created: 2026-09-30
updated: 2026-10-08
sources:
  - implementation-artifacts/spec-matrix-modal-layout-polish.md
  - implementation-artifacts/spec-window-modal-look-strategy.md
  - implementation-artifacts/guide-smoke-window-modal-look.md
---

# Matrix modal design rules

SSOT for product dialogs / overlays that use Matrix-Control chrome (not OS FileChooser, not host plugin chrome).

**Code SSOT:** `Source/GUI/Dialogs/DialogMatrixHelpers.h` (+ `.cpp`). Prefer those helpers over one-off layout numbers.

## Chrome

| Rule | Value (design px @ 100% UI scale) | Notes |
|------|-----------------------------------|--------|
| Outer border | 4 px, colour `kDialogBorderColour` (`0xFF5E5E5E`) | Full dialog frame |
| Title band | **24 px** tall (= button height), **black** (`kModalTitleBandColour`), full **inner** width | Stops at left/right grey border; not inset |
| Title text | UPPERCASE, PT Sans Bold (`scaledTitleFont`), colour = idle button text (`kButtonTextOff` / LightGrey2) | Centred in the title band; not pure white |
| Body panel fill | Skin `kHeaderPanelBackground` | Under the title band |
| Dim behind dialog | Body panel colour @ ~85% alpha | Host editor behind |

Settings, About, Audio Settings, bank progress, confirms, GETTING STARTED (and legacy DEVICE SETUP until absorbed) — all use `paintMatrixOverlayChrome` (or must match it).

GETTING STARTED: same design width as Settings; per-step height lower than Settings (few controls per step). Step titles live in the title band. Form steps reuse Settings/header control bricks — do not duplicate MIDI/audio device list logic.

## Vertical rhythm (text / confirm dialogs)

| Rule | Value | Notes |
|------|-------|--------|
| Gap title band → first body line | **24 px** | `kGapAfterTitle` |
| Gap last content → button row | **24 px** | `kGapBeforeButtons` — last element of any kind (body text, checkbox, progress bar, form row, …) |
| Extra controls in that zone | + control height + **24 px** again | Don't ask again / form-row dialogs (Settings / GETTING STARTED): 24 px above **and** below the control block |
| Gap above bottom border | **24 px** | `kButtonBottomMargin` |
| Gap side border → nearest button | **≥ 24 px** | `kButtonSideMargin`; pack shrinks if needed |
| Gap between buttons | **12 px** | `kButtonGap` |

## Body text (text / confirm dialogs)

| Rule | Value | Notes |
|------|-------|--------|
| Face | Montserrat via `getModalBodyFont` / `scaledModalBodyFont` | Readable lowercase |
| Side inset (default) | ~10% of modal **content** width each side | Invisible centred block; text **left-aligned** inside |
| Side inset (patch name mismatch only) | Left edge = CANCEL left edge; right ≥ 24 px | Opt-in via `alignBodyToCancel` |
| Paragraph breaks | Hard line breaks between **distinct** sentences/paragraphs use a **blank line** (`\n\n`) | Soft wrap inside one paragraph stays tight; do not stack separate sentences with a single `\n` |
| Button names cited in copy | UPPERCASE matching real labels | e.g. CONTINUE / CANCEL / DELETE |
| Punctuation | English: no space before `?` `!` `:` | ASCII only in display strings |

**Exceptions:** Settings / Audio Settings **forms** keep their own control grids (do not force 10% text inset on device rows). Bank progress keeps phase-lane layout; still uses shared chrome + centred Cancel + title rules.

## Buttons

| Rule | Value | Notes |
|------|-------|--------|
| Control | `TSS::Button` + Matrix Look | No stock `juce::TextButton` chrome |
| Labels | UPPERCASE English (`PluginDisplayNames`) | PT Sans from button Look; idle text = LightGrey2 |
| Default size | 88 × **24** design px (grow with glyph width) | `estimateButtonWidth` / `makeButton` |
| Gap between buttons | **12 px** | `kButtonGap` |
| Gap above bottom border | **24 px** | `kButtonBottomMargin` |
| Gap side border → nearest button | **≥ 24 px** | `kButtonSideMargin` |
| Group placement | Horizontally **centred** pack | LTR order unchanged: Cancel → [middle] → primary |
| Overflow | Shrink gaps then widths to keep the 24 px side margins | Never draw past chrome |

## Don't ask again (Mutator Delete)

- Vertically centred between body text and buttons, with **24 px** above and **24 px** below the checkbox
- Label uses Montserrat (`ModalToggleLookAndFeel`)
- Keep `juce::ToggleButton` unless a tiny Look tweak is required

## Semantics (do not change for look polish)

- Modal codes: Cancel / Escape / outside click → **0**; primary / Enter → **1**; middle → **2**
- Visual LTR: Cancel left, primary rightmost
- FileChooser stays OS-native

## About-specific

- Title bar "ABOUT": shared Matrix chrome (black 24 px band + button-grey caps title)
- Brand "MATRIX-CONTROL": Orbitron Bold (unchanged)
- Body (tagline, grid, BMAD, links): Montserrat (`scaledModalBodyFont`)

## New Matrix modal checklist

1. Paint with `DialogMatrixHelpers::paintMatrixOverlayChrome` (title string may be mixed case in code; chrome forces UPPERCASE).
2. Size title / border / vertical gaps from the shared constants — do not reintroduce local chrome metrics.
3. For text confirms: `computeTextModalLayout` / `computeModalGeometry` + `layoutCentredButtonRow` + `paintBodyText`.
4. Put copy in `PluginDisplayNames` (ASCII, English punctuation, UPPERCASE action citations, `\n\n` between paragraphs).
5. Preserve Enter / Escape / LTR codes.
6. Smoke: black title band 24 px, button-grey title, 24 px gaps above/below body and under buttons, ≥ 24 px side margins to buttons, centred buttons, 12 px button gaps.

## Out of scope for this chrome family

- OS FileChooser
- Host plugin window chrome / native standalone title bar colour
- Full Audio Settings control rebuild (separate chantier)
