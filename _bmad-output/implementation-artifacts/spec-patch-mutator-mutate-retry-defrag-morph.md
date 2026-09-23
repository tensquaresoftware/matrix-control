---
title: 'Patch Mutator MUTATE/RETRY Defrag Morph'
type: 'feature'
created: '2026-09-23'
status: 'done'
route: 'dispatch'
baseline_commit: 'f3329b08f4edcac5e919b63e69b0c59724db3923'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-mutation-history-defrag.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** When the next Mutation or Retry index cannot be allocated (100 entries or gap exhaustion such as M99/R99 still present), MUTATE/RETRY are greyed out, so the history-full Defrag confirm path is unreachable from those buttons. Auto-popup at M99 interrupts creative flow; Settings DEFRAG alone is not discoverable enough at the moment of blockage. Permanently replacing the button label with DEFRAG would feel like a function change and hide what the control normally does.

**Approach:** When allocation for that action is blocked and no other disable reason applies, keep the resting label MUTATE or RETRY, leave the button enabled, and route click to the existing Defrag confirm → `defragHistory()`. On hover only, temporarily show DEFRAG using the normal red hover text colour. Keep Settings DEFRAG for voluntary tidy-up. Independent per button; no auto-modal on create.

## Boundaries & Constraints

**Always:**
- MUTATE enters the Defrag-recovery state when next root index cannot be allocated (`peekNextRootIndex` nullopt: full 100 roots or gap exhaustion) **and** at least one recipe module is enabled **and** Compare is not locking actions.
- RETRY enters the Defrag-recovery state when next retry index under the **selected root** cannot be allocated **and** selection is a valid mutation root (not INITIAL / empty) **and** Compare is not locking.
- MUTATE and RETRY recovery states are independent; History selection changes must refresh RETRY immediately.
- At rest in recovery state: label stays MUTATE or RETRY; button enabled; click opens Defrag confirm (does not stamp Mutate/Retry APVTS actions).
- On hover in recovery state only: temporary label DEFRAG with normal red hover text (existing button hover colour convention). Leave hover → restore MUTATE/RETRY.
- After successful Defrag (or a delete that restores a free index): leave recovery state; normal enable + action wiring resume.
- Confirm path = same `openMutatorHistoryDefragConfirmDialog` + same copy as Settings / limit gate → same `defragHistory()` / rich success footer.
- Separate "allocation blocked" from "disabled for other reasons" so recovery stays clickable.

**Never:**
- Enter recovery (enabled click → Defrag, or hover DEFRAG) when greyed for another cause (no module, Compare lock, INITIAL selected for RETRY, empty history for RETRY, etc.).
- Permanent resting label DEFRAG; permanent red text at rest; full red alert fill (`buttonAlertLookFromSkin` / PANIC-style).
- Auto-open a modal at M99 / R99 creation; continuous background Defrag; second Settings button; third main-panel Defrag chrome button.
- Change max+1 / gap allocation policy, Settings DEFRAG wiring, or success footer copy.
- French / non-ASCII display strings in `PluginDisplayNames`.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Root slots full | 100 roots, recipe OK, Compare off | MUTATE enabled, label MUTATE at rest; hover → DEFRAG (red hover text); click → confirm → defrag | Cancel: no store change |
| Root gap exhaustion | Only M99 remains after deletes, recipe OK | Same recovery as full (not grey MUTATE) | Same confirm path |
| Retry full under root | Selected M50 has 100 retries | RETRY in recovery; MUTATE unchanged if roots free | Selection to free root → normal RETRY |
| Retry gap under root | Selected root has R99 only | RETRY in recovery | After Defrag → normal RETRY when place exists |
| Other disable | Allocation blocked but no modules / Compare on / INITIAL for RETRY | MUTATE/RETRY stay disabled; no hover DEFRAG; click does nothing / no Defrag | N/A |
| Independent recovery | Roots exhausted, selected root has retry room | MUTATE recovery + normal RETRY | N/A |
| Unblock without Defrag | Delete frees an index while in recovery | Leave recovery; normal enable + mutate/retry click | N/A |
| No hover (touch/keyboard) | Recovery state, click without hover | Confirm still opens; dialog title carries Defrag intent | N/A |

