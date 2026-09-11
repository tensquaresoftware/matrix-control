---
title: 'Matrix-1000 dual Unison (Keyboard Mode + Master override)'
type: 'feature'
created: '2026-09-11'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '29ec317f0a72da4002c0b244dd579930e3259bf7'
context:
  - '{project-root}/_local/References/Oberheim/oberheim-matrix-1000-owners-manual.md'
  - '{project-root}/_local/References/Oberheim/oberheim-matrix-1000-midi-sysex-implementation.md'
  - '{project-root}/_local/References/Oberheim/oberheim-matrix-6-6r-owners-manual.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Matrix-1000 has two independent Unison controls (patch Keyboard Mode and Master Unison). The plugin currently has inverted LEGATO PORTA graying, swapped Keyboard Mode choice order vs SysEx, no Master-override “M” badge, and no STRIG gating when Keyboard Mode is not UNISON — so the GUI misleads and can send wrong SysEx indices.

**Approach:** Keep patch Keyboard Mode (param 48) and Master Unison (octet 169) fully independent with no cross-write. Fix choice order and LEGATO polarity to match Oberheim docs, gate STRIG by Keyboard Mode only, and add a discrete Master-override “M” badge + footer on Matrix-1000 when Master Unison overrides a non-UNISON Keyboard Mode.

**Decisions (planning):**
- Keep the full spec in one delivery (token overrun accepted).
- STRIG gating = block STRIG only: when Keyboard Mode ≠ UNISON, non-STRIG trigger choices stay editable; STRIG must not be selectable/kept (disable popup item and/or auto-fallback + INFO footer). Inventory: Ramp 1/2 Trigger, Env 1/2/3 TRIGGER MODE, Lfo 1/2 TRIGGER MODE.

## Boundaries & Constraints

**Always:**
- Treat Keyboard Mode (patch param 48 / packed byte 8) and Master Unison (`miscUnisonEnable`, packed byte 169) as independent memory/SysEx values — never sync or rewrite one when the other changes.
- Keyboard Mode and Master Unison remain editable whenever their section is unlocked (Matrix-1000). No Master→Keyboard Mode cross-graying.
- LEGATO PORTA enabled only when Keyboard Mode = UNISON; grayed + footer otherwise (fix current inverted polarity).
- STRIG gated by Keyboard Mode (patch 48) only — never by Master Unison. When Keyboard Mode ≠ UNISON: block STRIG only (other trigger choices remain editable); when Keyboard Mode = UNISON: STRIG available again.
- Effective listening matrix (Master Unison × Keyboard Mode) is documentation for the badge/footer story only; do not invent a third “effective Unison” parameter.
- SysEx Keyboard Mode indices: 0=REASGN, 1=ROTATE, 2=UNISON, 3=REAROB.
- Badge “M” visible only when Master Unison = ON **and** Keyboard Mode ≠ UNISON; Matrix-1000 only (Matrix-6/6R: Master Edit already hidden — no badge / no Master Unison UI).
- Badge: discrete square, footer-badge spirit, bold “M”, smaller than KEYBOARD MODE label, fill = Master accent skin token, letter colour = body panel background token, sizes divisible by 4 (UI Scale). Place beside the fixed “KEYBOARD MODE” label (do not replace the label). No orange chrome on the combo.
- Click/hover badge → exact footer: `Master Unison is overriding Keyboard Mode.`
- Master Unison factory/init default remains OFF (already true).
- Dirty/undo: no special cross-domain rules beyond existing patch vs master separation.

