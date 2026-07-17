---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Plugin Full-Patch Push to Edit Buffer
author: BMad Agent
type: bugfix
created: 2026-07-17
status: done
baseline_commit: ad568fdfa47a51d4cf380e822fb691068811b338
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Matrix-1000, Mutator audition (and other plugin→synth full-patch pushes except STORE) send SysEx **0x01** to the current patch slot. That works on RAM banks 0–1 but is silently ignored on ROM banks 2–9, so the GUI updates while the synth sound does not. The intended model is: explore/edit in the synth **edit buffer**, then STORE or SAVE AS when the user wants persistence.

**Approach:** Align every non-STORE full-patch push with INIT: Matrix-1000 → **0x0D** (`sendPatchToEditBuffer`); Matrix-6/6R → **0x01** to current patch number. Keep PASTE/STORE grayed on ROM; PASTE on RAM writes the clipboard into the edit buffer so the user can audition before STORE.

## Boundaries & Constraints

**Always:**
- Branch on `DeviceMemoryLimits::hasBankConcept()` — same rule as Internal INIT (story 7-11).
- STORE remains **0x01** to the current patch slot (intentional RAM write); still blocked on ROM.
- PASTE remains **disabled / blocked on ROM banks**; on RAM it must audition via edit buffer (0x0D), not overwrite the slot until STORE.
- COPY stays available on ROM and RAM (no change).
- Device→plugin dump paths still must **not** echo a full patch back to the synth.
- Prefer one shared audition helper reused by INIT, PASTE, Computer Patches load, and Mutator `pushResultToEditorAndSynth` so the opcode rule cannot drift.

**Ask First:**
- Changing PASTE/STORE enable rules or ROM gating UX.
- Sending 0x0D on Matrix-6/6R (hardware does not support it).
- Implementing DirtyPatchTracker / FR-51 unsaved-edit modal (Epic 9 — deferred).

**Never:**
- Dirty-patch confirmation dialog or Settings policy (Epic 9).
- Changing per-parameter Remote Parameter Edit (0x06 / 0x0B) paths.
- Writing Mutator results into ROM slots or unlocking ROM for PASTE/STORE.
- French text in source; GUI dependency in `Source/Core/`.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Mutate on M-1000 ROM bank | Bank 2–9, Mutate succeeds | One **0x0D** edit-buffer SysEx; APVTS updated; audible on synth | N/A |
| Mutate on M-1000 RAM bank | Bank 0–1, Mutate succeeds | One **0x0D** (not 0x01); RAM slot unchanged until STORE | N/A |
| Mutate / load on Matrix-6 | No bank concept | One **0x01** to current patch number; never 0x0D | N/A |
| PASTE on ROM | Bank ≥ 2, Paste | Blocked; ROM footer; **no** full-patch SysEx | Footer warning |
| PASTE on RAM (M-1000) | Bank 0–1, clipboard ready | APVTS + one **0x0D**; slot not written until STORE | N/A |
| STORE on RAM | Bank 0–1 | One **0x01** to current patch number | N/A |
| Computer `.syx` load (M-1000) | Valid file selected | APVTS + one **0x0D** | Existing load/reconcile errors unchanged |
| History audition / Retry / Compare exit | Mutator push path | Same device branch as Mutate | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` -- `pushResultToEditorAndSynth` always `sendPatch` today; needs limits + audition send
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.h` -- inject `DeviceMemoryLimits` supplier (mirror handler)
- `Source/Core/Actions/PatchManagerActionHandler.cpp` -- INIT already branches; PASTE + `applyLoadedPatchToApvtsAndSynth` still 0x01; STORE stays 0x01
- `Source/Core/MIDI/MidiManager.h` / `.cpp` -- optional home for shared `sendFullPatchForAudition(packed, patchNumber, limits)`
- `Source/Core/PluginProcessor.cpp` -- wire Mutator limits supplier via `getResolvedDeviceMemoryLimits()`
- `Source/Core/Services/DeviceMemoryLimits.h` -- `hasBankConcept()`, `isPasteStoreAllowed()` (ROM gating unchanged)
- `Tests/Unit/PatchMutatorEngineTests.cpp` -- today counts only opcode 0x01; update for M-1000 → 0x0D
- `Tests/Unit/PatchManagerActionHandlerTests.cpp` -- INIT covered; extend PASTE RAM + Computer load; STORE stays 0x01

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/MIDI/MidiManager.*` (or small Core helper next to limits) -- add shared audition send: `hasBankConcept()` → `sendPatchToEditBuffer`, else `sendPatch(patchNumber, …)` -- single SSOT for INIT/PASTE/load/Mutator
- [x] `Source/Core/Actions/PatchManagerActionHandler.cpp` -- route INIT, PASTE, Computer load through helper; leave STORE on `sendPatch`
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngine.*` + `PluginProcessor.cpp` -- inject limits supplier; `pushResultToEditorAndSynth` uses helper (drop unconditional 0x01)
- [x] `Tests/Unit/PatchMutatorEngineTests.cpp` -- assert M-1000 Mutate/Retry/audition enqueue 0x0D once; M-6 path still 0x01; update counters that only matched 0x01
- [x] `Tests/Unit/PatchManagerActionHandlerTests.cpp` -- PASTE RAM M-1000 → 0x0D; PASTE ROM still blocked/no SysEx; Computer load M-1000 → 0x0D; STORE RAM still 0x01; M-6 PASTE/load → 0x01