**Decisions:**
- Keep full draft spec (slightly over token comfort band); single cohesive goal.
- Look / label: identity + hover — resting MUTATE/RETRY always; DEFRAG only while hovered in recovery; default button Look (no alert fill, no resting red text).
- Product override of prior "no Defrag control on main Patch Mutator panel": recovery reuses MUTATE/RETRY controls only (hover hint + click → confirm), not a third chrome button.
- No auto-modal at M99; voluntary click only.
- Recovery click does not stamp Mutate/Retry APVTS actions; opens Defrag confirm directly (Settings-equivalent path).

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchMutator/PatchMutatorEngineInternal.h` — `computeMutateEnabled` / `computeRetryEnabled` currently fold `peekNext*` into enabled (lines ~197–218); split allocation-blocked vs other gates.
- `Source/Core/Services/PatchMutator/PatchMutatorEngineHistory.cpp` — `refreshActionEnabledMirrors` publishes `kMutateEnabled` / `kRetryEnabled`; extend with recovery/allocation mirrors and refresh after history mutations / selection sync.
- `Source/Shared/Definitions/PluginIDs.h` — MutatorState property IDs (~692+); add recovery/allocation IDs if needed; strip in `MutatorSessionPersistence` like other ephemeral mirrors.
- `Source/GUI/Panels/.../PatchMutatorPanel.cpp` / `PatchMutatorPanelHistory.cpp` — `ActionEnabledPropertyListener` + `lockActionButton` Compare override; recovery enable + hover label swap + click → Defrag confirm instead of mutate/retry stamp.
- `Source/GUI/Widgets/Button.*` — hover/text colour path; reuse existing red hover text for temporary DEFRAG label (no new Look unless button API lacks hover text override).
- `Source/GUI/PluginEditor*` / `PluginEditorSettings.cpp` — reuse `openMutatorHistoryDefragConfirmDialog`; wire panel recovery click to same presenter + processor/engine defrag (prefer Settings wrapper for footer consistency).
- `Source/Shared/Definitions/PluginDisplayNames.h` — reuse Settings `kDefragButton` = `"DEFRAG"` for hover label.
- `Source/GUI/Looks/LookBuilders.*` — keep `buttonLookFromSkin` only for these controls; do not apply `buttonAlertLookFromSkin`.
- `Source/Core/Actions/MutatorActionHandler.*` — limit gate + `defragModalRequested` remain for programmatic mutate/retry; everyday UI recovery is the enabled click path.
- `Tests/Unit/PatchMutatorEngineEnabledResetAdvanceTests.cpp` — `enabled_rootLimit` / `enabled_retryLimit` must assert new enabled vs recovery split; gap-exhaustion cases.
- Do not change: `HistoryDefragService` algorithm; Settings DEFRAG row; allocation `peekNext*` policy.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngineInternal.h` (+ History refresh) -- Split mutate/retry "other gates" vs "allocation blocked"; publish mirrors -- UI can enable recovery without greying the only click path.
- [x] `Source/Shared/Definitions/PluginIDs.h` + `MutatorSessionPersistence.*` -- Add ephemeral recovery/allocation property IDs; strip on session persist -- mirrors stay non-persisted.
- [x] `Source/GUI/Panels/.../PatchMutatorPanel*.*` (+ Button hover if needed) -- Recovery enable, resting MUTATE/RETRY, hover DEFRAG with red hover text, Compare lock, selection refresh; recovery click → Defrag confirm hook -- main UX.
- [x] `Source/GUI/PluginEditor*.*` -- Bind panel recovery click to shared confirm + `defragMutationHistory` / engine -- one confirm family.
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Ensure DEFRAG hover label SSOT (reuse Settings string) -- ASCII English.
- [x] `Tests/Unit/PatchMutatorEngineEnabledResetAdvanceTests.cpp` (+ gap cases if missing) -- Lock enabled vs recovery predicates for full and gap exhaustion; after defrag/delete leave recovery.

**Acceptance Criteria:**
- Given root allocation blocked and recipe modules on with Compare off, when the panel refreshes, then MUTATE stays labeled MUTATE at rest, is enabled, shows DEFRAG only while hovered, and click opens Defrag confirm; confirm runs `defragHistory()` and rich footer.
- Given retry allocation blocked under the selected root (and valid root selection), when the panel refreshes, then RETRY enters the same recovery pattern independently of MUTATE.
- Given allocation blocked but no modules (or Compare on, or INITIAL selected for retry), when the panel refreshes, then the control stays disabled as MUTATE/RETRY with no hover DEFRAG and no Defrag confirm on click.
- Given recovery active, when Defrag succeeds or a delete frees an index, then hover DEFRAG stops and normal mutate/retry click wiring resumes.
- Given History selection changes between a full root and a free root, when selection updates, then RETRY recovery toggles accordingly without requiring a plugin relaunch.

