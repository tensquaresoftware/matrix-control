---
title: 'Internal Patches Prev/Next number oscillation after Computer'
type: 'bugfix'
created: '2026-10-01'
status: 'done'
route: 'dispatch'
baseline_commit: '90cc570333c77e629cce2cf3cae8ca17925c59d8'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-manager-nav-debounce.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After alternating between Computer Patches (`.syx`) and Internal Patches, rapid Internal `<` / `>` clicks sometimes make the current patch number move the wrong way (increment then decrement then increment) or briefly freeze, even though debounce already coalesces MIDI. Observed in Standalone with a live Matrix-1000.

**Approach:** Keep Internal NumberBox motion aligned with the user's click direction while Internal owns navigation. When Internal Prev/Next claims focus, cancel competing Computer settles/loads so a late Computer path cannot fight Internal. Separate dump-failure coordinate restore from that rule so a late or failed device dump cannot yank the NumberBox backward while the user is still stepping. Keep the existing 300 ms button debounce and confirm-at-settle.

## Boundaries & Constraints

**Always:**
- UI bank/patch NumberBoxes update immediately on each Internal Prev/Next click; heavy MIDI/dump work stays debounced (`kPatchNavButtonDebounceMs`).
- Competing Computer button/combo settles and in-flight Computer navigation abort when Internal Prev/Next claims ownership.
- On device dump fail/abort after Internal load: keep the displayed bank/patch NumberBox (do not restore `priorCoordinates`); still show the footer warning. Decision: DUMP_FAIL_COORDS = KEEP_DISPLAYED.
- Unsaved-edit Cancel at Internal settle still restores the pre-burst Internal baseline (existing FR-51 settle semantics).
- When Computer nav/select intentionally abandons a pending Internal settle mid-burst, baseline restore remains (existing cross-path contract).
- Unit tests cover the races without wall-clock sleeps (`flushPendingSynchronouslyForTests` / existing flush helpers).
- English-only source; `Scripts/quality/lint_touched.py` clean on touched C++.

**Ask First:**
- Changing debounce delays, unsaved confirm copy, or SysEx hardware timing profiles.
- Changing Computer mid-burst baseline-restore semantics when Computer wins.

**Never:**
- Disable or freeze Prev/Next during the debounce window.
- Confirm on every click inside a burst.
- GUI Look / layout changes.
- Core → GUI dependencies.
- French strings in source.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Internal burst after Computer load | Computer `.syx` loaded; then Internal `>` × N within 300 ms | NumberBox advances N steps monotonically; one settle dump for final index | N/A |
| Internal claims over pending Computer | Computer nav/select settle pending; Internal `>` | Computer settle cancelled/aborted; Internal owns focus; NumberBox advances from Internal coords | N/A |
| Late dump fail during further Internal steps | Internal settle started dump for index A; user already clicked to B; dump fails/aborts | NumberBox stays at B; footer warning | No restore to pre-burst / prior A |
| Computer abandons Internal mid-burst | Internal `>` burst pending; Computer select/nav | Internal baseline restored (existing); Computer load proceeds | Unchanged |
| Unsaved Cancel at settle | Dirty editor; Internal settle confirm Cancel | Restore pre-burst baseline; no MIDI for abandoned target | Unchanged |

</frozen-after-approval>

## Code Map

- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` — `tryHandleInternalPatchNavigation` (immediate UI + schedule settle; currently abandons device load + computer *select* settle only); `abandonPendingInternalNavSettle` (baseline restore); `settleInternalPatchNavigation`; `scheduleComputerNavPatchLoad` / `scheduleComputerSelectPatchLoad` (cross-path abandon Internal).
- `Source/Core/Actions/PatchManagerActionHandlerDeviceLoad.cpp` — `beginPendingDeviceLoad` / `abandonPendingDeviceLoad` / `failPendingDeviceLoad` → `restoreInternalCoordinates` (likely late snap-back); dump generation gate.
- `Source/Core/Actions/PatchManagerActionHandlerComputerBrowser.cpp` — `abortComputerPatchesNavigation` (cancels both debouncers + Internal baseline restore + combo revert); call from Internal claim path as needed without restoring Internal coords when Internal is the claimant.
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` — `commitLoadedComputerPatchFile` / `handleLoadSelectedPatchFile` (Computer focus reclaim after load).
- `Source/Core/Actions/PatchManagerActionHandlerHeaderClick.cpp` — known comment: baseline restore can put Computer focus back; pattern for re-claiming Internal after abandon.
- `Source/Core/Actions/PatchManagerActionHandler.h` — `pendingInternalNavBaseline_`, debouncers, pending device load.
- `Source/Core/Services/ComboboxPatchSendDebouncer.{h,cpp}` — shared timer; `schedule` replaces pending.
- `Source/GUI/Panels/Setup/InternalPatchesPanelSetup.cpp` — Prev/Next wiring only (likely untouched).
- `Tests/Unit/PatchManagerActionHandlerNavDebounceTests.cpp` — existing cross-path Internal→Computer baseline restore; extend with Internal-claim-over-Computer + dump-fail-while-advanced cases.
- Related done specs: `spec-patch-manager-nav-debounce.md`, `spec-patch-nav-focus-and-bank-marker.md`.

