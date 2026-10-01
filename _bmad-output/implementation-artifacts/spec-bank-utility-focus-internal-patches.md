---
title: 'Bank Utility reclaim Internal focus'
type: 'bugfix'
created: '2026-10-01'
status: 'done'
route: 'dispatch'
baseline_commit: '00acfa36c758554b0e0f98b3df601a91afef39d4'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-nav-internal-header-reload-bank-reclick.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After Computer Patches owns navigation focus (red combo while browsing `.syx`), re-clicking the already-current Bank Utility bank does nothing — focus stays on Computer until the Internal header is clicked. Clicking a different bank already works (Internal focus + load patch `00`); intermittent reports matched the same-bank case when OPEN left coordinates on that bank.

**Approach:** Keep same-bank reclick as a full no-op when Internal already owns focus (no `00` reset, no gate). When Computer owns focus and the user clicks the already-current bank, behave like clicking the Internal Patches module label: Internal focus + MIDI selection sync for the current slot + device dump (patch number unchanged). Different-bank clicks stay as today: load that bank’s patch `00` with Internal focus.

**Decision (Q1):** Same-bank reclaim while Computer owns focus = same outcome as Internal Patches header reclaim (focus + synth/editor reload of current slot), not focus-only and not keep no-op.

## Boundaries & Constraints

**Always:**
- Same-bank reclick while Internal already owns focus: keep full no-op (no load, no gate, no MIDI).
- Same-bank reclick while Computer owns focus: Internal focus + cancel pending Computer settle/debouncers + `syncSelection` for current bank/patch + device dump; keep patch number; skip unsaved gate; do not force `00`.
- Different-bank click: load destination bank’s patch `00`, marker move, Internal focus (existing behaviour — required product path).
- Reclaim must update `patchManagerNavigationFocus` to Internal so Internal NumberBoxes go red and Computer combo returns to normal.
- English UI/code only.

