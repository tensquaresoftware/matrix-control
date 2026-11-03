---
title: 'Skin Editor Build A — ProgressBar inventory + NumberBox caret'
type: 'feature'
created: '2026-11-03'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '3e4d2d5703ac5606f1d0b48fbd02e8ddbeacf7fc'
context:
  - '{project-root}/_local/Documents/Recap-Skin-Editor-Mappings-Ergonomie-et-Link.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Progress bars still paint from Slider colours (and at `k1`), so Skin Editor cannot address them as their own object, and modal bars miss the modal ColourSet. NumberBox blink caret reuses EditorText, so Duo / custom skins cannot tint the caret alone.

**Approach:** Add a first-class multi-set `ProgressBar` skin object (inventory, seeds, Mappings table) and wire the real progress paint site to `ProgressBar` at `ColourSet::k4`. Add a NumberBox `Caret` pastille and paint the blink caret from it. Leave Mappings UX (sort, Object filter, Link, Colour-set labels) to later builds.

## Decisions (planning)

- **ProgressBar properties:** `Track`, `ValueBar`, `Text` plus Disabled variants for those three — only what the progress UI paints today. No FocusBorder / Background / dialog-name rows.
- **ColourSet:** `ProgressBar` is multi-set (`k1`…`k4`). Modal progress paint uses **`k4`**. Base seed copies current Slider Track/ValueBar/Text (+ Disabled) ARGB onto all four sets; `ProgressBarK4` overlay mirrors `SliderK4` Track/ValueBar/Text so modal greys match other modal controls.
- **Visual change accepted:** Bank Transfer progress bar leaves Slider/`k1` emerald and follows ProgressBar/`k4` modal greys on Black & Cream — intentional product fix, not a regression to avoid.
- **NumberBox caret:** new `NumberBox::kCaret` using existing `SkinColourProperty::kCaret`; Black/Cream seed = current EditorText white so caret pixels stay identical until remapped. Paint uses a new `NumberBoxLook` field (e.g. `editorCaret`). Do not retarget Slider `editorCaret` in this build.
- **Mappings order:** insert `ProgressBar` immediately after `Slider` in `mappingsObjectDisplayOrder()` only. No other UX changes.
- **Lane disabled label** in the progress dialog uses ProgressBar Text + Disabled (was Slider Text Disabled).

## Boundaries & Constraints

**Always:**
- ProgressBar is a real `SkinObjectType` in inventory / seeds / Mappings and is what progress paint reads.
- Multi-set expands via `kMultiSetSkinObjects` + existing DevSkin seed expand; `k4` overlay via `putMappingExact` like other modal overlays.
- NumberBox caret is addressable in Mappings and drives blink paint.
- ASCII-only for any new user-visible copy (none expected beyond inventory name tokens).

