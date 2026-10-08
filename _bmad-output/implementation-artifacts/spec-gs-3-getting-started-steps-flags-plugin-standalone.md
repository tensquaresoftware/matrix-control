---
title: 'GS-3 Getting Started steps, flags, plugin vs Standalone'
type: 'feature'
created: '2026-10-08'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '6edc8509d2e32c694213fbc28fe80e7c469d20dc'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/epic-gs-context.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/acceptance-criteria.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/journey-and-flags.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/ui-copy.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/code-map.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-gs-2-getting-started-wizard-shell-and-navigation.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The GETTING STARTED shell (GS-2) shows help copy and navigation only. Live Scale/Skin, Synth MIDI/DEVICE/EPROM, Keyboard, and Audio controls are missing; durable per-step progress, launch auto-open/resume, Configure-later policy, and `RUN SETUP AGAIN` reset are not wired. Device Setup still owns first-run auto-open via `promptDone`, so plugin vs Standalone cannot unlock edit / play / hear through one guided path.

**Approach:** Fill wizard step bodies with shared Settings/header bricks (no forked device lists), add machine-scoped per-step flags plus Configure-later silence and auto-open resume, make `RUN SETUP AGAIN` reset applicable flags and open intro, absorb Device Setup onboarding into STEP 2 (Next marks Synth Communication done; retire CONFIRM / SPECIFY LATER / sole `promptDone` as the first-run gate), and keep audio-safety Input None orthogonal.

## Boundaries & Constraints

**Always:**
- Step content and buttons match `journey-and-flags.md` / `ui-copy.md` / AC-GS3-1…11 (ASCII `PluginDisplayNames`).
- STEP 1 Scale/Skin use the same apply/persist paths as Settings / logo.
- STEP 2 Synth From/To, DEVICE, EPROM write live like header/Settings; reuse `DeviceSetupDeviceRow` + `MidiPortComboPopulation`; Next marks Synth Communication done (no Confirm).
- STEP 3 Standalone: Keyboard From + Skip; plugin: informative host copy, no combo, no Skip; Finish on last applicable step.
- STEP 4 Standalone only: digeste driver/type, I/O, SYNTH FROM (drop sample rate + buffer — reserved 3 control rows from GS-2).
- Per-step durable flags (User Interface, Synth Communication, MIDI Keyboard, Audio); plugin and Standalone do not share one setup-done boolean; Audio flag only when Audio applicable.
- Auto-open when combo = `SHOW WHEN INCOMPLETE` and an applicable step is incomplete: open at first incomplete applicable step (not intro), except true first contact / never Continued / `RUN SETUP AGAIN` (step 0).
- Configure later: leave unvisited steps incomplete; one auto reminder then silence until `RUN SETUP AGAIN` or combo → `SHOW WHEN INCOMPLETE`; newly applicable step rearms one targeted open.
- `RUN SETUP AGAIN`: reset flags applicable to the current format; open step 0.
- Migration: legacy session or machine Device Setup `promptDone` true → Synth Communication complete; other step flags stay incomplete.
- After absorb: GETTING STARTED is the onboarding path; Device Setup one-shot must not auto-open in parallel.
- `sceneAudioSafetyDefaultsApplied` remains separate; wizard Audio must not set/clear it by implication.
- English UI; `lint_touched.py` on touched C++.

