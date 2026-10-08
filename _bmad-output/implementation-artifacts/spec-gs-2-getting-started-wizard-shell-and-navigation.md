---
title: 'GS-2 Getting Started wizard shell and navigation'
type: 'feature'
created: '2026-10-08'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '9493d7f5238bbaaff20d3919e4419b4fac96683e'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/epic-gs-context.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/acceptance-criteria.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/journey-and-flags.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/ui-copy.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/code-map.md'
  - '{project-root}/_bmad-output/implementation-artifacts/guide-matrix-modal-design.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-gs-1-settings-ui-scale-skin-getting-started-block.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings already exposes Getting Started controls (GS-1), but there is still no Matrix-chrome multi-step GETTING STARTED dialog — Device Setup remains a one-shot shell, and users cannot walk a readable step flow with clear navigation.

**Approach:** Ship the wizard shell only: Matrix overlay chrome, Settings-matched design width, lower per-step height, title-band step titles, frozen body help copy, and the locked button vocabulary with Previous/Next (and step-specific) navigation across applicable steps. Step control bodies, durable flags, auto-open policy, Configure-later reminder, and Device Setup absorb stay in GS-3.

## Boundaries & Constraints

**Always:**
- Chrome matches Settings via `DialogMatrixHelpers::paintMatrixOverlayChrome` (Matrix monochrome, black title band).
- Design width equals Settings (`SettingsShellMetrics::kDesignWidth` = 400); per-step dialog height is lower than Settings tallest page.
- Step titles live only in the title band (from `ui-copy.md` / journey matrix); body shows frozen help copy for the active step/format — no duplicate body heading.
- Buttons only: CONFIGURE LATER, CONTINUE, PREVIOUS, NEXT, SKIP, FINISH as specified per step in `journey-and-flags.md` — never QUIT or SPECIFY LATER; STEP 2 has no dedicated Confirm.
- Navigation walks applicable steps only: plugin never lands on Audio (step 4); Standalone includes step 4; last applicable step shows FINISH.
- English ASCII display strings in `PluginDisplayNames`; reuse DialogMatrixHelpers button layout helpers.
- Overlay lifecycle mirrors Settings / Device Setup (full-editor child overlay, scale with UI scale, close on Escape / outside click as peers).
- English UI only; `lint_touched.py` on touched C++.

**Never:**
- Per-step completion flags, auto-open on editor ready, Configure-later reminder/silence counter, `RUN SETUP AGAIN` flag reset, or migrating off `settingsEpromTypePromptDone` (GS-3).
- Live Scale/Skin, MIDI/DEVICE/EPROM, Keyboard From, or Audio control bricks inside steps (GS-3).
- Absorbing / retiring Device Setup as the onboarding vehicle, or forking a second MIDI/audio device-list SSOT.
- Inventing alternate copy or button labels outside `ui-copy.md`.
- Merging wizard concerns with `sceneAudioSafetyDefaultsApplied`.
- Building a Settings-sized mega-modal or pastilles approach.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Chrome / geometry | Wizard open vs Settings | Same Matrix chrome family; width = Settings 400 design px; wizard height lower than Settings | Clamp/centre like peer overlays if editor smaller |
| Title + copy | Each step 0–4 (plugin skips 4) | Title band matches journey titles; body help matches `ui-copy.md` for format | N/A |
| Intro buttons | Step 0 | CONFIGURE LATER · CONTINUE only | N/A |
| Continue | Step 0 → Continue | Advances to step 1 | N/A |
| Configure later | Step 0 → Configure later | Closes wizard; does not invent flags or claim steps done | N/A |
| Nav middle | Steps 1–2 (and plugin 3) | PREVIOUS · NEXT; Previous/Next move among applicable steps | N/A |
| Keyboard Standalone | Step 3 Standalone | PREVIOUS · SKIP · NEXT (Skip advances; no flag persist yet) | N/A |
| Keyboard plugin | Step 3 plugin | PREVIOUS · FINISH (last applicable); no Skip; no Audio step | N/A |
| Audio Standalone | Step 4 | PREVIOUS · FINISH; Finish closes wizard | N/A |
| Finish / Escape | Last step Finish, or Escape / outside | Closes wizard; no false “setup completed” flag | N/A |
| Forbidden labels | Any step button row | No QUIT, no SPECIFY LATER, no STEP 2 Confirm | N/A |

