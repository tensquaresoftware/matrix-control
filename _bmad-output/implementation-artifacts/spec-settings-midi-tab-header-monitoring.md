---
title: 'Settings MIDI tab and header monitoring'
type: 'feature'
created: '2026-10-05'
status: 'done'
route: 'dispatch'
baseline_commit: 'b7d7a236096c674dce26193a01e5e0de8aaa44f8'
review_loop_iteration: 0
context:
  - '{project-root}/Documentation/Development/Plans/2026/10/2026-10-04-Unified-Settings-Window-And-Header.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-tabs-shell.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-tab.md'
  - '{project-root}/_bmad-output/implementation-artifacts/guide-matrix-modal-design.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** MIDI port wiring still lives in the header as combo lists, while Settings already hosts AUDIO wiring. Delivery 3 must finish the cutover so Settings owns all MIDI cabling and the header only shows scene activity.

**Approach:** Add a MIDI Settings tab with port lists and activity LEDs, remove MIDI combos from the header, keep the three LEDs with monitoring labels plus a MIDI cartouche, and share one activity flux between Settings and header.

## Boundaries & Constraints

**Always:**
- Reuse delivery 1–2 contracts: `SettingsTabRail`, `SettingsShellMetrics`, Matrix Settings overlay, integer `scaledInt` metrics, key `settingsLastTab`, two content columns (labels left / controls+LED right, never mixed), ASCII display strings.
- Standalone tab order: USER INTERFACE, DEVICE, MIDI, AUDIO, PATCH, PATCH MUTATOR, MASTER. Plugin: same without AUDIO.
- Settings MIDI rows (standalone): KEYBOARD FROM + combo + LED; SYNTH FROM + combo + LED; SYNTH TO + combo + LED. Plugin Settings: omit KEYBOARD FROM row; keep SYNTH FROM / SYNTH TO + LEDs.
- Header after cutover: no MIDI combos; keep FROM KEYBOARD / FROM SYNTH / TO SYNTH labels + three LEDs + MIDI cartouche. Remove greyscale HOST combo. Keep FROM KEYBOARD LED in plugin.
- Settings and header LEDs share `Core::MidiActivityTracker` paths already used by the header timer (`kInstrument`, `kMidiFromInbound`, `kOutbound`) via the same `TSS::Led` + level mapping — no second activity model.
- Combo+LED packing mirrors AUDIO SYNTH FROM + peak: shortened combo + gap + 12 px LED inside the existing control column (no AffineTransform; stay on ÷4 grid).
- Unreleased app: renumber `LastTab` so MIDI inserts after DEVICE (AUDIO and later ids bump); document coerce — no shipped-user migration flag required.
- Product copy that names these ports for the user (header labels, Settings labels, sticky footer / contextual help that point at those controls) follows the new vocabulary. ASCII only.