**Never:**
- Build B / C (column sort, Object macro-filter, Link column, Colour-set free labels, rename `kN` to zone names).
- Dialog product names (« Bank Transfer », etc.) as Mappings rows or object labels.
- Reducing ColourSet count per object type; Skin Studio web; wholesale Mappings order reshuffle.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Modal progress paint | Bank Transfer dialog paints bar | Track / fill / % text from `ProgressBar` at `k4` | Missing `k4` → existing set→`k1` fallback |
| Disabled phase lane | Lane `enabled == false` | Track Disabled; no fill; % `0`; muted ProgressBar Text Disabled | N/A |
| Skin Editor inventory | Open Mappings on Black/Cream | Distinct ProgressBar table with `k1`…`k4` rows; no dialog titles | N/A |
| NumberBox edit caret | Enter edit on product NumberBox | Blink caret uses NumberBox Caret pastille (white at seed) | N/A |
| Remap caret only | Author remaps NumberBox Caret, leaves EditorText | Digits keep EditorText; caret uses new pastille | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h` — add `SkinObjectType::kProgressBar`; append to `kMultiSetSkinObjects`.
- `Source/GUI/Skins/SkinColoursWidgetsControls.h` — `Widgets::ProgressBar::*` ColourElements (+ `ProgressBarK4` Track/ValueBar/Text); keep Slider unchanged.
- `Source/GUI/Skins/SkinColoursWidgetsDisplays.h` — `NumberBox::kCaret` (property `kCaret`, white Black/Cream).
- `Source/GUI/Skins/Skin.cpp` — `putColour` ProgressBar (+ exact K4 overlay); `putColour` NumberBox caret in display init.
- `Source/GUI/Skins/Dev/DevSkinColourElementList.cpp` — push ProgressBar elements after Slider; push NumberBox caret after EditorText.
- `Source/GUI/Skins/Dev/DevSkinKeyNames.cpp` — `"ProgressBar"` object token (property `"Caret"` already exists).
- `Source/GUI/Skins/Dev/DevSkinSsotSeed.cpp` — overlay `ProgressBarK4` in the K4 former-ButtonLike overlay helper (alongside SliderK4).
- `Source/GUI/Dialogs/BankTransferProgressDialog.cpp` — `paintProgressBar` / disabled lane text → `getColour(kProgressBar, …, ColourSet::k4)` (enabled/disabled as today).
- `Source/GUI/Looks/WidgetLooks.h` + `LookBuildersMultiSet.cpp` + `Source/GUI/Widgets/NumberBox.cpp` — `editorCaret` on `NumberBoxLook`; resolve from NumberBox `kCaret`; blink paint uses it.
- `Tools/SkinEditor/SkinEditorMappingsColumnFilter.h` — insert `kProgressBar` after `kSlider` only.
- `Tests/Unit/SkinEditorMappingsColumnFilterTests.cpp` (+ substrate/seed tests if they assert object coverage) — ProgressBar present, after Slider, multi-set; caret key in inventory.
- Do **not** change: Mappings filters/Hold/Unbound UX; Object combo behaviour; Link; Colour-set label chrome; Slider Look/caret wiring; other dialogs without progress bars.

## Tasks & Acceptance

**Execution:**
- [x] `SkinValues.h` + `SkinColoursWidgetsControls.h` + `Skin.cpp` + Dev list/names/seed — introduce ProgressBar multi-set SSOT + K4 overlay — inventory substrate.
- [x] `BankTransferProgressDialog.cpp` — paint progress (and disabled lane mute) from ProgressBar/`k4` — no phantom table.
- [x] `SkinColoursWidgetsDisplays.h` + Skin init + Dev list + `NumberBoxLook` / LookBuilder / `NumberBox.cpp` — Caret pastille + blink paint — addressable caret.
- [x] `SkinEditorMappingsColumnFilter.h` + unit tests — ProgressBar table after Slider; inventory coverage — editor visibility.
- [x] `python3 Scripts/quality/lint_touched.py` + relevant unit tests / preset build — quality gate.

**Acceptance Criteria:**
- Given Black or Cream, when Skin Editor Mappings lists objects, then a ProgressBar table exists with `k1`…`k4` rows and no dialog product names.
- Given Bank Transfer progress dialog, when a phase paints its bar, then track/fill/% colours resolve from ProgressBar at `k4` (not Slider).
- Given Black or Cream seeds, when modal progress paints, then hues follow ProgressBarK4 (aligned with SliderK4 greys), not Slider `k1` emerald.
- Given NumberBox edit mode, when the caret blinks, then its colour is NumberBox Caret; remapping Caret alone does not change EditorText digits.
- Given `lint_touched.py` on touched C++ under Source/Tests/Tools/SkinEditor, when run, then it passes.

## Implementation Notes

## Spec Change Log

## Review Triage Log

- Blind: ValueBar Disabled unused by paint — verdict: `false` — Decisions intentionally seed Disabled for Track/ValueBar/Text; disabled paint skips fill by design, so the pastille is inventory-complete, not a dead wiring bug.
- Blind: baseSeed test does not assert ProgressBar == Slider — verdict: `medium` — name claims a copy but only checks absolute ColourChart stops; route: patch.
- Blind: DevSkin overlay asserts only ProgressBar Track K4 — verdict: `medium` — ValueBar/Text K4 overlays can drift without failing that suite; route: patch.
- Blind: no live-skin k1 emerald check after K4 overlay — verdict: `medium` — broken expand/overlay order could grey k1 while k4 tests pass; route: patch.
- Blind: Slider caret still bound to NumberBox EditorText — verdict: `low` — intentional Build A deferral in frozen Decisions; route: defer.
- Blind: caret tests bolted onto NumberBoxDisabledBackgroundColourTests — verdict: `low` — rejected (rename/move adds churn; filters already hit ProgressBarColourSet / NumberBoxDisabled).
- Blind: Spec Verification placeholders / empty Implementation Notes — verdict: `false` — rejected (fix would edit this build's spec).
- Blind: Skin Editor under-covers ProgressBar property rows × sets — verdict: `false` — `inventoryExpandsOnlyMultiSetObjects` already requires ProgressBar (via `kMultiSetSkinObjects`) to expose k1–k4 keys from the Dev inventory list.
- Blind: hard-coded ColourSet::k4 in dialog — verdict: `low` — rejected (named constant is YAGNI for a single dialog consumer).
- Blind: spec context points at gitignored `_local` Recap — verdict: `false` — rejected (intentional local vision input; frozen Intent already captures Build A).
- Edge Case Hunter: no findings — verdict: n/a.
- Verification Gap: ProgressBarColourSetK4Tests never observe BankTransferProgressDialog paint — verdict: `medium` (unverified paint-site adoption) — CONVENTIONS §8.5 forbids paint()/GUI unit tests; disposition defer; route: defer.

## Design Notes

Progress paint today is only `BankTransferProgressDialog::paintProgressBar` (plus muted phase label). Reuse property names `Track` / `ValueBar` / `Text` so Mappings vocabulary matches Slider bars without sharing the Slider object. Prefer direct `ISkin::getColour` at the dialog (no ProgressBarLook) — YAGNI until a second consumer appears.

NumberBox already disables the stock TextEditor caret and draws a custom blink rect with `look_.editorText`; mirror Slider’s `editorCaret` field shape on NumberBoxLook only.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds (or project’s usual preset).
- `ctest --preset macos-debug-arm64 -R 'SkinEditorMappings|SkinColour|DevSkin'` (adjust to available test names) -- expected: pass.
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++.

**Manual checks (if no CLI):**
- Skin Editor → Mappings → ProgressBar table + NumberBox Caret row; Bank Transfer progress bar colours follow ProgressBar/`k4`; NumberBox blink caret remappable independently of EditorText.