## Implementation Notes

- Reused `PluginDisplayNames::Settings::kDefragButton` for hover label (no new PluginDisplayNames literal).
- `ActionEnabledPropertyListener` ANDs Compare lock so recovery-enabled mirrors cannot re-enable buttons during Compare.
- Added `PatchMutatorEngineDefragRecoveryEnabledTests` (gap, no-module, delete-leave, selection toggle, INITIAL, independent root-full); advance cases in `PatchMutatorEngineAdvanceTests`; limit Defrag handler confirm in `MutatorActionHandlerDefragTests`.
- Hover/no-hover GUI label swap is code-path covered (`refreshMutateRetryHoverLabels`); click without hover still opens confirm via `connectMutateOrRetryButton`. Remaining smoke is manual Standalone.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---|---|---|
| No automated panel recovery-click test | low | Repo GUI not unit-tested; Core mirrors covered. Deferred (same as verification-gap disposition). |
| Panel path defragHistory + rich footer untested | low | Panel calls same `defragMutationHistory` / engine path already covered by Defrag footer tests; wiring gap only. Deferred with GUI click coverage. |
| Recovery mirrors stay true while Compare active | medium | Frozen Always requires Compare not locking for recovery. Engine publishes recovery without reading Compare; UI ANDs Compare only. Patch: gate published recovery on `!compareActive`. |
| No Compare+recovery button test | low | GUI boundary; Compare lock in `applyEnabled` is real. Deferred; engine gate patch reduces risk. |
| Contextual help still create/reroll only | false | Frozen intent uses confirm dialog for Defrag meaning without hover; help text change not required. |
| Silent no-op if recovery handler unset | low | Binding runs after `createUiShell` so handler is set in normal ctor; everyday path fine. Patch: `jassert` on recovery click without handler. |
| Button/Look not touched for red hover | false | Spec requires reusing existing red hover text; no new Look. |
| Allocation mirrors unused by production UI | false | Intentional ephemeral mirrors for predicates/tests; recovery flags drive UI. |
| Sticky hover after dialog dismiss | low | Cosmetic; label refreshes on next mouse enter/exit. Unlikely everyday fuss; reject (not a trivial unique patch worth complexity). |
| Defrag handler tests duplicate harness / no retry limit twin | low | Refactor hygiene; not user-facing. Reject. |
| Enabled mirrors semantic break undocumented | medium | `kMutateEnabled` no longer means "will mutate"; undocumented. Patch: comment at PluginIDs. |
| PluginEditorWindows format-only churn | low | Pure reformat of `isEscapeBlockedByOverlay`. Patch: revert to prior formatting. |
| Edge: recovery click with empty handler | low | Same as silent no-op; covered by jassert patch. |
| Edge: dialog dismiss while cursor over button | low | Same as sticky hover; rejected. |
| Persistence strip asserts never seed new recovery IDs | medium | Pre-verified verification-gap: seed omits the four IDs so strip regression still passes. Patch: seed true before strip checks. |
| Verification-gap: GUI recovery click untested | low | Carried defer — Core rules exclude panel harness. |
| Verification-gap: Compare lock at button untested | low | Carried defer — GUI boundary. |
| Other: hover path not executed by tests | false | Noted; manual Standalone is the project check for hover paint. |
| Other: limit-gate Defrag test is not morph UX | false | Accurate observation; not a defect. |

## Design Notes

Today `computeMutateEnabled` requires `peekNextRootIndex`, so greying blocks the only UI path that used to raise the limit modal. Recovery needs a distinct predicate: allocation blocked **and** other gates pass → enabled + Defrag-on-click + hover DEFRAG; allocation blocked **and** other gates fail → MUTATE/RETRY disabled (no recovery). Prefer publishing explicit recovery/allocation mirrors over overloading a single bool.

Hover DEFRAG is progressive disclosure for mouse users; the confirm dialog remains the contract for click-without-hover (touch/keyboard). Limit-gate `defragModalRequested` after a live mutate/retry remains valid for tests/programmatic calls.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- Run focused unit tests covering enabled/recovery mirrors and defrag footer -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Fill or gap-exhaust roots → MUTATE enabled with MUTATE at rest; hover shows DEFRAG in red; cancel confirm leaves history; confirm renumbers and restores normal MUTATE when room exists.
- Fill retries under one root; switch selection to a free root → RETRY leaves recovery; switch back → recovery again.
- Settings DEFRAG still works with empty-history disable and same confirm.
