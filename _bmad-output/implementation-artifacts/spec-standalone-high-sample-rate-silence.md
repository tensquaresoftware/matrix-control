---
title: 'Standalone silence at high Scarlett sample rates'
type: 'bugfix'
created: '2026-10-07'
status: 'done'
route: 'dispatch'
baseline_commit: 'ca51c056358bc85959b748e09dbc5885facf41b7'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-tab.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-audio-device-profiles.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In standalone Settings > AUDIO, choosing Scarlett sample rates 176400 Hz or 192000 Hz yields no sound — including PLAY TEST TONE — even with a comfortable BUFFER SIZE of 512. Lower rates work. This blocks using the interface at its high rates after the unified Settings AUDIO cutover.

**Approach:** Make SAMPLE RATE / BUFFER SIZE apply resilient and honest: check `setAudioDeviceSetup` results; when the chosen rate fails with the selected buffer, retry other available buffer sizes (including around 512); after apply, resync combos from the live device (`getCurrentSampleRate` / buffer). If the rate still cannot open, snap the UI back to the working live setup instead of leaving a silent half-state. Keep SYNTH FROM / scene-safety and profile payload contracts unchanged.

**Decision (SILENCE_SCOPE):** Confirmed option A — PLAY TEST TONE is silent at 176400 / 192000 Hz, including with BUFFER SIZE 512. Scope stays on device open / rate+buffer apply (not SYNTH FROM / passthrough-only).

## Boundaries & Constraints

**Always:**
- Standalone Settings AUDIO path (`SettingsAudioPage` / `AudioDeviceSetupSync`); check `setAudioDeviceSetup` result on rate/buffer apply.
- After a successful coerce, refresh SAMPLE RATE and BUFFER SIZE combos from the live device setup so the UI matches reality.
- Prefer keeping the user’s buffer when it still opens; otherwise try other sizes from `getAvailableBufferSizes()` (and device default when available).
- English/ASCII UI only; no Core→GUI dependency.