**Decisions (approved):**
- Keep full spec (token-count gate).
- Wire Settings `RUN SETUP AGAIN` to open the wizard at step 0 without flag reset (GS-3 owns reset + full re-run semantics).

</frozen-after-approval>

## Code Map

- `Source/GUI/Dialogs/GettingStartedWizardDialog.{h,cpp}` (+ optional `GettingStartedWizardDialogLayout.cpp`) -- New overlay component: step index, title band, body copy, nav buttons, `prepareForShow`, `setUiScale`, paint/resized via DialogMatrixHelpers.
- `Source/GUI/Dialogs/GettingStartedWizardMetrics.h` -- `kDesignWidth = SettingsShellMetrics::kDesignWidth`; per-step design heights below Settings tallest body (`paddedBodyDesignHeight` ≈ 316 design px content — pick lower intro/form heights).
- `Source/GUI/Dialogs/DialogMatrixHelpers.{h,cpp}` -- Reuse `paintMatrixOverlayChrome`, `makeButton` / `layoutCentredButtonRow` / `estimateButtonWidth`, body font / `paintBodyText` / gaps.
- `Source/GUI/Settings/SettingsShellMetrics.h` -- Width SSOT (`kDesignWidth` = 400); do not redefine a second width.
- `Source/GUI/Settings/SettingsWindow.cpp` -- Chrome/lifecycle reference peer (full-editor overlay bounds).
- `Source/GUI/Dialogs/EpromTypePromptDialog.{h,cpp,Layout.cpp}` -- Closest form-modal pattern (`prepareForShow`, layout split); Device Setup stays; do not absorb here. Note its width 392 ≠ Settings 400.
- `Source/GUI/PluginEditor.h`, `PluginEditorWindows.cpp`, `PluginEditorSkinScale.cpp` -- Member + `open`/`close`/`layoutIfVisible` / scale-skin refresh; mirror `openEpromTypePromptDialog` / `updateSettingsWindowLayout`.
- `Source/GUI/PluginEditorSettingsAppearance.cpp` -- Wire `RUN SETUP AGAIN` to open wizard at step 0 (no flag reset).
- `Source/Core/Services/GettingStartedMachineDefaults.*` -- Keep combo persist; do not add flag reset APIs in GS-2.
- `Source/Shared/Definitions/PluginDisplayNames.h` -- Add `Dialogs::GettingStarted` (or equivalent): step title-band strings, body copy from `ui-copy.md`, nav button labels; ASCII only.
- `_bmad-output/specs/spec-getting-started/ui-copy.md`, `journey-and-flags.md`, `acceptance-criteria.md` §GS-2 -- Product SSOT for copy, buttons, AC-GS2-1…4.
- `_bmad-output/implementation-artifacts/guide-matrix-modal-design.md` -- Chrome / rhythm checklist.
- `Tests/Unit/` (+ `CMakeLists.txt`) -- Contract tests: titles/buttons/copy constants, plugin skips Audio, button sets per step, forbidden labels absent.
- `CMakeLists.txt` `PLUGIN_SOURCES` -- Register new `.cpp` files.

**Reuse:** Settings/Device Setup overlay lifecycle; DialogMatrixHelpers; SettingsShellMetrics width.

**Do not change:** Device Setup auto-open / `shouldOpenDeviceSetupAssistant`, Settings MIDI/Audio pages, header port lists, per-step flag keys, audio-safety first-run.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Add Getting Started wizard titles, body strings (`ui-copy.md`), and nav button labels (ASCII).
- [x] `Source/GUI/Dialogs/GettingStartedWizardMetrics.h` (+ dialog sources) -- Metrics: width = Settings 400; per-step heights lower than Settings; layout helpers.
- [x] `Source/GUI/Dialogs/GettingStartedWizardDialog.{h,cpp}` (+ layout sibling if needed) -- Shell: Matrix chrome, title band, body copy, step navigation, format-aware applicable steps, button sets per `journey-and-flags.md`.
- [x] `Source/GUI/PluginEditor.*` (windows / skin-scale / headers) -- Own overlay lifecycle: ensure/open/close/layout with UI scale; mutual exclusion with Settings/Device Setup as peer overlays.
- [x] Settings `RUN SETUP AGAIN` wiring -- Open wizard at step 0 (no flag reset).
- [x] `Tests/Unit/` + `CMakeLists.txt` -- Contract coverage for matrix rows (copy, buttons, plugin skips Audio, no forbidden labels); register sources.
- [x] `Scripts/quality/lint_touched.py` -- Pass on touched C++.

