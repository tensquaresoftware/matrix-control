---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Matrix-6/6R presence heartbeat + footer label
author: BMad Agent
type: bugfix
created: '2026-10-29'
status: done
route: dispatch
baseline_commit: e77baa961292598d0d5d80401026d9174b61adb7
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
sources:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-gh-36-m6r-master-discovery-fallback.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After Matrix-6/6R Master discovery (Inquiry ignored), the ~5 s presence heartbeat re-sends Inquiry only. Silence hits soft-abort, SysEx pauses, and no Master re-check runs — so a silent-but-alive 6/6R "décroche". The footer also shows bare `Matrix-6` for the shared family identity.

**Approach:** When Inquiry times out and the device is already detected as Matrix-6 family on the same port pair, run the existing Master dump presence check instead of Inquiry-only soft-abort; soft-abort only if that Master check also fails. Keep first-connect / port-pair fallback and Matrix-1000 Inquiry presence unchanged. Show footer display `Matrix-6/6R` for `kMatrix6` (APVTS id stays `Matrix-6`; no invented `kMatrix6R` from Master).

## Boundaries & Constraints

**Always:**
- Matrix-1000 Inquiry success and Inquiry-presence soft-abort (detected M-1000, Inquiry silence) stay unchanged.
- Undetected first-connect Inquiry timeout and port-pair `forceMasterPullOnNextInquirySuccess_` still run Master discovery fallback as today.
- Master remains family proof only: empty firmware, preserve stored EPROM, shared `kMatrix6`, no M-1000 Master apply / pull-on-connect from the dump.
- When already Matrix-6-family-detected and Master presence fails (timeout or ports unavailable for the check), soft-abort unresponsive — keep detection / delay / lastInquiry pair; do **not** call full inquiry failure (clear detect).
- When Master presence succeeds while already detected, clear unresponsive via the existing success path (no invented version).
- Footer display SSOT: `toDisplayString(kMatrix6)` → ASCII `Matrix-6/6R`; `toApvtsString` / `fromApvtsString` keep `Matrix-6`.
- Unit-test the new presence policy (family detected → Master on Inquiry timeout; M-1000 detected → soft-abort; Master presence failure → soft-abort not clear). Smoke with Matrix-Simulator realistic 6/6R: stay unlocked past ~5–7 s.

**Never:**
- Invent firmware or distinct `kMatrix6R` from Master alone.
- Skip soft-abort for Inquiry silence on Matrix-6 family without a Master re-check (weaker alternative rejected).
- Soft-abort Matrix-1000 presence by running Master fallback.
- Change heartbeat interval or replace Inquiry as the first probe on the presence tick.
- Broad MidiManager refactor outside inquiry timeout / Master fallback failure branching / display SSOT + tests.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| M-6 family presence OK | Detected `kMatrix6` (or 6R); same pair; Inquiry timeout; valid Master 478/479 | Stay detected; clear unresponsive; empty version; EPROM preserved; no M-1000 Master apply | N/A |
| M-6 family presence fail | Detected Matrix-6 family; Inquiry timeout; Master timeout / no dump | Soft-abort unresponsive; keep detection | Do not `finishAsyncDeviceInquiryFailure` |
| M-1000 presence silence | Detected Matrix-1000; Inquiry timeout | Soft-abort; no Master fallback | Presence retries later |
| First connect / port force | Undetected **or** force flag; Inquiry timeout | Existing Master discovery fallback | Failure → clear detect (unchanged) |
| Footer family label | Detected type `kMatrix6` | Footer detail shows `MATRIX-6/6R` (uppercased display) | APVTS still `Matrix-6` |
| Distinct 6R type | Type `kMatrix6R` if ever set | Display stays `Matrix-6R` | No change |

**Decisions recorded:**
- Prefer Master presence re-check over "skip unresponsive for Matrix-6 Inquiry silence".
- Amends GH-36 presence rule for Matrix-6 family only (was: soft-abort never runs Master while detected without force).

</frozen-after-approval>

## Code Map

