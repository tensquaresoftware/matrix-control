---
organization: Ten Square Software
project: Matrix-Control
title: 'Master Edit SysEx outbound debounce'
type: 'feature'
created: '2026-09-09'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'a0724a57a77b34085e6d5964d04560030dbf15a7'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/CONVENTIONS.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-manager-nav-debounce.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Every Master Edit control change immediately sends a full Master Parameter Data SysEx (opcode 0x03). Dragging a slider or arrowing through CHANNEL floods the synth with large dumps, even though the hardware only accepts full Master messages.

**Approach:** Keep Master Edit UI and APVTS/model updates immediate. Coalesce outbound Master SysEx with a single Core trailing debounce shared by all Master parameters so only the latest packed Master buffer is sent after the user pauses (~150 ms, same order as combobox / Mutator History patch-send debounce). Immediate full sends (INIT Master, Master file load) cancel any pending debounce and send once.

**Decisions:**
- Spec size: keep full draft (token count slightly above comfort band; single goal retained).
- Debounce delay: **150 ms** (`kMasterEditSysExDebounceMs = 150`) — balance flood control vs perceived lag; align with `kComboboxPatchSendDebounceMs` policy family, named separately for Master full-dump semantics.

## Boundaries & Constraints

**Always:**
- Debounce lives in Core on the Master outbound path (`dispatchMasterParameterChange` / processor timer), not in GUI widgets.
- UI, APVTS, and `ApvtsMasterMapper::apvtsToBuffer()` stay immediate on every change; only `sendMaster` / enqueue is deferred.
- Trailing debounce window = **150 ms** via `kMasterEditSysExDebounceMs` (own constant; do not silently alias patch-nav 300 ms).
- One shared pending send for all Master parameter IDs (full 0x03 each time — no per-control or per-field coalesce map).
- Reuse `ComboboxPatchSendDebouncer` (ctor ms override) or an equivalent single-callback trailing timer with `flushPendingSynchronouslyForTests` — no second home-grown timer family.
- Cancel pending debounce (do not flush stale) before editorial undo/redo clear, matching Matrix Mod `cancelPending` policy.
- Cancel pending then keep the existing single `dispatchFull()` for Master module INIT and Master user-file load.
- Respect existing suppress / editorial-quiet / `isMasterEditOutboundAllowed` gates — do not arm debounce while suppress or quiet is true.
- Unit tests prove coalescing via synchronous flush (no wall-clock sleep). English-only source; `Scripts/quality/lint_touched.py` clean on touched C++.

**Ask First:**
- Changing FR-16 semantics beyond “full 0x03 still sent, but coalesced under burst”.
- Debouncing Patch granular (0x06) or Matrix Mod bus SysEx in this story.
- Changing `SysExDelayProfile` / inter-message delay.