**Never:**
- Pastilles, mega-modal, Settings-only auto-open, forked MIDI/audio device-list SSOT.
- Single shared setup-done flag for both formats; merge with audio-safety.
- QUIT / SPECIFY LATER / dedicated Confirm on STEP 2 / second full-setup button.
- Detailed DAW examples in wizard body (GS-4 manual).
- Inventing alternate frozen copy; Audio Settings Matrix rebuild; reopening Epic 7/8.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| STEP 1 live | Change Scale or Skin in wizard | Same visual + APVTS paths as Settings / logo | Invalid id → existing clamp helpers |
| STEP 2 live + Next | Select ports / EPROM, press Next | Ports/EPROM persist like Settings; Synth Communication done; no Confirm | Missing ports allowed; DEVICE row follows existing searching/connected helpers |
| Keyboard Skip | Standalone STEP 3 Skip | MIDI Keyboard done without a port | N/A |
| Plugin Keyboard | Plugin STEP 3 Next/Finish | No Keyboard From combo; Keyboard done; Finish if last | N/A |
| Audio Finish | Standalone STEP 4 Finish | Digeste audio applied; Audio done; wizard closes | N/A |
| Auto-open resume | SHOW WHEN INCOMPLETE; UI done, Synth incomplete | Editor ready opens wizard at STEP 2 (not intro) | NEVER AT LAUNCH → no auto-open |
| Plugin→Standalone Audio | Plugin 1–3 done; Standalone Audio incomplete | Targeted open at STEP 4 with resume Audio copy | N/A |
| Configure later | Intro CONFIGURE LATER once, then again | First: one later reminder allowed; second: silence until reset/combo | Newly applicable step rearms one open |
| RUN SETUP AGAIN | Click from Settings | Applicable flags reset; wizard at step 0 | Does not change NEVER AT LAUNCH combo |
| Absorb | Would have opened Device Setup | GETTING STARTED path instead; no parallel Device Setup auto-open | Inquiry pending must not reopen Device Setup |
| Migration | Legacy promptDone true, no wizard flags | Synth Communication complete; UI/Keyboard/Audio incomplete | Missing new keys → incomplete except migrated Synth |
| Audio-safety | Standalone cold Input None gate | Wizard Audio flags unchanged by that gate alone | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Dialogs/GettingStartedWizardDialog.{h,cpp}` (+ layout sibling if needed) -- Host live step controls in the reserved band; wire nav to mark flags / Configure-later / Finish; select `bodyFor` firmware suffix and Audio resume copy from live/flags state.
- `Source/GUI/Dialogs/GettingStartedWizardFlow.h` -- Extend pure helpers: first incomplete applicable step, format-applicable flag reset set, Configure-later / resume rules (keep GUI-free for tests).
- `Source/GUI/Dialogs/GettingStartedWizardMetrics.h` -- Keep GS-2 heights/row reserves; adjust only if live bricks force a measured bump still below Settings.
- `Source/Core/Services/GettingStartedMachineDefaults.{h,cpp}` -- Per-step done flags, Configure-later reminder/silence, migrate from Device Setup `promptDone`, reset-for-format, first-incomplete resolver; store `Matrix-Control-GettingStarted`.
- `Source/Shared/Definitions/PluginIDs.h` -- Machine keys for step flags + Configure-later (Build naming; document in Implementation Notes). Keep `kGettingStartedAutoOpen`.
- `Source/GUI/PluginEditorGettingStarted.cpp` (+ `PluginEditor.h` / UiConstruction / Audio as needed) -- Editor-ready auto-open when allowed; resume step; stop Device Setup cold/pending auto-open; peer mutual exclusion stays.
- `Source/GUI/PluginEditorSettingsAppearance.cpp` -- `RUN SETUP AGAIN` → reset applicable flags + open step 0.
- `Source/GUI/PluginEditorWindows.cpp` / `PluginEditorAudio.cpp` / `MidiManagerDeviceInquiry.cpp` -- Retire Device Setup as onboarding gate; STEP 2 Next (or equivalent) syncs legacy `promptDone` so pending/inquiry cannot revive the old modal; keep Device Setup dialog sources only if still useful for helpers — do not show as parallel first-run.
- `Source/Core/Services/DeviceSetupDeviceRow.h` (+ tests) -- Reuse DEVICE row / searching / body builders inside STEP 2; extend pure flag/applicability helpers if extracted.
- `Source/GUI/Helpers/MidiPortComboPopulation.h`, `SettingsMidiPage.*`, `SettingsAudioPage*.*`, `AudioDeviceSetupSync.*`, `PluginEditorSkinScale.cpp`, `PluginEditorSettingsAppearance.cpp` -- Reuse population/apply paths for Scale/Skin, Synth ports, Keyboard From, digeste Audio; do not fork lists.
- `Source/Core/Audio/SceneAudioSafety.h` -- Do not touch / merge.
- `Source/Shared/Definitions/PluginDisplayNames.h` -- Restore product-final Getting Started help if still GS-1 tempered; select resume/firmware strings already defined.
- `Tests/Unit/` (+ `CMakeLists.txt`) -- Pure Flow/MachineDefaults contract: flags, resume step, Configure-later, migration, plugin skips Audio, RUN SETUP AGAIN reset, Device Setup auto-open suppressed.
- `CMakeLists.txt` -- Register new `.cpp` if any.
- `_bmad-output/specs/spec-getting-started/{journey-and-flags,acceptance-criteria,ui-copy,code-map}.md` -- Product SSOT (read-only unless copy bug).

**Reuse:** GS-2 shell/Flow/metrics; DialogMatrixHelpers; Settings/header MIDI & audio bricks; DeviceSetupDeviceRow; GettingStarted auto-open combo store.

**Do not change:** Audio-safety first-run semantics; HeaderPanel MIDI list SSOT; EpromTypePolicy meaning; Settings width SSOT; GS-4 manual content.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginIDs.h` + `GettingStartedMachineDefaults.*` -- Add per-step + Configure-later machine keys/APIs; migrate legacy promptDone → Synth Communication done; reset applicable-to-format; first incomplete step.
- [x] `Source/GUI/Dialogs/GettingStartedWizardFlow.h` (+ contract tests) -- Pure resume / reset / Configure-later / body-variant selection rules.
- [x] `Source/GUI/Dialogs/GettingStartedWizardDialog.*` -- Embed live STEP 1–4 controls; wire Next/Skip/Finish/Configure later to flags; firmware suffix + Audio resume body when applicable.
- [x] `Source/GUI/PluginEditorGettingStarted.cpp` (+ UiConstruction/Audio/Windows/inquiry) -- Auto-open on editor ready; absorb Device Setup (no parallel auto-open); inquiry pending must not reopen Device Setup.
- [x] `Source/GUI/PluginEditorSettingsAppearance.cpp` -- RUN SETUP AGAIN resets applicable flags then opens intro.
- [x] Shared brick wiring -- Scale/Skin, Synth From/To, DEVICE, EPROM, Keyboard From, digeste Audio via existing population/apply paths.
- [x] `Tests/Unit/` + `CMakeLists.txt` -- Cover I/O matrix (flags, resume, migration, Configure later, absorb guard, audio-safety separation).
- [x] `Scripts/quality/lint_touched.py` -- Pass on touched C++.

