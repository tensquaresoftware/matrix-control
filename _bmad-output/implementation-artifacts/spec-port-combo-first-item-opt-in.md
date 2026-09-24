---
title: 'Port combo first-item popup chrome opt-in'
type: 'bugfix'
created: '2026-09-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** ButtonLike combo popups always treat row 0 as a port sentinel (bottom rule + shorter hover fill) whenever the first item id is 1. That was meant only for MIDI/audio port lists with NO INPUT / NO OUTPUT. Computer Patches (and EPROM TYPE) also use item id 1 for a normal first row, so they incorrectly get the port chrome.

**Approach:** Add an opt-in on `TSS::ComboBox` for port-sentinel popup chrome (default off). Gate `ScrollablePopupMenu` drawing on that flag. Enable it only for header MIDI/audio port combos and Device Startup MIDI port combos (via existing port population helpers / audio rebuild).

</frozen-after-approval>

## Implementation Notes

- API: `ComboBox::setUsesPortSentinelPopupChrome` / `usesPortSentinelPopupChrome` (default false).
- Gate: `ScrollablePopupMenu` draws chrome from the ComboBox flag alone (row 0 + more than one item); no dependency on `MidiPortComboPopulation`.
- Enable: `populatePortCombo` (header MIDI / Keyboard From / Device Setup MIDI) and `HeaderPanel::populateAudioFromCombo`.
- Clear flag in `configurePluginKeyboardFrom` (plugin HOST row is not a port sentinel).
- Computer Patches and EPROM TYPE stay default-off.
- Manual check: open Computer Patches → first row hover should be full height, no rule; open MIDI From → NO INPUT keeps rule + shorter hover.
- Review: decoupled popup paint from MIDI helper (flag-as-contract). Deferred GUI unit coverage and audio/MIDI enablement helper.

## Review Triage Log

- Empty Implementation Notes / status still in-progress — medium — patched in finalize.
- No regression tests for the flag contract — medium — deferred (GUI harness vs unit-suite convention).
- Dual gate (flag + item id 1) under-documented — low — patched ComboBox comment.
- Local magic `kPortSentinelItemId = 1` in ScrollablePopupMenu — medium — patched to `MidiPortComboPopulation::kPortSentinelItemId`.
- Sticky flag after Standalone→Plugin Keyboard From — medium — patched `configurePluginKeyboardFrom` clears flag (chrome was already inert via id/count).
- API silent about Standard/multi-column no-op — low — patched comment.
- Frozen Intent omits Keyboard From by name — false — covered by “header MIDI” + populateInputPortCombo enablement.
- Audio one-off enable vs MIDI helper — low — deferred (YAGNI).
- Missing ButtonLike+id-1 opt-out inventory in spec context — low — rejected (notes list Computer Patches / EPROM TYPE; no further inventory needed for this ship).

### Review Findings

- [x] [Review][Patch] Decouple ScrollablePopupMenu from MidiPortComboPopulation — gate chrome on ComboBox opt-in only (row 0 + multi-item); remove MIDI helper include; align ComboBox API comment with flag-as-contract (consumers enable for port lists) [ScrollablePopupMenu.cpp / ComboBox.h]
- [x] [Review][Defer] Dual HeaderPanel vs MidiPortComboPopulation sentinel id constants — deferred: pre-existing parallel `kPortSentinelItemId = 1` in HeaderPanel; paint now uses the MIDI helper constant; consolidate later if Audio From migrates off the HeaderPanel local.
- Decision resolved (2026-09-25): option 2 — flag on ComboBox is the drawing contract; no widget→MIDI-helper coupling.
- Rejected: sticky flag after `clear()` without Keyboard From clear — false: product call sites re-enable via populate helpers; Standalone→Plugin Keyboard From clears the flag before rebuilding HOST.
- Rejected: API comment still says bare “item id is still 1” — low: comment remains true (`kPortSentinelItemId == 1`); rename-only doc polish not worth a patch this pass.
- Rejected: asymmetric Audio From one-off enable — already deferred this ship (YAGNI shared helper).
- Rejected: no automated flag regression — already deferred this ship (GUI harness vs unit-suite convention).
- Rejected: expand manual UAT checklist — process advice, not a code defect; smoke already covered Computer Patches vs MIDI/audio ports.
