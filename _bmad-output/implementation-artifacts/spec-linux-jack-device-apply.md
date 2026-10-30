---
title: 'Linux Standalone: keep JACK Input/Output device selection'
type: 'bugfix'
created: '2026-10-30'
status: 'done'
route: 'oneshot'
baseline_commit: '7d7fec74'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-linux-jack-audio.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-gh-42-windows-standalone-asio.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Linux Standalone with Driver Type JACK, choosing Input/Output devices (e.g. Scarlett under PipeWire-JACK) does not stick — combos snap back to NONE — so Synth From stays empty and monitoring / test tone cannot work, while the same machine works in Bitwig and in JUCE AudioPluginHost with JACK.

**Approach:** Align Settings AUDIO apply with the Host model for JACK: accept server-dictated sample rate / buffer after a successful open (do not treat live-vs-requested rate mismatch as failure and restore previous NONE), keep Input/Output as independent JACK clients (no ASIO-style name link), and cover the success rule with unit tests.

</frozen-after-approval>

## Implementation Notes

- Root cause: `didBufferFallbackTrySucceed` required live sample rate == UI/setup rate; JACK `open()` ignores rate and uses the server clock, so a successful Scarlett open (e.g. 48000) after a stale ALSA rate (e.g. 44100) was treated as failure → restore previous setup → NONE.
- Fix: `isJackDeviceType` / `shouldEnforceRequestedSampleRate`; `tryApplySetupWithBufferFallback` passes non-positive requested rate for JACK so the live-rate gate is skipped. Empty `bufferTryOrder` still attempts once (no device open yet / no listed sizes).
- Kept JACK Input/Output independent (no ASIO name link). Stereo-pair channel masks unchanged (Host uses defaults; 2.1 first pair remains valid).
- Tests: PreferredSetup server-clock note; JACK type / enforce / resolve independence + `requestedSampleRateForBufferFallbackSuccess` seam in `LinuxJackDeviceTypeTests` (kept SettingsTabsShell under the 400-line gate).
- Review patches: extracted rate-for-success helper + tests; clarified empty try-order comment; trimmed redundant PreferredSetup asserts; tightened `isJackDeviceType` comment (rate only, not buffer policy).

## Review Triage Log

- false — Blind: empty try-order widens to all drivers — Intentional: empty order previously skipped `setAudioDeviceSetup` and restored previous setup; one attempt matches Host “apply once” and fixes JACK with no live buffer list yet.
- defer — Blind: no unit test for empty try-order / bufferSize <= 0 — Real but needs ADM mock or a new pure helper beyond this fix; deferred.
- low / patch — Blind: PreferredSetup JACK asserts redundant — Removed duplicate expects; gate already covered by non-positive requested rate.
- medium / patch — Blind: no test of JACK → 0.0 wiring seam — Extracted `requestedSampleRateForBufferFallbackSuccess` and unit-tested ALSA vs JACK.
- low / patch — Blind: `isJackDeviceType` comment overclaims buffer — Comment now says open() ignores requested rate only.
- low / patch — Blind: `shouldEnforceRequestedSampleRate("jack")` untested — Added lowercase expects + helper seam tests.
- false — Blind: `containsIgnoreCase("JACK")` too loose — Same policy as `isAsioDeviceType`; JUCE type name is JACK.
- defer — Blind: server-clock policy lives in GUI header not Core — YAGNI until a second server-clock backend; deferred.
- low / rejected — Blind: rename `bufferFallbackTrySucceedsOnlyWhenLiveRateMatches` — Comment in test already clarifies non-positive skip; rename alone not worth churn.
- defer — Blind+Verification: empty try-order fallback still has no executing unit test — Confirmed; already deferred (ADM / pure helper).
- defer — Blind: `tryApplySetupWithBufferFallback` call-site not covered by a test that would fail if the helper were bypassed — Residual seam gap; needs ADM fake; deferred.
- defer — Blind: preferred-setup overlay after JACK open is not server-clock-aware — Pre-existing sync path; out of this rate-gate fix; deferred.
- false — Blind: Intent “rate and buffer” vs code rate-only — Buffer was never part of `didBufferFallbackTrySucceed`; live buffer already accepted after open.
- false — Blind: preferOutput-only independence test — For JACK, `resolveEndpointNamesForApply` ignores prefer side; both paths stay unlinked by construction.
- false — Edge: zero live rate still succeeds when rate gate skipped — Path not shown reachable with a present JACK device; tightening would be speculative.
- rejected (edit-spec) — Blind: `status` / `review_loop_iteration` bookkeeping; smoke note missing from Implementation Notes.
- low / rejected — Blind: rename misleading PreferredSetup test — Same as prior rename reject.

