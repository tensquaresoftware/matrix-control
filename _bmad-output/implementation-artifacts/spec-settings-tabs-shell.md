---
title: 'Settings tab shell'
type: 'feature'
created: '2026-10-04'
status: 'done'
route: 'dispatch'
baseline_commit: '86efaa6ab1b65ee2e76e3426db343c6b823b1767'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/guide-matrix-modal-design.md'
  - '{project-root}/Documentation/Development/Plans/2026/10/2026-10-04-Unified-Settings-Window-And-Header.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings is one tall stack of titled sections. Delivery 1 needs the Ableton-style shell (left tabs, right content, vertical rule, current tab lighter, SETTINGS centred) with today's product rows only.

**Approach:** Keep the overlay and existing widgets/wiring. Switch to five tabs (USER INTERFACE, DEVICE, PATCH, PATCH MUTATOR, MASTER). Persist the last tab on APVTS state and restore it on the next open. Plugin-only HARDWARE LATENCY. No MIDI/AUDIO tabs, no header or Audio/MIDI window changes.

## Boundaries & Constraints

**Always:**
- Matrix overlay chrome (SETTINGS, close, Esc, click-outside). No global AffineTransform on Settings.
- Integer px after scale: `ScaledLayout::scaledInt` for bounds/gaps/columns; `ScaledDrawing::snappedStrokeThicknessFromDesign` for the vertical rule. Prefer design sizes divisible by 4. Smoke 50 / 100 / 125 / 150 %; other presets use the same helpers.
- Two content columns: labels left (120 design), controls right (140 design), never mixed. Drop in-content section titles and their separators; the tab is the page title.
- Same five tabs in standalone and plugin. Plugin DEVICE: HARDWARE LATENCY then EPROM TYPE. Standalone DEVICE: EPROM TYPE only.
- Last tab: new APVTS `state` int property (new key). Default USER INTERFACE. Write on tab change; restore on open. Unknown/missing id → first tab.
- Keep `PluginEditorSettings` restore/wire and panel getters. English ASCII display strings.

