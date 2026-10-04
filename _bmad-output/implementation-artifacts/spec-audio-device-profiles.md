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

**Approach:** Persist a per-interface profile under product Application Support keyed by a strict composite identity; when that interface is available again at launch (or on explicit re-pick), restore validated Input/Output + channels/rate/buffer; never restore AUDIO FROM.

**Decisions locked (smoke 2026-10-02 + Build prompt + smoke renegotiation 2026-10-02):**
1. AUDIO FROM stays None (or unchanged empty) after restore / safety clear — no per-device monitoring restore.
2. Strict exact key match only — no fuzzy Scarlett* / family restore.
3. Key v1 = driver type name + exact Input name + exact Output name (empty string when None) + soft fingerprint (available in/out channel counts at save). No USB UID / OS endpoint in v1.
4. **Launch restore:** if a remembered profile’s endpoints are currently available, auto-restore Input/Output + channels/rate/buffer (most recently used among available). Skip on the first-run Criterion C pass. Re-pick in Audio Settings still restores when identity changes.
5. Store beside `Init/` via `ProjectPaths` (`AudioDeviceProfiles/`) — not inside JUCE `audioSetup` (wiped by None policy).
6. On restore, apply only rate/buffer/channel bits still offered by the live device; truncate/skip invalid parts; never crash.
7. Purge v1: document folder path in Design Notes; no Settings “delete profile” UI; optional Reveal only if trivial — **skip Reveal in v1**.
8. Cap **20** profiles; eviction = LRU by `lastUsedUtcMs`.
9. Do not reintroduce `muteInput` / blue banner.
10. **Capture safety:** never overwrite an existing disk profile with live defaults after a failed restore attempt for that key.

**Agent-owned (not re-asked):** single `profiles.xml` index in that folder; profile payload = active Input/Output channel bits + sampleRate + bufferSize (+ key meta + lastUsed); soft fingerprint uses available channel-name counts (capability), not active-bit counts; wire capture/restore in standalone `AudioMidiSettingsWindow` and launch restore via `StandaloneAudioInputRouter`; missing-device / first-run None policies leave profiles on disk untouched.

## Boundaries & Constraints

**Always:**
- Keep `applyMissingAudioDeviceNonePolicy`, first-run None, and AUDIO FROM bound-identity rules intact.
- Exact key equality for restore; mismatch → behave as unknown device (no restore).
- Core pure helpers for key, match, validate/truncate, LRU upsert — unit-tested without hardware.
- ASCII / English UI unchanged; standalone-only path.

**Never:**
- Store profiles inside JUCE `audioSetup`.
- Restore `audioFromSourceId` from a profile (launch or re-pick).
- Fuzzy name matching; USB UID phase-2; First-run setup assistant; Audio Settings Matrix chrome rebuild (chantier 4).
- Profile-purge Settings UI; `muteInput` safety.
- Overwrite an existing profile with live defaults after a failed restore for that key.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Capture stable setup | User sets Scarlett In/Out + channels/rate/buffer | Upsert profile for strict key; touch lastUsed | Skip save if both endpoints None |
| Re-pick same key | Scarlett off→None then on; user reselects same names+fingerprint | Restore validated channels/rate/buffer; AUDIO FROM stays None | Unsupported parts skipped; failed restore does not overwrite disk |
| Launch restore | Scarlett available; profile on disk; not first-run Criterion C | Open Scarlett I/O + validated rate/buffer/channels; AUDIO FROM None | Missing interface → stay None |
| Different gen / name | 6i6 2nd vs 3rd (exact OS names differ) or channel-count fingerprint differs | No restore | Leave device defaults |
| Partial capability | Saved 96 kHz / ch7 missing after firmware | Apply remaining valid rate/buffer/bits | Truncate bits beyond available |
| Cap exceeded | 21st distinct key | Evict least-recently-used; save new | Never exceed 20 |
| Device missing | Safety forces None | Profiles untouched on disk; no auto open | N/A |
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

- Profiles restore on launch when endpoints are available (`applyAvailableAudioDeviceProfileAtLaunch`) and on Audio Settings identity re-pick. Cold start / missing-device None policies do not wipe the store; first-run Criterion C skips launch restore.
- Disk profile wins over in-session `AudioDevicePreferredSetup` when a matching key exists; otherwise the preferred overlay still applies.
- Review patches (2026-10-02): restore/overlay mutate a candidate setup only; refresh from the device manager after apply success or failure before capture; skip restore/capture when `device == nullptr`; collapse duplicate keys on load (newest `lastUsedUtcMs`).
- Smoke renegotiation (2026-10-02): launch auto-restore of I/O + rate/buffer; never overwrite disk profile after failed restore (`shouldPersistCapturedProfile`).
- Smoke mitigated (2026-10-04): launch profile restore is **deferred + retried** (USB/CoreAudio enumeration race); missing-device None skips when both device catalogs are empty; standalone MIDI restore is soft-first with deferred retries (hard align only on last attempt) so CoreAudio reopen does not wipe From/To.
- Review patches (2026-10-04): launch opens names first then validates; fingerprint mismatch keeps the device without payload; one-sided empty device list does not force None; first Audio Settings observation does not capture.

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

**Lifecycle vs safety:** None policy and first-run clear live `audioSetup` only. Profiles stay. After those policies (except on the first-run pass), launch picks the most recently used profile whose endpoints are currently available and opens it with validated channels/rate/buffer. AUDIO FROM is forced empty after that restore. Re-pick in Audio Settings still restores on identity change. Failed restore never overwrites an existing disk profile with live defaults. Missing-device None skips an endpoint while that side's device list is still empty. Launch open by name then apply payload only when live available-channel counts match the stored key; mismatch keeps the interface open with driver defaults and does not write the disk profile.

