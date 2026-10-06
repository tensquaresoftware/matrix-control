---
organization: Ten Square Software
project: Matrix-Control
title: Spec — PANIC Note Off spray for Matrix silence
author: BMad Agent
status: done
type: bugfix
route: oneshot
baseline_commit: 81e696db9c85efe9d35a6c9d1f2c5bf48bae80bb
created: 2026-10-06
updated: 2026-10-06
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-bug-midi-01-residual-panic-alert.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** PANIC already sends All Sound Off / All Notes Off / Reset All Controllers on the panic channel(s), but a Matrix-1000 with a held pad chord does not silence — MIDI Monitor confirms the CCs leave the host while audio continues until key Note Offs. Users expect PANIC to cut sound immediately.

**Approach:** Keep the existing CC 120 → 123 → 121 triple and channel rules (1–16 vs Omni/Mono → all 16). On each panic channel, also enqueue Note Off for pitches 0–127 via the realtime front path. Do not track held notes; full spray is the safety net. Accept a brief queue-pressure alert flash from the burst depth.

**Decisions locked:** Full 0–127 Note Off spray (option 1, 2026-10-06) in addition to the CC triple; same channel resolution as today.

</frozen-after-approval>

## Implementation Notes

Agent plan (oneshot):

- `MidiManager::sendPanic` (`Source/Core/MIDI/MidiManager.cpp`) — per panic channel, after assembling payload, enqueue Note Offs 0–127 plus CC 120/123/121. Dequeue order per channel: Note Off 0…127, then CC 120, 123, 121 (Matrix ignores CCs; Note Offs first for faster silence). Update stale comment in `MidiManager.h`.
- **Do not** call `enqueueRealtimeFront` once per message: each call rebuilds the whole realtime queue (O(n²) / UI hitch for ~2k msgs on Omni). Add a batch front-insert on `MidiOutboundQueue` (e.g. `enqueueRealtimeFrontMany`) that prepends an ordered sequence in one lock, then wake once. Unit-test the batch helper if cheap.
- Preserve: header wiring, enable-when-TO-set, pressure hysteresis thresholds, no GUI-from-Core, `notifyActivity` on panic.
- Tests: extend `Tests/Unit/MidiManagerRealtimeTests.cpp` — Omni depth = 16 × (128 + 3) = 2096; per channel assert Note Off 0…127 then CC 120/123/121. Add a single-channel case if `midiChannel` param is easy to fixture; otherwise Omni-only is enough if channel loop unchanged.
- Help string optional tweak only if copy is misleading; not required for AC.
- Verify: build + `ctest` filter on MidiManager realtime / Panic; `lint_touched.py` on touched C++.
- Hardware UAT (Guillaume): held pad chord + PANIC → silence without releasing keys; MIDI Monitor shows Note Offs then CCs.

Done 2026-10-06:

- Added `MidiOutboundQueue::enqueueRealtimeFrontMany` (single-lock prepend); kept `enqueueRealtimeFront` as a direct single-message rebuild (no one-element vector alloc).
- `MidiManager::sendPanic` lives in `MidiManagerPanic.cpp`: Note Off 0–127 then CC 120/123/121; Omni/Mono interleaves Note Offs across channels then CCs; basic channel stays channel-major.
- Help copy updated (no longer claims easing a backed-up queue).
- Tests: missing-param Omni, choice Omni index 0, basic channel 3, FrontMany prepend. Lint OK after extract.
- Expect brief queue-pressure alert flash on Omni Panic (depth ≫ 32) — accepted in Intent.

## Review Triage Log

- Blind: no Omni choice fixture — **medium** → patched (Omni index 0 test).
- Blind: channel-major Omni delays late channels — **medium** → patched (interleaved Note Offs).
- Blind: help still claims easing backed-up queue — **medium** → patched (help string).
- Blind: sibling done specs still describe CC-only Panic / Note Off out of scope — **defer** (housekeeping).
- Blind: status still in-progress / missing formal AC — **false** (oneshot finalize sets done; AC not required on oneshot route).
- Blind: `enqueueRealtimeFront` one-element vector alloc — **low** → patched (restore direct path).
- Blind: FrontMany no queue reserve under lock — **low** rejected (std::queue has no reserve; rare backlog + Panic).
- Blind: no Note Off velocity-0 assert — **low** rejected (JUCE noteOff default; unlikely everyday harm).
- Blind: no MidiManager-level prepend test + stale test name — **low** rejected (queue helper covers prepend; names updated in patch pass).