**Acceptance Criteria:**
- Given STEP 1, when Scale or Skin changes, then the same preference paths as Settings / logo apply.
- Given STEP 2, when ports/DEVICE/EPROM are used and Next pressed, then values persist like Settings/header, Synth Communication is done, and first-run no longer depends solely on Device Setup + `promptDone`.
- Given Standalone STEP 3, when Skip is used, then MIDI Keyboard is done without requiring a port.
- Given plugin STEP 3, when shown, then no Keyboard From combo and no Skip; Next/Finish marks Keyboard done.
- Given Standalone Audio, when Finish is used, then digeste controls apply and Audio is done; plugin never sees Audio.
- Given SHOW WHEN INCOMPLETE with a later step incomplete, when the editor opens, then the wizard resumes at the first incomplete applicable step (not intro).
- Given Configure later then a second silence, when launch would auto-open, then it stays closed until RUN SETUP AGAIN, combo reset, or a newly applicable step rearms one open.
- Given RUN SETUP AGAIN, when clicked, then applicable flags reset and the wizard opens at step 0.
- Given a machine that would have opened Device Setup, when the editor is ready, then GETTING STARTED owns onboarding and Device Setup does not auto-open in parallel.
- Given legacy promptDone true, when prefs migrate, then Synth Communication is complete and other step flags remain incomplete.
- Given audio-safety first-run Input None, when it applies, then wizard Audio flags are unchanged by that gate alone.

