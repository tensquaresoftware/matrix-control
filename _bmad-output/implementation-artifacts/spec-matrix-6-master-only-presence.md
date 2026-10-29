---
title: 'Matrix-6 Master-only periodic presence'
type: 'feature'
created: '2026-10-29'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After a Matrix-6/6R is already detected, each presence tick still sends a mute Device Inquiry and waits ~2 s before Master fallback. That lengthens every cycle and adds bus / log noise without improving identity proof.

**Approach:** When presence fires and the device is already detected as Matrix-6/6R, send the existing Master proof path directly (no Inquiry). Keep Inquiry-first for discovery (undetected, incomplete then open ports, port-pair change). Detected Matrix-1000 stays Inquiry-only presence. Do not add a periodic Inquiry safety net for the rare 6→1000 swap without port change. PRESENCE CHECK intervals and ONCE + unresponsive resume stay as delivered. Coordinate with the local silence-false-ERROR Master 478 work already in the tree; do not reimplement that log fix here beyond not regressing it.

</frozen-after-approval>

## Implementation Notes

- Added `shouldSendMasterOnlyPresenceProof` (detected + Matrix-6 family, not port-pair force) in `Matrix6MasterFallbackPolicy.h`; left Inquiry-timeout fallback predicates unchanged for discovery / port-pair force.
- `onDevicePresenceTimer` branches via `tryStartMasterOnlyPresenceProofIfEligible` → `performMatrix6MasterPresenceProof` → reuse `sendArmedMatrix6MasterFallback` (token bump + Master capture + timeout). Inquiry-first for undetected / Matrix-1000 / unknown / force flag.
- Master-only defers when outbound queue busy (next presence tick retries) so editor SysEx traffic does not false-unresponsive.
- Helpers live in `MidiManagerDeviceInquiryFallback.cpp` (file-size gate on Inquiry TU).
- Tests: policy matrix (incl. force); MidiManager eligibility + defensive no-ports; M-1000 soft-abort / PresenceCheckPolicy still green.
- Local silence-false-ERROR (chantier 1) left uncommitted alongside this work; not reimplemented here.

## Spec Change Log

## Review Triage Log

- Blind Hunter: Master-only skips Inquiry outbound-idle settle — verdict: `medium` — patched (defer when outbound busy; next tick retries).
- Blind Hunter: timeout/log still say Inquiry-then-Master / "fallback" — verdict: `low` — rejected (normal M6 presence soft-aborts without that footer copy; rename is cosmetic).
- Blind Hunter: leftover `forceMasterPull` can clear-detect on Master-only failure — verdict: `medium` — patched (force keeps Inquiry-first via policy + tryStart).
- Blind Hunter: no test of eligibility / timer branch — verdict: `medium` — patched (`tryStartMasterOnlyPresenceProofIfEligible` coverage for M6 / M-1000 / force).
- Blind Hunter: old Inquiry-timeout test still framed as "presence re-check" — verdict: `low` — rejected (residual forced-Inquiry path still valid; rename optional).
- Blind Hunter: Implementation Notes oversell file-size split — verdict: `false` — lint gate green after split; notes updated.
- Blind Hunter: success logs still say "Master fallback response" — verdict: `low` — rejected (shared path; cosmetic).
- Blind Hunter: Inquiry-timeout policy comments still say "presence" — verdict: `low` — rejected (comments already mention forced Inquiry; residual path remains).
