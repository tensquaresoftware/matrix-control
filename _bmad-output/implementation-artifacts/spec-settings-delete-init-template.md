---
title: 'Settings DELETE for system INIT templates'
type: 'feature'
created: '2026-09-08'
status: 'done'
baseline_commit: 'eddf49d8f34210f47e6a40e6849f4cddf3238d4e'
route: 'dispatch'
review_loop_iteration: 0
context:
    - '{project-root}/_bmad-output/project-context.md'
    - '{project-root}/_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After SAVE AS INIT, users cannot remove a custom system init file from Settings; the only recovery is Finder delete or overwriting via another SAVE AS INIT, so hardcoded InitDefaults stay unreachable from the product UI.

**Approach:** Add a DELETE button beside each SAVE AS INIT (PATCH and MASTER INIT TEMPLATE rows). DELETE removes that row’s system file (`PatchInit.syx` or `MasterInit.syx`) from the fixed Application Support `Init/` folder so the next INIT falls back to InitDefaults. Enable only when the file exists; refresh that state when Settings opens, after SAVE AS INIT, and after DELETE.

## Boundaries & Constraints

**Always:**
- Layout per INIT TEMPLATE row: `[ SAVE AS INIT 68 ]` + 4 px gap + `[ DELETE 68 ]` = 140 px control column (same height 20 as sibling buttons; scale with `uiScale_`).
- DELETE targets only the fixed-folder system file for that row (`InitTemplateLoader` filename SSOT + `ProjectPaths::getInitTemplatesDirectory()`).
- After successful delete, missing template → existing InitDefaults fallback path (unchanged load/init behavior).
- Grey (`setEnabled(false)`) when the corresponding file is absent; enabled when `existsAsFile`.
- Refresh enablement: Settings open/show, after SAVE AS INIT success, after DELETE attempt (success or fail that may change presence).
- Footer feedback on delete success / failure (English ALL-CAPS product vocabulary for the button label `DELETE`).
- Before delete: light confirm via `showOrderedConfirmAlert` (Cancel / DELETE; Escape cancels). No delete on cancel. Do not use `MasterInitConfirmDialog` for this path.