## Implementation Notes

- Machine keys (`PluginIDs::MachineDefaults`, store `Matrix-Control-GettingStarted`):
  `gettingStartedAutoOpen`, `gettingStartedUserInterfaceDone`,
  `gettingStartedSynthCommunicationDone`, `gettingStartedMidiKeyboardDone`,
  `gettingStartedAudioDone`, `gettingStartedConfigureLaterArm` (0/1/2),
  `gettingStartedLastSilencedWasPlugin`, `gettingStartedHasLeftIntro`.
- Auto-open: intro while `hasLeftIntro` is false; otherwise first incomplete applicable step.
  `RUN SETUP AGAIN` clears applicable flags + Configure-later arm + `hasLeftIntro`, opens step 0.
- STEP 2 Next marks Synth Communication done and syncs legacy session/machine `promptDone`
  (clears `promptPending`). Device Setup `openEpromTypePromptDialog` is a no-op; inquiry no longer
  arms pending. Digeste Audio rows: driver/type, input device, SYNTH FROM (FE/buffer stay in Settings).
- Build review patches: audio catalog fallthrough when Settings Audio page null; stopLiveTimers on peer close;
  Keyboard From refresh; Audio digeste refresh on step entry; visible-only port sync; expanded Flags contracts
  (migration hasLeftIntro, reset store, absorb arm helper, SHOW WHEN INCOMPLETE silence clear); softer auto-open help.

### Review Findings

- [x] [Review][Patch] refreshAudioFromCombo fallthrough when Settings open without Audio page
- [x] [Review][Patch] closeGettingStartedWizard stopLiveTimers + drop unused include
- [x] [Review][Patch] keyboardFromPortId refreshes wizard Keyboard From combo
- [x] [Review][Patch] Audio digeste refresh on showStep entry
- [x] [Review][Patch] Port sync only when wizard visible
- [x] [Review][Patch] Flags contract gaps (migration / reset / absorb / SHOW WHEN INCOMPLETE)
- [x] [Review][Patch] Soften Getting Started auto-open Settings help

#### Code review GS-3 (2026-10-08)

- [x] [Review][Patch] Configure-later one-reminder must be same-format; cross-format burn blocks plugin→Standalone Audio rearm [`GettingStartedMachineDefaults.h` / `PluginEditorGettingStarted.cpp`]
- [x] [Review][Patch] Cap measured STEP 2 body so live dialog stays below Settings when firmware suffix wraps [`GettingStartedWizardDialog.cpp` / `GettingStartedWizardMetrics.h`]
- [x] [Review][Patch] Resync EPROM combo when re-entering Synth after family coerce while wizard was on another step [`PluginEditorAudio.cpp` / `GettingStartedWizardDialog.cpp`]
- [x] [Review][Patch] Skip auto-open when Getting Started store fails to open (empty prefs must not reopen every launch) [`GettingStartedMachineDefaults.cpp` / `PluginEditorGettingStarted.cpp`]
- [x] [Review][Patch] Orphan digeste input device must keep live setup spelling (Settings pattern), not uppercased combo text [`GettingStartedWizardDialogDevice.cpp`]
- [x] [Review][Patch] Contract-test `markContentStepDone` for steps 1–4 [`GettingStartedFlagsContractTests.cpp`]
- [x] [Review][Patch] Contract-test STEP 2 → legacy Confirm sync (promptDone / pending / machine write) [`DeviceSetupDeviceRow.h` / `GettingStartedFlagsContractTests.cpp`]
- [x] [Review][Patch] Absorb contracts: retire/update `shouldOpenDeviceSetupAssistant` open-when-incomplete expectations; pin launch absorb beyond inquiry-arm only [`DeviceSetupDeviceRow.h` / tests]
- [x] [Review][Patch] Contract-test `decideAutoOpen` resume at Audio (step index 4) with `hasLeftIntro` [`GettingStartedFlagsContractTests.cpp`]
- [x] [Review][Defer] Null `AudioDeviceManager` at wizard bind leaves digeste empty — deferred: maybe-false; settle with Standalone holder timing at editor-ready auto-open
- [x] [Review][Defer] ExportMatrixModals Getting Started frames use empty HostBindings — deferred: tooling export pack, not product path