**Reuse:** `ComboboxPatchSendDebouncer`, `abortComputerPatchesNavigation` pieces (split cancel-without-Internal-rewind if needed), flush helpers in tests.

**Do not change:** debounce ms constants unless Ask First; Mutator History 150 ms path; Bank Utility reclaim semantics beyond what Internal claim already does.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` (+ `.h` if needed) -- On Internal Prev/Next claim, cancel pending Computer button/combo settles and abort in-flight Computer navigation without rewinding the just-updated Internal NumberBox; keep immediate UI + 300 ms settle. -- Stops Computer/Internal timer fights after alternating sources.
- [x] `Source/Core/Actions/PatchManagerActionHandlerDeviceLoad.cpp` (+ callers) -- On dump fail/abort, keep displayed NumberBox coords (KEEP_DISPLAYED); footer warning only — no `restoreInternalCoordinates` from `priorCoordinates`. -- Stops wrong-direction / freeze-like snaps under MIDI latency.
- [x] `Source/Core/Actions/PatchManagerActionHandlerComputerBrowser.cpp` (and/or LoadSave) -- Expose or adjust abort helpers so Internal claim can kill Computer pending work without invoking Internal baseline restore. -- Avoid using a hammer that undoes the click that claimed Internal.
- [x] `Tests/Unit/PatchManagerActionHandlerNavDebounceTests.cpp` (+ CMake if new file) -- Add races: Internal claim cancels pending Computer settle; dump fail after further Internal clicks leaves NumberBox at latest index; preserve existing Computer-abandons-Internal baseline restore. -- Locks the regression without hardware.

**Acceptance Criteria:**
- Given Computer Patches just loaded a `.syx`, when the user rapidly clicks Internal `>`, then the patch number only increases with each `>` (no mid-burst decrease) until settle or Cancel.
- Given a pending Computer nav/select settle, when Internal Prev/Next fires, then Computer settle does not complete and navigation focus stays Internal.
- Given a device dump fails after the user already stepped further with Internal Prev/Next, when the fail is reported, then the NumberBox stays at the latest clicked index and a footer warning still appears.
- Given Internal burst then Computer select mid-window, when Computer wins, then Internal NumberBox still restores to the pre-burst baseline (existing contract).
- Given existing nav-debounce unit tests, when the suite runs, then prior coalescing / Cancel-at-settle cases still pass.

## Implementation Notes

- Added `cancelPendingComputerPatchesWork()` (cancel Computer button/combo settles + revert unsettled combo selection without Internal baseline restore). `abortComputerPatchesNavigation()` = `abandonPendingInternalNavSettle()` + that helper.
- Internal Prev/Next claim calls `cancelPendingComputerPatchesWork()` then `abandonPendingDeviceLoad()` before scheduling Internal settle (replaces select-only abandon).
- `failPendingDeviceLoad` KEEP_DISPLAYED: clear pending + footer warning only; no `restoreInternalCoordinates`.
- Footer string `kDeviceDumpAbortedEditedFooter` updated to match KEEP_DISPLAYED wording.
- Tests: `crossPath_internalClaim_cancelsPendingComputerSettle`, `crossPath_dumpFailAfterFurtherInternalSteps_keepsLatestIndex`; DeferredGate expectations updated for KEEP_DISPLAYED.
- Manual Standalone + Matrix-1000 UAT still recommended (alternate Computer `.syx` ↔ Internal, rapid `<`/`>`).

## Spec Change Log

## Review Triage Log

- Blind: `priorCoordinates` still written, never read after KEEP_DISPLAYED — **low** → **defer** (dead field / API leftover; cleanup without behavior change).
- Blind: comments claim prior-coordinates overload serves editor-buffer gate — **low** → **patch** (comments/docs only; gate uses `bufferAtRequest`).
- Blind: `reloadCurrentInternalSlotFromDevice` still select-only abandon, not `cancelPendingComputerPatchesWork` — **medium** → **defer** (intent frozen to Internal Prev/Next claim; header/same-bank reclaim is adjacent).
- Blind: HeaderClick comment still warns `failPendingDeviceLoad` can restore Computer — **low** → **patch** (stale comment under KEEP_DISPLAYED).
- Blind: `failPendingDeviceLoad` unused `limits` parameter — **low** rejected (signature churn across callers; no user-facing harm).
- Blind: sticky-footer inventory still has old abort copy — **low** → **patch** (inventory string drift vs `PluginDisplayNames`).
- Blind: Spec Code Map / empty Spec Change Log while in-review — **false** (fixing by editing this build's spec is rejected).
- Blind: dump-fail race test first half may no-op via generation before KEEP_DISPLAYED — **low** rejected (settle-fail half + DeferredGate already cover KEEP_DISPLAYED).
- Blind: dump-fail test asserts severity not footer text — **low** rejected (unlikely everyday; DeferredGate already pins abort footer text).
- Blind: no mirrored Internal Prev (`<`) race tests — **false** (same `isNext` path as Next).
- Blind: every Internal click runs full Computer cancel even with no pending settle — **false** (`revertComputerPatchesSelectionIfNeeded` is a no-op when selection already matches committed/stable id).
- Blind: Internal claim does not gate in-flight `handleLoadSelectedPatchFile` after timer fired — **false** (message-thread settle is not concurrent with a later Internal click; post-commit Internal claim re-pins focus).
- Blind: DeferredGate bank dump-fail omits `uiMessageText` assert — **low** rejected.
- Edge: HeaderClick Internal reclaim during pending Computer button-nav leaves combo unsettled — **medium** → **defer** (same adjacent header path as Blind #3).
- Verification-gap: no gaps — noted.
- Verification other: dead `priorCoordinates` — **carried** same as Blind #1 → **defer**.
- Verification other: HeaderClick restore comment — **carried** same as Blind #4 → **patch**.
- Verification other: PluginProcessor comment "after dump rollback" — **low** → **patch** (stale wording; deferral-until-success still valid under KEEP_DISPLAYED).

## Design Notes

Primary failure modes from investigation (not prescriptions):

1. Shared `patchNavDebouncer_` + Computer paths calling `abandonPendingInternalNavSettle` rewind NumberBox to pre-burst baseline.
2. `failPendingDeviceLoad` restores `priorCoordinates` (often the pre-burst baseline) after a late/failed dump — visible as decrement while the user keeps clicking `>`.
3. Internal claim cancels combo settle and pending SysEx but does not fully abort Computer button-nav / in-flight load the way `abortComputerPatchesNavigation` does.

Prefer the smallest Core change that satisfies the matrix; keep GUI wiring untouched unless a test harness forces it.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R PatchManagerActionHandlerNavDebounce` -- expected: all matching tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Standalone + Matrix-1000: alternate Computer `.syx` ↔ Internal, then rapid Internal `>` / `<` — NumberBox must not oscillate; brief settle lag without wrong-direction snaps is OK.

