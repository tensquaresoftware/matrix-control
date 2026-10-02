---
title: 'Audio safety scene'
type: 'bugfix'
created: '2026-10-02'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '9c52bff03973da9d39cf0de0a6ff0d29d1702e83'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-window-modal-look-strategy.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-audio-from-none-no-device-mute.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In standalone, AUDIO FROM can disagree with the live Audio Settings Input device (device-agnostic `mono:N`/`stereo:N` ids + stale APVTS kept on refresh), so monitoring can open a feedback loop (Larsen) when the laptop lid or another interface becomes active. Cold start also invents a live source (`stereo:0` / `mono:*`) instead of safe None.

**Approach:** Ship the locked scene-safety package — keep AUDIO FROM catalog and selection tied to the current Input device identity and active channels; invalidate / clear to None when the selection is no longer valid for that device; apply first-run Input None + AUDIO FROM empty (software silence) without JUCE `muteInput` / blue banner; stop inventing a source id when the saved id is empty.

**Decisions locked (from smoke backlog / look strategy Design Notes + 2026-10-02 Build):**
- Full scene package: sync AUDIO FROM ↔ active Input device/channels + ghost invalidation + first-run None (Input + AUDIO FROM).
- **First-run criterion (C):** one-shot ApplicationProperties flag (e.g. `sceneAudioSafetyDefaultsApplied`) — on the first standalone pass where the flag is unset, force Audio Settings Input = None (no live input device/channels) **and** AUDIO FROM empty, then set the flag; never force that pair again solely because of the flag.
- **Cross-session / hot-plug safety:** persist the Input **device identity** bound to the current `audioFromSourceId` (APVTS or companion property). On launch or ADM change, if current Input identity ≠ bound identity **or** id ∉ current active-channel catalog → clear AUDIO FROM to empty (do not silent-remap indices). Do **not** also force Input = None on every unplug/fallback (Input may stay on OS-chosen device; only monitoring selection is invalidated).
- Do not brutally overwrite a valid saved user session after the one-shot has fired; identity mismatch only clears AUDIO FROM, not the whole `audioSetup`.
- AUDIO FROM None = software silence (passthrough off); never reintroduce `muteInput` / blue banner for this safety.
- Out of this Build: Audio Settings Matrix chrome rebuild (chantier 4), modal polish, punctuation, Lenovo except checklist note.
- Spec kept whole (~2150 tokens) — single scene-safety shippable; user accepted token risk.

## Boundaries & Constraints

**Always:**
- Catalogue for the header combo stays built from the **current** `AudioDeviceManager` device + active `inputChannels` only (do not feed `buildFromSystemScan` into the header).
- On Input **device identity** change, treat prior `audioFromSourceId` as invalid unless it remains listed for the **new** device catalog **and** the device name still matches the selection’s bound identity; otherwise clear to empty (None) and silence passthrough.
- Empty `audioFromSourceId` must remain empty across init/relaunch (no invent `stereo:0`/`mono:*` from channel mode).
- Core-only unit tests for pure decisions (catalog validity / device-change sync / cold-start gate); manual smoke for Larsen.
- ASCII display strings; English UI labels unchanged (`NO INPUT` sentinel).

**Never:**
- Call `disableInputMonitoring` / set `muteInput` to “secure” monitoring.
- Rebuild Audio Settings Matrix chrome (labels/combos/channel toggles/TEST/Peak placement) — chantier 4.
- Change plugin-host paths that have no standalone Input monitoring combo.
- Promise Larsen immunity without fixing source ↔ Input sync.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Live catalog | Active Input device D with channels C | Header entries only for D+C; display names include D | Empty C → catalog empty + None |
| Stale id after channel shrink | Saved `mono:3` not in new catalog | Clear `audioFromSourceId` to empty; UI None; passthrough off | Do not keep stale APVTS |
| Device identity change | Input was Scarlett, now MacBook mic; old id `mono:0` still listed on MacBook | Clear to empty (None) — do not silent-remap index across devices | Same if name changes |
| Same device, channels still valid | Device name unchanged; id still in catalog | Keep selection and passthrough maps | N/A |
| Empty id (user chose None) | `audioFromSourceId` empty on init/relaunch | Stay empty; passthrough inactive; no invent | N/A |
| First-run cold start | `sceneAudioSafetyDefaultsApplied` unset | Force Input None + AUDIO FROM empty; set flag | Later launches skip this force |
| Cross-session unplug / OS fallback | Bound identity Scarlett; launch opens MacBook mic | Clear AUDIO FROM to empty; leave Input as OS/JUCE chose | No full audioSetup wipe |
| Preferred after Device Inquiry | Matrix detected; preferred mono/stereo | Existing `applyPreferredStandaloneAudioFromForDeviceType` may pick first matching catalog id — unchanged by this story | Only when preferred kind ≠ None |

