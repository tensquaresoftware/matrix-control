---
title: 'Restore SYNTH FROM persistence across standalone launches'
type: 'bugfix'
created: '2026-10-06'
status: 'done'
route: 'oneshot'
baseline_commit: 'd868f50b6592bde4106fe9668a9fc14cceee858a'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-audio-device-profiles.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-tab.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In standalone, Settings > AUDIO > SYNTH FROM no longer keeps the chosen Scarlett input across relaunches. After the unified Settings AUDIO cutover (and reliable AudioDeviceProfiles launch restore), the saved listen source is wiped every session even when the same interface comes back.

**Approach:** Stop treating profile launch restore and empty early catalogs as a hard clear of the APVTS listen source. Keep a previously saved SYNTH FROM when the Input device identity and active-channel catalog still match; clear only when scene-safety says the selection is truly invalid. Do not store SYNTH FROM inside device profiles.

</frozen-after-approval>

## Implementation Notes

- Root cause: `applyAvailableAudioDeviceProfileAtLaunch` always called `setAudioFromSourceId({})` after a successful profile open; once Settings AUDIO made profile capture/restore reliable, every relaunch wiped APVTS SYNTH FROM even for the same Scarlett. Secondary: `decideAudioFromSelectionSync` treated an empty early catalog as invalid and cleared the saved id (HeaderRefreshTimer retried while empty).
- Fix: remove launch wipe; add `shouldDefer` when catalog is empty so APVTS is left alone until channels appear; editor honors defer; scene-safety still clears on real identity/catalog mismatch.
- Files: `SceneAudioSafety.h`, `StandaloneAudioInputRouterStandalone.cpp`, `StandaloneAudioInputRouter.h`, `PluginEditorAudio.cpp`, `SceneAudioSafetyTests.cpp`. Removed unused `PluginProcessor.h` include from the standalone router TU.
- Verification: `lint_touched.py` OK; `Matrix-Control_Tests --category SceneAudioSafety` → 0 failures.
- Review patch: clear on known Input-identity mismatch even when the catalog is still empty; document that callers must honor `shouldDefer` before clearing.

## Review Triage Log

- Empty-catalog defer never clears lasting Input None / zero-channel — medium — deferred (cold-start vs settled None ambiguous without a new settle signal).
- Defer ignored known identity mismatch while catalog empty — medium — patched (clear when both identities non-empty and differ).
- Settings combo shows None while APVTS still holds deferred id — low — rejected (transient until catalog refresh; expected).
- Missing unit cases for empty-catalog edges — medium — patched (identity-mismatch-while-empty test added; defer keep already covered).
- Catalog fill without ADM change after timer stops — maybe-false — deferred (unverified medium; needs live harness).
- Spec still `in-progress` / no acceptance block — false — oneshot finalize sets `done`; route intentionally omits full AC sections.
- Spec Verification omits manual Scarlett relaunch — low — rejected (human smoke remains the product gate; noted in Present).
- AUDIO FROM / SYNTH FROM naming mix in comments — low — rejected (pre-existing dual names; not worth churn).
- No structural guard that callers check `shouldDefer` — low — patched (struct + function comments state the contract).