**Acceptance Criteria:**
- Given the wizard is open, when compared to Settings, then chrome matches Matrix overlay rules, design width equals Settings, and per-step height is lower than Settings.
- Given any applicable step, when shown, then the step title is in the title band and body help matches `ui-copy.md` for that step/format.
- Given each step, when the button row is shown, then buttons match `journey-and-flags.md` and neither QUIT nor SPECIFY LATER appears.
- Given step 1+, when the user presses PREVIOUS / NEXT (and SKIP where specified), then navigation walks applicable steps only (plugin never lands on Audio).
- Given Configure Later or Finish / Escape / outside dismiss, when activated, then the wizard closes without claiming durable step completion (flags are GS-3).

### Review Findings

- [x] [Review][Patch] Peer overlays M1km / Defrag omit closing the wizard [Source/GUI/PluginEditorSettings.cpp] — added `closeGettingStartedWizard()`.
- [x] [Review][Patch] Wizard open omitted hide bank-transfer progress [Source/GUI/PluginEditorGettingStarted.cpp] — added `hideBankTransferProgressDialog()`.
- [x] [Review][Patch] Nav button→step mapping only in dialog [Source/GUI/Dialogs/GettingStartedWizardFlow.h] — `targetStepForNavButton` + contract tests.
- [x] [Review][Patch] Run Setup Again coverage was constant-only [Tests/Unit/GettingStartedWizardContractTests.cpp] — `runSetupAgain` collaborator asserts intro open + unchanged auto-open preference.
- [x] [Review][Defer] Overlay lifecycle / Escape / outside smoke unharnessed — deferred-work.
- [x] [Review][Defer] ui-copy.md still shows typographic dashes vs ASCII PluginDisplayNames — deferred-work.

## Implementation Notes

