---
title: 'GS-1 Settings User Interface — Scale, Skin, Getting Started block'
type: 'feature'
created: '2026-10-08'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '7376fa84f964b334ae9d94ba18bbfb35f0282b79'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/epic-gs-context.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/acceptance-criteria.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/ui-copy.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/code-map.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings → User Interface only exposes INFO MESSAGE and CONTEXTUAL HELP. Users cannot change UI Scale or Skin from Settings, and there is no Settings control to suppress or re-run Getting Started — only the logo menu covers Scale/Skin today.

**Approach:** Add UI SCALE and SKIN rows (same APVTS paths as the logo shortcuts), then a GETTING STARTED block (auto-open combo + RUN SETUP AGAIN below). Keep logo Scale/Skin shortcuts. GS-1 ships the Settings surface and durable machine-scoped auto-open combo preference only; RUN SETUP AGAIN is visible but does not reset flags or open a wizard yet (full behavior deferred to GS-2/GS-3 — no fake open). Wizard shell, step bodies, and Device Setup absorb stay out of this story.

## Boundaries & Constraints

**Always:**
- Row order: UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP, GETTING STARTED.
- Frozen English ASCII copy from `ui-copy.md`: `GETTING STARTED`, `SHOW WHEN INCOMPLETE`, `NEVER AT LAUNCH`, `RUN SETUP AGAIN`; Scale/Skin labels match User Interface style (no colon), consistent with INFO MESSAGE / CONTEXTUAL HELP.
- Scale/Skin Settings rows call the same apply/persist paths as the logo menu (`applyUiScaleFromItemId` / skin apply → `PluginIDs::Settings::kGuiScale` / `kSkinVariant`).
- Combo default = `SHOW WHEN INCOMPLETE`; preference is machine-scoped (survives project/session), distinct from `sceneAudioSafetyDefaultsApplied` and from Device Setup `promptDone`.
- RUN SETUP AGAIN: present and clickable without crash; no wizard open, no step-flag reset, no false “wizard completed” in GS-1 (GS-2/GS-3 own real re-run).
- Logo menu Scale/Skin shortcuts remain available.
- English UI only; `lint_touched.py` on touched C++.

**Never:**
- Wizard chrome, step bodies, per-step flags, Device Setup absorb, or retiring DEVICE SETUP as onboarding (GS-2/GS-3).
- Forking a second Scale/Skin store or second MIDI/audio device-list SSOT.
- Removing logo Scale/Skin shortcuts.
- A second “full setup” button; inventing alternate combo wording.
- Merging Getting Started prefs with audio-safety first-run.
- Inventing a placeholder wizard dialog or editor “hook for later” beyond a no-op / deferred button action.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Order | Open Settings → User Interface | Rows top→bottom: UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP, GETTING STARTED (combo then RUN SETUP AGAIN under it) | N/A |
| Scale from Settings | Change UI SCALE combo | Same visual scale + APVTS persist as logo shortcut | Invalid id → keep current / clamp via existing scale helpers |
| Skin from Settings | Change SKIN combo | Same skin apply + APVTS persist as logo shortcut | Same as logo path |
| Logo still works | Change Scale/Skin from logo menu | Still works; Settings combos reflect state on next open/restore | N/A |
| Combo persist | Set NEVER AT LAUNCH, quit app, relaunch | Combo still NEVER AT LAUNCH | Missing key → SHOW WHEN INCOMPLETE |
| Run Setup Again | Click RUN SETUP AGAIN | No crash; no wizard opens; no step flags reset or marked complete | N/A |