### Review Findings

- [x] [Review][Patch] Add Internal-claim-over-pending-Computer-combo race test — Mirror `crossPath_internalClaim_cancelsPendingComputerSettle` but arm pending `kSelectPatchFile` / `flushComputerSelectDebouncerForTests`; today only button-nav (`kLoadNextPatchFile`) is covered, so removing `computerSelectDebouncer_.cancel()` from `cancelPendingComputerPatchesWork` would still leave CI green. [Tests/Unit/PatchManagerActionHandlerNavDebounceTests.cpp:341]
- [x] [Review][Patch] Align dump-fail footer with KEEP_DISPLAYED banking/patch wording — `kDeviceDumpAbortedEditedFooter` now says displayed bank/patch were kept; `kDeviceDumpFailedFooter` still only mentions keeping the editor buffer. Same policy, asymmetric user messaging; also polish abort "were kept ... were kept" redundancy. [Source/Shared/Definitions/PluginDisplayNames.h:2145]
- [x] [Review][Defer] Dead `PendingDeviceLoad::priorCoordinates` still written, never read — deferred: already recorded 2026-10-01; cleanup without behavior change. [Source/Core/Actions/PatchManagerActionHandlerDeviceLoad.cpp:59]
- [x] [Review][Defer] Header / same-bank reclaim still select-only Computer abandon — deferred: already recorded 2026-10-01; intent froze abort-on-claim to Internal Prev/Next. [Source/Core/Actions/PatchManagerActionHandlerHeaderClick.cpp:50]
- [x] [Review][Defer] Sibling done stories still document dump-fail coordinate rollback — deferred: agent-context / other specs (`v1-1`, `v1-2`); annotate or supersede outside this delivery. [_bmad-output/implementation-artifacts/v1-1-unsaved-navigation-consistency.md]

#### Rejected

- Spec Code Map / Design Notes / Change Log still describe pre-fix restore paths — `false` (fixing by editing the spec under review is rejected).
- Verification commands omit DeferredGate filter — `false` (would only edit this spec's Verification section).
- `crossPath_dumpFailAfterFurtherInternalSteps` first half may no-op via abandoned generation — `low` rejected (settle-fail half + DeferredGate KEEP_DISPLAYED renames already pin the fail path).
- Dump-fail race asserts severity not footer text — `low` rejected (DeferredGate already pins abort footer text).
- Shared `patchNavDebouncer_` cancel on every Internal Prev/Next — `false` (cancel then re-schedule is the coalesce path; `revertComputerPatchesSelectionIfNeeded` is a no-op when selection already matches).
- Mutator Export / `patchLoadContext_` unasserted under KEEP_DISPLAYED NumberBox after failed dump — `low` rejected (intentional defer-until-success documented in PluginProcessor; everyday Export naming edge case needs its own story if pursued).
