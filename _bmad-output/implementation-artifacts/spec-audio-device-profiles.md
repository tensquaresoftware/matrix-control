---
title: 'Audio device profiles'
type: 'feature'
created: '2026-10-02'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'a40dcc6fdbdbf9e0619be62fd3c371128e9795db'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-audio-safety-scene.md'
  - '{project-root}/_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After scene-safety forces Input/Output to None when an interface disappears, re-selecting that same Scarlett (or other interface) in Audio Settings loses the user’s active channels, sample rate, and buffer — only the in-session preferred overlay survives while the settings window stays open.

**Approach:** Persist a per-interface profile under product Application Support keyed by a strict composite identity; on explicit user re-pick of a matching interface, restore validated channels/rate/buffer; never restore AUDIO FROM; never auto-open a remembered interface at cold start.

**Decisions locked (smoke 2026-10-02 + Build prompt):**
1. AUDIO FROM stays None (or unchanged empty) after restore / safety clear — no per-device monitoring restore.
2. Strict exact key match only — no fuzzy Scarlett* / family restore.
3. Key v1 = driver type name + exact Input name + exact Output name (empty string when None) + soft fingerprint (available in/out channel counts at save). No USB UID / OS endpoint in v1.
4. Restore only on explicit re-pick in Audio Settings (user-driven identity change to non-None) — never auto-select at cold start.
5. Store beside `Init/` via `ProjectPaths` (`AudioDeviceProfiles/`) — not inside JUCE `audioSetup` (wiped by None policy).
6. On restore, apply only rate/buffer/channel bits still offered by the live device; truncate/skip invalid parts; never crash.
7. Purge v1: document folder path in Design Notes; no Settings “delete profile” UI; optional Reveal only if trivial — **skip Reveal in v1**.
8. Cap **20** profiles; eviction = LRU by `lastUsedUtcMs`.
9. Do not reintroduce `muteInput` / blue banner.

**Agent-owned (not re-asked):** single `profiles.xml` index in that folder; profile payload = active Input/Output channel bits + sampleRate + bufferSize (+ key meta + lastUsed); soft fingerprint uses available channel-name counts (capability), not active-bit counts; wire capture/restore in standalone `AudioMidiSettingsWindow` only; missing-device / first-run None policies leave profiles on disk untouched.

## Boundaries & Constraints

**Always:**
- Keep `applyMissingAudioDeviceNonePolicy`, first-run None, and AUDIO FROM bound-identity rules intact.
- Exact key equality for restore; mismatch → behave as unknown device (no restore).
- Core pure helpers for key, match, validate/truncate, LRU upsert — unit-tested without hardware.
- ASCII / English UI unchanged; standalone-only path.

**Never:**
- Store profiles inside JUCE `audioSetup` / force cold-start reopen of last Scarlett.
- Restore `audioFromSourceId` from a profile.
- Fuzzy name matching; USB UID phase-2; First-run setup assistant; Audio Settings Matrix chrome rebuild (chantier 4).
- Profile-purge Settings UI; `muteInput` safety.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Capture stable setup | User sets Scarlett In/Out + channels/rate/buffer | Upsert profile for strict key; touch lastUsed | Skip save if both endpoints None |
| Re-pick same key | Scarlett off→None then on; user reselects same names+fingerprint | Restore validated channels/rate/buffer; AUDIO FROM stays None | Unsupported parts skipped |
| Different gen / name | 6i6 2nd vs 3rd (exact OS names differ) or channel-count fingerprint differs | No restore | Leave device defaults |
| Partial capability | Saved 96 kHz / ch7 missing after firmware | Apply remaining valid rate/buffer/bits | Truncate bits beyond available |
| Cap exceeded | 21st distinct key | Evict least-recently-used; save new | Never exceed 20 |
| Cold start / missing device | Safety forces None | Profiles untouched on disk; no auto re-open | N/A |
| Identical OS names | Two same-model boxes indistinguishable | Same key may collide | Documented v1 limit |

</frozen-after-approval>

## Code Map