**Never:**
- Reopen AUDIO / RadioButtonGroup / Audio-MIDI door work (delivery 2 done).
- Skin / UI Scale / About inside Settings; first-launch Device Setup assistant (leave EpromTypePrompt MIDI labels alone).
- Change Core MIDI protocols / SysEx behaviour; invent a second LED pulse/decay path; duplicate port-open logic outside existing processor setters.
- French UI strings; global AffineTransform on Settings or header; Core→GUI dependencies.
- Ship the MIDI tab without removing header MIDI combos in the same delivery.
- User manual rewrite (defer unless a one-line path is blocking).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Standalone Settings | Open Settings | MIDI tab after DEVICE; three port rows + LEDs | N/A |
| Plugin Settings | Open Settings | MIDI tab; SYNTH FROM / SYNTH TO only (no KEYBOARD FROM) | N/A |
| Plugin header | Plugin mode | FROM KEYBOARD LED+label present; no HOST combo | N/A |
| Header cutover | Any host mode | No MIDI combos; MIDI cartouche + three LED+label packets | N/A |
| Last tab MIDI | Close on MIDI, reopen | MIDI restored | N/A |
| Stale last-tab after renumber | Old stored ids from delivery 2 | `normalize` / coerce; wrong tab until user re-picks is OK (unreleased) | Coerce invalid to USER INTERFACE |
| Port change in Settings | Select SYNTH FROM / TO / KEYBOARD FROM | Same processor setters as former header combos; lists refresh on popup | Keep existing distinct-input guard |
| LED parity | Traffic on keyboard / synth-in / synth-out | Settings LEDs (when MIDI tab visible) match header LEDs | Message-thread poll only |
| UI Scale | 50 / 100 / 125 / 150 % | Integer metrics; combo+LED stay in control column | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginIDs.h` (`Settings::LastTab`) -- insert `kMidi` after DEVICE; bump AUDIO/PATCH/MUTATOR/MASTER; extend `kStandaloneIds` / `kPluginIds` / counts / `isValid` (plugin still rejects AUDIO only).
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `kMidiTab`; Settings row labels KEYBOARD FROM / SYNTH FROM / SYNTH TO; header monitoring labels FROM KEYBOARD / FROM SYNTH / TO SYNTH; `kMidiCartoucheLabel`; drop HOST combo copy usage; update sticky/help strings that still say MIDI FROM / MIDI TO as the user-facing control names (not the assistant dialog).
- `Source/GUI/Settings/SettingsShellMetrics.h`, `SettingsTabRail.*`, `SettingsWindow.*`, `SettingsPanel.*` -- MIDI tab label/visibility; attach MIDI page; `updatePageVisibility` / layout; `setMonitoringActive` on Settings close like AUDIO.
- New `Source/GUI/Settings/SettingsMidiPage.*` (+ CMake) -- three (or two in plugin) labeled combo+LED rows; reuse `TSS::MidiPortComboPopulation`; Config callbacks for populate/select/change mirroring header wiring; LED poll while tab visible.
- Small GUI helper (e.g. under `GUI/Helpers/` or next to timers) -- `applyMidiActivityLevels(tracker, keyboardLed, synthFromLed, synthToLed)` shared by `PluginEditor::HeaderRefreshTimer` and Settings MIDI monitoring.
- `Source/GUI/PluginEditorSettingsOverlay.cpp`, `PluginEditorUiConstruction.cpp`, `PluginEditorHeader.cpp`, `PluginEditorTimers.cpp` -- wire Settings MIDI page to `setMidiInputPort` / `setMidiOutputPort` / `setKeyboardFromPort` + list refresh; stop wiring header combos; keep LED poll via shared helper.
- `Source/GUI/Panels/MainComponent/HeaderPanel/*`, `DesignPanels.h` / dimensions -- remove MIDI combo packets + HOST path; pack LED+label only; paint MIDI cartouche (mirror AUDIO cartouche); keep standalone AUDIO cartouche/gain/peak.
- `Source/GUI/Helpers/MidiPortComboPopulation.h` -- reuse from Settings page; do not fork population rules.
- `Source/Core/MIDI/MidiActivityTracker.*` -- read-only reuse; do not add a parallel tracker.
- `Tests/Unit/SettingsTabsShellTests.cpp` -- pin MIDI rail order (standalone + plugin), last-tab coerce after renumber, product-copy asserts for new labels; pure helper tests only (no PluginEditor/SettingsPanel construction).
- Do not change: Core SysEx protocols; `SettingsAudioPage` behaviour; Device Setup assistant UI; delivery-2 RadioButtonGroup.

## Tasks & Acceptance

**Execution:**
- [x] `PluginIDs.h` / `PluginDisplayNames.h` -- MIDI tab id + copy; LastTab renumber; header/Settings vocabulary
- [x] Shared MIDI LED level helper + `PluginEditorTimers.cpp` -- one flux for header and Settings
- [x] `SettingsMidiPage.*` + `CMakeLists.txt` -- port rows + LEDs; plugin hides KEYBOARD FROM
- [x] `SettingsShellMetrics` / rail / panel / overlay -- register MIDI tab; monitoring lifecycle; wire processor port APIs
- [x] `HeaderPanel*` / design metrics -- remove combos + HOST; MIDI cartouche; FROM KEYBOARD / FROM SYNTH / TO SYNTH
- [x] Footer/help product strings naming old MIDI FROM/TO control labels -- align to SYNTH FROM / SYNTH TO (assistant deferred)
- [x] `SettingsTabsShellTests.cpp` -- rail order, coerce, copy asserts; build + lint + manual smoke

**Acceptance Criteria:**
- Given standalone Settings, when MIDI is selected, then three rows KEYBOARD FROM / SYNTH FROM / SYNTH TO show combo + LED in the control column.
- Given plugin Settings, when MIDI is selected, then KEYBOARD FROM is absent and SYNTH FROM / SYNTH TO remain.
- Given any header, when shown after cutover, then MIDI combos and HOST are gone; MIDI cartouche + three LED+label packets remain (FROM KEYBOARD kept in plugin).
- Given MIDI traffic, when Settings MIDI tab is visible, then Settings LEDs track the same activity paths as the header LEDs.
- Given last-tab persistence, when MIDI was active, then reopen restores MIDI; invalid ids coerce to USER INTERFACE.
- Given UI Scale 50 / 100 / 125 / 150 %, when Settings MIDI is open, then integer metrics hold and combo+LED stay in the control column.

## Implementation Notes

- LastTab absolute ids after MIDI insert: UI=1, DEVICE=2, MIDI=3, AUDIO=4, PATCH=5, MUTATOR=6, MASTER=7. Delivery-2 stored AUDIO=3 becomes MIDI (valid id, not coerced); invalid / plugin-stale ids coerce to USER INTERFACE (unreleased app).
- Shared flux: `GUI/Helpers/MidiActivityLedLevels.h` applied by header timer and Settings MIDI page timer (paths `kInstrument` / `kMidiFromInbound` / `kOutbound`).
- Header MIDI combos + HOST removed; MIDI cartouche + FROM KEYBOARD / FROM SYNTH / TO SYNTH LED packets. Panic enablement syncs from APVTS `midiOutputPortId`.
- Settings MIDI page reuses `MidiPortComboPopulation` and processor setters; plugin hides KEYBOARD FROM via `SettingsShellMetrics::showsKeyboardFromRow`.
- Unit coverage: rail order, MIDI last-tab restore, keyboard-from host policy, MIDI LED path contract, product copy; GUI smoke still manual.

## Spec Change Log

## Review Triage Log

- medium — Edge/VG: reopen Settings with existing MIDI page skipped port refresh. Real: early return left stale combos until popup. Patched: refresh populate/select on every attach; wire only on first create.
- medium — VG/Blind: `midiActivityLedPathContract` never called `MidiActivityLedLevels::apply`. Real: swapped paths would still pass. Patched: named path constants owned by `apply` + unit asserts.
- low — Blind: `PluginEditor.h` comment still said header MIDI combos. Real stale doc. Patched: comment now Settings MIDI page.
- low — Blind: LastTab comment claimed old AUDIO=3 coerces to UI; standalone `3` is valid MIDI. Real comment bug (behavior intentional for unreleased). Patched: comment + `normalize(3)==kMidi` assert.
- low — Blind: trailing `endPacket` after last MIDI cartouche packet added empty air vs AUDIO pattern. Real. Patched: remove trailing `endPacket`.
- low — Blind: unused `kHostDisplay` after HOST combo removal. Real dead constant. Patched: deleted.
- false — Blind: header LED help must point to Settings > MIDI for discoverability. LED help correctly describes activity; Settings door already documented elsewhere; not a defect.
- false — Blind: plugin Settings needs HOST substitute copy for keyboard. Spec keeps FROM KEYBOARD LED and omits KEYBOARD FROM row; no Settings HOST line required.
- false — Blind: rename header members `midiFromLabel_` / `editorActivityLed_`. Cosmetic; no wrong wiring found; rename cost exceeds value in this delivery.
- false — Blind: always constructing hidden KEYBOARD FROM widgets in plugin mode. Hide-via-visibility matches AUDIO-style page patterns; not a user-facing defect.
- false — Blind: no automated onChange/panic/popup-refresh GUI tests. Project policy avoids PluginEditor construction; covered by wiring reuse + manual smoke (same as delivery 2).
- defer — Blind: `inventory-contextual-help-messages.md` still lists old MIDI FROM/TO combo help. Docs inventory lag; product strings in `PluginDisplayNames.h` are updated.
- carried/rejected low — Blind: discoverability and rename/construction findings above stay rejected after patch pass (no re-open).

## Design Notes

**LastTab after insert (absolute ids):** `kUserInterface=1`, `kDevice=2`, `kMidi=3`, `kAudio=4`, `kPatch=5`, `kPatchMutator=6`, `kMaster=7`. Plugin `idAt` omits AUDIO. Stored delivery-2 ids (AUDIO was 3) are not remapped with a version flag — unreleased app; document in Implementation Notes after change.

**LED packing:** reuse AUDIO peak budget (`kPeakWidth`/`kPeakGap` or MIDI aliases with same 12/8) so `combo + gap + LED == kControlColumnWidth` (140). LED widget size matches `Atoms::Widths::ModulationBusHeader::kLedSize` / header activity LED.

**Vocabulary split (intentional):** Settings cabling uses KEYBOARD FROM / SYNTH FROM / SYNTH TO; header monitoring uses FROM KEYBOARD / FROM SYNTH / TO SYNTH. Do not force one phrasing onto both surfaces.

**Golden row (Settings):** `[label 120] | [combo ~120][gap 8][LED 12]` inside control column; blank gaps only if needed for readability — default three tight rows, no extra blank rows unless layout smoke shows need.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- plugin + standalone targets succeed
- `ctest --preset macos-debug-arm64 -R SettingsTabsShell` (or project unit-test target equivalent) -- new/updated shell tests pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++ under Source/ and Tests/

**Manual checks (if no CLI):**
- Standalone: Settings > MIDI change each port; header LEDs still light; no header combos; MIDI cartouche visible.
- Plugin: Settings MIDI has no KEYBOARD FROM; header still shows FROM KEYBOARD LED; no HOST combo.
- Close Settings on MIDI, reopen — MIDI tab restored.
- UI Scale sweep 50–150% on Settings MIDI and header MIDI cartouche.
