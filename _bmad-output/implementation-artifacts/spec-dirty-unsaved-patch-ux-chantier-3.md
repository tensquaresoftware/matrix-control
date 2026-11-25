---
title: 'Dirty / unsaved patch UX — chantier 3 (session / close warning)'
type: 'feature'
created: '2026-08-22'
status: 'done'
baseline_commit: 'a7cd217a92ab464709fa020d02799d82dec5f03b'
review_loop_iteration: 0
context:
  - '{project-root}/Documentation/Devs/Plans/2026/08/2026-08-03-Dirty-Vs-Unsaved-Patch-UX.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-2.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Closing the editor, unloading the plugin, or quitting the host can discard at-risk work (dirty edits and/or INIT / device Save As without STORE) with no FR-51-family alert. Standalone quit (`systemRequestedQuit`) saves plugin state and exits without checking risk.

**Approach:** Reuse chantier 1–2 risk predicate and `confirmUnsavedEditGateIfNeeded()` (Cancel / Discard / Persist via `resolveUnsavedEditPersistKind`) on every close path JUCE exposes reliably in this project — primarily Standalone window close and app quit. Do not run the Mutator history gate on close. Document honest host limits (no universal DAW quit block; no modal in `releaseResources` / destructor / `getStateInformation`). Do not persist `patchNotStoredInRam_` in session state for this chantier.

## Boundaries & Constraints

**Always:**
- Product SSOT §0 decision 9, §5.4, §8 point 8; build on chantiers 1–2 (`75f397d`, chantier-2 spec done).
- At-risk = dirty **or** `patchNotStoredInRam_`; `UnsavedEditWarning::shouldPrompt` + Settings never-warn suppresses the whole family.
- Close gate = `confirmUnsavedEditGateIfNeeded()` only (no `confirmPatchContextChangeGate` / Mutator history on close).
- Cancel blocks close where the hook runs; Discard proceeds with close; Persist runs existing `tryPersistCurrentPatchFromUnsavedGate` then closes only on success.
- Message-thread only (`requireMessageThreadForModalGate`); Core ↛ AlertWindow; reuse `PluginEditorPatchBindings` gate wiring while editor is alive.
- English UI strings — reuse `Dialogs::UnsavedEditConfirm` copy unless close-specific wording is unavoidable (prefer existing bodies).
- Append host-limit notes to `deferred-work.md` after implementation.

**Ask First:**
- Persisting `patchNotStoredInRam_` (or a session flag) via `getStateInformation` for re-prompt after DAW project reload.
- Changing close modal copy or adding a fourth button.
- Running Mutator history gate on plugin close.

**Never:**
- Regress chantier 1–2 leave-context gates; merge Mutator / IMPORT / name-recon / Master INIT into the close dialog.
- Promise every DAW will block quit or plugin unload; modal in processor destructor, `releaseResources`, or off message thread.
- Persist unsaved patch bytes in project state instead of Store / Save / Save As.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Standalone window close | At-risk; warn-always; editor + gate wired | Modal Cancel / Discard / Persist; Cancel keeps window open | Persist fail / picker cancel → stay open |
| Standalone Cmd+Q / quit | Same | Gate before `savePluginState` + `quit`; Cancel aborts quit | Same as close |
| Standalone clean / stored | Not at-risk | Close / quit silently (existing save-then-quit) | N/A |
| Never-warn | At-risk | No modal; close / quit proceeds | N/A |
| Persist on close | User chooses Store / Save / Save As | Existing persist paths; close only after success | Fail → stay open |
| Discard on close | User chooses Discard | Close / quit without persist | N/A |
| Plugin in DAW — editor close | At-risk | **Best effort only** — document if no reliable pre-destroy hook | No crash; no off-thread modal |
| Plugin unload / DAW quit | At-risk | **Cannot block** — processor teardown / `getStateInformation` without UI | Document in deferred-work |
| Headless / tests | At-risk; no modal gate | `confirmUnsavedEditGateIfNeeded` returns true (proceed) — unchanged | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/PluginProcessorGates.cpp` -- `confirmUnsavedEditGateIfNeeded()` (reuse as-is); optional thin `confirmSessionCloseGateIfNeeded()` alias for call-site clarity
- `Source/Core/PluginProcessor.h` -- declare close-gate accessor if alias added; `isCurrentPatchAtRisk()` query helper for tests / standalone hooks (dirty sync + `isPatchNotStoredInRam`)
- `Source/Core/Services/UnsavedEditWarningPolicy.h` -- `shouldPrompt` (unchanged predicate)
- `Source/Core/Actions/PatchManagerActionHandler.*` -- `isPatchNotStoredInRam`, `resolveUnsavedEditPersistKind`, `tryPersistCurrentPatchFromUnsavedGate` (chantier 2)
- `Source/GUI/PluginEditorPatchBindings.cpp` -- existing `setUnsavedEditConfirmGateBinding` (gate must stay wired until editor destroyed)
- `Source/GUI/PluginEditor.cpp` -- destructor clears gates (close must run **before** destruction)
- `Source/Standalone/MatrixControlStandaloneFilterWindow.h` -- override `closeButtonPressed()` (JUCE default: save + quit with no risk gate — `juce_StandaloneFilterWindow.h:782`)
- `Source/Standalone/MatrixControlStandaloneApp.cpp` -- `systemRequestedQuit()` (today: save + quit; insert gate on `PluginProcessor` while main window / editor alive)
- `Tests/Unit/DirtyPatchTrackerTests.cpp` -- extend for `isCurrentPatchAtRisk` / close-policy helper if added
- `Tests/Unit/PluginProcessorCloseGateTests.cpp` (new) -- processor-level at-risk query + gate proceed/block with mock modal gate (no AlertWindow)
- `_bmad-output/implementation-artifacts/deferred-work.md` -- append host-limit entries after implementation

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/PluginProcessor.h` + `PluginProcessorGates.cpp` -- add `isCurrentPatchAtRisk()` (sync dirty + not-STORED) and optional `confirmSessionCloseGateIfNeeded()` delegating to `confirmUnsavedEditGateIfNeeded()`; no Mutator history
- [x] `Source/Standalone/MatrixControlStandaloneFilterWindow.h` -- override `closeButtonPressed()`: if `confirmSessionCloseGateIfNeeded()` false → return; else `savePluginState` + `quit` (match JUCE order after gate)
- [x] `Source/Standalone/MatrixControlStandaloneApp.cpp` -- gate in `systemRequestedQuit()` before state save / quit; Cancel leaves app running
- [x] Investigate plugin editor close hook -- if no safe JUCE/host hook exists, document only (no fake `~PluginEditor` modal)
- [x] `Tests/Unit/PluginProcessorCloseGateTests.cpp` + `Tests/CMakeLists.txt` -- at-risk query; mock gate Cancel blocks / Discard proceeds; never-warn skips
- [x] `_bmad-output/implementation-artifacts/deferred-work.md` -- honest limits: DAW unload/quit, editor close in plugin mode, no `patchNotStoredInRam_` session persistence

