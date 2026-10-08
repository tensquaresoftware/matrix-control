# Code map and implementation order

Entry points for Build. Prefer reuse over parallel dialog trees when practical.

## Implementation order

1. **GS-1 — Settings User Interface**  
   Add UI SCALE + SKIN rows (strings already exist in `PluginDisplayNames`) and GETTING STARTED combo + `RUN SETUP AGAIN`. Wire prefs. Keep logo Scale/Skin shortcuts.
2. **GS-2 — Wizard shell**  
   Matrix overlay chrome, width = Settings, lower heights, title band, navigation chrome, frozen copy strings, button set.
3. **GS-3 — Steps + flags + absorb Device Setup**  
   Step bodies; format applicability; per-step flags; auto-open / resume; Configure later; migrate off Device Setup one-shot gate; retire CONFIRM / SPECIFY LATER as onboarding completion.

GS-4 (manual) is parallel/after — not this Spec’s code map.

Build may ship GS-1 in the same change as wizard opening work, but Settings UI must not land *after* a wizard that already expects those controls.

## Primary entry points (current Device Setup / editor)

| Area | Paths | Role |
|------|-------|------|
| Device Setup dialog | `Source/GUI/Dialogs/EpromTypePromptDialog.{h,cpp}` (+ layout sibling if present) | Absorb into STEP 2 / replace as onboarding shell |
| Editor open/close | `Source/GUI/PluginEditor.h`, `PluginEditorWindows.cpp`, `PluginEditorUiConstruction.cpp`, `PluginEditorAudio.cpp`, `PluginEditorSettings.cpp` | Open guards, pending wake-up, close on Settings, layout scale hooks |
| Skin/scale layout hook | `Source/GUI/PluginEditorSkinScale.cpp` | Keep wizard layout updates with UI scale |
| Device row / body helpers | `Source/Core/Services/DeviceSetupDeviceRow.h` (+ tests) | Reuse DEVICE/SEARCHING logic and assistant body builders where still valid |
| EPROM policy | `Source/Core/Services/EpromTypePolicy.*` | Unchanged semantics; live STEP 2 |
| Machine defaults / seed | `Source/Core/Services/DeviceConnectionMachineDefaults.*`, `PluginProcessorClipboard.cpp` (`initializeEpromTypeProperties`) | Seed / migrate promptDone → step flags |
| Inquiry / promptDone read | `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` | Stop treating sole `promptDone` as the only first-run gate once wizard owns onboarding |
| IDs / strings | `Source/Shared/Definitions/PluginIDs.h`, `PluginDisplayNames.h` | New Getting Started strings/keys; retire DEVICE SETUP / SPECIFY LATER from onboarding path (confirm dialogs elsewhere may keep Confirm labels) |

## Settings UI

| Area | Paths | Role |
|------|-------|------|
| Panel / tabs | `Source/GUI/Settings/SettingsPanel.{h,cpp}`, `SettingsPanelSetup.cpp`, `SettingsTabRail.*` | User Interface section order and new rows |
| Shell metrics | `Source/GUI/Settings/SettingsShellMetrics.h` | Width SSOT (`kDesignWidth` = 400) for wizard match |
| MIDI page | `Source/GUI/Settings/SettingsMidiPage.*` | Brick reuse for Synth From/To / related |
| Audio page | `Source/GUI/Settings/SettingsAudioPage*.*` , `AudioDeviceSetupSync.*` | Brick reuse for STEP 4 digeste controls |
| Header ports | `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.*` | Shared MIDI port list population — do not fork |

## Prefs / first-run distinct from wizard

| Concern | Paths | Note |
|---------|-------|------|
| Audio-safety first-run | `Source/Core/Audio/SceneAudioSafety.h`, Standalone app holder apply path | `sceneAudioSafetyDefaultsApplied` — **do not** merge with wizard flags |
| Legacy Device Setup done | `PluginIDs::Settings::kEpromTypePromptDone` (`settingsEpromTypePromptDone`) | Migrate → Synth Communication complete; stop sole auto-open gate |

## Tests / quality

| Area | Paths | Role |
|------|-------|------|
| Device row unit tests | `Tests/Unit/DeviceSetupDeviceRowTests.cpp` | Extend or add pure helpers for flag applicability / body text if extracted |
| Lint gate | `Scripts/quality/lint_touched.py` | Touched C++ under `Source/` / `Tests/` |
| CMake | `CMakeLists.txt` `PLUGIN_SOURCES` | Register new `.cpp` files |

## UX companions (adopted)

- `_bmad-output/implementation-artifacts/guide-matrix-modal-design.md` — chrome, rhythm, GETTING STARTED width/height rules
- `_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md` — § GETTING STARTED smoke checklist

## Historical (superseded for future work)

- `_bmad-output/implementation-artifacts/spec-device-setup-assistant.md`
- `_bmad-output/implementation-artifacts/spec-device-setup-cross-project-friction.md`
- `_bmad-output/implementation-artifacts/spec-device-setup-welcome-intro.md`

Keep as implementation record of the one-shot; do not extend as the First-run vehicle.