- `Source/Shared/ProjectPaths.h/.cpp` — add `getAudioDeviceProfilesDirectory()` mirroring `getInitTemplatesDirectory()` (`AudioDeviceProfiles` under app data). Extend `ProjectPathsTests` child-of-product check.
- `Source/Core/Audio/AudioDevicePreferredSetup.h` — in-session rate/buffer only; **do not** turn it into the disk store. Reuse `shouldRestorePreferredSampleRate/BufferSize`, `didAudioDeviceIdentityChange` ideas; profiles are a separate Core module.
- **New** `Source/Core/Audio/AudioDeviceProfiles.h` (+ `.cpp` if I/O) — pure: `AudioDeviceProfileKey`, `buildProfileKey`, `keysMatch`, `validateProfileAgainstCapabilities`, `upsertProfileLru` (cap 20); store: load/save `profiles.xml` via `ProjectPaths::getAudioDeviceProfilesDirectory()`.
- `Source/GUI/Dialogs/AudioMidiSettingsWindow.cpp` (`syncPreferredSetupFromDeviceManager` ~214–241) — on identity change to non-None: lookup+validate+apply channels/rate/buffer from profile then `setAudioDeviceSetup`; after stable live setup (non-None): capture+upsert. Guard with existing `restoringSetup_`. Standalone-only window (`PluginEditor::openAudioMidiSettingsWindow` already gates).
- `Source/Core/Audio/StandaloneAudioInputRouter*` / `MatrixControlStandaloneApp.cpp` — **do not** call profile restore on cold start; keep None policies; clearing endpoints must not delete profiles.
- `Source/Core/PluginProcessorAudio.cpp` / AUDIO FROM — unchanged contract; restore path must not write `audioFromSourceId`.
- Pattern ref: `DeviceConnectionMachineDefaults` (PropertiesFile) vs Init multi-file — prefer one XML index for LRU.
- Tests: `Tests/Unit/AudioDeviceProfilesTests.cpp` — key equality, mismatch skip, validate truncate, LRU eviction; register in root `CMakeLists.txt` `MATRIX_BUILD_TESTS` `target_sources` (near `AudioDevicePreferredSetupTests`). Add new `.cpp` to `PLUGIN_SOURCES` if non-header-only.
- Guide: `_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md` — add profiles smoke bullets; order: … safety ✓ · **profiles** · First-run · rebuild Audio Settings.

**Reuse:** preferred sample-rate/buffer support checks; scene-safety None policies; ProjectPaths app-data pattern.