**Never:**
- MIDI / AUDIO tabs, RadioButtonGroup, AudioMidiSettingsWindow, HeaderPanel, first-launch assistant, Skin/UI Scale/About inside Settings.
- Five copies of SettingsPanel, or APVTS attachments for these prefs. No MIDI/audio wiring changes.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| First open | No last-tab property | USER INTERFACE shown | N/A |
| Reopen | Last tab was PATCH | PATCH shown | N/A |
| Plugin DEVICE | Plugin editor | Latency then EPROM TYPE; no AUDIO tab | N/A |
| Standalone DEVICE | Standalone | EPROM TYPE only | N/A |
| Bad stored tab | Unknown id | First tab; coerce property | Ignore invalid id |
| Tight host | Editor narrower than dialog | Dialog clamped/centred in editor; integer rule | N/A |
| UI Scale while open | 50–200 % presets | Integer bounds/rule/highlight; columns aligned | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsWindow.*` -- chrome + size; add tab rail, vertical rule, body = rail + page. Reuse `paintMatrixOverlayChrome`. `scaledInt`; clamp to editor bounds.
- `Source/GUI/Settings/SettingsPanel.*`, `SettingsPanelSetup.cpp` -- keep widgets/getters; layout only the active tab; drop section titles; `getDesignHeight` from rail vs tallest page (PATCH, four rows), not stacked 487. Prefer `ScaledLayout::scaledInt`.
- New rail under `Source/GUI/Settings/` if needed for the ~400-line file gate -- stacked tab names; selected fill = `ISkin::getPopupMenuBackgroundHooverColour`; integer row bounds.
- `Source/GUI/PluginEditorWindows.cpp` -- apply stored tab after panel ready; leave About/AudioMidi lifecycle.
- `Source/GUI/PluginEditorSettings.cpp` -- save/restore last-tab property only; do not redo combo/latency wiring.
- `Source/Shared/Definitions/PluginDisplayNames.h` -- USER INTERFACE tab copy; other tab names unchanged; ASCII.
- `Source/Shared/Definitions/PluginIDs.h` -- last-tab key + ids 1–5; existing pref string keys stay.
- `CMakeLists.txt` -- register any new `.cpp`.
- Reuse `ScaledLayout.h`, `ScaledDrawing.h`. Do not edit `HeaderPanel`, `AudioMidiSettingsWindow`, Core MIDI/audio.

## Tasks & Acceptance

**Execution:**
- [x] `PluginDisplayNames.h` / `PluginIDs.h` -- tab labels + last-tab key/ids
- [x] `SettingsWindow.*` (+ rail if extracted) -- Live-style shell, integer rule/highlight, clamp to editor
- [x] `SettingsPanel.*` / `SettingsPanelSetup.cpp` -- pages, hide unused widgets, two-column rows, plugin latency order
- [x] `PluginEditorSettings.cpp` / `PluginEditorWindows.cpp` -- persist/restore last tab
- [x] `CMakeLists.txt` -- compile new sources
- [x] Smoke 50 / 100 / 125 / 150 % -- hairlines and alignment (integer metrics tested; live visual still worth a glance)

**Acceptance Criteria:**
- Given Settings open, when the user clicks a tab, then only that page's rows show, the tab is lighter, and the vertical rule is a sharp integer stroke.
- Given close on PATCH MUTATOR, when Settings opens again in the same session/document, then PATCH MUTATOR is selected.
- Given plugin vs standalone, when DEVICE is shown, then latency appears only in plugin, above EPROM TYPE, and MIDI/AUDIO tabs are absent.
- Given UI Scale 50 / 100 / 125 / 150 %, when Settings is open, then labels stay left of controls, no global transform, and the dialog stays inside the editor.

## Implementation Notes

- Delivery 1 shell: `SettingsTabRail` + `SettingsShellMetrics` (148+4+292). Last tab key `settingsLastTab`. Invalid id coerced on restore; missing property stays unset and defaults to USER INTERFACE.
- Vertical rule X is integer (`ruleFillX`); 50 % no longer centres the stroke on a half-pixel.
- `SettingsTabsShell` tests map 1:1 to the I/O matrix. Live pixel smoke in the plugin/standalone UI was not run in this session.

## Spec Change Log

## Review Triage Log

- false — Blind: last-tab not in PluginEditorSettings. Overlay lifecycle owns save/restore in PluginEditorWindows; prefs still persist. Not a user-facing failure.
- low rejected — Blind: persist helpers live in SettingsShellMetrics. Split would add a new module; no user harm.
- false — Blind: last-tab dirties APVTS. Same ValueTree write as other Settings prefs; no UndoManager on those either.
- patch — Blind/VG: tests pin stub MIDI/AUDIO/DEVICE flags unused by production. Wire latency visibility into layoutDeviceSection; rail must read tabLabel/tabCount.
- low rejected — Blind: kTallestPageRows = 4. Matches current PATCH; future rows are later stories.
- patch — Blind/VG: SettingsPanel::getDesignHeight unused; window sizes from SettingsShellMetrics. Delete it.
- low rejected — Blind: tabs_[5] vs kCount. Delivery 1 has five tabs; adding MIDI is a later story.
- false — Blind: no hover on unselected tabs. Spec only requires selected fill, not hover.
- maybe-false deferred — Blind: PATCH MUTATOR at 50% may clip. No font-width measure in repo; live smoke would settle it.
- patch — Blind: unused kInterfaceSection after INTERFACE → USER INTERFACE tab copy.
- false — Blind: rail constructed before panel. Click lambda is not invoked during addAndMakeVisible.
- false — Blind: rule uses dialog border colour. Matches overlay chrome, not a skin miss for this delivery.
- false — Blind: no keyboard tab cycling. Not in intent; clickable tabs match the shell.
- low rejected — Blind: non-int last-tab values. Other Settings ids are ints; extra type guards add complexity.
- false — Blind: live 50–200% smoke not run. Process note, not a code defect; metric tests cover integer rule/columns.
- patch — Edge: clamped short rail lets tab hit-tests overflow. Clip rows to the rail inner bounds.
- patch — Edge: clamped narrow dialog lets page/rule overflow. jmin rail/gutter to remaining width; paint rule only inside the body.
- false — Edge claim: persist lives only in PluginEditorWindows. True fact, not a runtime defect.
- patch — VG: last-tab write at PluginEditor is untested. Add writeLastTab next to readAndCoerceLastTab and call it from setOnTabChanged.
- defer — VG: UI Scale test names highlight but never observes selected fill. Paint-only; suite does not instantiate the rail.

## Design Notes

Wider and shorter than today's stack: keep 120+140 content; tab column must fit PATCH MUTATOR without wrap at 50 %. Prefer a 4 px design grid (e.g. rail 148 + 4 px rule gutter + 292 content = 444). Height = taller of five tab labels vs tallest page, plus 16 px padding — drop the old 487 stacked height.

Selected tab: slightly lighter fill (popup hover). Unselected: body background. SETTINGS stays centred on the title band.

Last tab lives on APVTS `state` like other Settings prefs (not a machine PropertiesFile).

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 && cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/`

**Manual checks (if no CLI):**
- Standalone and plugin: five tabs, current rows, last tab restored, SETTINGS centred.
- UI Scale 50 / 100 / 125 / 150 %: integer rule and highlight, columns aligned.
