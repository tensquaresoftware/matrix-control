---
title: 'System INIT templates — fixed folder, Settings SAVE AS INIT, Patch Name sentinel'
type: 'feature'
created: '2026-09-08'
status: 'done'
baseline_commit: 'eddf49d8f34210f47e6a40e6849f4cddf3238d4e'
route: 'dispatch'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Users cannot cleanly author Patch/Master init templates: filenames were treated like library Save As targets, the templates folder was a free session path with no Settings UI, Master Settings ops are still “Coming soon…”, and after INIT the Patch Name `INIT` does not read as a system sentinel.

**Approach:** Keep on-disk names `PatchInit.syx` / `MasterInit.syx` in a **fixed** OS app-data `Init/` folder (no Settings browse). Author via Settings **SAVE AS INIT** (Patch + Master). Ship Master Settings `UTILITY` (`LOAD` | `SAVE AS` | `INIT`) plus `INIT TEMPLATE` (`SAVE AS INIT`). After Internal Patches INIT, force Patch Name `* INIT *` and block Computer Patches Save until the user picks a Matrix-legal name.

## Boundaries & Constraints

**Always:**
- Filenames remain `PatchInit.syx` / `MasterInit.syx` (`InitTemplateLoader` SSOT).
- Templates folder = fixed product app-data location + `Init/` subfolder (create on demand); ignore prior empty/custom `settingsInitTemplatesFolderPath` as the live resolve path (migrate wiring off the free-form property or stop using it for resolve).
- Init load stays outside Computer Patches / D-025 reconciliation (D-040).
- Missing or invalid template → `InitDefaults` fallback + footer naming the expected file.
- Settings PATCH: row `INIT TEMPLATE` with `SAVE AS INIT` → write current patch to `PatchInit.syx` (prefer blank name bytes in the written file so the live sentinel is not baked in).
- Settings MASTER: row `UTILITY` with `LOAD` | `SAVE AS` | `INIT` (order fixed; `INIT` = global apply of `MasterInit.syx` to all Master modules, with confirmation consistent with Master init risk); row `INIT TEMPLATE` with `SAVE AS INIT` only.
- Widen Settings control column as needed so multi-button rows fit (asymmetric widths OK).
- After Internal Patches INIT: Patch Name = `* INIT *` (runtime); block Save / Save As while that sentinel is showing; Save As suggests empty stem.
- Module-level Master header **I** unchanged (single-module init from the same template).
- Manual: discourage Finder edits of the system `Init/` files.
- Update decision-log / PRD notes where D-034a “user-chosen Settings folder” / “Save as default init” wording must match this lock.

**Never:**
- Do not add Settings `INIT FOLDER` browse.
- Do not require users to Save As / Finder-rename to create init templates.
- Do not use Computer Patches library files as init templates.
- Do not silently save sentinel as `INIT.syx` or strip `*` into a spaced stem.
- Do not add Matrix Mod init files.
- Do not change cold-start / session-idle Patch Name `--------`.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Patch INIT, template present | Valid `PatchInit.syx` in fixed `Init/` | Full patch from file; Patch Name forced to `* INIT *` | N/A |
| Patch INIT, missing/invalid | No usable file | `InitDefaults` + footer; still `* INIT *` after successful init path | Fallback |
| SAVE AS INIT (Patch) | User confirms in Settings | Writes `PatchInit.syx` under fixed `Init/`; blank name bytes preferred | Unwritable → footer |
| Save / Save As with sentinel | Patch Name is `* INIT *` | Blocked until user renames to Matrix-legal name; Save As empty suggestion | Footer guidance |
| Master SAVE AS INIT | Current Master in editor | Writes `MasterInit.syx` under fixed `Init/` | Unwritable → footer |
| Master Settings INIT | User confirms | All Master modules from `MasterInit.syx` (or hardcoded fallback) | Confirm cancel aborts |
| Master LOAD / SAVE AS | User picks `.syx` | Load into editor / export current Master | Existing file errors |
| Master module header I | Single module | Slice from same template as today | Confirm as today |

</frozen-after-approval>

## Code Map