**Rejected (code review 2026-10-08):**
- false: stale `promptPending` at attach — no product path reopens Device Setup from it after absorb
- false: `openEpromTypePromptDialog` no-op — intentional GS-3 absorb
- false: plugin `RUN SETUP AGAIN` leaves `audioDone` — format-applicable reset by design
- false: STEP 4 omits output row — digeste is driver/input/SYNTH FROM; output only if space (3 rows used)
- false: `shouldArm…AfterInquiry` ignores `sessionPromptDone` — intentional always-false absorb gate
- low: digeste refresh on live device-graph change while staying on Audio — uncommon; needs ChangeListener complexity
- low: Settings auto-open help omits NEVER AT LAUNCH / silence — intentional Build soften
- low: OneReminder persist before `callAsync` if editor dies — exotic; prior Build triage
- reject: spec `status: done` vs sprint `review` — artifact hygiene, not product defect

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| BH+EC: refreshAudioFromCombo skips applyAudioCatalogSelectionOnly when Settings open but Audio page null | medium | Confirmed vs baseline: old path fell through after null audioPage; new else-only-on-closed-Settings drops catalog apply. |
| EC: closeGettingStartedWizard does not stop searching timer | medium | Confirmed: close only setVisible(false); requestDismiss/stopTimer skipped on peer close. |
| BH: keyboardFromPortId excluded from wizard refresh | medium | Confirmed: valueTreePropertyChanged refreshes wizard ports only for non-keyboard; no keyboard combo sync while wizard open (header can still change). |
| BH: Audio digeste lists not refreshed on step entry | low | prepareForShow refreshes once; showStep only toggles visibility — stale if devices change mid-wizard. Everyday harm low; still a direct refresh on Audio entry. |
| BH: applyEpromTypePromptMidiPortChange syncs wizard without isVisible | low | Confirmed on failed-open path; hidden instance can sync. Trivial isVisible guard. |
| BH: unused EditorOutboundGate include in PluginEditorGettingStarted.cpp | low | Confirmed include unused — delete. |
| VG+BH: migrateLegacyPromptDone hasLeftIntro unasserted | medium | Confirmed: migrate writes hasLeftIntro; migration test never reads it. |
| VG+BH: resetForRunSetupAgain store wipe untested | medium | Confirmed: tests stub reset callback / pure resetApplicableFlags only. |
| VG+BH: absorb test only pins shouldOpenDeviceSetupAssistant | medium | Confirmed: product path is inquiry pending + no-op open; test would stay green if pending arming restored. |
| VG+BH: SHOW WHEN INCOMPLETE silence clear untested | medium | Confirmed: Settings onChange clears arm; no Core/helper test. |
| BH: migration machine-only promptDone untested | low | Session-true covered; machine-only and both-false easy to pin in same test. |
| BH: help copy overstates launch open | low | Auto-open help omits NEVER AT LAUNCH / silence; soft wording tweak. |
| BH: Scale/Skin no live refresh while wizard open | false | Settings open closes wizard (mutual exclusion); logo/Settings cannot change Scale/Skin with wizard visible in normal flow. |
| EC: Configure-later OneReminder burned before async open | low | Editor death between persist and callAsync is exotic; reject complexity. |
| EC: Finish does not call applyDigeste | false | Digeste applies live on driver/input/SYNTH FROM change (Settings-like); Finish marks Audio done. |
| EC: Audio resume body stale mid-wizard walk | false | Resume copy is for targeted launch reopen when prior steps already done; intro walk uses first-pass body by design. |
| BH: spec status done vs sprint in-progress | false | Premature status flip during implement; corrected to in-review for this pass — not a product defect. |
| AA: Configure-later OneReminder not same-format; Standalone can silence and block Audio rearm | high | `decideAutoOpen` consumes reminder with no format check; `persistConfigureLaterArm(kSilenced, isPluginMode)` on Standalone leaves `lastSilencedWasPlugin=false`, so `rearmIfNewlyApplicable` never fires for Audio. |
| AA: measured Synth body + firmware suffix can exceed Settings height | medium | Design headroom STEP 2 ≈ 34 px; `computeGeometry` uses `jmax(minBody, measuredBody)` including suffix outside Metrics budget; tests only pin `dialogDesignHeight`. |
| EC: EPROM coerce while wizard visible off Synth step | medium | Guard only `wizardSynthOpen`; `showStep` does not repopulate EPROM on Synth re-entry. |
| EC: `loadAndMigrate` null store → empty prefs auto-open loop | medium | `openStore()==nullptr` returns `{}`; silence/flags never persist; `decideAutoOpen` still opens. |
| EC: orphan digeste input returns `combo.getText()` uppercased | medium | Settings `selectedDeviceName` keeps `liveSetupName`; wizard path does not. |
| VG: `markContentStepDone` never called from tests | medium | Pre-verified: only production caller; suite seeds flags manually. |
| VG: STEP 2 Confirm bridge untested | medium | Pre-verified: synth-complete → promptDone/pending/machine write has no contract. |
| VG+BH: absorb still under-pinned; `shouldOpenDeviceSetupAssistant` tests expect open-when-incomplete | medium | Helper has zero Source callers; Device Setup tests still encode pre-GS-3 gate; absorb test only inquiry-arm. |
| BH: `decideAutoOpen` Audio resume index 4 unasserted | low | `firstIncomplete`→Audio covered; auto-open startStepIndex=4 with hasLeftIntro not pinned. |
| EC: null ADM at bind | maybe-false | Defer — holder timing at editor-ready not settled from diff alone. |
| BH: ExportMatrixModals empty HostBindings | low | Defer — export tooling, not product onboarding path. |

