---
title: 'Internal header recall + bank reclick no-op'
type: 'bugfix'
created: '2026-09-07'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-clickable-patch-module-headers-reload.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Clicking the INTERNAL PATCHES module header updates focus and PATCH NAME but leaves the synth playing the last Computer `.syx` audition, because reclaim dumps device memory into the editor without recalling that slot on the hardware. Re-clicking the already-current Bank Utility bank also surprises users by forcing patch `00`.

**Approach:** On Internal header reclaim when coordinates are established, reuse the same MIDI selection sync that Internal bank/Prev/Next use (Set Bank + Program Change) before the existing device dump, so editor and synth both return to the current internal slot. When the clicked bank is already the current established bank, no-op: keep patch number, skip load/`00` reset, skip focus/MIDI side effects.

</frozen-after-approval>

## Implementation Notes

- Agent decision: bank reclick no-op is full early-return when coordinates established and bank equals `getCurrentBank` — including when focus is Computer (user reclaims via Internal header). Guard sits before unsaved-edit confirm so a redundant bank click does not prompt.
- Root cause Internal header: `tryHandleModuleHeaderClicks` called `loadCurrentPatchFromDevice` but never `PatchSelectionMidiSync::syncSelection`; Computer header uses `sendFullPatchForAudition` via `.syx` load. After a Computer audition, dump alone updates APVTS/name; synth edit buffer stays on `.syx` until Program Change recalls memory.
- Fix: Internal header path calls `syncSelection(bank, patch, limits, true)` then existing device dump. Bank reclick early-returns when established + same bank.
- Tests: strengthened HeaderClick assertions (`lastProgramChange`, Set Bank, empty queue on gate cancel); new `PatchManagerActionHandlerBankReclickTests` (no-op, Computer-focus no-op, skip gate, different bank still loads `00`).
- No commit (user request). Standalone Debug built for Matrix-1000 UAT.

## Review Triage Log

- Blind Hunter: gate-cancel lacked empty-queue assert — **medium** → **patch** (assert added).
- Blind Hunter: PC assertions lacked `lastProgramChange` — **low** → **patch** (asserted for slots 17/8/12).
- Blind Hunter: ordering test name vs dump-hook timing — **low** rejected (fake dump cannot snapshot mid-call outbound idle; production MidiManager still waits for idle).
- Blind Hunter: Computer-focus bank reclick untested — **medium** → **patch** (new test).
- Blind Hunter: reclick should skip unsaved gate when dirty — **medium** → **patch** (gate allow=false, calls==0).
- Blind Hunter: different-bank missing PC to 00 — **low** → **patch**.
- Blind Hunter: already-focused Internal missing forced Set Bank assert — **low** → **patch**.
- Blind Hunter: oneshot lacks full AC/UAT sections — **false** (oneshot route intentionally Intent + Implementation Notes only).
- Blind Hunter: sibling done spec still dump-only wording — **defer** (historical done artifact; do not rewrite in this bugfix).
- Blind Hunter: status still in-progress — **low** → **patch** (set `done`).