**Acceptance Criteria:**
- Given Standalone at-risk patch and warn-always, when closing the window or quitting the app, then the chantier 1–2 modal family appears.
- Given Cancel on that modal, when closing or quitting, then the app stays open.
- Given Discard or successful Persist, when closing or quitting, then the app exits (after save state as today).
- Given never-warn, when at-risk and closing Standalone, then no modal.
- Given chantier 1–2 navigation gates, when re-tested, then no regression.
- Given plugin unload in a DAW, when documented, then limitations are recorded in deferred-work (no false universal guarantee).

## Spec Change Log

## Design Notes

**Reuse, don't fork.** Close uses the same gate as leave-context; only hook sites differ. Do not call `confirmPatchContextChangeGate` on close — closing is not patch navigation; Mutator history stays out of scope per §9 of product plan.

**Cancel = block close** when the hook runs (Standalone). Aligns with chantier 1 Cancel semantics (“keeps editing” → keeps app/window open).

**No session flag in chantier 3.** `patchNotStoredInRam_` remains in-memory (chantier-1 deferred-work). Close prompts use live state; DAW project reload without STORE still won't re-prompt — acceptable deferral unless Guillaume reopens persistence.

**Standalone only is the reliability bar.** JUCE `StandaloneFilterWindow::closeButtonPressed` and `JUCEApplication::systemRequestedQuit` are the only guaranteed pre-UI-teardown hooks found. Plugin `~PluginEditor` runs after gates are cleared — too late.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'DirtyPatch|CloseGate|UnsavedEdit|PatchManagerActionHandlerGate'` -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Standalone: edit → quit → modal; Cancel stays; Discard quits; Store then quits; INIT not-STORED → modal on quit; never-warn → silent quit.
- Optional AU/VST3: note whether host plugin-window close shows modal (expect often no); unload plugin — expect no block.

## Suggested Review Order

**Close gate (Core)**

- Session close reuses the FR-51 gate without Mutator history.
  [`PluginProcessorGates.cpp:133`](../../Source/Core/PluginProcessorGates.cpp#L133)

- Shared at-risk predicate extracted for dirty ∨ not-STORED.
  [`UnsavedEditWarningPolicy.h:25`](../../Source/Core/Services/UnsavedEditWarningPolicy.h#L25)

**Standalone hooks**

- Window close routes through app quit so gating runs once.
  [`MatrixControlStandaloneFilterWindow.h:45`](../../Source/Standalone/MatrixControlStandaloneFilterWindow.h#L45)

- Cmd+Q and unified quit path gate before save + exit.
  [`MatrixControlStandaloneApp.cpp:111`](../../Source/Standalone/MatrixControlStandaloneApp.cpp#L111)

**Tests & limits**

- Close-gate policy simulation including Persist failure.
  [`PluginProcessorCloseGateTests.cpp:19`](../../Tests/Unit/PluginProcessorCloseGateTests.cpp#L19)

- Documented DAW / plugin-mode limitations.
  [`deferred-work.md:1130`](deferred-work.md#L1130)
