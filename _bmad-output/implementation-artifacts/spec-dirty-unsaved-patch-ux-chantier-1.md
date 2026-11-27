---
title: 'Dirty / unsaved patch UX — chantier 1 (modal + INIT risk)'
type: 'feature'
created: '2026-08-03'
status: 'done'
baseline_commit: '49b294a'
review_loop_iteration: 0
context:
  - '{project-root}/Documentation/Devs/Plans/2026/08/2026-08-03-Dirty-Vs-Unsaved-Patch-UX.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Leaving a patch can lose work that is either dirty (≠ last plugin snapshot) or never STORED to the current RAM slot — notably after full-patch INIT, which refreshes the snapshot so FR-51 stays silent today. The existing Cancel / Continue modal also offers no Store / Save As path.

**Approach:** Treat “at risk” as dirty **or** not-yet-STORED-in-RAM. Enrich the leave-context modal (RAM: Cancel / Discard / Store; ROM: Cancel / Discard / Save As). Cover INIT-at-risk on nav and `.syx` load, warn on second INIT when already at risk, keep PASTE silent on entry, and let Settings never-warn suppress the whole family.

## Boundaries & Constraints

**Always:**
- Product SSOT §0 decisions 1–4, 7, 10 for chantier 1; Core ↛ GUI (gates from PluginEditor only).
- Risk modal **before** Mutator history modal; never merge with Mutator / name-recon / IMPORT / Defrag / Master INIT dialogs.
- English-only UI strings; Store copy must make clear it writes the current RAM slot.
- Never-warn Settings suppresses dirty **and** not-persisted-in-RAM prompts (Mutator history may still run when that gate is reached).
- RAM vs ROM via existing `DeviceMemoryLimits::isPasteStoreAllowed` (Store only when allowed).
- Preserve Cancel revert behaviour from story 9.2 / v1-1 (selection / NumberBox / OPEN / browser restore).
- Do not break INIT blank-name → `INIT` naming (`spec-init-patch-name`).

**Ask First:**
- Any urge to implement chantier 2 (SAVE AS from device keeps RAM risk; `.syx`-origin Save/Save As buttons) or chantier 3 (DAW/plugin close warning).
- Changing never-warn so it would still show Mutator history differently than today (out of scope — leave history behaviour as-is).

