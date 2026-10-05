---
title: 'Settings AUDIO tab'
type: 'feature'
created: '2026-10-04'
status: 'done'
route: 'dispatch'
baseline_commit: '7e01242659a0c37215fcf33697900117265d9a95'
review_loop_iteration: 0
context:
  - '{project-root}/Documentation/Development/Plans/2026/10/2026-10-04-Unified-Settings-Window-And-Header.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-tabs-shell.md'
  - '{project-root}/_bmad-output/implementation-artifacts/guide-matrix-modal-design.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Standalone audio still lives in a separate JUCE-looking Audio/MIDI window and menu, while Settings already has the Ableton tab shell. Delivery 2 must put audio wiring in Settings and stop the second door.

**Approach:** Add a standalone-only AUDIO tab with Matrix chrome and a new RadioButtonGroup for exclusive stereo pairs. Retire Audio/MIDI menu, shortcut, and window. Move listen-source (SYNTH FROM) out of the header; keep gain + peak in a framed AUDIO cartouche. Plugin host audio wiring stays unchanged (no AUDIO tab).

## Boundaries & Constraints

**Always:**
- Reuse delivery-1 shell contracts: `SettingsTabRail`, `SettingsShellMetrics`, overlay chrome, integer scaled metrics, last-tab persist key `settingsLastTab`, two content columns (labels left / controls right, never mixed).
- Standalone tab order: USER INTERFACE, DEVICE, AUDIO, PATCH, PATCH MUTATOR, MASTER. Plugin: same without AUDIO.
- AUDIO page rows (order): DRIVER TYPE → INPUT DEVICE → OUTPUT DEVICE → SAMPLE RATE → BUFFER SIZE → blank → INPUT CHANNELS (RadioButtonGroup) → SYNTH FROM + peak (shorter combo) → blank → OUTPUT CHANNELS (RadioButtonGroup) → PLAY TEST TONE (control column only, no left label).
- ASIO: keep both INPUT DEVICE and OUTPUT DEVICE visible and linked (same device both sides). Never collapse to one "Device" row by OS.
- Screen copy: SYNTH FROM, INPUT CHANNELS, OUTPUT CHANNELS, PLAY TEST TONE, INPUT GAIN — no repeated AUDIO inside row labels. ASCII display strings.
- Gain only in header; peak is a meter (header + right of SYNTH FROM in Settings). Preserve Core preferred-setup / profiles / scene-safety (no silent AUDIO FROM remap).
- Invalid or plugin-stale last-tab ids coerce to first tab. Missing property stays unset and defaults to USER INTERFACE.
- Alt/Option+logo does nothing after cutover: remove the Audio/MIDI gesture and its dead code (callback, pending flag, wiring). Shift+logo and Settings... remain the Settings doors.