**Do not change:** mute/banner contract; preferred-after-inquiry when None; Audio Settings chrome rebuild.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/ProjectPaths.*` (+ tests) — `getAudioDeviceProfilesDirectory()` — durable store path beside Init
- [x] `Source/Core/Audio/AudioDeviceProfiles.h` (+ `.cpp` if needed) — key, match, validate/truncate, LRU upsert, XML load/save — Core SSOT
- [x] `Source/GUI/Dialogs/AudioMidiSettingsWindow.*` — capture on stable non-None; restore on user identity re-pick — product behavior
- [x] Confirm router/app None paths never auto-restore profiles / never wipe store — safety coexistence
- [x] `Tests/Unit/AudioDeviceProfilesTests.cpp` + `CMakeLists.txt` — cover I/O matrix pure cases
- [x] Design Notes + smoke guide checklist/order — purge path + identical-name limit + Mac smoke
- [x] Verify — macOS Debug build + unit tests + `lint_touched.py`

**Acceptance Criteria:**
- Given a Scarlett setup saved as a profile, when the device disappears (None) then the user reselects the same OS Input/Output names with the same fingerprint, then active channels, sample rate, and buffer are restored when still supported, and AUDIO FROM remains None.
- Given a different exact device name or fingerprint (e.g. other Scarlett generation), when the user selects it, then no prior profile is applied.
- Given a saved rate/buffer/channel bit unsupported by the live device, when restore runs, then only valid parts apply and the app stays stable.
- Given 20 profiles already stored, when a 21st distinct key is saved, then the least-recently-used profile is removed.
- Given cold start or missing-device None policy, when standalone launches, then no remembered interface is auto-opened from profiles.

## Implementation Notes

- Profiles are restored only from `AudioMidiSettingsWindow::syncPreferredSetupFromDeviceManager` after a seeded identity change to non-None. Cold start / router None policies do not call the store.
- Disk profile wins over in-session `AudioDevicePreferredSetup` when a matching key exists; otherwise the preferred overlay still applies.
- Review patches (2026-10-02): restore/overlay mutate a candidate setup only; refresh from the device manager after apply success or failure before capture; skip restore/capture when `device == nullptr`; collapse duplicate keys on load (newest `lastUsedUtcMs`).

## Spec Change Log

## Review Triage Log

- BH restore without explicit selector re-pick (any ADM identity change while settings open) — verdict: low | evidence: everyday path is user picking in Audio Settings; CoreAudio rarely auto-adopts a returning interface while the window is open; cold start still never restores. Route: reject (everyday harm none; detecting “explicit” vs OS would add non-trivial surface).
- BH None policy uses persisted XML vs live Input wipe — verdict: medium | evidence: pre-existing scene-safety path in dirty tree, not introduced by profiles store. Route: defer.
- BH dual-clear failure leaves Output OS fallback — verdict: medium | evidence: scene-safety `applyClearedEndpoints` Input-only retry; pre-profiles. Route: defer.
- BH/ECH failed profile apply still captures mutated setup — verdict: medium | evidence: real; `tryRestoreMatchingProfile` mutated live `setup`/`preferred_` before apply. Patched — candidate copy + refresh before capture. Route: patch (applied).
- BH write profiles.xml on every device-manager change — verdict: low | evidence: true but bounded store; gating adds complexity. Route: reject (everyday harm negligible).
- BH silent save failure — verdict: low | evidence: return ignored; rare Application Support write failure. Route: reject.
- BH/ECH null device 0/0 fingerprint / cleared bits — verdict: medium | evidence: real when `getCurrentAudioDevice()` is null during restore/capture. Patched — skip restore and capture. Route: patch (applied).
- BH empty catalog forces all endpoints None — verdict: maybe-false | evidence: would need a live empty-scan harness; pre-profiles safety. Route: defer (unverified medium).
- BH/VG legacy `audioDeviceName` parse untested — verdict: medium | evidence: helper exists; only modern attrs tested; safety coexistence, not profiles payload. Route: defer.
- BH no tests for `applyClearedEndpoints` / missing-device apply — verdict: medium | evidence: ADM/stub barrier; smoke gate. Route: defer.
- BH/VG validate positive supported rate/buffer untested — verdict: low | evidence: real gap. Patched — `validate_appliesSupportedRateAndBuffer`. Route: patch (applied).
- BH spec status done / empty triage during review — verdict: false | evidence: process status mid-workflow; corrected to `in-review` then triage filled. Route: reject.
- ECH `clearEndpointToNone` useDefault early-exit — verdict: medium | evidence: scene-safety; pre-profiles. Route: defer.
- ECH clearOutput-only reject / dual-clear Input-only fallback — verdict: medium | evidence: same safety path as BH dual-clear. Route: defer.
- ECH missing-device already-None still need clear AUDIO FROM — verdict: medium | evidence: caller clears AUDIO FROM when policy returns true; early false may skip. Pre-profiles. Route: defer.
- ECH XML already stores OS fallback names so policy never fires — verdict: medium | evidence: known safety residual; smoke checklist. Route: defer.
- ECH preferred overlay fail then capture — verdict: medium | evidence: real sibling of failed restore capture. Patched — candidate + always refresh. Route: patch (applied).
- ECH duplicate keys in profiles.xml — verdict: medium | evidence: load kept multiples; lookup/LRU confusing. Patched — `collapseDuplicateProfilesByKey`. Route: patch (applied).
- ECH claim null-device validation / failed-apply capture — verdict: medium | evidence: same as BH/ECH patches above. Route: patch (applied).
- VG clearEndpoint mutation untested — verdict: medium | evidence: no pure setup-mutation helper under Tests; smoke. Route: defer.
- VG catalog Input label prefers `inputDeviceName` untested — verdict: medium | evidence: safety smoke fix; no router-level unit. Route: defer.
- VG first-run Output None / Input-only fallback untested — verdict: medium | evidence: ADM barrier; smoke. Route: defer.
- VG Audio Settings capture/restore wiring only pure-helper tested — verdict: medium | evidence: GUI excluded from Matrix-Control_Tests; smoke checklist is the gate. Route: defer.
- VG preferred-after-inquiry processor early-return untested — verdict: medium | evidence: helper tested; no processor harness. Route: defer.

## Design Notes

**Key v1 (exact):** `driverTypeName | inputDeviceName | outputDeviceName | availableInputChannelCount | availableOutputChannelCount`. Names are OS-exact strings from `AudioDeviceSetup` (empty when None). Fingerprint = `getInputChannelNames().size()` / `getOutputChannelNames().size()` at save (or equivalent available counts). 6i6 2nd Gen ≠ 3rd Gen when OS names differ; if names collide, channel-count fingerprint still separates many models — two identical boxes remain indistinguishable (document as v1 limit; UID later).

**Storage:** `ProjectPaths::getApplicationDataDirectory()/AudioDeviceProfiles/profiles.xml`. Manual purge = delete that folder/file. No in-app purge UI v1. Skip Reveal in v1.

**Cap / LRU:** `kMaxAudioDeviceProfiles = 20`; each upsert sets `lastUsedUtcMs`; when over cap, drop smallest lastUsed.

**Lifecycle vs safety:** None policy and first-run clear live `audioSetup` only. Profiles stay. Restore runs only after user picks a non-None identity in Audio Settings (`syncPreferredSetupFromDeviceManager` identity-change branch), never in `createPluginHolder` / cold start.

**Payload:** `inputChannels` / `outputChannels` (bitmask serialization), `sampleRate`, `bufferSize`. Never `audioFromSourceId`.

**Session overlay:** Keep `AudioDevicePreferredSetup` for in-window switches; disk profile is cross-session SSOT — on re-pick, prefer disk profile values (validated) over stale in-memory preferred when a matching profile exists.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 -DMATRIX_BUILD_TESTS=ON && cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: tests target builds
- `./Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests --category AudioDeviceProfiles` -- expected: 0 failures
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched `Source/` / `Tests/` C++

**Manual checks (if no CLI):**
- Same Scarlett: adjust channels/rate/buffer → power off (None) → power on → reselect → settings restored; AUDIO FROM None
- Other generation / different OS name → no restore
- Confirm cold start still opens Input/Output None after safety (no auto Scarlett)