**Payload:** `inputChannels` / `outputChannels` (bitmask serialization), `sampleRate`, `bufferSize`. Never `audioFromSourceId`.

**Session overlay:** Keep `AudioDevicePreferredSetup` for in-window switches; disk profile is cross-session SSOT — on re-pick, prefer disk profile values (validated) over stale in-memory preferred when a matching profile exists.

**Corruption / safety:** A bad or partial `profiles.xml` must not brick device selection — validate/truncate, skip unsupported parts, ignore unreadable store. The main integrity risk is overwrite-after-failed-restore (mitigated by `shouldPersistCapturedProfile`).

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

### Review Findings

- [x] [Review][Decision] Launch restore when live channel-count fingerprint differs — resolved 2026-10-04: option 2 (keep the box open, do not apply stored channels/rate/buffer). Helper `doesOpenedDeviceFingerprintMatchProfileKey`; skip payload + lastUsed touch on mismatch.
- [x] [Review][Decision] Missing-device None when only one catalog is still empty — resolved 2026-10-04: option 1 (do not force that endpoint to None while its list is empty).

- [x] [Review][Patch] Launch restore applies raw saved rate/buffer/bits before validation [Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:234] — open names first, then validate after open
- [x] [Review][Patch] Null current device after a “successful” open still ends the retry series [Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:291] — return false to keep retries
- [x] [Review][Patch] Launch availability is looser than the v1 key (empty live driver matches; names union every driver type) [Source/Core/Audio/AudioDeviceProfiles.h:128] — empty live type waits; names from current type only
- [x] [Review][Patch] First Audio Settings observation captures live defaults and can overwrite a richer disk profile [Source/GUI/Dialogs/AudioMidiSettingsWindow.cpp:418] — `shouldCaptureLiveSetupAsProfile`
- [x] [Review][Patch] XML load collapse of duplicate keys is untested (in-memory helper only) [Source/Core/Audio/AudioDeviceProfiles.cpp:82] — `loadXml_collapsesDuplicateKeysKeepingNewest`
- [x] [Review][Patch] First-run skip and dual-empty catalog abort are untested at the policy-gate level [Source/Standalone/MatrixControlStandaloneApp.cpp:84] — helpers + SceneAudioSafety / profile tests

- [x] [Review][Defer] `restoreMidiPortsForHost` soft-first + deferred retries have no PluginProcessor harness [Source/Core/PluginProcessorMidiPorts.cpp:337] — deferred: no processor test target for host restore; Decision 10 is implemented; smoke MIDI retry remains the gate
- [x] [Review][Defer] Dual-clear / Output-only fallback can leave OS speakers [Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:117] — deferred: scene-safety residual already logged 2026-10-02; Input-only retry unchanged
- [x] [Review][Defer] `clearEndpointToNone` early-exit when name+bits already empty can leave `useDefault*` [Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:69] — deferred: same safety residual; persisted-name clears still set the flags
- [x] [Review][Defer] Legacy `audioDeviceName` parse and GUI capture/restore sequencing lack tests [Source/Core/Audio/SceneAudioSafety.h:123] — deferred: ADM/stub + GUI-out-of-tests; reconfirmed 2026-10-04

- [x] [Review][Defer] `restoreMidiPortsForHost` soft-first + deferred retries have no PluginProcessor harness [Source/Core/PluginProcessorMidiPorts.cpp:337] — deferred: no processor test target for host restore; Decision 10 is implemented; smoke MIDI retry remains the gate
- [x] [Review][Defer] Dual-clear / Output-only fallback can leave OS speakers [Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:117] — deferred: scene-safety residual already logged 2026-10-02; Input-only retry unchanged
- [x] [Review][Defer] `clearEndpointToNone` early-exit when name+bits already empty can leave `useDefault*` [Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:69] — deferred: same safety residual; persisted-name clears still set the flags
- [x] [Review][Defer] Legacy `audioDeviceName` parse and GUI capture/restore sequencing lack tests [Source/Core/Audio/SceneAudioSafety.h:123] — deferred: ADM/stub + GUI-out-of-tests; reconfirmed 2026-10-04

#### Rejected

- Spec Tasks AC5 / Code Map / Verification still forbid launch auto-open — reject: frozen Decision 4 + Implementation Notes + code are the product contract; fixing the leftover bullets is a spec edit, not a code defect. Do not treat “cold start must stay None” as a bug.
- Claims “restore only on Audio Settings re-pick / never at cold start” — false: stale vs Decision 4; `scheduleAvailableAudioDeviceProfileRestoreAtLaunch` is intentional.
- `upsertProfileLru` can drop an incoming row with the oldest stamp — false: capture and `touchProfileLastUsed` always set `lastUsedUtcMs` to now before upsert.
- Static launch Timer not stopped on quit — low: `getInstance()` null-check makes late callbacks no-ops; quit teardown is extra surface.
- In-place `profiles.xml` write (no temp replace) — low: crash-during-write is not everyday; atomic replace adds protocol.
- `StandaloneAudioInputRouterStandalone.cpp` past ~400 lines / duplicated apply helpers — low: split/DRY is extra; launch vs re-pick drift is watched by the validation patch.
- Smoke guide packs several safety checks in one row — low: checklist hygiene, not a runtime defect.
- `restoreMidiPortsForHost` first sync without message-thread marshal — false for standalone (editor construction is the message thread); plugin `prepareToPlay` path is pre-existing and non-standalone.