**Never:**
- Cap or hide 176.4 / 192 kHz from the rate list when the device reports them.
- Store SYNTH FROM in AudioDeviceProfiles; change scene-safety defer/clear rules; rewrite JUCE StandalonePluginHolder.
- Full product error-toast UX for every failed open (out of scope for this fix).
- Hosted-plugin bus / AudioPassthroughProcessor redesign.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| High SR + stale buffer | User picks 192000 while buffer was valid only at 48000 | Device opens at 192000 with a coerced supported buffer; combos show live values | If all buffer tries fail, snap combos to previous live setup (no silent half-state) |
| High SR + buffer 512 | 192000 + 512 (user-confirmed silent today) | Retry open across available buffers; if one opens at 192000, keep it and show live buffer; if none open, snap UI to prior live rate/buffer so PLAY TEST TONE works again | Never leave device null / silent while combo shows 192000 |
| Buffer-only change | Same rate, new buffer | Apply as today; on failure snap UI to live | Same snap-back |
| Lower rates | 44100 / 48000 / 96000 | Unchanged success path | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsAudioPageDevices.cpp` — `applySetupFromUi` (~192–236) sets rate+buffer then ignores `setAudioDeviceSetup` error; `refreshSampleRateAndBufferCombos` (~100–135) lists device capabilities with no post-failure snap.
- `Source/GUI/Settings/SettingsAudioPage.cpp` — SR/buffer `onChange` → immediate `applySetupFromUi`.
- `Source/GUI/Settings/AudioDeviceSetupSync.cpp` / `.h` — reuse `deviceSupportsSampleRate` / `deviceSupportsBufferSize` / `applySetupWithRestoreGuard` pattern; extract a small apply-with-buffer-fallback helper if it stays GUI-local, or a pure Core buffer-try-order helper under `AudioDevicePreferredSetup.h` (or sibling) for unit tests.
- `Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp` — profile refine already notes keep-open-on-refine-fail; do not widen launch wipe/SYNTH FROM work.
- Do not change: `SceneAudioSafety.h`, `AudioPassthroughProcessor` SR path (none), hosted buses.

**Reuse:** checked `setAudioDeviceSetup` in `applySetupWithRestoreGuard`; profile validate skip-unsupported rate/buffer.

**Do not change:** AudioDeviceProfiles payload; first-run / missing-device None policies; Synth From persistence fix from `spec-synth-from-audio-input-persistence.md`.

## Tasks & Acceptance

**Execution:**
- [x] Pure helper for buffer try-order (requested → default → remaining available) + unit tests — deterministic coerce without hardware
- [x] `SettingsAudioPageDevices.cpp` `applySetupFromUi` (and buffer-only apply if shared) — apply with fallback; on total failure refresh from live setup
- [x] Wire refresh so coerced buffer appears selected after high-SR change
- [x] Register tests in CMake if a new test TU is added

**Acceptance Criteria:**
- Given Scarlett open at a working mid rate, when the user selects 192000 Hz (or 176400 Hz) and CoreAudio can open that rate with some available buffer, then PLAY TEST TONE produces sound and BUFFER SIZE shows the live size.
- Given the same pick when no buffer opens at that rate, when apply finishes, then SAMPLE RATE / BUFFER SIZE snap to the still-working live setup and PLAY TEST TONE works again at that live rate.
- Given an apply that cannot open any buffer at the chosen rate, when apply finishes, then the UI rate/buffer match the still-running live setup (no fake selection).

## Implementation Notes

- `Core::buildBufferSizeTryOrder` / `didBufferFallbackTrySucceed` / `shouldRestorePreviousSetupAfterBufferFallback` in `AudioDevicePreferredSetup.h` — pure policy for try order, try acceptance, and snap-back; covered by `AudioDevicePreferredSetupTests` (matrix rows).
- `AudioDeviceSetupSync::tryApplySetupWithBufferFallback` — tries each buffer until setup succeeds and live sample rate matches the requested rate.
- `SettingsAudioPage::applySetupFromUi` — on total failure restores the previous live setup, then always refreshes combos from the device so coerced/snapped values show.
- Verification: `AudioDevicePreferredSetup` unit tests 0 failures; Standalone Debug build; `lint_touched.py` OK. Hardware Scarlett smoke still required for AC.
- Review patches: combos select from live `getCurrentSampleRate` / `getCurrentBufferSizeSamples`; `restoringSetup` guard around fallback apply + restore; restore checks error and retries previous setup once; comment fix on `tryApplySetupWithBufferFallback`.

## Spec Change Log

## Review Triage Log

- Open succeeds at 192k+512 but PLAY TEST TONE still silent — medium — deferred (no audible probe; gate correctly stops when live rate matches; Scarlett/driver silent-open residual).
- Matrix requires always retrying every buffer after a green first try — false — matrix means try until one opens at the requested rate; short-circuit on success is correct.
- Combos keyed off `getAudioDeviceSetup` not live clock — medium — patched (select from live device rate/buffer).
- Restore ignores `setAudioDeviceSetup` error — medium — patched (check + one retry of previous setup).
- Buffer try-order built before new rate / new endpoints — medium — deferred (CoreAudio list usually shared across rates; endpoint+rate same apply needs a later open-then-rescan).
- Comment “try stuck” — low — patched.
- `shouldRestorePreviousSetupAfterBufferFallback` thin wrapper — low — rejected (keeps policy unit-testable).
- Stale Code Map / empty triage / smoke checklist in spec — false — rejected (fix would only edit this build’s spec / process artifacts).
- Not reusing `applySetupWithRestoreGuard` — low — rejected (fallback loop needs multi-try; guard flag applied around the whole apply instead).
- No unit test executes `tryApplySetupWithBufferFallback` — medium — deferred (Verification Gap; no AudioDeviceManager fake in repo; Core helpers + Scarlett smoke).
- Named restore test does not pin `applySetupFromUi` — medium — deferred (Verification Gap; same perimeter).
- Nested ChangeListener during fallback without restoringSetup — medium — patched (`syncState_.restoringSetup` around try + restore).
- Endpoint change uses prior device buffer catalog — medium — deferred (grouped with pre-rate buffer list; user’s Scarlett case is rate-only on same interface).