**Never:**
- Chantier 2–3 behaviours; merging unrelated modals; AlertWindow / dialog includes in Core.
- Offering Store on ROM; forcing a `.syx` save on internal RAM navigation.
- Warning on PASTE entry; skipping the risk family on second INIT when at risk and warn-always.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Dirty RAM leave | Dirty, RAM bank, warn-always, nav / `.syx` / EXPORT | Modal Cancel / Discard / Store | Cancel abort; Discard proceed; Store runs STORE then proceed; Store fail → stay + existing footer |
| Dirty ROM leave | Dirty, ROM bank, warn-always | Modal Cancel / Discard / Save As (no Store) | Cancel abort; Discard proceed; Save As → existing picker/save; picker cancel or save fail → stay |
| INIT then leave | After full INIT, not dirty, not STORED, warn-always, nav or `.syx` load | Same at-risk modal family (RAM buttons) | Same as dirty leave |
| Second INIT | At-risk current (dirty and/or not STORED), warn-always, INIT | At-risk modal before overwrite | Cancel keeps current; Discard/Store then INIT; after INIT set not-STORED again |
| PASTE entry | Any prior risk/dirty, PASTE | No risk modal and no Mutator history gate on entry (module C/P friction) | Later leave without STORE → at-risk (dirty covers pasted content) |
| Never warn | At-risk + never-warn policy | No risk modal; requested action proceeds | Mutator history gate unchanged when reached |
| Mutator order | At-risk + non-empty history, user Discard/Store then continue | Risk modal first; history modal may follow | Cancel on either aborts |
| IMPORT | At-risk then IMPORT | Risk gate first; existing IMPORT confirm second; texts not merged | Cancel on risk aborts before IMPORT confirm |
| Clean stored | Clean snapshot and STORED (or never INIT-unstored) | Silent leave | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Services/DirtyPatchTracker.{h,cpp}` -- byte snapshot dirty compare; keep; do not overload as RAM-persisted flag
- `Source/Core/Services/UnsavedEditWarningPolicy.h` -- extend `shouldPrompt` (or sibling) for dirty **or** not-STORED risk vs never-warn
- `Source/Core/PluginProcessorGates.cpp` -- widen FR-51 gate from bool Continue to choice enum; run Store/Save As side effects before returning proceed; then Mutator history
- `Source/Core/PluginProcessor.h` -- modal gate typedef + any not-STORED state ownership / accessors
- `Source/Core/Actions/ActionExecutionHooks.h` -- `confirmPatchContextChange(bool)` contract unchanged at call sites except INIT/PASTE flags
- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` -- INIT: set not-STORED after `captureCleanSnapshot`; second INIT uses includeUnsaved=true; PASTE: skip full context gate; STORE clears not-STORED when capture succeeds
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` -- `.syx` load already gates with true; SAVE AS may clear dirty (chantier 2 RAM-risk retention out of scope)
- `Source/Core/Actions/PatchManagerActionHandlerBankImport.cpp` / `BankExport.cpp` -- keep risk-before-IMPORT; EXPORT uses same at-risk gate (Store when RAM-allowed)
- `Source/Core/Services/DeviceMemoryLimits.*` -- `isPasteStoreAllowed` / ROM detection for button set
- `Source/GUI/PluginEditorPatchBindings.cpp` -- 3-button binding via `showOrderedConfirmAlert`
- `Source/GUI/PluginEditorAlerts.cpp` -- existing 2/3-button helper
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `Dialogs::UnsavedEditConfirm` title/body/buttons (Discard, Store, Save As; clarify Store→RAM)
- `Tests/Unit/DirtyPatchTrackerTests.cpp` -- policy / risk helper
- `Tests/Unit/PatchManagerActionHandlerGateTests.cpp` -- INIT flag, captures, Cancel paths
- `Tests/Unit/PatchManagerActionHandlerDeferredGateTests.cpp` -- Continue/Discard races must still pass with new enum
- `Tests/Unit/PatchManagerActionHandlerTestSupport.*` -- harness for gate choice + includeUnsaved recording

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/UnsavedEditWarningPolicy.h` (+ tracker-adjacent Core state on processor/handler) -- add not-STORED-in-RAM risk signal; `shouldPrompt` when dirty **or** at-risk; clear on successful STORE (and on successful replacing load paths that already recapture clean); set after full-patch INIT -- separate from dirty bytes
- [x] `Source/Core/PluginProcessorGates.cpp` + gate typedefs -- replace bool FR-51 gate with Cancel / Discard / StoreOrSaveAs choice; on Store/SaveAs invoke existing STORE / Save As paths; fail/cancel → abort context change; success → fall through to Mutator history
- [x] `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` -- INIT: `confirmPatchContextChange(true)` when gate should see risk; set not-STORED after INIT snapshot; PASTE: do not call context gate (silent entry); STORE clears not-STORED with clean capture
- [x] `Source/GUI/PluginEditorPatchBindings.cpp` + `PluginDisplayNames.h` -- RAM vs ROM button sets and copy (Discard replaces Continue; Store / Save As); English only
- [x] `Tests/Unit/DirtyPatchTrackerTests.cpp` + `PatchManagerActionHandlerGateTests.cpp` (+ harness) -- cover policy+risk, INIT at-risk leave, second INIT prompts, PASTE skips includeUnsaved/gate, Store choice path, never-warn skips -- lock I/O matrix edges without GUI AlertWindow