**Never:**
- Delay or freeze Master Edit widgets during the debounce window.
- Alter Patch `PatchParameterSysExDispatcher` / remote parameter edit path.
- Alter `MatrixModSysExCoalesceTimer` behaviour or Matrix Mod opcodes.
- Weaken FR-46 Master outbound device gates in `MidiManager::sendMaster`.
- Send Master SysEx from GUI code.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Rapid Master slider drag | Many APVTS Master changes within debounce window | UI follows each sample; after settle, one full 0x03 with final buffer | Superseded schedules replace pending callback |
| Keyboard skim CHANNEL combo | Rapid choice changes | UI immediate; one 0x03 after settle for last value | Same shared debouncer |
| Isolated slow tweak | One change, wait ≥ debounce ms | One 0x03 after delay (same end state as today) | N/A |
| Master module INIT | INIT while debounce pending | Cancel pending; one `dispatchFull` as today | Suppress wrap unchanged |
| Load Master from user file | Load while debounce pending | Cancel pending; one `dispatchFull` | N/A |
| Editorial undo/redo | Undo/redo with pending Master debounce | Cancel pending; no auto-flush of abandoned Master send | Existing editorial quiet / patch resync unchanged |
| Suppress / quiet window | `suppressMasterParameterSysEx_` or editorial quiet true | No schedule, no send from listener path | Existing gates |
| Device / Master Edit not allowed | Gate false at schedule or at fire | No outbound enqueue (re-check gate at fire) | Drop pending send if gate false at fire |
| Host automation burst | Rapid host-driven Master param writes | Same coalesce as UI | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/PluginProcessorValueTree.cpp` — `dispatchMasterParameterChange` (~L175): today immediate `apvtsToBuffer` + `dispatch`; schedule debounce here; fire → `dispatchFull` or `dispatch` with latest buffer; re-check outbound gate at fire.
- `Source/Core/PluginProcessor.h` / `PluginProcessorConstruction.cpp` — own debouncer member (like Matrix Mod timer ownership); wire ms; cancel on editorial undo/redo beside `matrixModSysExCoalesceTimer_->cancelPending()`.
- `Source/Core/Util/ComboboxPatchSendDebouncer.h` / `.cpp` — reuse `schedule` / `cancel` / `flushPendingSynchronouslyForTests`; add named constant e.g. `kMasterEditSysExDebounceMs` once delay chosen (do not overload patch-nav constants unless intentional policy share).
- `Source/Core/MIDI/MasterParameterSysExDispatcher.h` / `.cpp` — keep `dispatch` / `dispatchFull`; fire path should call `dispatchFull()` so id filter is irrelevant at settle.
- `Source/Core/Init/MasterModuleInitService.cpp` — `dispatchFull` after init; ensure processor cancels pending before/around this path.
- `Source/Core/PluginProcessor*.cpp` — `loadMasterFromUserFile` / editorial undo-redo: cancel pending Master debounce.
- Tests: extend `MasterParameterSysExDispatcherTests` and/or add processor-level coalesce tests with injectable ms + sync flush; mirror `ComboboxPatchSendDebouncerTests` / PatchManager flush seams. Do not sleep in CI.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Util/ComboboxPatchSendDebouncer.h` — add `kMasterEditSysExDebounceMs = 150` with comment distinguishing Master full-dump policy from patch-nav 300 / shared combobox 150 constants
- [x] `Source/Core/PluginProcessor.h` (+ construction / value-tree / editorial paths) — own Master SysEx debouncer; schedule from `dispatchMasterParameterChange`; fire → `apvtsToBuffer` (if needed) + gated `dispatchFull`; cancel on undo/redo, Master INIT, Master file load; test flush seam
- [x] `Source/Core/PluginProcessorValueTree.cpp` — replace immediate `dispatch(parameterId)` with schedule; keep immediate mapper update; honour suppress/quiet before schedule
- [x] `Tests/Unit/` — unit tests: N rapid Master param changes → one enqueue after sync flush; cancel drops pending; INIT/load path does not double-send from stale timer; gate false at fire → no enqueue
- [x] `CMakeLists.txt` / `Tests/CMakeLists.txt` — register any new test file

**Acceptance Criteria:**
- Given Master Edit outbound allowed, when a Master slider is dragged through many values within the debounce window, then the synth receives one full 0x03 (or very few) reflecting the final value, while the UI tracks continuously.
- Given a CHANNEL (or other Master) combobox skimmed with keyboard arrows, when the user pauses, then one full 0x03 is sent for the last selection.
- Given a pending debounced Master send, when Master module INIT or Master file load runs, then the pending send is cancelled and exactly one intentional `dispatchFull` occurs for that action.
- Given a pending debounced Master send, when editorial undo/redo runs, then the pending Master send is cancelled and does not fire afterward.
- Given unit tests, when coalesce behaviour is asserted, then tests use synchronous flush (no wall-clock sleeps) and pass under `MATRIX_BUILD_TESTS=ON`.

## Implementation Notes

