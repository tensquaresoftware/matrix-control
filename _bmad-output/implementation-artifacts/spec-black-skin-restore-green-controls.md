---
title: 'BLACK skin restore green Master Header Latency'
type: 'bugfix'
created: '2026-11-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '5f3ea1060c804f6f98fbda26539931be2553f035'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On the classic BLACK product skin, Master Edit sliders/combos, the header Input Gain slider, and the plugin Settings Hardware Latency slider paint monochrome greys instead of the Patch emerald greens they previously shared.

**Approach:** Retint only BLACK (`blackVariant`) enabled accent seeds for Master (`k2`), Header (`k3`), and Modales slider (`k4`) so those controls match Patch emerald stops; leave BLACK/DUO and CREAM unchanged.

</frozen-after-approval>

## Implementation Notes

- Retinted BLACK `blackVariant` enabled accents only: SliderK2/K3/K4 + ComboBoxK2 + PopupMenuK2 to match Patch (`Slider` / `ComboBox` / `PopupMenu`) emerald stops.
- Left BLACK/DUO and CREAM variants untouched; Settings Modales combos (ComboBoxK3) stay monochrome on BLACK.
- Hardware Latency stays on Modales (`k4`); greening that set on BLACK restores the latency slider without call-site rewiring.
- Added `SliderZoneSeedTests` pin `black_masterHeaderModalesSharePatchEmerald` (sliders + ComboBoxK2/PopupMenuK2) and run Modales look for Black; lint OK; SliderZoneSeed + SkinColourWiringCoverage tests pass.
- Blind Hunter: patched missing combo/popup pins and Black Modales look coverage; rejected hover/disabled popup retints and TextDisabled emerald as intentional Patch parity; rejected SliderK4 “all Modales” scare because Hardware Latency is the only product Modales slider call site.
- Course correction (Guillaume): keep Master/Header emerald; revert SliderK4 BLACK to mono greys; Modales LedK2 + PeakIndicatorK2 MaxOn BLACK → Grey200 (match BLACK/DUO Settings mono).

## Review Triage Log

- ComboBoxK2/PopupMenuK2 untested — medium → patched (seed equality pins added).
- PopupMenu hover/disabled beyond enabled accents — false (Patch popup parity requires those roles for Master combos).
- assertModalesLook skipped Black — medium → patched (Black variant exercised).
- SliderK4 greening all Modales sliders — false (only Hardware Latency uses Modales slider look in product).
- Test name overstates coverage — low → patched by expanding combo/popup pins.
- No blackDuo/cream untouched pins for ComboBoxK2 — low rejected (Slider duo/cream pins already guard the zone matrix; mistaken three-slot writes on selection seeds are low risk this change).
- PopupMenuK2 TextDisabled Common→Emerald700 — false (matches Patch PopupMenu disabled text).
- Spec status still in-progress — process → finalized to done.