**Never:**
- Re-open Matrix-6/6R Master Edit visibility.
- Hardcoded colours for the badge (use skin tokens).
- Global audit of all GUI↔SysEx combo encodings (deferred).
- Cross-linking LEGATO/STRIG enablement to Master Unison.
- Graying entire Ramp/Env/LFO trigger combos solely because Keyboard Mode ≠ UNISON.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Independent edit | Change Keyboard Mode or Master Unison | Only that domain’s APVTS/buffer/SysEx path updates; the other is unchanged | N/A |
| LEGATO enable | Keyboard Mode = UNISON | LEGATO PORTA combo enabled | N/A |
| LEGATO gray | Keyboard Mode ≠ UNISON | LEGATO PORTA combo disabled/grayed; click shows corrected INFO footer | N/A |
| STRIG blocked | Keyboard Mode ≠ UNISON; trigger combo that offers STRIG | Non-STRIG choices editable; STRIG not selectable; if current value was STRIG, auto-fallback away from STRIG + INFO footer | N/A |
| STRIG allowed | Keyboard Mode = UNISON | STRIG selectable again on those same combos | N/A |
| Badge show | Master Unison ON + Keyboard Mode ≠ UNISON (M-1000) | “M” badge beside KEYBOARD MODE label; hover/click → exact override footer | N/A |
| Badge hide | Master Unison OFF, or Keyboard Mode = UNISON, or Matrix-6/6R | No badge | N/A |
| Choice order | User picks REASGN / ROTATE / UNISON / REAROB | SysEx packed byte 8 = 0 / 1 / 2 / 3 respectively | N/A |
| Master Unison RT | Toggle Master UNISON | Byte 169 0/1 round-trips APVTS ↔ MasterModel ↔ full 0x03 SysEx | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDescriptorsPatchEditFmRamp.cpp` — `kPortamentoKeyboardMode` choices currently ROTATE/REASGN/… (swap first two); `kPortamentoLegato`; Ramp1/2 Trigger include STRIG; `defaultIndex` 1 stays (InitDefaults byte 8 = `0x01` = ROTATE after fix).
- `Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp` — `kUnisonEnable` sysExOffset 169; defaultIndex 0 OFF — verify only, do not change offset.
- `Source/Shared/Definitions/PluginDisplayNames.h` — Keyboard Mode / LEGATO labels; `kLegatoPortaUnisonBlockedFooter` currently inverted; add Master-override footer string + STRIG footer if needed.
- `Source/Shared/Definitions/PluginIDs.h` — `miscUnisonEnable`, `rampPortamentoPortamentoKeyboardMode` — do not rename.
- `Source/GUI/Panels/.../RampPortamentoPanel.{h,cpp}` — `kUnisonKeyboardModeIndex = 2`; `refreshLegatoPortaGraying()` currently grays **when** UNISON (invert); host badge + Ramp trigger STRIG-only block.
- `Source/GUI/Panels/.../Env{1,2,3}Panel.cpp` + `Lfo` panels — TRIGGER MODE combos exposing STRIG; wire Keyboard Mode listener; block STRIG only (not whole combo).
- `Source/GUI/Helpers/GrayedControlHelper.*` — reuse grayed-click + footer INFO pattern; do not invent a second footer API.
- `Source/GUI/Panels/.../FooterPanel.cpp` — `paintBadgeAndDetail` spirit for “M” geometry (do not fork footer severity colours for this badge).
- `Source/GUI/Widgets/ParameterCell.*` — label is private; badge likely sibling overlay in `RampPortamentoPanel` near Keyboard Mode cell, or a small additive API — prefer minimal surface.
- `Source/GUI/Skins/SkinColoursWidgetsLayout.h` / `SkinColoursPanels.h` — Master orange (`kSectionHeaderLineOrange` / `ColourChart::kOrange`) + `kBodyPanelBackground` for badge fill/letter.
- `Source/Core/MIDI/MasterEditGate.h` — Matrix-1000-only Master Edit — do not broaden.
- `Source/Core/Models/ApvtsMasterMapper.*` + `MasterModel.*` + `MasterParameterSysExDispatcher` — generic choice path already covers Unison Enable; verify round-trip, add targeted test if cheap.
- `Source/Core/Init/InitDefaults.cpp` — patch[8]=0x01, master[169]=0x00 — keep.
- `_local/References/Oberheim/*` — param 48 table; Master Unison override wording; Matrix-6 LEGATO/STRIG “only in UNISON”.

**Do not change:** Master Edit hide for Matrix-6/6R; cross-domain dirty/undo architecture; unrelated combo encodings.

## Tasks & Acceptance

**Execution:**
- [x] `PluginDescriptorsPatchEditFmRamp.cpp` -- Reorder Keyboard Mode choices to REASGN, ROTATE, UNISON, REAROB; keep `defaultIndex` 1 -- Align UI index with SysEx.
- [x] `PluginDisplayNames.h` -- Fix LEGATO blocked footer polarity; add Master override footer string (exact); add STRIG-blocked INFO footer -- SSOT English copy.
- [x] `RampPortamentoPanel.{h,cpp}` -- Invert LEGATO gray polarity; add “M” badge (skin tokens, UI Scale ÷4); listen Master Unison + Keyboard Mode for badge; block STRIG only on Ramp 1/2 Trigger -- Primary UX surface.
- [x] Env1/2/3 + Lfo1/2 panels (and shared helper if 2nd copy) -- Same Keyboard Mode→block-STRIG-only behaviour as Ramp -- Complete STRIG inventory.
- [x] Skin colour IDs / chart if needed -- Expose or reuse Master accent + body bg for badge without hardcoding -- Token policy.
- [x] Master Unison path (`ApvtsMasterMapper` / test) -- Confirm octet 169 round-trip; add unit assert if missing and cheap -- Mandatory tech check #3.
- [x] `deferred-work.md` -- Note global GUI↔SysEx combo audit as deferred -- Hors scope capture.

**Acceptance Criteria:**
- Given Matrix-1000 with Master Unison OFF and Keyboard Mode UNISON, when user edits LEGATO PORTA, then the control is enabled and sends patch SysEx without touching master byte 169.
- Given Keyboard Mode ≠ UNISON, when user views LEGATO PORTA, then it is grayed and click shows a footer stating LEGATO is only available when KEYBOARD MODE is UNISON.
- Given Master Unison ON and Keyboard Mode ROTATE (or REASGN/REAROB), when UI paints Keyboard Mode, then badge “M” appears beside the label and hover/click shows `Master Unison is overriding Keyboard Mode.`
- Given Master Unison OFF or Keyboard Mode UNISON, when UI paints Keyboard Mode, then no “M” badge.
- Given user selects Keyboard Mode choices in order REASGN→ROTATE→UNISON→REAROB, when SysEx packed byte 8 is inspected, then values are 0,1,2,3.
- Given Master Unison toggled OFF↔ON, when master buffer/SysEx 0x03 is inspected, then byte 169 is 0↔1 and Keyboard Mode is unchanged.
- Given Matrix-6/6R, when Patch Edit is shown, then no Master Unison badge appears (Master Edit remains hidden).
- Given Keyboard Mode ≠ UNISON, when using any inventoried Ramp/Env/LFO trigger combo that offers STRIG, then STRIG is not selectable (and is cleared if it was current), other choices remain editable, and gating does not depend on Master Unison.

## Implementation Notes

- Keyboard Mode choice order corrected to REASGN/ROTATE/UNISON/REAROB (`defaultIndex` 1 = ROTATE / SysEx `0x01`).
- LEGATO PORTA gray polarity inverted (enabled only in UNISON); footer copy corrected.
- Shared `UnisonKeyboardModePolicy` (header-only) holds LEGATO/STRIG/badge predicates for Core-testable coverage; `StrigUnisonGateHelper` disables STRIG popup items when Keyboard Mode ≠ UNISON and clears STRIG only when Keyboard Mode *leaves* UNISON. Popup renderer/select honor `juce::ComboBox::isItemEnabled`.
- Matrix-1000 “M” badge on Ramp/Portamento Keyboard Mode label uses `kSectionHeaderLineOrange` fill + `kBodyPanelBackground` letter; 12×12 design (÷4).
- `ApvtsMasterMapperTests` adds Unison Enable octet 169 OFF/ON round-trip; `UnisonKeyboardModePolicyTests` covers matrix I/O predicates + choice order + domain independence.

## Spec Change Log

## Review Triage Log

- false — Blind: Keyboard Mode 0/1 “migration needed”: indices always mapped to SysEx values; only labels were swapped, so sessions now display the label that matched the wire value.
- false — Blind/Edge/Verification: STRIG sticky on construct when ≠ UNISON: frozen Decisions allow disable popup item and/or auto-fallback; implementation disables always and clears on leaving UNISON.
- patch — Verification: STRIG descriptor indices not pinned vs panel constants — fixed by `inventoriedTriggerStrigChoiceIndices` tests.
- patch — Blind: null `skin_` deref in `layoutMasterOverrideBadge` — early return added.
- patch — Blind: unused `SkinValues.h` include — removed.
- patch — Blind: `isStrigAllowed` unused in production gate — `StrigUnisonGateHelper` now uses it.
- low (rejected) — Blind: hardcoded cell indices — existing module-panel pattern; layout drift unlikely in daily use and fix would add brittle layout asserts.
- low (rejected) — Blind: Master Unison ON as bare `== 1` — matches OFF/ON descriptor; no named harm beyond style.
- low (rejected) — Blind: STRIG footer string under RampPortamentoModule used by Env/LFO — works; relocating is cleanup not defect.
- false — Blind: empty Spec Change Log / Review Triage Log at in-review — process artifact, not a product defect; triage now filled.
- false — Blind: Code Map/Design Notes still describing pre-fix state — rejected (must not edit this build’s spec to “fix” narrative Code Map during review).
- defer — Verification: no automated test for `StrigUnisonGateHelper` ComboBox disable/clear path — GUI helper outside Core test harness; manual GUI checks remain.
- defer — Verification: no automated test for PopupMenu `isItemEnabled` select gate — custom popup GUI; manual verification.
- defer — Blind: no assert on full Master 0x03 SysEx encode for Unison — mapper byte-169 covered; encode path is shared generic dispatcher.
- maybe-false (rejected as low) — Edge: APVTS listener off message thread — no evidence this project fires Keyboard Mode changes off the message thread.
- maybe-false (rejected as low) — Edge: combo `getNumItems() <= strigChoiceIndex` early return — only before items populated; refresh runs after widgets exist.
- low (rejected) — Blind: six KeyboardModeChangeListeners — intentional per-module wiring; undo batching not required by intent.
- low (rejected) — Blind: badge stale on full APVTS state replace — device-type property listener covers connected-device changes; full replace unproven.

## Design Notes

**Effective listening (docs only — do not encode as a third param):**

| Master Unison | Keyboard Mode | Heard |
| ------------- | ------------- | ----- |
| OFF | other | Poly |
| OFF | UNISON | Patch Unison |
| ON | other | Master override Unison |
| ON | UNISON | Unison |

**LEGATO polarity (Oberheim Matrix-6/6R owners manual, applied to shared patch architecture):** LEGATO PORTAMENTO only works when Keyboard Mode is UNISON; polyphonic modes disable it. Current code grays when index==UNISON — invert to gray when index≠UNISON.

**Badge:** Prefer a small child component owned by `RampPortamentoPanel` positioned next to the Keyboard Mode label bounds (avoid orange combo border). Dimensions: pick design sizes divisible by 4 before scale.

**STRIG:** Prefer shared helper over copy-paste across seven panels. Prefer disabling the STRIG popup item when possible; if the custom ComboBox lacks per-item disable, auto-fallback from STRIG to the first non-STRIG choice when Keyboard Mode leaves UNISON, with INFO footer explaining STRIG needs UNISON.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64` (or project’s usual test invoke for Master mapper if present) -- expected: relevant tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under Source/

**Manual checks (if no CLI):**
- Matrix-1000: toggle Master Unison vs Keyboard Mode independently; confirm badge matrix and LEGATO/STRIG gating; confirm no orange combo chrome.
- Matrix-6/6R: Master Edit hidden; no “M” badge.