</frozen-after-approval>

## Code Map

- `Source/Core/Audio/AudioInputSourceCatalog.*` — builds `mono:N`/`stereo:N` (compacted bus index); display names prefix **device name**; header uses `buildForProcessor` → active device only; `buildFromSystemScan` unused by header — do not wire it in.
- `Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp` (`buildActiveDeviceCatalogEntries`) — sole live catalog source; reuse.
- `Source/Core/Audio/AudioFromSourceSync.*` + `Tests/Unit/AudioFromSourceSyncTests.cpp` — empty id → passthrough off; extend with **pure** validity/device-change/cold-start helpers (new small Core API preferred over bloating GUI).
- `Source/Core/PluginProcessorAudio.cpp` — `initializeAudioProperties` **invents** `stereo:0`/`mono:*` when id empty (**remove** that invent); `setAudioFromSourceId` / `applyPreferredStandaloneAudioFromForDeviceType` reuse; keep mute-clear contract.
- `Source/GUI/PluginEditorAudio.cpp` (`applyAudioCatalogToHeader` ~90–94) — **keeps stale APVTS** when id not in catalog while UI shows NO INPUT — clear to empty instead; stop auto-picking `ids[0]` when restore empty unless a frozen decision requires it (default: leave None).
- `Source/GUI/PluginEditor.cpp` / `PluginEditorTimers.cpp` — ADM change + retry refresh triggers; keep listeners, change decision outcome only.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.*` — populate/select; `kNoInputSentinel`; no Core deps reverse.
- `Source/Core/Audio/DeviceAudioInputPreference.h` — preferred after inquiry only; **do not** use as cold-start Input=None.
- JUCE `StandalonePluginHolder` via `juce_StandaloneFilterWindow.h` — `audioSetup` / `filterState` in ApplicationProperties; cold-start Input None must go through device setup (empty input device / cleared input channels) without mute banner path.
- `Source/GUI/Dialogs/AudioMidiSettingsWindow.*` — read-only this story (rate/buffer restore stays); no Matrix chrome rebuild.
- Tests: `AudioInputSourceCatalogTests.cpp`, `DeviceAudioInputPreferenceTests.cpp` — extend or add focused Core tests; register in `Tests/CMakeLists.txt` if new file.
- Guide: `_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md` — tick Audio / sécurité monitoring when done; Design Notes here record first-run criterion.

**Reuse:** empty-id silence path; active-channel catalog; enableInputMonitoring-on-startup banner avoidance.

**Do not change:** Audio Settings rebuild UI; `muteInput` as safety; plugin non-standalone catalog (empty).

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Audio/*` (new or extend `AudioFromSourceSync` / small helper) — pure decisions: source valid for device identity + catalog; post-device-change selection (keep vs clear empty); first-run gate from frozen criterion — unit-testable Core
- [x] `Source/Core/PluginProcessorAudio.cpp` — stop inventing source id when empty; apply cold-start AUDIO FROM empty when gate says so — empty survives relaunch
- [x] Standalone Input cold-start path (`StandaloneAudioInputRouter*` / holder setup hook as needed) — when gate says so, open with Input None / no input channels without `muteInput` — scene-safe default
- [x] `Source/GUI/PluginEditorAudio.cpp` (+ Header populate if needed) — on refresh after ADM change: clear invalid/stale ids to None; bind validity to device identity so Scarlett labels cannot linger for MacBook Input — fix Larsen desync
- [x] Persist Input device identity with AUDIO FROM selection (APVTS companion property) + in-session last-seen name — cross-launch and live invalidation
- [x] `Tests/Unit/*` (+ `Tests/CMakeLists.txt` if new) — cover I/O matrix pure cases (stale id, identity change, empty persist, first-run gate true/false)
- [x] Design Notes + `guide-smoke-window-modal-look.md` checklist — record criterion; leave Audio Settings rebuild unchecked
- [x] Verify — macOS Debug build + targeted unit tests + `lint_touched.py` on touched C++