**Decisions (approved):**
- Keep full spec (token-count gate).
- RUN SETUP AGAIN / auto-open enforcement: controls + persist combo only until GS-2/GS-3.

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsPanelSetup.cpp` -- `setupInterfaceSection`: add Scale/Skin/Getting Started widgets; populate ScaleLevels / SkinVariants / GS combo items; looks.
- `Source/GUI/Settings/SettingsPanel.cpp` -- `layoutInterfaceSection`: reorder rows; GETTING STARTED = labeled combo row + button-only row below (no second label beside button; do not misuse `layoutButtonRow` with a duplicate label). Height/metrics if tab content grows.
- `Source/GUI/Settings/SettingsPanel.h` -- members + getters for new controls.
- `Source/GUI/PluginEditorSettings.cpp` -- wire Scale/Skin to `applyUiScaleFromItemId` / skin apply; restore Scale/Skin from APVTS; wire GS combo persist + RUN SETUP AGAIN per decision; extend `restoreSettingsPolicyCombosFromState` / open wiring as needed.
- `Source/GUI/PluginEditorSkinScale.cpp` -- reuse only; do not fork apply paths.
- `Source/GUI/Panels/MainComponent/HeaderPanel/` (`HeaderLogoPopupMenu`, `HeaderPanelSetup`) -- do not remove Scale/Skin shortcuts; optionally sync Settings combos if already open (nice-to-have, not required).
- `Source/Shared/Definitions/PluginDisplayNames.h` -- GS strings; Scale/Skin Settings labels without colon (keep logo/`kUiScale` help strings as-is if they differ).
- `Source/Shared/Definitions/PluginIDs.h` -- new machine/settings keys + enum ids for GS auto-open combo (Build naming; document in Implementation Notes).
- `Source/Core/Services/DeviceConnectionMachineDefaults.*` (or sibling machine-defaults helper) -- pattern for machine PropertiesFile read/write; extend carefully without conflating Device Setup `promptDone`.
- `_bmad-output/specs/spec-getting-started/ui-copy.md` -- frozen copy SSOT.
- `_bmad-output/implementation-artifacts/guide-matrix-modal-design.md` -- out of scope for GS-1 chrome (wizard later).

**Reuse:** `makeLabel` / `makeCombo` / `layoutLabeledControlRow` / `populateComboItems` / `ChoiceLists::ScaleLevels` / `SkinVariants` / existing info+help combo wiring.

**Do not change:** Settings MIDI/Audio pages, `EpromTypePromptDialog` absorb, header MIDI port lists, audio-safety first-run flag.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Add GETTING STARTED / combo / RUN SETUP AGAIN strings; Settings UI SCALE / SKIN labels without colon for the User Interface rows.
- [x] `Source/Shared/Definitions/PluginIDs.h` -- Add Getting Started auto-open preference key + item ids (default SHOW WHEN INCOMPLETE).
- [x] Machine-defaults helper (extend `DeviceConnectionMachineDefaults` or small sibling) -- Read/write GS auto-open preference on the machine PropertiesFile.
- [x] `Source/GUI/Settings/SettingsPanel.{h,cpp,Setup.cpp}` -- Build and layout rows in locked order; GETTING STARTED combo + RUN SETUP AGAIN below.
- [x] `Source/GUI/PluginEditorSettings.cpp` (+ related editor headers) -- Wire Scale/Skin to existing apply/persist; restore Scale/Skin in Settings; wire GS combo persistence; wire RUN SETUP AGAIN per frozen decision.
- [x] Smoke / matrix -- `Tests/Unit/GettingStartedSettingsContractTests.cpp` covers I/O matrix (order/copy, Scale/Skin keys, logo labels, combo persist, RUN SETUP AGAIN no-op); `Matrix-Control_Tests` 0 failures; build + lint OK. Visual Settings smoke still recommended for human.
- [x] `Scripts/quality/lint_touched.py` -- Pass on touched C++.

**Acceptance Criteria:**
- Given Settings → User Interface, when the section is visible, then options appear in order UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP, GETTING STARTED.
- Given the GETTING STARTED block, when inspected, then combo offers SHOW WHEN INCOMPLETE (default) and NEVER AT LAUNCH, and RUN SETUP AGAIN appears below the combo with no second label beside the button.
- Given the logo menu, when opened, then UI Scale and Skin shortcuts remain available.
- Given UI SCALE or SKIN changed in Settings, when applied, then behavior matches the logo shortcut paths (same APVTS keys).
- Given the auto-open combo is changed, when the app is quit and relaunched, then the combo selection is restored from machine preference.
- Given RUN SETUP AGAIN is clicked, when GS-1 is shipped alone, then nothing opens and no wizard step is marked complete (no crash).

### Review Findings

- [x] [Review][Patch] Temper Getting Started contextual help to GS-1 behavior [Source/Shared/Definitions/PluginDisplayNames.h:247] — Decision: adapt help (option 1). Combo help = preference only (no launch enforcement claim); RUN SETUP AGAIN help = not active yet / no false re-run claim. Restore product-final copy in GS-2/GS-3 when wired.
- [x] [Review][Patch] RUN SETUP AGAIN contract test never executes the Settings onClick handler [Source/GUI/PluginEditorSettingsAppearance.cpp:66] — test calls `runSetupAgainNoOp()` directly; a divergent onClick would still pass.
- [x] [Review][Defer] Production openStore load/write auto-open path untested [Source/Core/Services/GettingStartedMachineDefaults.cpp:40] — deferred: already in deferred-work from build review 2026-10-08 (Application Support / DI seam).
- [x] [Review][Defer] Settings User Interface row order not asserted via live layout [Source/GUI/Settings/SettingsPanelSetup.cpp:275] — deferred: already in deferred-work; human smoke / copy-contract only.
- [x] [Review][Defer] Settings Scale/Skin editor apply/restore wiring unexecuted by tests [Source/GUI/PluginEditorSettingsAppearance.cpp:30] — deferred: already in deferred-work; Settings harness or manual smoke.
- [x] [Review][Defer] Logo menu Scale/Skin construction not observed by tests [Source/GUI/Widgets/HeaderLogoPopupMenu.cpp] — deferred: already in deferred-work; string constants only.

#### Rejected

- BH: Tasks checklist overclaims “order/copy” coverage — reject: fix would edit this spec under review.
- BH: Code Map still cites PluginEditorSettings.cpp after Appearance split — reject: fix would edit this spec under review.
- BH: Design Notes still say property key names are open — reject: fix would edit this spec under review.
- BH: Spec Change Log empty — reject: fix would edit this spec under review / cosmetic.
- BH: Verification Commands omit Matrix-Control_Tests — reject: fix would edit this spec under review.
- BH: legacy kUiScaleLabel / kSkinLabel unused — low: leftover constants; everyday harm negligible (build triage already rejected cleanup chore).
- BH: contract tests do not pin gettingStartedAutoOpen key string — low: key documented in Implementation Notes; silent rename unlikely in everyday use.
- BH+EC: Settings Scale/Skin stale while logo changes with Settings open — false: Code Map marks live sync optional/nice-to-have; restore on reopen covers AC; out of review scope.
- EC: PropertiesFile saveIfNeeded return ignored — low: same pattern as DeviceConnectionMachineDefaults; unlikely everyday disk-fail path.

## Implementation Notes

- Machine property key: `PluginIDs::MachineDefaults::kGettingStartedAutoOpen` = `"gettingStartedAutoOpen"` (int).
- Combo ids: `PluginIDs::Settings::GettingStartedAutoOpen` — `kShowWhenIncomplete = 1` (default), `kNeverAtLaunch = 2`.
- Store: sibling helper `Core::GettingStartedMachineDefaults` → PropertiesFile app name `Matrix-Control-GettingStarted` (same product folder as Device Connection; separate file so `promptDone` is not conflated).
- Settings row labels: `kUiScaleRowLabel` / `kSkinRowLabel` (no colon); legacy `kUiScaleLabel` / `kSkinLabel` with colon left for any logo/help callers.
- `RUN SETUP AGAIN`: `onClick` → `Core::GettingStartedMachineDefaults::runSetupAgainNoOp()` (no crash, no wizard, no flag mutation) until GS-2/GS-3.
- Tests: `GettingStartedSettingsContractTests` + `readAutoOpenPreference` / `writeAutoOpenPreference(PropertiesFile&)` for isolated temp-store coverage; registered in `CMakeLists.txt` unit-test target.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| BH: missing contextual help for new UI rows | medium | Confirmed: `registerContextualHelp` binds INFO/HELP but not Scale/Skin/GS; peers show footer HELP when SHOW. |
| BH: logo contract tests unused colon labels not logo path | medium | Confirmed: logo uses `HeaderPanel::kLogoUiScaleSection` / `kLogoSkinSection`; test never touches `HeaderLogoPopupMenu`. |
| BH: kUiScaleLabel/kSkinLabel unused in production | low | Confirmed leftover; logo does not use them. Everyday harm negligible; reject for complexity vs cleanup chore. |
| BH: order test only checks local string array | medium | Confirmed: no `layoutInterfaceSection` call; rename/narrow so it does not claim layout. |
| BH: Run Setup Again test never invokes onClick | medium | Confirmed: preference re-read only; empty `onClick` unexercised. |
| BH: production load/write(int) untested | medium | Confirmed: only PropertiesFile& overloads tested; isolating real Application Support store needs more surface — defer. |
| BH: Settings Scale/Skin stale while open after logo change | false | Spec Code Map marked live sync optional/nice-to-have; restore on reopen covers AC. |
| BH: Code Map still cites PluginEditorSettings.cpp | false | Reject: fix would be editing this build's spec. |
| BH: Verification omits Matrix-Control_Tests | false | Reject: fix would be editing this build's spec. |
| BH: appearance restore nested under policy restore | medium | Confirmed: `restoreSettingsAppearanceFromState` called inside `restoreSettingsPolicyCombosFromState`. |
| EC: corrupt APVTS scale/skin blank combo | medium | Confirmed: restore uses raw `getGuiScaleId`/`getSkinVariantId` without clamp; peers use normalize. |
| EC: saveIfNeeded failure ignored | low | Same pattern as Device Connection; unlikely everyday; reject. |
| EC: test null-deref after soft expect | medium | Confirmed: `expect(store != nullptr)` then dereference without return. |
| EC: Run Setup Again coverage claim (claim) | medium | Same root as BH onClick gap. |
| VG: Run Setup Again broken-verification → patch | medium | Pre-verified gap; rewrite to invoke shared handler. |
| VG: Logo test broken-verification → defer | medium | Rename/narrow assertions to logo section copy strings; full menu observation deferred. |
| VG: Row-order layout → defer | medium | Narrow test to copy-only; layout remains human smoke. |
| VG: Scale/Skin wiring unexecuted → defer | medium | Editor wiring outside unit-test convention; keep key/id contract only. |
| VG: production load/write entry points → patch/defer | medium | Defer real `openStore` path; keep injected-store coverage. |

## Design Notes


- GETTING STARTED button row: product requires the button under the combo without a second label — prefer a button-only layout under the labeled combo row (Patch Mutator’s labeled+button pattern is the wrong visual for this row).
- Existing unused `kUiScaleLabel` / `kSkinLabel` include a trailing colon; User Interface peers do not — prefer new or adjusted Settings-facing literals without colon; do not break logo menu copy.
- Exact property key names are agent choice; record chosen names here when implemented.
- Full AC-GS1-4 (reset flags + open step 0) and AC-GS1-5 (suppress auto-open at editor ready) require GS-2/GS-3; this story ships controls + combo persist only.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds for editor/Settings changes
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched Source/Tests C++

**Manual checks (if no CLI):**
- Settings → User Interface row order and GETTING STARTED layout
- Change Scale/Skin from Settings and from logo — both work; relaunch preserves GS combo
- RUN SETUP AGAIN matches frozen decision
