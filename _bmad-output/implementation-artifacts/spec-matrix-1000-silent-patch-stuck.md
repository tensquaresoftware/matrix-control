---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Matrix-1000 silent after MIDI CHANNEL combo
author: BMad Agent
type: bugfix
created: 2026-09-07
updated: 2026-09-07
status: done
route: dispatch
baseline_commit: 8b6222afbd8cb817306889b513ef04f3f6a9cb78
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/1-2-mastermodel-packed-buffer.md'
  - '{project-root}/_bmad-output/implementation-artifacts/1-4-apvtsmastermapper-round-trip.md'
  - '{project-root}/_local/Archives/Study/Specifications/matrix-1000-parameters-gui-logic.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Scrolling Master Edit → MIDI → CHANNEL can leave the Matrix-1000 silent and stuck (here: front-panel patch 4) until an Enter-hold power-on reset. The CHANNEL combo is meant to drive three Master fields (basic channel, Omni, Mono), but the code writes the raw menu index into byte 11 only — Story 1.2 deferred the real composition to 1.4, and 1.4 shipped without it. Notes and Program Change still leave on MIDI channel 1, so a Matrix listening elsewhere ignores them.

**Approach:** Fix only the CHANNEL composition (Omni / channel / Mono) so the combo writes and reads the three Oberheim bytes correctly. Out of scope this run: retargeting Program Change channel, closed-combo label fit, browse/presence hardening.

**Decisions (Guillaume 2026-09-07):**
- Scope = CHANNEL composition only.
- Soft power-cycle was not enough; Enter-hold boot reset restored sound; unit now on Omni.
- “Stuck on 4” = Matrix front-panel patch number.

## Boundaries & Constraints

**Always:**
- Encode/decode CHANNEL using the product mapping from `matrix-1000-parameters-gui-logic.md`:
  - **OMNI** → basic channel 1, Omni=1, Mono=0
  - **CHANNEL n** (n=1…16) → basic channel n, Omni=0, Mono=0
  - **MONO Gx** (x=1…9) → basic channel x, Omni=0, Mono=1
- Basic channel on the wire is 0–15 (channel − 1) in Master byte 11; Omni in byte 12; Mono in byte 35.
- Composition lives in Core (mapper or small codec called from the mapper) — not in GUI panels.
- Full master `0x03` send on CHANNEL change may stay as today once the buffer bytes are correct.
- Unit-test encode + buffer→UI for OMNI, CHANNEL 1, CHANNEL 16, MONO G1, MONO G9.
- English-only source; no GUI includes from Core.

**Never:**
- Writing the raw combo index 0–26 into byte 11.
- Expanding this run to Program Change channel retarget, combo truncation, or select-dump coalescing (defer).
- Claiming software replaces Enter-hold reset when the Matrix is already wedged.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Select OMNI | Combo → OMNI | Byte 11=0, byte 12 Omni=1, byte 35 Mono=0 | Full master dump |
| Select CHANNEL n | Combo → CHANNEL n | Byte 11=n−1, Omni=0, Mono=0 | Clamp n to 1…16 |
| Select MONO Gx | Combo → MONO Gx | Byte 11=x−1, Omni=0, Mono=1 | Clamp x to 1…9 |
| Buffer → UI Omni | Omni=1, Mono=0 | Combo shows OMNI | Ignore stale channel for label |
| Buffer → UI poly | Omni=0, Mono=0 | Combo shows CHANNEL (byte11+1) | |
| Buffer → UI mono | Mono=1 | Combo shows MONO G(byte11+1) if 1…9 | If channel>9: clamp display to G9 (document) |
| Host notes on ch1 after OMNI / CHANNEL 1 | Post-fix master | Matrix accepts ch1 traffic | Pre-fix wedge still needs hardware reset |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp` — `kChannel` choice list; offsets comment 11/12/35
- `Source/Core/Models/MasterModel.{h,cpp}` — generic `setChoiceIndex` (byte 11 only today)
- `Source/Core/Models/ApvtsMasterMapper.cpp` — special-case CHANNEL sync both directions (or call codec)
- New small Core helper preferred (e.g. `MidiChannelMasterCodec`) if mapper special-case would bloat — keep MasterModel generic accessors clean per Story 1.2 intent
- `Source/Core/MIDI/MasterParameterSysExDispatcher.cpp` — still sends full master after buffer update
- `Tests/Unit/ApvtsMasterMapperTests.cpp` (or new unit file) — CHANNEL composition cases
- `_local/Archives/Study/Specifications/matrix-1000-parameters-gui-logic.md` — SSOT menu mapping (§ MIDI Channel Menu Mapping)
- `_local/References/Oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` — global params 11 / 12 / 35

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Models/` — add CHANNEL encode/decode (codec or mapper special-case) for bytes 11/12/35 per frozen mapping
- [x] `Source/Core/Models/ApvtsMasterMapper.cpp` — use composition for `midiChannel` both APVTS→buffer and buffer→APVTS; other choices unchanged
- [x] `Tests/Unit/` — OMNI / CHANNEL 1 / CHANNEL 16 / MONO G1 / MONO G9 round-trips + one odd Mono channel>9 display rule
- [x] Build + `python3 Scripts/quality/lint_touched.py` on touched C++