**Acceptance Criteria:**
- Given standalone with Input device A selected and AUDIO FROM showing a source for A, when Input switches to device B, then AUDIO FROM becomes None (empty id, silence) rather than remapping the same channel index onto B.
- Given a saved `audioFromSourceId` not present in the current active-channel catalog, when the combo refreshes, then the property is cleared to empty and the UI shows NO INPUT (no stale APVTS keep).
- Given first-run per frozen criterion, when standalone cold-starts, then Audio Settings Input is None (no live input device/channels) and AUDIO FROM is None, with no blue mute banner.
- Given a user who already saved a valid `audioSetup` / non-empty `audioFromSourceId` outside the first-run gate, when they relaunch, then that session is restored (not force-wiped to None).
- Given empty `audioFromSourceId`, when `initializeAudioProperties` runs, then it stays empty (no invent stereo/mono).
- Given AUDIO FROM None, when monitoring is silent, then silence is passthrough-off only — not `muteInput`.

## Implementation Notes

- Core pure API: `Source/Core/Audio/SceneAudioSafety.h` — validity, post-change keep/clear, first-run gate; tests in `SceneAudioSafetyTests`.
- APVTS companion `audioFromBoundInputDeviceName` written on non-empty selection / preferred-after-inquiry; cleared with empty source id.
- Standalone one-shot `sceneAudioSafetyDefaultsApplied` in ApplicationProperties; forces Input None (empty device + cleared channels) without `muteInput`.
- `initializeAudioProperties` no longer invents `stereo:0`/`mono:*` when id empty.
- Header refresh clears invalid/stale ids to None (no APVTS keep, no auto-pick `ids[0]`).
- Review patches (2026-10-02): early first-run apply in `MatrixControlStandaloneApp::createPluginHolder` (Input None + clear AUDIO FROM); refuse empty-device bind; abort one-shot if `setAudioDeviceSetup` errors; `resolveAudioFromSourceIdAtInit` + empty-catalog unit tests.
- Smoke follow-up (2026-10-02): `shouldApplyPreferredAudioFrom` blocks preferred re-arm when source empty; catalog labels use `inputDeviceName`; first-run also tries Output None (Input-only fallback if OS rejects).

## Spec Change Log

## Review Triage Log