- Debouncer: `ComboboxPatchSendDebouncer` at `kMasterEditSysExDebounceMs` (150), owned by `PluginProcessor` as `masterEditSysExDebouncer_`.
- Listener path keeps `apvtsToBuffer()` immediate; schedule → `firePendingMasterEditSysEx()` re-checks suppress / quiet / outbound gate then `dispatchFull()`.
- Cancel sites: editorial undo/redo (both sides of undo like Matrix Mod), `setSuppressMasterSysEx(true)` (module INIT), `initAllMasterModulesFromTemplate` / `loadMasterFromUserFile` (cancel before and after `bufferToApvts` so listener-armed schedules cannot double-send after intentional `dispatchFull`).
- Tests: `Tests/Unit/MasterEditSysExDebounceTests.cpp` harness mirrors processor schedule/fire/cancel (sync flush only); review added null-guard, suppress/quiet-at-fire, and second-burst cases; Unicode arrows removed from `beginTest` titles.

## Spec Change Log

- 2026-09-09 — Implemented Master Edit outbound SysEx trailing debounce (150 ms); unit tests + lint green.

## Review Triage Log

| Finding | Verdict | Evidence / route |
|--------|---------|------------------|
| BH: no editorial undo/redo Master cancel test | medium | True gap vs AC wording; harness `cancel` covers cancel semantics; full undo MIDI path deferred — **defer** |
| BH/VG: harness ≠ PluginProcessor; flush seam unused | medium | True; AC + Implementation Notes allow mirrored harness; real-processor flush coverage deferred — **defer** |
| BH: suppress/quiet after schedule untested | medium | Production re-checks at fire; test gap real — **patch** (added `suppressOrQuietAfterSchedule_dropsAtFire`) |
| BH: coalesce test does not decode SysEx payload | low | Model final value asserted; `dispatchFull` sends model buffer — **reject** (low / everyday risk nil) |
| BH: harness omits `apvtsToBuffer` on fire | low | Production fire refreshes mapper; harness models immediate buffer by caller — **reject** |
| BH: init-all lacks suppress wrap asymmetry | false | Double `cancelMasterEditSysExDebounce` around `bufferToApvts` prevents stale fire — **reject** |
| BH: destructor does not cancel pending debounce | false | `ComboboxPatchSendDebouncer` dtor `stopTimer()`; APVTS listener removed in `~PluginProcessor` first — **reject** |
| BH: null debouncer silent drop | false | Debouncer always created in `createSysExDispatchers`; unreachable in normal construction — **reject** |
| BH: no second-burst after flush test | low | Useful; patched — **patch** (`secondBurstAfterFlush_sendsAgain`) |
| BH: dual hard-coded 150 without static_assert | low | Named constants intentional per frozen decisions — **reject** |
| BH: unused `#include <cstring>` | low | True — **patch** (removed) |
| BH: checklist claims Tests/CMakeLists.txt | false | Project registers tests in root `CMakeLists.txt` only — **reject** |
| EC: pedal1 nullptr soft-expect then dereference | medium | True test UB path — **patch** (early return) |
| EC: cancel then INIT/load with gate false drops edit | false | Matches frozen matrix “gate false at fire → drop”; INIT/load replaces Master intentionally — **reject** |
| VG: editorial undo cancel unobserved in UndoManager tests | medium | True CI-blind spot; not trivial to wire — **defer** (with BH editorial) |

Review patches applied 2026-09-09; deferred entries appended to `deferred-work.md`.

## Design Notes

Prefer **trailing debounce → one `dispatchFull()`** over Matrix Mod’s per-bus bitmask: every Master change already encodes the full packed buffer, so pending state is a boolean (or replaced callback), not a set of IDs.

Keep `apvtsToBuffer()` on each change (current behaviour) so the model stays current for UI, dirty tracking, and any immediate `dispatchFull` path; the timer only defers MIDI enqueue.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` — tests green including new Master debounce cases
- `ctest --test-dir Builds/macOS/... -R Master` (or project’s usual test invoke) — Master-related tests pass
- `python3 Scripts/quality/lint_touched.py` — clean on touched C++ under `Source/` and `Tests/`

**Manual checks:**
- Standalone + Matrix-1000: drag a Master slider rapidly — UI fluid; MIDI monitor shows far fewer full Master dumps than today, ending on the final value.
- Arrow through Master CHANNEL quickly — same coalesce behaviour.
- Master INIT while twiddling — one clean Master dump, no stale deferred send afterward.