**Never:**
- Force patch `00` on same-bank reclick.
- Prompt the unsaved gate on a pure same-bank reclaim.
- Change Computer OPEN/Prev/Next/combo focus ownership.
- Change Bank Utility badge-marker paint rules beyond what focus already drives.
- Rewrite historical done frozen specs; this story renegotiates reclaim on Computer-owned same-bank only.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Same bank + Computer focus | Coords bank 2 / patch 17; focus Computer; click bank 2 | Focus → Internal; patch stays 17; pending Computer settle cancelled; MIDI sync + device dump for 2/17; no `00` reset; no unsaved gate | Gate not called |
| Same bank + Internal focus | Focus Internal; click current bank | Full no-op (today) | N/A |
| Different bank | Bank 2 → click bank 4 | Load 4/00; focus Internal (unchanged path) | Unsaved confirm as today |
| Matrix-6/6R bank buttons | No bank concept | Handler no-op as today | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` — `tryHandleBankButtonSelection` delegates same-bank to `tryHandleSameBankReclick`; different-bank path unchanged
- `Source/Core/Actions/PatchManagerActionHandlerHeaderClick.cpp` — `tryHandleSameBankReclick` + shared `reloadCurrentInternalSlotFromDevice` (also used by Internal header reclaim)
- `Source/Core/Actions/PatchManagerActionHandler.h` — private helpers above; no new public API
- `Source/Shared/Definitions/PluginIDs.h` — `NavigationFocus::{kNone,kInternal,kComputer}` / `kNavigationFocus` (read-only)
- `Tests/Unit/PatchManagerActionHandlerBankReclickTests.cpp` — Computer-focus reclaim assertions; Internal-focus no-op and different-bank `00` kept
- Read-only GUI: `InternalPatchesPanel` / `ComputerPatchesPanel` / `BankUtilityPanel` already react to `kNavigationFocus`

## Tasks & Acceptance

**Execution:**
- [x] `PatchManagerActionHandlerInternalPatches.cpp` -- on same-bank click while Computer owns focus, reclaim like Internal header (minus unsaved gate): focus + abandon Computer settle/debouncers + syncSelection + dump; keep Internal-focus same-bank no-op; update stale early-return comment -- fix Computer-focus same-bank bug
- [x] `PatchManagerActionHandlerBankReclickTests.cpp` -- replace Computer-focus no-op test with reclaim assertions (focus Internal, patch unchanged, dump request, Program Change for current slot, gate not called); keep Internal-focus no-op and different-bank `00` cases -- prevent regression
- [x] `Scripts/quality/lint_touched.py` -- pass on touched C++ -- quality gate

**Acceptance Criteria:**
- Given Computer focus and established bank N / patch P, when the user clicks bank N, then focus becomes Internal, patch stays P, pending Computer settle is cancelled, and MIDI selection sync + device dump run for that slot (no forced `00`, no unsaved gate).
- Given Internal focus and bank N / patch P, when the user clicks bank N, then behaviour stays a full no-op.
- Given bank N, when the user clicks a different bank M, then load M/`00` with Internal focus as today.

## Implementation Notes

Mirror `tryHandleModuleHeaderClicks` Internal path for Computer-owned same-bank reclaim, except skip `confirmPatchContextChange`. Required steps: `setNavigationFocus(kInternal)`, abandon pending Computer select settle + cancel patch/computer debouncers, `syncSelection(current bank, current patch)`, `beginPendingDeviceLoad` + `loadCurrentPatchFromDevice`. Do not force patch `00`. When Internal already owns focus, keep the unconditional same-bank early-return.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Blind: no test that pending Computer settle is cancelled on Computer same-bank reclaim | medium | Real: reclaim test never arms/flushes Computer select settle; production cancel could be deleted and suite stay green. → patch |
| Verification-gap: same missing pending Computer settle cancel assertion | medium | Pre-verified gap; same root cause as Blind row above. → patch (grouped) |
| Edge: `abandonPendingInternalNavSettle` after `setNavigationFocus(kInternal)` can restore Computer focus | false for claim as Computer+pending Internal | Computer focus paths abandon Internal settle before setting Computer focus (`scheduleComputer*` / `loadSelectedPatchFileImmediately`); Computer reclaim therefore cannot meet a live Internal baseline. Order still improved defensively in patch (set focus after reload). |
| Blind: `kNone` same-bank untested | low | Same early-return branch as Internal-focus no-op; everyday path is Internal vs Computer. Rejected (extra test complexity, negligible product risk). |
| Blind: Computer reclaim never fires deferred dump callback | false | Test asserts dump request + MIDI for current slot; dump completion is shared device-load infra already covered elsewhere. |
| Blind: `skipsUnsavedGate` does not set focus | false | Internal no-op still skips gate; Computer reclaim test already sets `gate.allow = false` and asserts `calls == 0`. |
| Blind: call-site lost “no patch 00” comment | low | Real comment gap after extract. → patch |
| Blind: no Computer-focus + different-bank unit case | false | Different-bank path unchanged; existing different-bank test covers load `00` + Internal focus; pre-click focus does not alter that branch. |

## Implementation Notes

Mirror `tryHandleModuleHeaderClicks` Internal path for Computer-owned same-bank reclaim, except skip `confirmPatchContextChange`. Required steps: abandon pending Computer/Internal settles + cancel debouncers, `syncSelection(current bank, current patch)`, `beginPendingDeviceLoad` + `loadCurrentPatchFromDevice`, then `setNavigationFocus(kInternal)` after reload so an Internal-nav baseline restore cannot leave Computer focus. Do not force patch `00`. When Internal already owns focus, keep the unconditional same-bank early-return.

Review patches applied: Computer settle cancel asserted in BankReclick tests; Internal focus set after reload on reclaim and Internal header; same-bank call-site comment restored.

## Design Notes

Prior done story `spec-patch-nav-internal-header-reload-bank-reclick` made same-bank reclick a full early-return, including Computer focus. Product now wants Bank Utility same-bank click to reclaim when Computer owns focus, matching a click on the Internal Patches label. Different-bank already sets Internal focus and loads `00` — successful UAT of “random other bank” matches that path; the reported bug is specifically re-clicking the already-current bank.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: success
- Run unit target covering `PatchManagerActionHandlerBankReclick` -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- M-1000: Internal bank 2 / patch 17 → OPEN/browse `.syx` (Computer red) → click bank **2** → Internal red, patch stays 17, synth/editor return to that slot
- M-1000: Computer focus → click a **different** bank → Internal red + patch `00` of that bank