- `Source/Core/MIDI/Matrix6MasterFallbackPolicy.h` -- Gate also true when detected + Matrix-6 family (`isMatrix6Family`); keep undetected + force; document presence.
- `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` -- Pass detected APVTS type into timeout gate; soft-abort / send fallback call sites unchanged in shape.
- `Source/Core/MIDI/MidiManagerDeviceInquiryFallback.cpp` -- On Master failure: if still Matrix-6-family-detected → `softAbortDeviceInquiryUnresponsive`; else clear-detect as today. Success path already clears unresponsive. Prefer inferring presence vs discovery at failure time over a long-lived flag (`MidiManager.h` only if inference fails).
- `Source/Shared/Definitions/MatrixDeviceTypes.h` -- `kMatrix6FamilyDisplayName = "Matrix-6/6R"`; `toDisplayString(kMatrix6)` uses it; APVTS id stays `Matrix-6`.
- `Source/GUI/.../FooterPanel.cpp` -- No logic change expected (`toDisplayString` already).
- `Tests/Unit/Matrix6MasterFallbackPolicyTests.cpp` -- Family detected → run; M-1000 detected → skip; keep undetected / force.
- `Tests/Unit/Matrix6MasterFallbackMidiManagerTests.cpp` -- Presence success stay unlocked; presence Master fail → soft-abort; M-1000 soft-abort; discovery fail still clears.
- `Tests/Unit/DeviceTypeRegistryTests.cpp` (or equivalent) -- Display `Matrix-6/6R` vs APVTS `Matrix-6`.
- Prior GH-36 spec -- context only; do not reopen its frozen block.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/MIDI/Matrix6MasterFallbackPolicy.h` -- Gate accepts Matrix-6-family-detected presence Inquiry timeout; M-1000 detected still false without force.
- [x] `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` -- Wire detected APVTS type into the timeout gate.
- [x] `Source/Core/MIDI/MidiManagerDeviceInquiryFallback.cpp` (+ `MidiManager.h` if needed) -- Presence Master failure → soft-abort; discovery failure → clear detect as today.
- [x] `Source/Shared/Definitions/MatrixDeviceTypes.h` -- Footer display `Matrix-6/6R` for `kMatrix6`; APVTS id unchanged.
- [x] `Tests/Unit/Matrix6MasterFallbackPolicyTests.cpp` + `Matrix6MasterFallbackMidiManagerTests.cpp` (+ display assert) -- Cover I/O matrix rows; update obsolete "no Master while detected" expectations.
- [x] `Tests/CMakeLists.txt` -- Only if a new test file is added.

**Acceptance Criteria:**
- Given a Matrix-6 family already detected via Master fallback, when the presence heartbeat Inquiry times out and a valid Master dump returns, then the editor stays unlocked and is not marked unresponsive.
- Given the same detected state, when Inquiry times out and Master also times out, then the device stays detected but unresponsive (soft-abort), and detection is not cleared.
- Given a detected Matrix-1000, when presence Inquiry times out, then soft-abort runs and Master fallback does not.
- Given undetected or port-pair force Inquiry timeout, when Master fails, then behavior remains full identity failure (clear detect) as today.
- Given detected type `kMatrix6`, when the footer builds device detail text, then the visible model label is `MATRIX-6/6R` (from display SSOT) while APVTS `deviceType` remains `Matrix-6`.
- Given unit tests and `lint_touched.py` on touched C++, when the agent finishes, then they pass.

## Implementation Notes

- 2026-10-29: Extended Inquiry-timeout gate with detected Matrix-6 family; Master failure soft-aborts when family-detected without force; `toDisplayString(kMatrix6)` → `Matrix-6/6R`. Unit tests + lint_touched OK. Matrix-Simulator realistic smoke left for human UAT.
- 2026-10-29 review patches: removed gate default type; MidiManager presence test asserts Master token bump; port-pair force test seeds Matrix-6 APVTS; Master log copy no longer says "discovery".

## Spec Change Log

## Review Triage Log

- Blind: default `detectedType = kUnknown` on Inquiry-timeout gate is a silent two-arg footgun — verdict: `low` — production always passes type; remove default and update two-arg test call sites. Route: patch.
- Blind + VerifGap: `testMatrix6FamilyPresenceStartsFallbackWithoutPorts` cannot distinguish Inquiry-only soft-abort from Master-then-soft-abort — verdict: `medium` — assert token bump (Inquiry 6 → Master arm 7 → soft-abort 8) or equivalent Master-entry proof. Route: patch.
- Blind: no MidiManager E2E Inquiry-timeout → Master send → dump success — verdict: `low` — reject; wiring covered once Master-entry assert lands; success handler already unit-tested.
- Blind: no MidiManager fixture with APVTS `Matrix-6R` — verdict: `low` — reject; policy covers family; detection never writes `kMatrix6R` today.
- Blind: soft-abort UX still says Device Inquiry after Master presence fail — verdict: `low` — reject; pause semantics correct; new copy needs product wording not settled here.
- Blind: 2× presence-silence latency undocumented in spec — verdict: `false` — reject; fixing would edit this build's spec; latency follows approved Master re-check approach.
- Blind: logs still say Master "discovery" fallback — verdict: `low` — rename log/send labels to Master fallback / presence where cheap. Route: patch.
- Blind: empty Spec Change Log / French in frozen Intent — verdict: `false` — reject; triage log filled here; frozen Intent is human-owned.
- Blind: Master success overwrites APVTS `kMatrix6R` with `kMatrix6` — verdict: `false` — detection never assigns `kMatrix6R`; Master family proof intentionally `kMatrix6`.
- Blind: soft-abort tests omit lastInquiry/delay retention asserts — verdict: `low` — reject; soft-abort path unchanged and already keeps those fields.
- Edge: empty findings — verdict: `false` — nothing to route.
- VerifGap: port-pair force + Matrix-6 APVTS Master failure clear-detect untested at MidiManager — verdict: `medium` — seed `kMatrix6Id` on port-pair MidiManager test and assert clear-detect + Master error. Route: patch.

## Design Notes

GH-36 soft-aborted any detected Inquiry silence without Master. For Inquiry-silent Matrix-6/6R, presence must re-prove with Master. At Master **failure**, if still Matrix-6-family-detected → soft-abort; else clear (first-connect / force). Prefer that inference over a long-lived presence flag.

Suggested review order: policy → Inquiry timeout → fallback failure → display SSOT → tests.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` (or project equivalent) then run Matrix6MasterFallback* + DeviceTypeRegistry display asserts -- expected: pass
- `python3 Scripts/quality/lint_touched.py` on touched C++ -- expected: pass

**Manual checks:**
- Matrix-Simulator **Matrix-6/6R (realistic)**: connect → detect → remain unlocked after ~5–7 s (and beyond); footer shows Matrix-6/6R without a fake firmware version; stored EPROM unchanged.
- Matrix-1000 (or simulator Inquiry-responsive profile): presence silence still soft-aborts without Master spam if applicable to that profile.