**Never:**
- MIDI tab, MIDI header combo removal, MIDI cartouche, FROM KEYBOARD / FROM SYNTH / TO SYNTH renames (delivery 3).
- First-launch assistant; About / Skin / UI Scale inside Settings; plugin AUDIO tab (even empty); host audio rewiring in plugin.
- Keep a second Audio/MIDI menu item, Cmd/Ctrl+Alt+, shortcut, or separate AudioMidi Settings overlay after cutover.
- Repurpose Alt/Option+logo to open Settings or jump to AUDIO — delete that path instead.
- Round radio widgets; French UI strings; global AffineTransform on Settings; Core→GUI dependencies.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Standalone Settings | Open Settings | AUDIO tab present after DEVICE | N/A |
| Plugin Settings | Open Settings | No AUDIO tab; five other tabs unchanged | N/A |
| Last tab AUDIO | Close on AUDIO, reopen standalone | AUDIO restored | N/A |
| Stale AUDIO id in plugin | Stored last-tab = AUDIO | First tab; coerce property | Ignore invalid for host |
| Channel pairs | Device with 6 inputs | Radio pairs 1+2 / 3+4 / 5+6; one selected | N/A |
| Many pairs | Wider than one control row | Wrap inside control column only | N/A |
| ASIO device change | Change INPUT or OUTPUT list | Other list follows same device | N/A |
| Logo menu | Open logo menu (standalone) | Settings... only for prefs; no Audio/MIDI... | N/A |
| Header AUDIO | Standalone header | No Audio From; cartouche AUDIO + INPUT GAIN + peak | N/A |
| Profile restore | Device identity change | Preferred/profile path still applies | Keep restoringSetup guard |

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsShellMetrics.h`, `SettingsTabRail.*`, `SettingsWindow.*`, `SettingsPanel.*`, `SettingsPanelSetup.cpp` -- extend host-aware tab list/labels/height; AUDIO page layout with blanks + wrap; keep rail/rule/last-tab helpers.
- `Source/Shared/Definitions/PluginIDs.h` (`LastTab`), `PluginDisplayNames.h` -- AUDIO tab id/label; SYNTH FROM / channel / PLAY TEST TONE / AUDIO cartouche copy; drop Audio/MIDI menu copy usage.
- New `Source/GUI/Widgets/RadioButtonGroup.{h,cpp}` (+ CMake) -- square Matrix exclusive options; multi-per-line wrap in a given width; Look from skin (reuse square Button/Toggle paint language).
- `Source/GUI/Dialogs/AudioMidiSettingsWindow.*` -- retire overlay UI; migrate preferred-setup/profile sync + test-tone/peak wiring into Settings AUDIO page (or a small non-window helper under Settings/Audio). Do not regress `AudioDevicePreferredSetup` / `AudioDeviceProfiles` / `SceneAudioSafety`.
- `Source/GUI/PluginEditorAudio.cpp`, `PluginEditorWindows.cpp`, `PluginEditor.cpp`, `StandaloneAudioInputRouter*` -- single Settings entry; remove open/close AudioMidi window path; mutual-close with Settings/About stays Settings-only.
- `Source/GUI/Widgets/Logo.cpp`, `HeaderLogoPopupMenu*`, `EditorChromeShortcuts.h` -- remove Audio/MIDI menu item + Cmd/Ctrl+Alt+, ; delete Alt/Option+logo Audio/MIDI gesture and dead code.
- `Source/GUI/Panels/MainComponent/HeaderPanel/*`, `DesignPanels.h` / dimensions -- remove Audio From packet; paint AUDIO cartouche around INPUT GAIN + peak; keep MIDI packets untouched.
- Header→Settings move: `populateAudioFromCombo` / `wireAudioFromComboChange` / APVTS `audioFromSourceId` -- relocate combo to AUDIO page as SYNTH FROM (shorter width + peak); Core sync unchanged.
- `Tests/Unit/SettingsTabsShellTests.cpp` (+ pure RadioButtonGroup / last-tab host coerce tests as policy allows) -- pin standalone AUDIO presence, plugin absence, coerce, wrap policy if extractable.
- Do not change: Core MIDI, plugin host buses, delivery-3 header MIDI lists, first-launch assistant.

## Tasks & Acceptance

**Execution:**
- [x] `PluginIDs.h` / `PluginDisplayNames.h` -- AUDIO tab + labels; host-aware LastTab normalize
- [x] `RadioButtonGroup.*` + `CMakeLists.txt` -- exclusive square pairs with wrap
- [x] `SettingsShellMetrics` / `SettingsTabRail` / `SettingsWindow` / `SettingsPanel*` -- AUDIO page + taller/wider shell as needed for rows/wrap
- [x] Settings AUDIO wiring -- devices/SR/buffer/channels/SYNTH FROM/peak/test tone; migrate profile sync; delete AudioMidi overlay lifecycle
- [x] Logo menu / shortcuts / Logo Alt gesture -- single Settings door (per frozen Alt decision)
- [x] `HeaderPanel*` -- remove Audio From; AUDIO cartouche + INPUT GAIN + peak; update gain help to Settings > AUDIO > SYNTH FROM
- [x] Unit tests -- tab host policy + coerce; RadioButtonGroup pure logic if extracted
- [x] Build + lint + smoke -- standalone AUDIO + single door; plugin no AUDIO

**Acceptance Criteria:**
- Given standalone Settings, when AUDIO is selected, then Matrix rows match the frozen order and channel pairs use RadioButtonGroup (not JUCE checkboxes).
- Given plugin Settings, when opened, then AUDIO is absent and host audio wiring is untouched.
- Given logo menu / chrome shortcuts, when used, then only Settings... opens prefs (no Audio/MIDI... / no second shortcut).
- Given standalone header, when shown, then Audio From is gone and gain + peak sit in an AUDIO cartouche; MIDI lists unchanged.
- Given ASIO, when either device list changes, then both lists stay visible and show the same linked device.
- Given UI Scale 50 / 100 / 125 / 150 %, when Settings AUDIO is open, then integer metrics hold and wraps stay in the control column.

## Implementation Notes

- Host-aware `LastTab` ids: AUDIO=3; PATCH/MUTATOR/MASTER renumbered; plugin omits AUDIO via `isValid`/`idAt`.
- New `SettingsAudioPage` + `AudioDeviceSetupSync` (profile/preferred migrate from deleted `AudioMidiSettingsWindow`).
- `RadioButtonGroup` + `RadioButtonGroupLayout` wrap helpers; control column widened to 200 design px (shell 504).
- Single Settings door: removed Audio/MIDI window, menu item, Cmd/Ctrl+Alt+,, Alt/Option+logo gesture.
- Header: Audio From removed; AUDIO cartouche around INPUT GAIN + peak; SYNTH FROM lives on Settings AUDIO.
- Dead-code note: Core `showAudioMidiSettingsDialog` API remains as no-op without editor handler (no Settings reopen path).
- Matrix audit extras: ASIO `linkAsioDeviceNames` header-inline; AUDIO last-tab restore + product-copy asserts in SettingsTabsShellTests.
- Review patches: clear `useDefault*Channels` on apply; empty mask → no radio selection; shell height 14; NONE sentinel; orphan device combo item; ASIO None clears both; monitoring start/stop on Settings open/close; attach retry; pair-mask pure tests; retired dead selector-policy test.

## Spec Change Log

## Review Triage Log

- medium — Blind: shell height can clip when both channel groups wrap (`kTallestPageRows = 12`). Real: two wrapped channel rows need ~13–14 budget rows.
- low rejected — Blind: hardcoded `"None"` casing. Real but cosmetic; use `PluginDisplayNames` NONE sentinel in patch group.
- high — Blind/VG/Edge: `applyChannelPair` omits `useDefaultInputChannels`/`useDefaultOutputChannels = false`. Real: Core None policies clear these; JUCE ignores bitmasks while defaults stay true.
- medium — Blind/Edge: missing scanned device name maps combo to None via `indexOf + 2` while setup keeps the name. Real mismatch on refresh.
- low rejected — Blind: ignored `setAudioDeviceSetup` errors. Pre-existing pattern; user-facing error UX needs product copy (not a trivial patch).
- medium — Blind: Settings hide leaves AUDIO page timer/ChangeListener running. Real: close only `setVisible(false)`.
- defer — Blind: `scanForDevices()` on every refresh. Possible stall; needs driver smoke before aggressive caching.
- low rejected — Blind: `enableInputMonitoring()` on first Settings construct. Same unmute intent as old Audio/MIDI open.
- medium — Blind/Edge: null `getAudioDeviceManager()` on first attach skips AUDIO page with no retry. Real session hole.
- defer — Blind: Core `showAudioMidiSettingsDialog` remains no-op API. Harmless leftover; broader Core cleanup later.
- medium — Blind/Edge/VG: `selectedPairIndex` falls back to 0 when mask empty. Real: radios look selected while no channels open.
- low rejected — Blind: magic 108/12/8/140 widths. Already localized in layout metrics; renaming adds noise without behavior change.
- false — Edge: odd mono channel option. Frozen intent is stereo pairs only; no mono at this level.
- false — Edge: LastTab renumber remaps delivery-1 ids. Design Notes accepted for unreleased app; no migration flag.
- medium — Edge: ASIO None on one side rewritten to the other device via `linkAsioDeviceNames`. Real when preferred side is empty.
- false — Edge: channel pairs beyond old bus maxima. New intent uses live device pair counts, not retired selector max.
- patch — VG: pairMask/selectedPairIndex have no pure tests. Real verification gap.
- patch — VG: orphaned `matrixAudioMidiSettingsSelectorPolicy` test no longer guards AUDIO UI. Real broken-verification.
- defer — VG: `headerAudioProductCopy` only checks string constants. Misleading name; GUI menu structure stays smoke/manual per policy.

## Design Notes

Renumber `LastTab` so rail order matches product order with AUDIO after DEVICE (MIDI slot arrives in delivery 3). App not shipped — persisted last-tab from delivery 1 may coerce once; acceptable.

Prefer custom Matrix combos + RadioButtonGroup driving `AudioDeviceManager` over restyling `AudioDeviceSelectorComponent`. Keep profile/preferred-setup logic as a ChangeListener helper owned by the AUDIO page, with the same `restoringSetup_` reentrancy guard.

Widen content column only as far as needed so three short pairs fit one row at 100%; further pairs wrap in the control column. SYNTH FROM combo shorter than device rows so combo + peak match upper row width.

Header AUDIO cartouche: filled left badge "AUDIO", hairlines top/bottom continuing across INPUT GAIN + peak, vertical close on the right; interior stays header background (plan sketch). Do not implement MIDI cartouche yet.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 && cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- unit test binary for SettingsTabsShell / new pure tests -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks (if no CLI):**
- Standalone: Settings AUDIO functional; no Audio/MIDI menu; header cartouche; profiles still restore.
- Plugin: no AUDIO tab; header MIDI unchanged; no audio Settings page.

### Review Findings

- [x] [Review][Patch] Monitoring only while AUDIO tab is visible (decision: option 1) — remove force-on in `showReadySettingsWindow`; keep `setVisible` + Settings close as the sole start/stop; sync preferred on show [`Source/GUI/PluginEditorSettingsOverlay.cpp:86`]
- [x] [Review][Patch] UI device/channel applies skip preferred-setup and profile sync [`Source/GUI/Settings/SettingsAudioPageDevices.cpp:191`]
- [x] [Review][Patch] Late AUDIO attach does not re-register contextual help [`Source/GUI/PluginEditorSettingsOverlay.cpp:69`]
- [x] [Review][Patch] `selectedStereoPairIndex` treats any set bit as the pair [`Source/GUI/Widgets/RadioButtonGroupLayout.h:89`]
- [x] [Review][Patch] `applyChannelPair` does not refresh UI after `setAudioDeviceSetup` [`Source/GUI/Settings/SettingsAudioPageDevices.cpp:197`]
- [x] [Review][Patch] Device change keeps prior channel masks without revalidation [`Source/GUI/Settings/SettingsAudioPageDevices.cpp:163`]
- [x] [Review][Patch] Shell height budget can clip when both channel groups wrap to three rows [`Source/GUI/Settings/SettingsShellMetrics.h:28`]
- [x] [Review][Patch] Logo Settings help copy omits standalone AUDIO wiring [`Source/Shared/Definitions/PluginDisplayNames.h:52`]
- [x] [Review][Patch] No unit test that channel-pair apply clears `useDefault*Channels` [`Tests/Unit/SettingsTabsShellTests.cpp`]
- [x] [Review][Patch] ASIO link verified only as free helpers, not on apply-path resolution [`Tests/Unit/SettingsTabsShellTests.cpp`]
- [x] [Review][Defer] User manual still documents Audio/MIDI door / AUDIO FROM / Alt+logo [`Documentation/User/manuel-utilisateur.md`] — deferred: docs pass outside this delivery cutover
- [x] [Review][Defer] `headerAudioProductCopy` only asserts display-name constants [`Tests/Unit/SettingsTabsShellTests.cpp`] — deferred: already recorded; GUI menu structure stays smoke/manual

Rejected:
- false — Cartouche top/bottom hairlines through badge: matches Design Notes continuous chrome from badge across gain/peak.
- false (spec-edit) — Implementation Notes claim 200/504 vs shipped 140/400: frozen Always reuses delivery-1 column width; notes are stale, not an AC miss.
- low rejected — Ignored `setAudioDeviceSetup` errors: pre-existing; already rejected in prior triage log.
- rejected (re-litigate) — `scanForDevices` every refresh: already deferred.
- rejected (re-litigate) — Core `showAudioMidiSettingsDialog` no-op APIs: already deferred.