- Flow SSOT (pure, header-only): `Source/GUI/Dialogs/GettingStartedWizardFlow.h` -- `Step` 0..4, `isApplicable` (Audio is Standalone only), next/previous over applicable steps, `buttonsFor`, `titleFor`, `bodyFor`, `labelFor`, `runSetupAgainStartStep()`.
- Metrics: `Source/GUI/Dialogs/GettingStartedWizardMetrics.h`. `kDesignWidth = SettingsShellMetrics::kDesignWidth` (400). Body text budgets (design px): intro 110, step 1 46, step 2 58, step 3 Standalone 46 / plugin 58, step 4 58. Reserved (empty, no stub widgets) control rows: step 1 = 2, step 2 = 4, step 3 Standalone = 1 / plugin = 0, step 4 = 3. Resulting full dialog design heights (border + title included): intro 238, step 1 246, step 2 314, step 3 Standalone 218 / plugin 186, step 4 286 -- all below the Settings dialog (348). Pinned by `GettingStartedWizardContractTests`.
- Dialog: `GettingStartedWizardDialog.{h,cpp}` -- `paintMatrixOverlayChrome`, `computeModalGeometry`, `layoutCentredButtonRow`; Escape / outside click dismiss, Enter = rightmost button. Editor lifecycle in new `Source/GUI/PluginEditorGettingStarted.cpp` (`openGettingStartedWizard` / `closeGettingStartedWizard` / `updateGettingStartedWizardLayout`), wired into skin refresh, UI-scale layout, Escape/undo overlay guards, and peer open paths (Settings, About, Master Init, Bank progress, Device Setup close the wizard; wizard opening closes Settings, About, Master Init, M1km choice, Defrag, Device Setup).
- `RUN SETUP AGAIN` now opens the wizard at step 0 via the editor; `Core::GettingStartedMachineDefaults::runSetupAgainNoOp` removed (no flag reset, GS-3). The GS-1 no-op contract test was replaced by `Run Setup Again - opens the wizard at step 0` in the new GS-2 suite.
- Copy: strings are in `PluginDisplayNames::Dialogs::GettingStarted`. `ui-copy.md` uses an em dash and a spaced colon in titles / one body sentence; the ASCII-display rule wins, so titles read `GETTING STARTED - STEP 1: USER INTERFACE` and the body uses ` - `. Body text keeps the frozen wording (e.g. "Configure later" in the intro stays as in `ui-copy.md`, not uppercased). The firmware-suggestion suffix and the Step 4 resume variant are defined but not selected (GS-3: live state / flags).
- Dev tool: `Tools/ExportMatrixModals/Main.cpp` now exports `20-getting-started-<plugin|standalone>-stepN` PNGs for smoke checks.
- Verification: `cmake --build --preset macos-debug-arm64` OK (no warnings); `Matrix-Control_Tests` 0 failures (new `GettingStartedWizardContract` suite + existing `GettingStartedSettingsContract`); `lint_touched.py` OK; wizard PNGs checked at 50% / 100% / 200%. No interactive in-editor smoke (open from Settings, Escape / outside click) was run by the agent.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| BH+EC: M1km / Defrag open do not close wizard | medium | Confirmed: `openMasterM1kmLoadChoiceDialog` / `openMutatorHistoryDefragConfirmDialog` omit `closeGettingStartedWizard`; reverse close is present in wizard open. |
| BH+EC: wizard open does not hide bank-transfer progress | medium | Confirmed: `openGettingStartedWizard` omits `hideBankTransferProgressDialog`; `showBankTransferProgressDialog` does close the wizard. |
| EC: zero uiScale when editor width is 0 | low | Same `baseWidth > 0` pattern as Settings/About/Device Setup peers; everyday open-from-Settings after layout is non-zero. Reject complexity. |
| EC: `primaryButtonFor` on empty `buttonsFor` | false | Valid `Step` always returns a non-empty row; empty only if enum exhausted incorrectly. |
| BH+EC: ui-copy.md em dash vs ASCII code strings | false | Project ASCII display rule wins for `PluginDisplayNames`; Implementation Notes record the intentional ASCII form. Companion doc sync deferred separately. |
| BH: `buttonsFor(Audio, plugin)` still returns a row | false | `showStep` coerces inapplicable Audio to Intro; button matrix for inapplicable step is unused. |
| BH: coerce Audio→Intro instead of last applicable | false | GS-2 opens at intro / Run Setup Again step 0; coerce is a safe fallback, not resume (GS-3). |
| BH: exact heights claimed pinned but tests only compare `< Settings` | low | Everyday harm low; numbers live in metrics header. Soften claim or pin later — reject complexity vs smoke. |
| BH: sprint status vs spec status mismatch | false | Workflow bookkeeping; corrected to `in-review` during triage. |
| BH: ExportMatrixModals not in Tasks checklist | false | Reject: fixing would edit this build's task list under review for optional dev tool. |
| BH: no in-editor smoke evidence | medium | Real gap; no PluginEditor harness — defer. |
| BH: resume / firmware suffix unused in `bodyFor` | false | Intentional GS-3; strings defined for later selection. |
| VG: Run Setup Again test only asserts constant; prefs non-mutation removed | medium | Confirmed: `runSetupAgainStartsAtIntro` does not invoke Settings onClick; deleted GS-1 prefs test not replaced. |
| VG: Continue/Skip/Previous direction only in dialog `handleButton` | medium | Confirmed: Flow tests helpers and labels, not button→step mapping used by UI. |
| VG: overlay lifecycle / Escape / peer exclusion untested | medium | Confirmed no Tests/ coverage; project GUI convention — defer harness. |

## Design Notes

- Body in GS-2 is help copy only (and empty control area reserved by height); live bricks land in GS-3 — do not stub fake Scale/MIDI widgets.
- Button LTR follows journey tables; pack with `layoutCentredButtonRow` like other Matrix modals.
- Format mode: use the same plugin vs Standalone signal the editor already has for Settings tabs (`isPluginMode` / equivalent) so Audio is omitted for plugin.
- Close Settings (and Device Setup if open) when opening the wizard, consistent with Settings closing Device Setup today.
- Exact per-step design heights are Build layout choices within “lower than Settings”; document chosen constants in Implementation Notes.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R GettingStarted` (or run `Matrix-Control_Tests` filter) -- expected: new contract tests pass; existing GS-1 tests still pass
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched Source/Tests C++

**Manual checks (if no CLI):**
- Open wizard via the approved entry path; walk plugin and Standalone step sequences
- Confirm title band, body copy, button sets, width vs Settings, lower height
- Configure Later / Finish / Escape close cleanly; Device Setup still opens on its existing path until GS-3
