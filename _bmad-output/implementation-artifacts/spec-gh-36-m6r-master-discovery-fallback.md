---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Matrix-6/6R Master discovery fallback
author: BMad Agent
type: bugfix
created: '2026-10-28'
status: done
route: dispatch
baseline_commit: f8e599bd1235dde14e883a37e5497385f7bf303a
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/8-2-device-inquiry-and-footer-identity.md'
sources:
  - 'https://github.com/tensquaresoftware/matrix-control/pull/36'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Some Matrix-6 / 6R units ignore Universal Device Inquiry, so Matrix-Control never detects the synth even when Oberheim MIDI works. The editor stays locked and the user looks disconnected.

**Approach:** Keep Device Inquiry first. Only when that identity attempt times out without a soft-abort, request Master parameters and treat a valid Matrix-6/6R Master dump as proof of the Matrix-6 / 6R family. Reimplement the idea from PR #36 at repo quality — do not merge that branch as-is.

## Boundaries & Constraints

**Always:**
- Inquiry request stays `F0 7E 7F 06 01 F7`.
- Matrix-1000 happy path unchanged when Inquiry replies (type, version, EPROM, SysEx delay, Master pull-on-connect).
- Fallback runs after Inquiry timeout when detection is absent, or when the port pair changed and a fresh identity attempt timed out (`forceMasterPullOnNextInquirySuccess_`) — not on soft-abort presence while already detected on the same pair, and not on outbound-busy deferral.
- Discovery Master-frame filter accepts wire length **478 or 479** only (plus Oberheim header `F0 10 06`, opcode `03H`, EOX). Reject Matrix-1000 Master length **351** and other sizes (including printed youngmonkey **477** alone).
- After fallback success: no invented firmware version; device type stays the shared Matrix-6 / 6R family (`kMatrix6` display) — do not invent a distinct 6R type from Master alone.
- Preserve stored EPROM TYPE on fallback success (no firmware version to map); apply Matrix-6/6R SysEx delay family from detected type / family, not from a fake Inquiry reply.
- Cancel / token semantics must abort a stale fallback if a new inquiry starts.
- Unit-test the Master frame filter and the inquiry→fallback→success/failure sequence; smoke that Matrix-1000 Inquiry success never enters fallback.
- Register any new `.cpp` in `PLUGIN_SOURCES` and the test target list.
- Include short hardware validation notes for Matias (what to send / what to verify). Do not claim 6R hardware validated in this chat.

**Never:**
- Merge PR #36 unchanged.
- Use malformed `F0 10 7E 7F 06 01 F7`.
- Blindly trust Google-AI length writeups; do not accept length **477** as a discovery match without a counted capture that proves it.
- Invent provisional Inquiry member bytes as product truth beyond what wiring needs, and never invent firmware version strings.
- Support MASTER EDIT / SPLIT Matrix-6 (v2).
- Broad MidiManager refactor outside inquiry / fallback / minimal outbound activity for direct SysEx.
- Change Max for Live Matrix-1000 Editor protocol.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| M-1000 Inquiry success | Identity reply within timeout | Detect M-1000 + version; EPROM/delay/Master pull as today; no Master fallback | N/A |
| Soft-abort presence | Already detected on same pair; Inquiry timeout | Soft unresponsive; keep detection; no Master fallback | Presence retries later |
| Port-pair change + Inquiry timeout | Previous detect; `forceMasterPullOnNextInquirySuccess_`; Inquiry timeout | Run Master fallback (re-identify silent 6/6R on new ports) | If fallback fails → same failure path as undetected timeout |
| Outbound busy deferral | Queue never idles before Inquiry send | Soft deferral; no fallback | Presence retries |
| 6/6R Inquiry silent + Master OK | Undetected; Inquiry timeout; Master dump length 478 or 479, opcode 03H | Detect Matrix-6 family; empty version; preserve EPROM; 6/6R delay profile; unlock editor; no M-1000 Master apply/pull | Log fallback response |
| Fallback timeout | Undetected; Inquiry timeout; no valid Master | Same failure UX as today after Inquiry timeout (clear detection + timeout error) | Clear message that identity failed (Inquiry then Master) |
| M-1000 Master during fallback | Undetected; dump length 351 / wrong frame | Reject; keep waiting until fallback timeout (or re-arm filter) | Do not detect as 6/6R |
| New inquiry cancels fallback | Fallback armed; new port/inquiry token | Stale fallback handlers no-op | Token CAS / cancel capture |