## Design Notes

- Property keys (Build naming): extend `Matrix-Control-GettingStarted` with step-done booleans + Configure-later reminder/silence counter; document exact `PluginIDs::MachineDefaults` names when implemented. Auto-open combo key already exists.
- STEP 2 Next should also mark legacy session/machine `promptDone` complete so inquiry/pending cannot revive Device Setup after absorb.
- STEP 4: three digeste rows (driver/type, I/O, SYNTH FROM) matching GS-2 reserved control count; FE/buffer stay in Settings.
- Keep `EpromTypePromptDialog` sources if helpers remain useful; do not ship it as a first-run vehicle. Manual Settings entry for Device Setup does not exist today — do not invent one.
- Continuity from GS-2: empty control band reserved; `bodyFor` resume/firmware strings defined but unused until this story; RUN SETUP AGAIN opens step 0 without reset until this story.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R GettingStarted` (or `Matrix-Control_Tests` filter) -- expected: new + existing Getting Started contract tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched Source/Tests C++

**Manual checks (if no CLI):**
- Fresh prefs: Standalone auto-opens GETTING STARTED; walk Scale → Synth → Keyboard (Skip) → Audio Finish; hear synth path
- Plugin: no Audio; Keyboard informative; Finish at STEP 3
- Partial complete + relaunch: resume at first incomplete step; NEVER AT LAUNCH suppresses
- Configure later twice: second silences; RUN SETUP AGAIN restores intro + flags
- Confirm Device Setup no longer auto-opens beside the wizard