**Never:**
- Do not add INIT FOLDER browse or change fixed filenames / folder.
- Do not delete anything outside the fixed `Init/` system templates.
- Do not change Master UTILITY (`LOAD` | `SAVE AS` | `INIT`) or Master header **I** behavior.
- Do not use Computer Patches paths as delete targets.
- Do not re-open the done SAVE AS INIT / sentinel story scope.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Open Settings, file present | `PatchInit.syx` or `MasterInit.syx` exists | Matching DELETE enabled | N/A |
| Open Settings, file absent | No file | Matching DELETE greyed | N/A |
| DELETE success | User confirms (if required) + file exists | File removed; button greys; footer success; next INIT uses InitDefaults | N/A |
| DELETE when already absent | Race / stale UI | No-op or fail footer; button stays grey after refresh | Footer warning |
| DELETE I/O failure | `deleteFile` fails | File may remain; footer warning; refresh enablement from disk | Warning footer |
| SAVE AS INIT then | Write succeeds | Matching DELETE becomes enabled | Unchanged write errors |
| Cancel confirm | User cancels | No delete; enablement unchanged | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsPanel.h` / `SettingsPanelSetup.cpp` / `SettingsPanel.cpp` — add `patchDeleteInitButton_` / `masterDeleteInitButton_` (width 68); extend `layoutButtonRow` for INIT TEMPLATE to two buttons + widths; expose getters; assert `68+4+68 <= kComboWidth_` (140)
- `Source/Shared/Definitions/PluginDisplayNames.h` — `kDeleteButton` / footer strings for delete success & failure (Patch + Master or shared)
- `Source/GUI/PluginEditorSettings.cpp` — wire DELETE clicks; call refresh helper; hook SAVE AS INIT success path to refresh; Settings show/open refresh
- `Source/GUI/PluginEditorAlerts.cpp` / `PluginEditorInternal.h` — reuse `showOrderedConfirmAlert` if confirm chosen (not `MasterInitConfirmDialog`)
- `Source/Core/PluginProcessor.h` / `PluginProcessorInitTemplates.cpp` — `deletePatchInitTemplate` / `deleteMasterInitTemplate` (or shared) + `publishSettingsFooter`; existence helpers for UI refresh
- `Source/Core/Init/InitTemplateWriter.*` (or thin sibling) — delete via `resolvePatchInitFile` / `resolveMasterInitFile` + `juce::File::deleteFile`; keep Core free of GUI
- `Source/Core/Init/InitTemplateLoader.*` / `Source/Shared/ProjectPaths.*` — reuse resolve + fixed folder; no path redesign
- `Tests/Unit/InitTemplateWriterTests.cpp` (or new focused tests) — delete removes file; missing file handled; existence check for enablement logic if extracted

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- add DELETE label + footer messages -- UI string SSOT
- [x] `Source/Core/Init/InitTemplateWriter.*` (or delete helper) -- delete Patch/Master init files under fixed Init/ -- Core I/O SSOT
- [x] `Source/Core/PluginProcessor.h` + `PluginProcessorInitTemplates.cpp` -- expose delete + optional exists helpers; footer on outcome -- editor calls Core only
- [x] `Source/GUI/Settings/SettingsPanel.*` + `SettingsPanelSetup.cpp` -- CREATE/layout DELETE 68 beside SAVE AS INIT on both INIT TEMPLATE rows -- chrome + layout contract
- [x] `Source/GUI/PluginEditorSettings.cpp` (+ alerts if confirm) -- wire clicks, confirm policy, refresh enablement on open / save / delete -- behavior
- [x] `Tests/Unit/InitTemplateWriterTests.cpp` -- cover delete success, absent file, and that load falls back after delete -- I/O matrix

**Acceptance Criteria:**
- Given Settings open and `PatchInit.syx` present, when the PATCH INIT TEMPLATE row is shown, then DELETE is enabled at 68×20 with 4 px gap after SAVE AS INIT within the 140 px column.
- Given the matching system file is absent, when Settings opens (or after successful DELETE), then that DELETE control is disabled.
- Given an enabled DELETE, when the user confirms in the light alert, then the system file is gone, footer reports success, and a subsequent INIT uses InitDefaults; when they cancel, nothing is deleted.
- Given SAVE AS INIT just wrote the file, when the write succeeds, then the matching DELETE becomes enabled without reopening Settings.
- Given Master UTILITY or module header I, when this story ships, then those flows are unchanged.

### Review Findings

Combined code review (`eddf49d8...HEAD`, 2026-09-09) with `spec-system-init-syx-filenames.md`. DELETE ACs themselves: no acceptance gaps.

- [x] [Review][Decision→Defer] After INIT, protect hardware from `* INIT *` on STORE via PATCH NAME UI (not greyed STORE) — deferred: same pause as sibling system-init review; resume after PATCH NAME modes chantier.
- [ ] [Review][Patch] SAVE AS INIT silent no-op when processor deps are null — publish footer instead of bare `return` [`PluginProcessorInitTemplates.cpp:36-38` / `50-51`] (action item; not applied in this pause)
- [ ] [Review][Patch] Add `initAllModules` empty-folder fallback unit test [`MasterModuleInitService` / `InitTemplateWriterTests.cpp`] (action item; not applied in this pause)
- [x] [Review][Defer] Live AppData Init/ no-arg resolve path untested — deferred: reconfirmed; already in deferred-work
- [x] [Review][Defer] `loadMasterFromUserFile` failure does not assert MasterModel unchanged — deferred: reconfirmed; already in deferred-work
- [x] [Review][Defer] Mutator export/history basename can use sentinel `* INIT *` — deferred: out of DELETE / system-init AC scope
- [x] [Review][Defer] Master UTILITY SAVE AS chooser starts at process CWD — deferred: UX polish outside DELETE ACs

#### Rejected
- See `spec-system-init-syx-filenames.md` Review Findings Rejected list (same combined triage). DELETE-focused acceptance auditor: no findings.

## Implementation Notes

- DELETE beside SAVE AS INIT (68+4+68=140) on PATCH and MASTER INIT TEMPLATE rows; enabled only when the matching fixed Init/ file exists.
- Core delete/exists SSOT on `InitTemplateWriter`; processor publishes footer; editor confirms via `showOrderedConfirmAlert` (Cancel / DELETE) then refreshes enablement on Settings open, SAVE AS INIT, and DELETE.
- Files: `PluginDisplayNames.h`, `InitTemplateWriter.*`, `PluginProcessor.h`, `PluginProcessorInitTemplates.cpp`, `SettingsPanel.*` / `SettingsPanelSetup.cpp`, `PluginEditor.h`, `PluginEditorSettings.cpp`, `InitTemplateWriterTests.cpp`.
- Verification (2026-09-08): `cmake --build --preset macos-debug-arm64` OK; `Matrix-Control_Tests InitTemplateWriter` 0 failures; `lint_touched.py` OK. No ctest preset named macos-debug-arm64 — used the test binary filter instead.
- Parent Build pass: added `deleteMasterInit_removesFileAndLoadFallsBack` + `initExists_reflectsWriteAndDelete` for matrix coverage of Master delete success and exists-driven enablement.
- Review patches: manual DELETE docs; `deletePatchInit_absentFileFails`; delete/exists guard when init folder is not a directory.

## Spec Change Log

## Review Triage Log

- BlindHunter: manual omits DELETE / recovery via Settings — medium — evidence: `manuel-utilisateur.md` INIT TEMPLATE rows document SAVE AS INIT only; product recovery path from this story is undocumented.
- BlindHunter: Init/ paragraph omits DELETE as in-product reset — medium — evidence: same manual paragraph; grouped with prior (same root cause).
- BlindHunter: no Patch absent-delete unit test — low — evidence: only `deleteMasterInit_absentFileFails`; Patch half of matrix “already absent” unchecked.
- BlindHunter: identical footer for missing vs deleteFile I/O fail — low — evidence: shared `kInitTemplateDeleteFailed`; rejected (unlikely everyday; distinguishing messages adds surface).
- BlindHunter: failure footers omit Patch/Master filename — low — evidence: success strings name files, failure does not; rejected (cosmetic).
- BlindHunter: confirm title generic (body distinguishes) — low — evidence: `DeleteInitTemplateConfirm::kTitle`; rejected (body already Patch/Master-specific).
- BlindHunter: confirm title case vs MasterInitConfirmDialog ALL CAPS — low — evidence: different dialog systems by design; rejected.
- BlindHunter: exists probe creates empty Init/ via getInitTemplatesDirectory — low — evidence: create-on-demand SSOT intentional; rejected (empty folder is harmless expected side effect).
- BlindHunter: Verification cites missing ctest preset — false — evidence: finding asks to edit this build’s spec; rejected per review rules.
- BlindHunter: success footer omits “next INIT uses defaults” — low — evidence: confirm body already states recovery; rejected.
- BlindHunter: Return key confirms DELETE — false — evidence: same Cancel/primary pattern as Mutator Delete; Escape cancels; matches approved light-confirm intent.
- BlindHunter: no locked-file I/O-failure test — low — evidence: hard to simulate; rejected.
- EdgeCase: empty/non-directory initFolder on delete/exists — medium — evidence: write guards `isDirectory`, delete/exists do not; empty File + getChildFile could resolve oddly if AppData Init/ creation fails.
- EdgeCase: external file appear while Settings open — false — evidence: frozen intent only requires refresh on open / SAVE AS INIT / DELETE, not continuous watch.
- EdgeCase: silent null early-return on SAVE AS INIT — defer — evidence: pre-existing prior SAVE AS INIT work in same dirty tree, not caused by DELETE.
- EdgeCase: AC “Master UTILITY unchanged” vs UTILITY in same diff — false — evidence: UTILITY comes from prior delivered SAVE AS INIT context; DELETE story must not regress it.
- VerificationGap: live fixed Init/ no-arg path untested — defer — evidence: pre-existing SAVE AS INIT verification gap; delete/exists logic covered via tempDir overloads.
- VerificationGap: loadMasterFromUserFile failure leaves model untouched untested — defer — evidence: Master LOAD path from prior story, out of DELETE scope.## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R InitTemplate` (or project’s unit-test target filter for InitTemplate*) -- expected: new/updated delete tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under Source/ and Tests/

**Manual checks (if no CLI):**
- Open Settings: DELETE grey when no system init files; after SAVE AS INIT, DELETE enables; DELETE then greys and INIT uses defaults; repeat for Master row.
