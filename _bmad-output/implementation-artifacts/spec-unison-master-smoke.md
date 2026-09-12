---
title: 'Matrix-1000 Unison smoke: Master override vs Keyboard Mode listening'
type: 'bugfix'
created: '2026-09-12'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '1f5028632d1adcc64d3d17011c4ca2faca1af6a6'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-unison-matrix-1000.md'
  - '{project-root}/_local/References/Oberheim/oberheim-matrix-1000-owners-manual.md'
  - '{project-root}/_local/References/Oberheim/oberheim-matrix-1000-midi-sysex-implementation.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On a real Matrix-1000, smoke listening can disagree with Oberheim: the UI may show Master Unison ON (badge M) while the sound thins after Keyboard Mode leaves UNISON, or patch UNISON fails to stay thick after Master Unison turns OFF.

**Approach:** Diagnose Master Unison (byte 169 → debounced full Master dump 0x03) and Keyboard Mode (param 48 → Remote Edit 0x06). Fix the plugin only if a clear local bug is proven; otherwise document evidence plus a hardware verification protocol (panel `Uon`/`Uof` + MIDI log). Never invent patch↔master sync to hide the symptom.

## Boundaries & Constraints

**Always:**
- Keyboard Mode (param 48 / byte 8) and Master Unison (`miscUnisonEnable`, byte 169) stay independent — never sync or rewrite one when the other changes.
- Badge M is UI-only (Master ON ∧ Keyboard Mode ≠ UNISON, Matrix-1000) — not hardware proof.
- Listening matrix: Master OFF + non-UNISON → thin; Master OFF + UNISON → thick patch; Master ON + non-UNISON → thick Master override; Master ON + UNISON → thick.
- Scenario A: Master ON must stay thick when Keyboard Mode leaves UNISON.
- Scenario B: Master OFF with Keyboard Mode still UNISON must stay thick (patch Unison).
- If plugin code is correct but hardware remains ambiguous: proofs + panel/MIDI verification protocol only.

**Never:**
- Force Keyboard Mode = UNISON when Master is ON.
- Global Oberheim audit, Unison GUI/badge/STRIG/LEGATO refactors (unless proven cause), Matrix-6/6R Master Unison UI, commit/PR unless asked.
- Change the listening matrix without hard hardware evidence.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| A override holds | Master ON; Keyboard Mode → ROTATE (≠ UNISON) | Byte 169 stays 1; no side-clear; listening stays thick if device accepted Master | If thin with UI Master ON: prove 0x03 / panel `Uon` vs UI-only |
| B patch resumes | Keyboard Mode = UNISON; Master ON then OFF | Byte 8 stays UNISON (2); 169 → 0; listening stays thick | If thin: prove param 48 on device |
| Independence | Change only one domain | Only that domain’s SysEx/buffer updates | N/A |

</frozen-after-approval>

## Code Map

- `PluginProcessorValueTree.cpp` — Master: debounce 150 ms → `dispatchFull` → `sendMaster(0x03)`; patch: separate Remote Edit path.
- `ApvtsMasterMapper` / `MasterModel` / `MasterParameterSysExDispatcher` / `MidiManager::sendMaster` / `SysExEncoder::encodeMasterSysEx` — choice → byte 169 → full 172-byte Master dump; no Unison special-case.
- `ApvtsPatchMapper` / `PatchParameterSysExDispatcher` / `enqueueRemoteParameterEdit` — Keyboard Mode → param **48**, packed byte **8** only (not Master 0x03).
- Descriptors: `PluginDescriptorsMasterEdit.cpp` offset 169; `PluginDescriptorsPatchEditFmRamp.cpp` offset 8 / id 48 (0–3 = REASGN/ROTATE/UNISON/REAROB).
- `UnisonKeyboardModePolicy.h`, `StrigUnisonGateHelper.*` — UI/STRIG reads; STRIG clear may emit extra patch 0x06, must not write 169.
- Tests: `UnisonKeyboardModePolicyTests.cpp` (8↔169 independence), `ApvtsMasterMapperTests.cpp` (Unison ↔ 169). Deferred: dedicated Unison→0x03 encode assert.
- SSOT: owners-manual Ext. Funct. Unison override; MIDI impl param 48 / octet 169 / 0x03. Prior UX: `spec-unison-matrix-1000.md`.