**Acceptance Criteria:**
- Given CHANNEL → CHANNEL 4, when master is sent, then byte 11=3, Omni=0, Mono=0 — never raw index 4 alone with stale Omni/Mono.
- Given CHANNEL → OMNI, when master is sent, then Omni=1, Mono=0, basic channel 1 (byte 11=0).
- Given CHANNEL → MONO G3, when master is sent, then Mono=1, Omni=0, byte 11=2.
- Given a master buffer with those three patterns, when pushed to APVTS, then the combo index matches OMNI / CHANNEL n / MONO Gx.
- Given hardware UAT after fix, when user selects OMNI then CHANNEL 1 then back to OMNI, then the Matrix keeps responding to channel-1 notes without requiring Enter-hold reset.

## Implementation Notes

- Added `Core::MidiChannelMasterCodec` (encode/decode combo ↔ bytes 11/12/35).
- `ApvtsMasterMapper` special-cases `midiChannel` both directions; other choices unchanged.
- `MasterModel` accessors stay single-offset (byte 11 only).
- `MasterModuleInitService` copies CHANNEL via the codec so Omni/Mono survive MIDI module init.
- Tests: `MidiChannelMasterCodecTests` + mapper composition round-trip; init suppress test asserts CHANNEL via codec.

## Spec Change Log

- 2026-09-07 — Implemented CHANNEL composition; tasks marked done; status → done.

## Review Triage Log

- `false` — Spec status vs Spec Change Log contradiction — artifact housekeeping only; reject findings whose fix is to edit this build's spec.
- `false` — Code Map still says byte-11-only — docs-only; reject (edit-spec).
- `medium` → **defer** — `MasterModel::setChoiceIndex` still writes raw CHANNEL index into byte 11 — intentional generic accessor; live edit path uses codec via mapper; remaining callers are tests / future misuse.
- `medium` → **patch** — `moduleBytesMatch` compares CHANNEL via `getChoiceIndex` (byte 11 only) — real gap: Omni/Mono init regressions stay green (verification-gap).
- `medium` → **patch** — No MIDI-init assertion on bytes 12/35 with divergent Omni/Mono — same root cause as above.
- `low` — Codec out-of-range combo index tests missing — clamp is trivial; unlikely everyday failure; reject (not worth extra complexity).
- `low` → **patch** — Omni+Mono both non-zero decode (Mono wins) undocumented by tests — one-line case next to existing clamp test.
- `low` — Mapper composition only 0/4/19 — CHANNEL 1/16/G1/G9 already covered in `MidiChannelMasterCodecTests`; reject.
- `medium` → **defer** — `MasterParameterSysExDispatcherTests` seeds CHANNEL with `setChoiceIndex` — pre-existing test outside this diff; not caused by production CHANNEL path.
- `medium` → **patch** — `fillModelWithDistinctValues` dirties CHANNEL byte 11 only — same init-test root cause.
- `false` — Spec Never “0–26” off-by-one — frozen wording; reject (edit-spec).
- `false` — Null / undersized buffer in codec — production always passes `MasterModel::data()` with `kBufferSize`; unreachable (edge-case-hunter).
- `medium` → **patch** — Verification-gap: MIDI module init CHANNEL copy does not assert Omni/Mono — confirmed; fixture Omni/Mono are 0.

## Design Notes

Locked mapping (product SSOT):

```
OMNI     → basic=1, Omni=1, Mono=0
CHANNEL n → basic=n, Omni=0, Mono=0   (n=1…16)
MONO Gx  → basic=x, Omni=0, Mono=1   (x=1…9)
```

Wire byte 11 stores basic−1. Do not change Panic’s existing channel logic in this run. Defer: PC outbound channel, 60 px combo clip, dump coalescing.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` — expected: success
- Targeted unit tests for CHANNEL composition — expected: pass
- `python3 Scripts/quality/lint_touched.py` — expected: clean on touched C++

**Manual checks:**
- After Enter-hold recovery (already done): confirm Omni.
- Post-fix: change CHANNEL OMNI ↔ CHANNEL 1 ↔ one MONO Gx ↔ OMNI; notes on ch1 still sound; no need for Enter-hold reset.
