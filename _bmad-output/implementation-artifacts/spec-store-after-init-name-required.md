---
title: 'STORE after INIT name-required gate'
type: 'feature'
created: '2026-09-09'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '77cdbf53ed05f7ed36b4c13dc6c04d9f9863d75b'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-name-display-modes.md'
  - '{project-root}/_bmad-output/implementation-artifacts/deferred-work.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After Internal Patches INIT, STORE still writes the patch including sentinel name `* INIT *` to the synth. The PATCH NAME name-required UI contract is already shipped, but nothing arms it or holds STORE until a real Matrix name is committed.

**Approach:** Gate `handleInternalPatchStore` on the same init-name sentinel used by Save/Save As: when active, arm name-required, remember a pending STORE, and only run the real SysEx store after a successful name-required outcome. Cancel / empty commit aborts the pending STORE. STORE stays clickable (no grey-for-sentinel).

## Boundaries & Constraints

**Always:**
- Reuse `isInitPatchNameSentinelActive` / sentinel detection shared with Save (no parallel “is INIT name” rules).
- Reuse shipped panel APIs: `armNameRequired` / `clearNameRequired` / `setNameRequiredOutcomeHandler` (+ Core `resolveNameRequiredCommitEndState`).
- Keep STORE enabled under existing ROM / Compare rules only — do not grey STORE for sentinel.
- Pending STORE applies to every call path into `handleInternalPatchStore` (Internal STORE button and unsaved Persist→STORE).
- Re-click STORE while sentinel / name-required still applies re-arms name-required and keeps/replaces the pending STORE.

**Never:**
- Grey STORE for sentinel; footer-only “clear name” gate; rewrite the five PATCH NAME presentation modes; reopen DELETE / SAVE AS INIT / Init folder work; expand Mutator export sentinel unless a tiny Open Question asks for it; French UI/source strings.