**Planning findings:** No plugin cross-write 48↔169. KM change does not clear Master buffer in code. Smoke A/B with UI Master still ON likely means outbound/gate/device/`Uon` mismatch or firmware clearing Master on patch edit — confirm with MIDI log + panel before inventing fixes. Do not confuse STRIG clear with losing six-voice Unison fatness.

## Tasks & Acceptance

**Execution:**
- [x] Core Master/Patch MIDI chain (`PluginProcessorValueTree.cpp` and Code Map) -- Walk scenarios A/B; cite whether any path touches the other domain or drops Master 0x03 -- Prove or refute plugin bug
- [x] `Tests/Unit/*` (+ `Tests/CMakeLists.txt` if new) -- Non-regression: KM change does not alter byte 169 and vice versa; if cheap, Unison Enable in full 0x03 encode/dispatch -- Lock independence
- [x] Plugin Core only if bug confirmed -- Minimal fix on proven fault; never cross-sync 48↔169 -- Restore documented listening
- [x] This spec Implementation Notes -- If no local bug: evidence + verification protocol (panel `Uon`/`Uof`, MIDI log of 0x03/0x06, order vs 150 ms debounce) -- Actionable smoke without inventing sync

**Acceptance Criteria:**
- Given Master Unison ON in APVTS/buffer, when Keyboard Mode changes, then byte 169 is unchanged and no side path writes it from param 48.
- Given Keyboard Mode = UNISON, when Master Unison turns OFF, then byte 8 stays UNISON and is not cleared.
- Given a confirmed plugin bug, when the minimal fix lands, then tests guard it and independence still holds.
- Given correct plugin behaviour, when the story closes, then Implementation Notes hold proofs + panel/MIDI protocol — with no new patch↔master Unison sync.

## Implementation Notes

### Verdict (2026-09-12)

**No local plugin bug proven.** Keyboard Mode (param 48 / packed byte 8) and Master Unison (`miscUnisonEnable` / packed byte 169) remain independent end-to-end. No production Core/GUI change; no patch↔master Unison sync added.

### Code-walk evidence (scenarios A/B)

- `PluginProcessor::valueTreePropertyChanged` calls both `dispatchPatchOrMatrixModParameterChange` and `dispatchMasterParameterChange`, but each early-returns on disjoint ID sets (0 overlap across patch vs master descriptors).
- Keyboard Mode → `PatchParameterSysExDispatcher::dispatch` → `enqueueRemoteParameterEdit` (opcode Remote Edit / param **48** only). Never writes Master buffer / never schedules Master 0x03.
- Master Unison → `apvtsMasterMapper_->apvtsToBuffer()` then 150 ms debounce → `dispatchFull` → `MidiManager::sendMaster(0x03, …)` full 172-byte dump including octet **169**. Never writes patch byte 8.
- STRIG clear on leaving UNISON may enqueue extra **patch** Remote Edits only; it does not touch byte 169.
- Outbound gates that can **drop** Master 0x03 while the UI still shows Master ON (not a cross-domain rewrite): `isMasterEditOutboundAllowed()` (device detected + Matrix-1000), `isEditorOutboundAllowed()`, `suppressMasterParameterSysEx_`, debounce cancel before fire. Treat thin sound with badge M as **outbound/device/`Uon` mismatch** until MIDI log + panel prove otherwise.

### SSOT refs

- Owners manual: front-panel Unison (`Uon`/`Uof`) overrides pre-programmed Unison (listening priority when Master is ON — not permission to rewrite Keyboard Mode).
- MIDI impl: Keyboard Mode param 48 / packed octet 8 (0–3); Master Unison Enable packed octet 169; Master dump opcode 0x03.

### Tests added / extended

- `UnisonKeyboardModePolicyTests`: Scenario A (KM → Remote Edit 48, Master 169 unchanged, Master dispatcher ignores KM id); Scenario B (Master Unison OFF → 0x03 with decoded 169=0, patch byte 8 stays UNISON, Patch dispatcher ignores Master id); Unison Enable ON survives full 0x03 encode/decode at octet 169.
- `MasterParameterSysExDispatcherTests`: Unison Enable → full 0x03 with decoded octet 169 ON.

### Verification run

- `cmake --build --preset macos-debug-arm64` — OK
- `ninja -C Builds/macOS/ARM/Tests Matrix-Control_Tests` then `--category "Unison Keyboard Mode Policy"` and `--category "MasterParameterSysExDispatcher"` — 0 failures
- `python3 Scripts/quality/lint_touched.py` — clean on touched C++