**Decisions recorded:**
- Spec kept full (token override).
- Port-pair Inquiry timeout → run Master fallback.
- Discovery Master length acceptance = **478 or 479** (not 477 alone; not Google-AI structure).

</frozen-after-approval>

## Code Map

- `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` -- Inquiry arm/send/timeout; soft-abort vs failure; on timeout run fallback when undetected **or** port-pair force flag set; presence soft-abort when detected without force flag.
- `Source/Core/MIDI/MidiManager.h` -- Private inquiry helpers; add fallback send/arm/handle/timeout; optional success-finisher flag or family-aware delay path without fake version.
- `Source/Core/MIDI/MidiManagerDeviceInquiryFallback.cpp` (new) -- Fallback sequence inspired by PR #36; rewrite for token/WeakReference/logging clarity; register in root `CMakeLists.txt` plugin + test source lists.
- `Source/Core/MIDI/Matrix6MasterResponseFilter.h` (new, name may vary) -- Pure header predicate: Oberheim header, opcode `03H`, EOX, length in {478, 479}; reject 351.
- `Source/Core/MIDI/SysEx/SysExConstants.h` -- Add Matrix-6/6R Master length constants (min/max or dual); reuse `kDefaultTimeoutMs` for fallback timeout unless evidence needs longer; do not add malformed Inquiry bytes.
- `Source/Core/MIDI/MidiManagerOutbound.cpp` -- Optional minimal: pulse outbound activity on `sendSysExWithDelay` so Inquiry/fallback LEDs match queued SysEx (PR #36 insight).
- `Source/Core/MIDI/DeviceInquiryCaptureFilter.h` -- Reuse pattern only; do not weaken identity filter.
- `Source/Core/MIDI/MasterPullOnConnectPolicy.h` + `MidiManagerAsyncMaster.cpp` -- After fallback sets `kMatrix6`, pull-on-connect must stay off (`isMasterEditAllowed`); do not decode/apply 236-byte Master into M-1000 Master model.
- `Source/Core/Services/EpromTypePolicy.*` / `SysExDelayProfile.*` / `DeviceTypeRegistry.*` -- Preserve EPROM on fallback; set delay from Matrix-6/6R family via detected type (prefer `deviceFamilyFromType` / `fromSettings`) rather than provisional Inquiry member bytes as truth.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- Smoke empty `deviceVersion` with detected Matrix-6; ASCII-only copy if any new footer string.
- `Tests/Unit/DeviceInquiryCaptureFilterTests.cpp` -- Pattern for filter unit tests.
- `Tests/Unit/MasterPullOnConnectTests.cpp` -- Pattern for policy / sequence tests.
- New `Tests/Unit/*Matrix6Master*Tests.cpp` (and/or inquiry-fallback sequence tests) -- Filter lengths + inquiry→fallback success/fail; Matrix-1000 success never arms fallback.
- `_local/References/Oberheim/oberheim-matrix-6-6r-midi-sysex-implementation.md` -- From youngmonkey; Master `03H` / request `04H` type 3; packed 236; printed 477 vs parts 478 (editorial note); Alpes 479 cited.
- PR #36 `feat/m6r-master-discovery-fallback` -- Idea + structure only; rewrite (filter accepts 478 or 479; prefer 2 s timeout).

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/MIDI/SysEx/SysExConstants.h` -- Add Matrix-6/6R Master length constants for **478 and 479**; document packed 236 vs M-1000 172/351.
- [x] `Source/Core/MIDI/Matrix6MasterResponseFilter.h` (or equivalent) -- Header-only frame predicate: accept 478 or 479; reject 351 / wrong opcode.
- [x] `Source/Core/MIDI/MidiManager.h` + `MidiManagerDeviceInquiry.cpp` + `MidiManagerDeviceInquiryFallback.cpp` -- Wire Inquiry timeout → fallback → success/failure; soft-abort presence unchanged; port-pair force runs fallback; preserve EPROM; empty version; Matrix-6 family detect; token cancel; no M-1000 Master apply from fallback dump.
- [x] `Source/Core/MIDI/MidiManagerOutbound.cpp` -- Notify outbound/editor activity on direct `sendSysExWithDelay` (Inquiry + fallback visibility).
- [x] `CMakeLists.txt` -- Register new `.cpp` in plugin and test source lists.
- [x] `Tests/Unit/` (+ `Tests/CMakeLists.txt` if required) -- Unit tests for filter (accept 478 and 479; reject 351 / 477 / wrong opcode) and sequence (timeout→fallback success; fallback timeout failure; soft-abort does not fallback; port-pair force does fallback; M-1000 Inquiry success skips fallback).
- [x] Spec / Implementation Notes -- Append Matias hardware checklist (ports, SysEx ON, expected Inquiry silence, Master request, footer detect without version).

**Acceptance Criteria:**
- Given an undetected synth that ignores Inquiry but returns a valid Matrix-6/6R Master dump, when discovery runs, then the plugin detects the Matrix-6 / 6R family, unlocks the editor, leaves version empty, and does not invent firmware or apply Master edit data.
- Given a Matrix-1000 that answers Inquiry, when discovery runs, then behaviour matches today and Master fallback never runs.
- Given already-detected soft-abort Inquiry timeout on the same port pair (presence), when timeout fires, then detection is kept, unresponsive is set, and Master fallback does not run.
- Given a port-pair change with previous detection and Inquiry timeout, when timeout fires, then Master fallback runs; on a valid 6/6R Master dump the family is detected.
- Given fallback armed and a Matrix-1000-length Master (351) arrives, when the filter evaluates it, then it is rejected as 6/6R proof.
- Given `lint_touched.py` on touched C++ and unit tests for this change, when the agent finishes, then lint and those tests pass.

## Implementation Notes

- 2026-10-28: Spec approved (continue in this session). Deferred project note: after hardware confirms Master dump length, update `_local/References/Oberheim/` Matrix-6/6R MD docs and align filter constants — tracked in `deferred-work.md`.
- 2026-10-28: Implemented Inquiry-first Master fallback (lengths 478|479, `kDefaultTimeoutMs`, preserve EPROM, empty version, `kMatrix6` family). Filter + fallback policy unit-tested; no 6R hardware validation in this chat.
- 2026-10-28 review patches: invalidate Inquiry token when Master fallback starts; `onDeviceInquiryIdentityTimeout` test hook; MidiManager sequence tests; filter header/EOX rejects; version helper wired; packed-size-derived length constants.

**Matias hardware checklist (Matrix-6R):**
1. MIDI From / MIDI To to the 6R; SYSTEM EXCLUSIVE ON on the synth.
2. Confirm Universal Device Inquiry (`F0 7E 7F 06 01 F7`) stays silent (no identity reply).
3. After Inquiry timeout (~2 s), plugin sends Master request (`F0 10 06 04 03 00 F7`); expect Master dump `F0 10 06 03…F7`.
4. Footer: Matrix-6 detected, no firmware version string; editor unlocked.
5. Optional: count Master dump byte length (478 vs 479) and note it for filter narrowing.
6. Regression: Matrix-1000 still detects via Inquiry only (no Master fallback on that path).
7. Do not claim 6R hardware validated until this checklist is run on Matias’s unit.

## Spec Change Log

## Review Triage Log

- high — late Device Inquiry identity can finish after Master fallback starts (same token) — verified: timeout kept Inquiry token until fallback send; fixed by CAS bump + cancel capture on fallback entry; route was patch (applied).
- medium — Inquiry timeout → fallback MidiManager path only covered by pure bool helpers — verified: no prior call of `onDeviceInquiryIdentityTimeout` / `sendArmedMatrix6MasterFallback` in Tests; fixed with `Matrix6MasterFallbackMidiManagerTests`; route was patch (applied).
- medium — fallback success EPROM/empty-version effects not observed — verified: only helper returns tested; fixed by MidiManager success fixture asserting APVTS; route was patch (applied).
- medium — filter header/EOX bytes untested — verified: rejects were length/opcode only; fixed with manuf/device/F0/F7 rejects; route was patch (applied).
- medium — sequence / M-1000 “skips fallback” smoke overclaimed in policy test names — verified: pull-on-connect assert ≠ Inquiry-success skips fallback; renamed + MidiManager soft-abort/fallback tests cover the gate; route was patch (applied).
- medium — `matrix6MasterFallbackDeviceVersion()` unused by production — verified: success used `{}`; wired through `applyDeviceInquirySuccessDetection`; route was patch (applied).
- low — `kMatrix6MasterPackedDataSize` unused / lengths not derived — verified; derived min/max from packed size + static_assert; route was patch (applied).
- low — Inquiry timeout log misleading before fallback — verified generic SysEx timeout wording; clarified Inquiry identity log + fallback info line; route was patch (applied).
- low — LED comment framed as Inquiry/fallback-only while all `sendSysExWithDelay` callers pulse — verified; comment corrected; route was patch (applied).
- false — Device Setup prompt re-arm risk on fallback preserve path — refuted: `shouldArmDeviceSetupPromptPendingAfterInquiry` always returns false (GS-3); still skipped explicitly when preserve=true.
- false — re-arm on non-matching dump in handler is “unreachable so wrong” — refuted: defensive re-check matches capture filter; keep for safety.
- false — vacuous `outboundBusyDefersInquiryWithoutMasterFallback` proves busy deferral — refuted as coverage; stub removed; outbound-busy remains a separate soft-abort path unchanged by this story.
- false — spec frontmatter/process metadata as a code defect — rejected (finding asks to edit build process/spec status, not product behaviour).
- defer — no test that `sendSysExWithDelay` pulses activity LEDs — real optional UX gap; appended to deferred-work.md.
- defer — no Footer contract test for empty version after Matrix-6 fallback — GUI out of unit suite; Matias checklist; deferred-work.md.

## Design Notes

**Why Master fallback (not a fake Inquiry):** Matrix-6/6R docs omit Device Inquiry; hardware evidence (Matias / Pocket MIDI) shows Inquiry `F0 7E 7F 06 01 F7` unanswered while Master request `F0 10 06 04 03 00 F7` yields `F0 10 06 03…`.

**Length calibration (research 2026-10-28):**
- youngmonkey Appendix E (community transcription of Oberheim M-6 SysEx): `236` packed → `472` nybbles + `4` header + CS + EOX, but prints total **477** — the sum line is arithmetically wrong (`472+4+1+1=478`). This inconsistency is already in youngmonkey HTML; it was not introduced by the local markdown rebuild from PDF.
- Corrected formula total = **478**. Opcode for Master dump remains **`03H`** (not `0BH`).
- Alpes Machines Matrix Ctrlr docs state Matrix-6 Global size **479** (memory limit note vs M-1000 351).
- PR #36 final constant **479** (after iteration); no counted byte length readable from the Pocket MIDI screenshot alone.
- Google AI “479” writeup is **not** usable: it invents opcode `0x0B`, a padding byte, and a 473-byte data block that contradict Oberheim/youngmonkey.
- **Decision:** discovery filter accepts **478 or 479**. Matias counted capture may later narrow to one length.

**EPROM / delay without version:** Skip `writeStoredTypeAfterInquirySuccess` on fallback; set delay with `SysExDelayProfile::fromSettings(eprom, kMatrix6Or6R)` (or equivalent from `deviceType`) so provisional Inquiry member bytes are not required as product truth.

**Double Master:** Fallback uses Master only as presence proof and must not decode into the M-1000 Master model. Pull-on-connect remains M-1000-only after success.

**Timeout budget:** Prefer `kDefaultTimeoutMs` (2 s) for fallback as well (~4 s worst-case first connect). Document in failure copy if extended.

**PR #36 reuse:** Separate fallback `.cpp`, capture filter predicate, Inquiry-first sequencing — keep. Soften: length, timeout, synthetic `DeviceIdInfo` member bytes, unconditional LED scope review.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'DeviceInquiry|Matrix6Master|MasterPull' --output-on-failure` (adjust regex to new test names) -- expected: relevant unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/` / `Tests/`

**Manual checks (if no CLI):**
- Hardware Matias Matrix-6R: From/To ports, SYSTEM EXCLUSIVE ON, confirm Inquiry silence then Master dump detection, footer shows Matrix-6 without firmware version, editor unlocked, Matrix-1000 still detects via Inquiry only.