- BH unused `shouldClearBoundIdentity` at call sites — verdict: low | evidence: callers clear via `setAudioFromSourceId({})` which removes the bound property; field remains a pure-decision signal covered by unit tests. Route: reject (everyday harm none; removing it is cosmetic churn).
- BH router helper Input-only / incomplete pair — verdict: medium | evidence: was real when only editor cleared AUDIO FROM; patched — `MatrixControlStandaloneApp::createPluginHolder` applies Input None early and clears processor `audioFromSourceId` when apply returns true; editor path still idempotent. Route: patch (applied).
- BH bind empty device name — verdict: medium | evidence: `bindAudioFromInputDeviceIdentity` allowed empty name with non-empty source. Patched — empty deviceName removes bound property and refuses bind. Route: patch (applied).
- BH missing empty-catalog unit case — verdict: medium | evidence: matrix “Empty C → clear” lacked a direct test. Patched — `testEmptyCatalogClearsNonEmptySource`. Route: patch (applied).
- BH Verification omits SceneAudioSafety category — verdict: false | evidence: finding asks to edit this build’s spec Verification section; rejected by review rules.
- BH Code Map / CMakeLists drift — verdict: false | evidence: finding asks to edit non-frozen spec Code Map; rejected by review rules.
- BH status in-progress / empty triage — verdict: false | evidence: workflow status is managed by Build steps (`in-review`); not a product defect.
- BH PropertiesFile cast / flag may not flush — verdict: low | evidence: standalone settings come from `ApplicationProperties::getUserSettings()` (PropertiesFile); cast succeeds in normal path; residual save failure is rare. Route: reject (unlikely everyday; added no extra complexity).
- BH first-run does not clear muteInput — verdict: false | evidence: `PluginEditor` ctor calls `enableInputMonitoring()` after header restore; AC “no blue banner” holds without mute for AUDIO FROM None.
- BH guide marks unrelated polish items — verdict: low | evidence: guide edits document parallel smoke status; not an audio-safety product defect. Route: reject.
- ECH `setAudioDeviceSetup` failure still sets flag — verdict: medium | evidence: was unchecked. Patched — non-empty error string → return false without setting flag. Route: patch (applied).
- ECH flag set but PropertiesFile save fails — verdict: maybe-false | evidence: same PropertiesFile path as above; would need a failing save harness to settle. If true would be medium. Route: defer (unverified medium) — settle with forced save-failure harness or accept manual.
- ECH first-run only in editor after ADM open — verdict: medium | evidence: brief live Input before editor was real. Patched — apply + clear AUDIO FROM in `createPluginHolder` immediately after holder construction. Route: patch (applied).
- ECH bind empty deviceName — verdict: medium | evidence: same as BH bind empty; patched. Route: patch (applied).
- ECH identity TOCTOU between read and bind — verdict: maybe-false | evidence: single-threaded message-thread refresh; no demonstrated concurrent ADM callback interleaving in this path. Route: reject (would only be low if true).
- ECH restore non-empty without bound identity — verdict: false | evidence: intentional validity rule (legacy unbound selections clear on refresh); covered by `testEmptyBoundIdentityInvalidatesLegacySelection`.
- ECH claim cold-start late apply — verdict: medium | evidence: same as early-holder patch. Route: patch (applied).
- ECH claim flag after unchecked setup — verdict: medium | evidence: same as setup-error patch. Route: patch (applied).
- VG init invent contract untested — verdict: medium | evidence: invent removal had no pure pin. Patched — `resolveAudioFromSourceIdAtInit` + `testInitEmptySourceNeverInventsFromChannelMode`. Route: patch (applied).
- VG first-run Input None only gate-tested — verdict: medium (unverified automation gap) | evidence: stubs return false by design; no ADM harness. Route: defer → deferred-work.md.
- VG editor clear-to-None only helper-tested — verdict: medium (unverified automation gap) | evidence: GUI excluded from Matrix-Control_Tests. Route: defer → deferred-work.md.

## Design Notes

**Root cause:** Source ids are device-agnostic indices; display names show the current device name, so after an Input switch the combo can look “updated” while the same `mono:0` still arms passthrough on the new hardware. Stale-id path keeps APVTS and shows NO INPUT — UI/state/audio diverge. Init invents a live id when empty.

**Invalidation rule:** Persist bound Input device identity with a non-empty `audioFromSourceId`. Valid iff current Input identity equals bound identity **and** id ∈ current active-channel catalog. Else clear id (+ clear bound identity) to empty — including Scarlett unplugged → OS falls back to built-in mic between sessions. Same device + id still listed → keep. Do not force Input = None on mismatch after the one-shot.

**First-run criterion (locked C):** ApplicationProperties boolean `sceneAudioSafetyDefaultsApplied` (name may match existing Properties style). Unset → force Input None + AUDIO FROM empty once, then set true. Subsequent launches never use this gate; identity sync handles interface changes. Recorded 2026-10-02 in smoke guide Audio / sécurité monitoring checklist.

**Preferred-after-inquiry:** Leave `applyPreferredStandaloneAudioFromForDeviceType` as-is (may select first mono/stereo after Matrix detect). When it sets a source, write the current Input identity as the bound identity.

**Mute contract:** Keep `spec-audio-from-none-no-device-mute` — empty id = software silence; startup may `enableInputMonitoring()` so banner stays off.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 -DMATRIX_BUILD_TESTS=ON && cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: tests target builds
- `./Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests --category AudioFromSourceSync --category AudioInputSourceCatalog` (plus any new category) -- expected: 0 failures
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched `Source/` / `Tests/` C++

**Manual checks (if no CLI):**
- Mac: Scarlett as AUDIO FROM while Input set to MacBook mic → after fix, cannot stay on Scarlett source; switch Input → AUDIO FROM None; open lid must not Larsen from stale monitoring
- Cold start per frozen criterion: Input None + AUDIO FROM NO INPUT; no blue banner
- Relaunch with saved Scarlett session (outside gate): Input + AUDIO FROM restored