### Hardware verification protocol (if smoke A/B still disagrees)

1. Connect Matrix-1000; confirm device detected and Master Edit outbound allowed.
2. Capture MIDI out (plugin → synth). Prefer a log that shows SysEx opcodes and timestamps.
3. **Scenario A:** set Master Unison ON (expect one debounced **0x03** after ~150 ms; inspect packed/nibbled octet 169 = 1). Change Keyboard Mode UNISON → ROTATE (expect **0x06** Remote Edit param 48 value 1; **no** second Master rewrite of 169). On the panel Ext. Funct. Unison, read `Uon` vs `Uof`. Listen: should stay thick if device accepted Master Unison.
4. **Scenario B:** Keyboard Mode = UNISON; Master Unison ON then OFF (expect **0x03** with 169 = 0; **no** Remote Edit clearing param 48). Panel should show `Uof`. Listen: should stay thick from patch Unison (byte 8 = 2).
5. If UI Master ON but no outbound 0x03 → investigate gates / debounce cancel (plugin send path). If 0x03 with 169=1 on the wire but panel `Uof` or thin sound → device/firmware acceptance, not plugin cross-sync.
6. Do **not** invent plugin sync of 48↔169 to hide the symptom.

## Spec Change Log

- 2026-09-12: Diagnosis complete — no Core fix; independence tests + Implementation Notes / hardware protocol.

## Review Triage Log

- blind: Code Map still says deferred Unison→0x03 assert — `false` — reject (fix would edit this build's spec); tests already deliver the assert; map hygiene only.
- blind: Review Triage Log empty at in-review — `false` — process timing; this log is the triage outcome.
- blind: Conditional Core-fix task marked [x] without N/A note — `false` — reject (spec edit); task means “fix only if bug”; no-bug path correctly marks complete with no Core change.
- blind: No explicit AC that smoke closure requires hardware protocol — `false` — frozen AC already requires Implementation Notes + panel/MIDI protocol when plugin is correct.
- blind: No new test for Master 0x03 drop via outbound gates while UI Master ON — `false` — intent chose hardware MIDI/panel protocol for that failure mode; gates pre-exist and were not changed.
- blind: Scenario A/B tests omit STRIG clear path — `false` — STRIG emits patch Remote Edits only; smoke claim is 48↔169 independence, already asserted.
- blind: Debounce 150 ms not asserted in new tests — `false` — Master debounce covered elsewhere; this diff does not change debounce behaviour.
- blind: Master dispatcher Unison test covers ON only — `low` — rejected (Scenario B already asserts OFF→0x03 with 169=0; everyday harm negligible).
- blind: Encode/decode survival bypasses dispatcher — `false` — `MasterParameterSysExDispatcherTests` Unison case exercises dispatcher enqueue + decode of octet 169.
- blind: Magic numbers 48/8/169 hard-coded — `low` — rejected (cosmetic; descriptor offset already checked on Unison Enable path).
- blind: Scenario A uses index 1 as ROTATE without re-asserting choice label — `false` — same suite already asserts indices 0–3 = REASGN/ROTATE/UNISON/REAROB.
- blind: Duplicate Verification sections can drift — `false` — reject (spec edit only).
- blind: No automated bridge from listening matrix thick/thin to unit tests — `false` — listening is hardware; unit tests intentionally prove SysEx/buffer independence only.
- blind: `_local` Oberheim paths may be unavailable in clean clone — `false` — environment/docs availability, not a defect in this change.
- blind: Spec does not separate pre-known vs newly proven evidence — `false` — reject (spec edit); Implementation Notes already cite code-walk + new tests.
- edge-case-hunter: no findings.
- verification-gap: no verification gaps found.

## Design Notes

Diagnose in order: independence → enqueued SysEx (0x03 byte 169 vs Remote Edit 48) → outbound gates → panel `Uon`/`Uof` + MIDI log → fix only with a smoking gun. Owners-manual “front-panel Unison overrides the pre-programmed Unison” = listening priority when Master is ON, not permission to rewrite Keyboard Mode in the plugin.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- build OK
- Targeted unit tests for Unison independence / any new 0x03 assert -- pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks:**
- Matrix-1000 smoke A/B with MIDI log + Ext. Funct. Unison (`Uon`/`Uof`) after each step vs listening matrix.