**Decisions:**
- Pending-STORE name-required success: update the name in the plugin model, but **suppress** the rename outbound audition (`sendFullPatchForAudition`). Only the subsequent STORE `sendPatch` goes to the device (one write to the current RAM slot with the valid name embedded). Applies to Matrix-1000 and Matrix-6/6R (on 6/6R audition already equals slot write — suppress avoids a duplicate 0x01).
- Name-required is **not** a UI lock: Escape, outside click, blur, or context interrupts that cancel the edit abort the pending STORE (no SysEx). Re-click STORE while sentinel remains active re-arms name-required and replaces/restores pending STORE (implementation must order cancel-then-rearm so the new pending is not lost).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| STORE while sentinel | INIT left `* INIT *`; user clicks STORE | No store SysEx; arm name-required; pending STORE set | N/A |
| Name-required success | Non-empty Matrix-legal name + Enter | Commit name; outcome true; then execute real STORE to current slot | Illegal char: existing ignore + footer |
| Name-required cancel | Escape / blur / outside click | Outcome false; clear pending STORE; no store SysEx | N/A |
| Empty / spaces Enter | Empty or whitespace Return in name-required | Keep previous name (often `* INIT *`); outcome false; abort pending STORE | Must not unlock STORE on kept sentinel |
| Re-click STORE | Sentinel still active / after abort | Re-arm name-required; pending STORE active again | N/A |
| STORE without sentinel | Normal named patch | Existing STORE path unchanged (sendPatch + snapshot) | ROM/Compare gates unchanged |
| Persist→STORE + sentinel | Unsaved modal Persist chooses STORE while sentinel | Same gate as button STORE (arm + pending; no immediate SysEx) | N/A |
| Name-required success + pending STORE | Non-empty Enter while STORE pending | Local name commit only (no audition SysEx); then STORE `sendPatch` once | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` (`handleInternalPatchStore` ~480–509) — insert sentinel gate before SysEx; call UI request hook; on clear path keep existing sendPatch body
- `Source/Core/Actions/PatchManagerActionHandlerInitSentinel.cpp` / `isInitPatchNameSentinelActive` — reuse for STORE gate; do not change Save refuse semantics
- `Source/Core/Actions/ActionExecutionHooks.h` (+ Editor hook install site) — add a Core→GUI request (e.g. `requestNameRequiredBeforeStore`) so Core never includes GUI types
- `Source/GUI/PluginEditorUiConstruction.cpp` (`wirePatchEditDisplayBindings` ~62–83) — `setNameRequiredOutcomeHandler`; arm on hook; success → clear pending + re-enter store; failure → abort pending
- `Source/GUI/Panels/.../Modules/PatchNameDisplayPanel.{h,cpp}` — reuse arm/clear/outcome only; touch only if wiring needs a tiny accessor
- `Source/GUI/Widgets/PatchNameDisplay.*` — do not change presentation contract unless a wiring bug forces a minimal fix
- `Source/Core/PluginProcessorClipboard.cpp` (`commitPatchNameRename` ~323–354) — audition policy decision applies here or at the name-required commit wrapper
- `Source/Core/Services/PatchNameEditRules.*` + `Tests/Unit/PatchNameEditRulesTests.cpp` — already cover end-state; add handler/pending unit coverage if extractable without GUI
- `Source/GUI/Panels/.../InternalPatchesPanel.cpp` (`updatePasteStoreEnabled` / wire STORE) — leave ROM/Compare enable rules alone
- `_bmad-output/implementation-artifacts/deferred-work.md` — close/supersede 2026-09-09 STORE wiring entries; note Settings Init review resume (GPC B)
- Continuity: `spec-patch-name-display-modes.md` (done); parent product why `spec-system-init-syx-filenames.md`

## Tasks & Acceptance

**Execution:**
- [x] `ActionExecutionHooks.h` (+ Editor install) -- add name-required-before-STORE request hook -- Core asks GUI to arm without depending on GUI
- [x] `PatchManagerActionHandlerInternalPatches.cpp` (+ header if needed) -- gate STORE on sentinel; invoke hook + remember pending; execute real store only when allowed -- prevent `* INIT *` SysEx store
- [x] `PluginEditorUiConstruction.cpp` / Editor helpers -- install `setNameRequiredOutcomeHandler`; arm on hook; map outcome true/false to complete/abort pending STORE -- product loop
- [x] Rename/commit path -- when committing name-required with pending STORE, update model/APVTS but suppress `sendFullPatchForAudition`; STORE alone sends SysEx -- frozen one-write policy (M-1000 + M-6/6R)
- [x] Unit tests (handler / sentinel / pending outcome extractable Core) + optional harness outcome assert -- lock gate semantics
- [x] `deferred-work.md` (+ short note for Settings Init review resume if needed) -- replace STORE wiring deferrals with done/next-step clarity
- [x] `lint_touched.py` + preset build -- quality gate

**Acceptance Criteria:**
- Given INIT left sentinel `* INIT *`, when user clicks STORE, then no store SysEx is sent and PATCH NAME shows name-required (L1 empty+caret, L2 `NAME REQUIRED`).
- Given name-required success with a non-empty Matrix-legal name, when Enter commits, then the name is stored in the model and STORE SysEx runs for the current slot with that name.
- Given Escape, blur, outside click, or empty/spaces Enter, when name-required ends, then pending STORE is aborted and no store SysEx is sent.
- Given sentinel still active after abort, when user clicks STORE again, then name-required re-arms.
- Given no sentinel, when user clicks STORE, then existing STORE behavior is unchanged.
- Given ROM or Compare rules that already disable STORE, when those rules apply, then they still apply; sentinel alone does not grey STORE.
- Given this Build ends, when deferred-work is updated, then STORE wiring is no longer “next step” and Settings Init combined review resume is noted.

## Implementation Notes

- `ActionExecutionHooks::requestNameRequiredBeforeStore` + Processor `setNameRequiredBeforeStoreRequest` (Editor installs).
- `handleInternalPatchStore` returns early on `isInitPatchNameSentinelActive()` after invoking the hook (no SysEx even if hook empty).
- Editor owns `pendingInternalStore_`: set before arm; outcome false aborts; outcome true clears pending then `executeInternalPatchStore()`.
- `commitPatchNameRename(..., suppressAuditionSysEx)` — pending STORE rename skips audition; STORE `sendPatch` is the single device write. Editor uses `PatchNameEditRules::shouldSuppressRenameAuditionForPendingStore`.
- Unit tests: sentinel STORE invokes hook + empty queue; rename then STORE sends patch SysEx; re-click while sentinel re-invokes hook; pending complete/abort + audition-suppress policies in `PatchNameEditRulesTests`.
- Matrix coverage: StoreSentinel + PatchNameEditRules (+ Basic `store_ramBank_success` for non-sentinel). GUI Escape/blur still smoke-manual / harness.

## Review Triage Log

| Finding | Verdict | Evidence / route |
|--------|---------|------------------|
| VG: Gate INIT→STORE / Persist tests red under sentinel deferral | high | Verified: suite failed; retargeted Gate tests + drain INIT queue. **patch (applied)** |
| EC/VG: Hardware Names / `!canEditPatchName` blocks rename then STORE re-arms | high | Verified: `commitPatchNameRename` early-return left sentinel. **patch (applied):** allow rename when `suppressAuditionSysEx` |
| BH/EC: orphan `pendingInternalStore_` when panel null | medium | Verified: pending set before null check. **patch (applied):** set pending only after panel present |
| EC: drag-armed name-required not cancelled on context interrupt | medium | Verified: panel cancelled only `isEditing()`. **patch (applied):** also cancel when `isNameRequiredArmed()` |
| VG: `commitPatchNameRename` suppress branch only helper-tested | medium | Real: invert suppress still green helpers. **defer** — needs Processor/MIDI harness for audition queue; Core gate + policy helper cover intent for this Build |
| VG: Editor outcome → `executeInternalPatchStore` untested | medium | Real for GUI loop. **defer** — manual/harness; Core `shouldCompletePendingStore` + StoreSentinel cover extractable pieces |
| BH: sibling Review Findings still say “modes chantier” | low | Intentional — refresh on Settings Init review resume (already deferred-work). **false** for this story’s ship gate |
| BH: Persist→STORE modal vs name-required UX awkward | medium | Spec frozen: same gate as button; Persist returns false until renamed. **false** as defect vs frozen matrix; smoke-manual residual |
| BH: empty-hook “no SysEx” untested | low | Code returns true from `tryDefer` even if hook empty. Unlikely everyday (Editor always installs). **reject low** |
| BH: hook wired inside `createPatchMutatorEngine` | low | Shared `ActionExecutionHooks` mutated before PatchManager create — no bad outcome. **false** |
| BH: re-arm comment vs outcome clear race | low | After panel-null fix, pending set immediately before arm; arm does not emit cancel on first entry. **false** for claimed wipe |
| BH: Design Notes `pendingStore_` vs `pendingInternalStore_` | low | Docs only. **reject low** |
| BH: no Matrix-6/6R automated one-write lock | low | Policy shared; M-1000 StoreSentinel + suppress helper. **defer** optional M6 assert |
| BH: tautological policy helpers | low | Document frozen policy for matrix audit; not harmful. **reject low** |
| BH: empty Spec Change/Triage logs at in-review | false | Filled by this triage pass |

## Spec Change Log

- 2026-09-09 review patches: Gate tests retargeted for sentinel deferral; pending-STORE rename bypasses `canEditPatchName` when suppressing audition; Editor pending set only with panel; panel interrupts cancel name-required when armed under drag; Review Triage Log filled.

## Design Notes

Core must not call GUI. Prefer a small `ActionExecutionHooks` callback invoked when STORE hits an active sentinel; Editor owns `pendingStore_` (or equivalent), calls `armNameRequired()`, and on outcome success re-invokes the real store path once the name is no longer sentinel. Outcome ordering already commits rename before `onNameRequiredOutcome(true)` in the widget — keep that; do not invert.

Do not invent a second sentinel string check: call `isInitPatchNameSentinelActive()`.

When name-required success finishes a pending STORE: commit name locally (model + APVTS + overlay/export basename as today) but skip `sendFullPatchForAudition`. Then run STORE `sendPatch` once. Normal rename (no pending STORE) keeps audition unchanged.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- build succeeds
- `ctest --preset macos-debug-arm64 -R 'PatchName|PatchManager|InitSentinel|Store' --output-on-failure` (adjust filter to tests added) -- targeted tests pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++ under Source/ and Tests/

**Manual checks:**
- INIT → STORE → see name-required, no store; type legal name + Enter → STORE completes
- INIT → STORE → Escape → no store; STORE again → name-required re-arms
- Named patch STORE still works without name-required

### Review Findings

_(Code review 2026-09-09 — range `77cdbf53..60c71605`)_

- [x] [Review][Defer] Automate `commitPatchNameRename` suppressAuditionSysEx vs MIDI outbound queue (incl. `!canEditPatchName` + suppress) [`PluginProcessorClipboard.cpp` ~323] — deferred: already in deferred-work; helpers/StoreSentinel stay green if suppress inverted
- [x] [Review][Defer] Automate Editor pending-STORE outcome → `executeInternalPatchStore` [`PluginEditorUiConstruction.cpp` ~83] — deferred: GUI loop outside Core unit pyramid; smoke-manual covered
- [x] [Review][Defer] Optional Matrix-6/6R one-write assert on deferred rename+STORE — deferred: policy shared; M-1000 StoreSentinel present
- [x] [Review][Defer] Automate panel interrupt cancel when `isNameRequiredArmed` [`PatchNameDisplayPanel.cpp` ~237] — deferred: GUI interrupt path; Escape/blur already manual/harness

#### Rejected

- Retype `* INIT *` counts as name-required success — **false**: `*` is not an allowed Matrix character; keyboard path cannot re-enter the sentinel
- `suppressAuditionSysEx` doubles as `canEditPatchName` bypass — **false** as defect: intentional prior review patch for Hardware Names pending-STORE; not a new hole in this wiring
- Persist→STORE modal UX awkward under sentinel — **false** vs frozen matrix (same gate as button STORE)
- Empty hook / null panel silent STORE block — **low** reject: Editor always installs hook; unlikely everyday
- Hook assigned inside `createPatchMutatorEngine` — **false** as defect: shared hooks bag filled before PatchManager create; speculative future cleanup risk only
- Design Notes `pendingStore_` vs `pendingInternalStore_` — reject: docs-only; fixing would edit the spec under review
- Near-identity policy helpers — **low** reject: document frozen policy for audit; not everyday harm
- No automated STORE-enabled-under-sentinel regression — **low** reject: enablement unchanged by not touching InternalPatchesPanel; optional test not worth new surface
- Drag-overlay delays name-required visuals after STORE — **low** reject: STORE mid-drag overlay uncommon; arm flag still set and interrupt cancel covers armed-under-drag