- `Source/Core/Init/InitTemplateLoader.h` / `.cpp` -- filename SSOT + resolve under supplied folder
- `Source/Shared/ProjectPaths.*` -- extend (or sibling helper) for fixed app-data `Init/` directory
- `Source/Shared/Definitions/PluginIDs.h` -- retire or stop using `kInitTemplatesFolderPath` for live resolve
- `Source/Core/PluginProcessorConstruction.cpp` / `PluginProcessorClipboard.cpp` -- wire templates folder supplier to fixed path
- `Source/Core/Init/PatchInitService.*` / `PatchModuleInitService.*` / `MasterModuleInitService.*` -- load from fixed folder
- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` -- INIT → `* INIT *` (replace blank→`INIT` policy)
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `kInitPatchName` → `* INIT *`; Settings labels/buttons
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` -- block Save/Save As on sentinel; empty suggested stem
- `Source/Core/Services/PatchFileNameSanitizer.*` -- sentinel detection helper if needed (reuse charset rules)
- `Source/GUI/Settings/SettingsPanel*.*` -- PATCH INIT TEMPLATE + SAVE AS INIT; MASTER UTILITY + INIT TEMPLATE rows; widen layout
- `Tests/Fixtures/Init/*` + unit tests -- fixed-path / SAVE AS INIT / sentinel / Master Settings actions as scoped
- Decision-log D-034 / D-034a / D-035 / D-040 -- align folder + Settings chrome wording

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/ProjectPaths.*` (or dedicated helper) -- resolve/create fixed `…/Matrix-Control/Init/` -- SSOT location
- [x] `InitTemplateLoader` + processor suppliers -- always resolve via fixed folder; drop free-form path for runtime -- one place to look
- [x] `PluginDisplayNames.h` + Settings panel -- PATCH/MASTER chrome per Intent; widen dual-button rows -- UI SSOT
- [x] Core/Settings actions -- Patch/Master SAVE AS INIT writers; Master LOAD / SAVE AS / INIT (all modules) -- FR-18 scope
- [x] `PatchManagerActionHandlerInternalPatches.cpp` + display names -- force `* INIT *` after full-patch INIT -- sentinel
- [x] Computer Patches save path -- refuse sentinel; empty Save As suggestion -- no bogus library stems
- [x] Tests for I/O matrix (load/missing, write init, sentinel gate, Master INIT confirm path as feasible) -- regression
- [x] Decision-log / PRD filename+folder notes -- planning SSOT matches product lock
- [x] Manual note path for user docs (discourage Finder edits) -- if touched this Build, else follow-up

**Acceptance Criteria:**
- Given fixed `Init/PatchInit.syx` exists, when Internal Patches INIT runs, then the patch loads from that file and Patch Name shows `* INIT *`.
- Given Settings PATCH `SAVE AS INIT`, when invoked, then `PatchInit.syx` is written under the fixed `Init/` folder.
- Given Patch Name is `* INIT *`, when Save or Save As is attempted, then it is blocked until a Matrix-legal name is set.
- Given Settings MASTER `SAVE AS INIT`, when invoked, then `MasterInit.syx` is written under the same folder.
- Given Settings MASTER `INIT` (with confirm), when confirmed, then all Master modules reset from the Master init template (or hardcoded fallback).
- Given Settings MASTER `LOAD` / `SAVE AS`, when used, then Master loads from / exports to a user-chosen `.syx` without using the Computer Patches combobox as init storage.
- Given no Settings folder browse control, when Settings opens, then no `INIT FOLDER` row appears.

### Review Findings

Combined code review (`eddf49d8...HEAD`, 2026-09-09) with `spec-settings-delete-init-template.md`.

- [x] [Review][Decision→Defer] After INIT, protect hardware from `* INIT *` on STORE (button stays enabled; require valid Patch Name via proper PATCH NAME UI modes) — deferred: pause this review; implement PATCH NAME display modes first, then resume review / wire STORE. Audition policy TBD in that follow-up.
- [ ] [Review][Patch] SAVE AS INIT silent no-op when processor deps are null — publish footer instead of bare `return` [`PluginProcessorInitTemplates.cpp:36-38` / `50-51`] (left as action item while PATCH NAME chantier runs)
- [ ] [Review][Patch] Add `initAllModules` empty-folder fallback unit test [`MasterModuleInitService` / `InitTemplateWriterTests.cpp`] (left as action item while PATCH NAME chantier runs)
- [x] [Review][Defer] Live AppData Init/ no-arg resolve path untested — deferred: pre-existing verification gap; tempDir overloads cover I/O logic
- [x] [Review][Defer] `loadMasterFromUserFile` failure does not assert MasterModel unchanged — deferred: secondary isolation gap; success path covered
- [x] [Review][Defer] Mutator export/history basename can use sentinel `* INIT *` — deferred: out of frozen AC scope (Computer Patches Save/Save As only)
- [x] [Review][Defer] Master UTILITY SAVE AS chooser starts at process CWD — deferred: UX polish; AC does not require a Documents start folder

#### Rejected
- Settings height overflow in plugin mode (`false`) — content ~423 px vs design 456 (1 px spare at 100%); not ~23 px overflow
- Legacy custom Init folder migration (`false`) — frozen intent ignores prior free-form path
- DELETE vs “SAVE AS INIT only” on Master INIT TEMPLATE (`false` for code) — superseded by sibling DELETE spec in same commit; do not remove DELETE
- Orphaned `kComingSoon` (`false`) — still used by Patch Mutator Defrag History placeholder
- Unused `MasterGlobalInitConfirm::kConfirm`/`kCancel` (`low`) — identical to per-module strings; prior review already accepted
- Generic Master SAVE AS INIT fail footer overwriting `kMasterFileFailed` (`false`) — intentional prior patch for INIT TEMPLATE vocabulary
- InitTemplateWriter also owns Master LOAD/user SAVE (`low`) — intentional Core SSOT for this chrome; no demonstrated caller divergence
- Duplicated Settings footer helpers (`low`) — both paths work; no named harm
- AppData Init create-fail UX / exists creates empty Init/ (`low` / intentional) — create-on-demand is frozen behavior
- Manual omits STORE sentinel warning (`low`) — follows Computer Patches-only AC until Decision above

## Implementation Notes

- Fixed Init/ under Application Support; Settings PATCH/MASTER chrome locked (INIT TEMPLATE, UTILITY).
- Review patches: writer directory overloads + stronger InitTemplateWriter tests; display-name SSOT for Master file dialogs; rename footer and Master init fail copy; manual typo/confirm/blank-name notes.

## Spec Change Log

## Review Triage Log
| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Manual omits full OS path to Init/ | low → defer | Intent hides folder; full path is support docs, not blocking |
| Manual typo Preférer | low → patch | Fixed |
| Manual omits Master INIT confirm | low → patch | Fixed (brief note) |
| No overwrite confirm on SAVE AS INIT | false | Intent: dedicated write, no confirm dialog required |
| Manual omits blank name bytes | low → patch | Fixed (short note) |
| No migration for old folder property | false | Property never had Settings UI; intent: ignore old path |
| D-042 stale mockup follow-up | low → defer | Pre-existing decision-log hygiene |
| Spec empty notes / status done claim | false | Status was in-review; notes filled at close |
| writePatchInit test never called API | high → patch | Fixed: overload + test calls writer |
| Missing Master INIT fallback unit case | low → reject | Fallback already via InitTemplateLoader; low everyday risk |
| FileChooser titles outside PluginDisplayNames | medium → patch | Fixed: SSOT titles |
| MasterGlobalInitConfirm button captions unused | false | kConfirm/kCancel identical to per-module ("Reset"/"Cancel") |
| Master init write uses generic fail footer | medium → patch | Fixed: kInitTemplateWriteFailed on writeMasterInit fail |
| Duplicate global vs module confirm open helpers | low → defer | Works; DRY later |
| Rename footer says Save not SAVE/SAVE AS | low → patch | Fixed |
| Init/ createDirectory parents missing | false | juce::File::createDirectory creates parents |
| create-on-demand claim false | false | Same as above |
| writePatchInit blanksNameBytes claim (edge) | high → patch | Fixed with writer call |
| writeMasterInit untested (verification-gap) | high → patch | Fixed |
| initAllModules no module-state assert (vg) | high → patch | Fixed |
| kSavePatchFile sentinel untested (vg) | high → patch | Fixed |
| Dead suggestedStem expect in Save As test | low → patch | Removed dead callback assert |

## Design Notes

**Filename decision:** Keep `PatchInit.syx` / `MasterInit.syx` — user never types them; dedicated SAVE AS INIT writes them.

**Fixed folder:** Product app-data (`Ten Square Software/Matrix-Control/Init/` via existing Application Support pattern). No browse reduces accidental delete/move/mix with Computer Patches.

**Master chrome:** `UTILITY` hosts `LOAD` | `SAVE AS` | `INIT` (echoes M4L prototype + Bank Utility vocabulary). `INIT TEMPLATE` hosts only `SAVE AS INIT` so `INIT` is not misread as “init the template.” Widen panel / asymmetric widths for the three-button row.

**Sentinel `* INIT *`:** Runtime only after Internal Patches INIT; template file should store blank name bytes when written via SAVE AS INIT.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlTests` -- unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks:**
- SAVE AS INIT Patch/Master → files appear under fixed `Init/`
- Internal Patches INIT → `* INIT *`; Save blocked until rename
- Master Settings INIT → all modules reset after confirm
- No INIT FOLDER row in Settings