**Acceptance Criteria:**
- Given a dirty RAM patch, when leaving context, then Cancel / Discard / Store; Store writes current RAM slot then continues.
- Given a dirty ROM patch, when leaving context, then Cancel / Discard / Save As (no Store).
- Given full INIT with no further edits, when changing patch or loading `.syx`, then the same at-risk modal family (lecture B).
- Given at-risk current, when INIT again, then at-risk modal before overwrite.
- Given PASTE, when pasting, then no entry alert; when leaving later without STORE, then at-risk alert.
- Given Settings never-warn, when leaving an at-risk patch, then no risk-family alert.
- Given non-empty Mutator history and at-risk, when Discard/Store proceeds, then history modal may still follow as today.

## Spec Change Log

## Design Notes

**Risk = dirty ∨ notSTORED.** INIT must keep `captureCleanSnapshot()` (audition baseline) **and** set notSTORED so leave prompts. Do not invent dirty bytes after INIT.

**Modal enum** (mirror Mutator history choice style): `kCancel` | `kDiscard` | `kPersist` where Persist means Store (RAM) or Save As (ROM) chosen by limits — GUI picks labels; Core runs the matching action.

**PASTE:** skip `confirmPatchContextChange` entirely on entry so Mutator history is also silent (module C/P). Exit coverage relies on post-paste dirty (and notSTORED if still set — prefer clearing notSTORED only on STORE/load, or leaving dirty as sufficient; either way leave must warn when pasted content ≠ stored).

**Chantier 2 deferral:** after device-origin SAVE AS, today’s clean snapshot may silence RAM risk — do **not** fix here.

**Suggested review order:** policy/risk flag → `PluginProcessorGates` → InternalPatches INIT/PASTE/STORE → display names + editor binding → gate unit tests.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'DirtyPatch|PatchManagerActionHandlerGate|UnsavedEdit'` (adjust regex to matching test names) -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ files

**Manual checks (if no CLI):**
- Standalone: edit → leave → Store then confirm slot; INIT → leave without knob → modal; INIT → INIT → modal; PASTE → no modal; never-warn → silent; Mutator history still after Discard when history non-empty; ROM bank → Save As not Store.

## Suggested Review Order

**Risk signal (dirty ∨ not STORED)**

- Policy treats not-STORED like dirty unless never-warn.
  [`UnsavedEditWarningPolicy.h:18`](../../Source/Core/Services/UnsavedEditWarningPolicy.h#L18)

- INIT refreshes snapshot then marks not-STORED so leave still warns.
  [`PatchManagerActionHandlerInternalPatches.cpp:280`](../../Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp#L280)

- Successful capture (load/STORE/SAVE) clears the not-STORED flag.
  [`PatchManagerActionHandler.cpp:237`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L237)

**Leave-context gate**

- Gate prompts on dirty or not-STORED, then applies Cancel/Discard/Persist.
  [`PluginProcessorGates.cpp:133`](../../Source/Core/PluginProcessorGates.cpp#L133)

- Persist runs STORE (RAM) or Save As (ROM); failure aborts leave.
  [`PatchManagerActionHandler.cpp:252`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L252)

- Mutator history still runs after a successful risk gate.
  [`PluginProcessorGates.cpp:166`](../../Source/Core/PluginProcessorGates.cpp#L166)

**INIT / PASTE call sites**

- Second INIT now goes through the risk gate (`includeUnsaved=true`).
  [`PatchManagerActionHandlerInternalPatches.cpp:267`](../../Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp#L267)

- PASTE entry skips the context gate entirely (module C/P friction).
  [`PatchManagerActionHandlerInternalPatches.cpp:318`](../../Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp#L318)

**UI copy and buttons**

- RAM vs ROM bodies and Cancel / Discard / Store|Save As binding.
  [`PluginEditorPatchBindings.cpp:169`](../../Source/GUI/PluginEditorPatchBindings.cpp#L169)

- English strings clarifying Store writes the current RAM slot.
  [`PluginDisplayNames.h:122`](../../Source/Shared/Definitions/PluginDisplayNames.h#L122)

**Tests**

- Policy, INIT not-STORED, second INIT, PASTE silence, Persist Store paths.
  [`PatchManagerActionHandlerGateTests.cpp:12`](../../Tests/Unit/PatchManagerActionHandlerGateTests.cpp#L12)
