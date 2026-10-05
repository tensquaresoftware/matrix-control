---
title: 'Settings AUDIO channel pairs as combos'
type: 'feature'
created: '2026-10-05'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Exclusive stereo INPUT/OUTPUT channel pairs use wrapping RadioButtonGroups that force a tall Settings shell and do not scale cleanly to high channel counts.

**Approach:** Replace both channel RadioButtonGroups with Matrix ComboBoxes (one row each). Shrink Settings body height so it closes under PLAY TEST TONE with normal content padding. Keep exclusive stereo-pair apply semantics and remove the unused radio widget once nothing references it.

</frozen-after-approval>

## Implementation Notes

- Agent decisions: keep `RadioButtonGroupLayout` stereo-pair helpers (mask/count/label/selection) used by `AudioDeviceSetupSync`; revert temporary zero-padded labels and radio width/gap trials; channel combo items use `1 + 2` style; empty device → empty combo with no selection; drop wrap-height unit tests tied to radio layout, keep mask encode/decode tests; `kTallestPageRows = 11` (5 device rows + blank + INPUT CHANNELS + SYNTH FROM + blank + OUTPUT CHANNELS + PLAY TEST TONE).
- Replaced `inputChannelsGroup_` / `outputChannelsGroup_` with Matrix `ComboBox`es; layout uses the same labeled row helper as other AUDIO combos.
- Deleted `RadioButtonGroup.{h,cpp}` and removed from `CMakeLists.txt`; slimmed `RadioButtonGroupLayout.h` to stereo helpers only.
- Review patches: disable channel combos when pairCount == 0; assert `kTallestPageRows == 11` in SettingsTabsShellTests.
- Build Standalone + `Matrix-Control_Tests` OK; SettingsTabsShell tests 0 failures; lint LIGHT only on pre-existing HeaderLayoutMetrics length.
- Commit skipped: standing user rule requires an explicit commit request.

## Review Triage Log

- Empty channel combos stay enabled — **medium / patch**: `setEnabled(pairCount > 0)` in `refreshChannelCombos`.
- Non-exclusive live mask leaves combo blank — **false**: same as former RadioButtonGroup `selectedIndex == -1`; healing would silently rewrite audio.
- Channel combos lack ComboBoxLiveRefresh — **medium / defer**: rare hotplug-while-open; deferred.
- `applyChannelPair` no clamp — **low / rejected**: UI only emits ids from populated items.
- `RadioButtonGroupLayout` name leftover — **low / defer**: rename cleanup.
- No `kTallestPageRows == 11` assert — **low / patch**: added in SettingsTabsShellTests.
- EDIT cartouche design-check gaps — **low / defer**: prior header polish, not caused by combo cutover.
- Header label truncation TBD note — **low / rejected**: prior polish, not this delivery.
- ColourChart hardcodes for cartouche/rule — **low / rejected**: prior polish iteration, not this delivery.
- Spec/docs lag RadioButtonGroup narrative — **low / patch+defer**: this spec marked done; older plans deferred for docs pass.

### Review Findings

- [x] [Review][Patch] Channel-pair ComboBox item-id ↔ pair-index mapping has no unit coverage — extract tiny pure helpers and extend SettingsTabsShellTests so a wrong offset cannot ship while mask/apply tests still pass [`SettingsAudioPageDevices.cpp` / `SettingsAudioPage.h`]
- [x] [Review][Patch] DesignChecks miss EDIT cartouche inset and cluster nudge ÷4 asserts; cartouche-gap message still says MIDI-AUDIO [`DesignChecks.h`]
- [x] [Review][Patch] `HeaderLayoutMetrics::rightPaddingPx` is computed but unread after the right-anchored action cluster was removed [`HeaderPanelLayout.cpp`]
- [x] [Review][Defer] `kTallestPageRows == 11` assert pins the constant, not AUDIO layout fit [`SettingsTabsShellTests.cpp`] — deferred: GUI/harness work; Standalone smoke remains the fit check
- [x] [Review][Defer] Monitoring label widths still marked truncation TBD [`DesignPanels.h`] — deferred: prior header polish; measure/clip/tooltip follow-up
- [x] [Review][Defer] `kCartoucheClusterNudgeX` scaled ad hoc in resized, not via HeaderPanelDimensions [`HeaderPanelLayout.cpp`] — deferred: SSOT polish, behaviour already correct
- [x] [Review][Defer] `RadioButtonGroupLayout` name leftover after widget deletion — deferred: already logged 2026-10-05
- [x] [Review][Defer] Channel combos lack ComboBoxLiveRefresh — deferred: already logged 2026-10-05
- [x] [Review][Defer] Older plans still document RadioButtonGroup channel UI — deferred: already logged 2026-10-05

#### Rejected

- Header polish bundled vs combo-only Intent — **false**: this review explicitly scopes the full uncommitted working tree (AUDIO + header)
- Plugin mode leaves a gap where AUDIO would sit — **false**: EDIT/MIDI keep Standalone X by probing the full cluster; intentional host parity
- ColourChart hardcodes for cartouche/rule — **false**: product decision already accepted for this pass
- Logo ink centering only on Y — **false**: vertical ink centering was the accepted behaviour; box centering is separate
- Logo paint with empty local bounds — **low**: pathological zero-size; not everyday
- Cartouche cluster can draw past header edge — **false**: fixed GUI width + design tokens; no demonstrated overflow at current scales
- Claim `kCartoucheClusterNudgeX = 12` is off ÷4 grid — **false**: `12 % 4 == 0`
- Unused `PacketPlacer::endPacket` — **low**: private leftover; unlikely everyday harm
- `endPacket` / docs / rename noise already covered above or out of requested scope