**Acceptance Criteria:**
- Given Matrix-1000 on a ROM bank with a loaded patch, when the user Mutates (or auditions history), then the synth edit buffer receives 0x0D and the sound changes while the ROM slot is not written.
- Given Matrix-1000 on a RAM bank, when the user Mutates or loads a `.syx` or PASTEs, then the plugin sends 0x0D (edit buffer), not 0x01; STORE still sends 0x01 to the slot.
- Given Matrix-6/6R, when any of those audition paths run, then exactly one 0x01 to the current patch is sent and never 0x0D.
- Given a ROM bank, when PASTE or STORE is invoked, then behavior and enablement stay blocked as today (COPY still allowed).

## Spec Change Log

## Design Notes

INIT already encodes the product rule (7-11). Centralize it so Mutator/Computer/PASTE cannot regress to slot writes. Mutator currently only has `getCurrentPatchNumber_`; it needs the same limits supplier the handler already uses. Existing Mutator tests that count only `kSinglePatchData` will false-fail after the change — count edit-buffer opcode (or both) explicitly per device.

Prototype flow to preserve: COPY anywhere → select RAM slot → PASTE → hear edit buffer → optional STORE to RAM.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- build succeeds
- `./Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` -- exit code 0 (all unit tests pass)

**Manual checks (if no CLI):**
- M-1000 hardware: bank 2+, Mutate → audible change without STORE; bank 0, PASTE → hear clipboard before STORE; STORE writes slot; PASTE gray on ROM.

## Suggested Review Order

**Shared audition opcode**

- Single SSOT: banks → 0x0D edit buffer; else 0x01 to slot
  [`MidiManager.cpp:251`](../../Source/Core/MIDI/MidiManager.cpp#L251)

- API + STORE-vs-audition comment
  [`MidiManager.h:45`](../../Source/Core/MIDI/MidiManager.h#L45)

**Mutator push path**

- Mutate/Retry/history/Compare use helper; skip MIDI if limits supplier missing
  [`PatchMutatorEngine.cpp:1025`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L1025)

- Wire device-limits supplier from processor
  [`PluginProcessor.cpp:257`](../../Source/Core/PluginProcessor.cpp#L257)

**Patch Manager call sites**

- INIT now shares audition helper
  [`PatchManagerActionHandler.cpp:297`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L297)

- PASTE (RAM) → edit buffer; ROM gating unchanged above
  [`PatchManagerActionHandler.cpp:334`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L334)

- STORE remains intentional slot write (0x01)
  [`PatchManagerActionHandler.cpp:359`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L359)

- Computer Patches load → edit buffer on M-1000
  [`PatchManagerActionHandler.cpp:638`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L638)

**Tests**

- Opcode asserts for Mutate M-1000 / M-6
  [`PatchMutatorEngineTests.cpp:452`](../../Tests/Unit/PatchMutatorEngineTests.cpp#L452)

- PASTE / load opcode coverage + STORE exclusivity
  [`PatchManagerActionHandlerTests.cpp:377`](../../Tests/Unit/PatchManagerActionHandlerTests.cpp#L377)
