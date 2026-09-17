---
title: 'Master pull on connect (sync Master Edit from Matrix-1000)'
type: 'feature'
created: '2026-09-12'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'c0765a9f8d443e40289b1d387da2a5db48381bc5'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-unison-master-smoke.md'
  - '{project-root}/_local/References/Oberheim/oberheim-matrix-1000-midi-sysex-implementation.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Master parameters (including Master Unison) live in the Matrix-1000 Master memory and persist across patch changes, but Matrix-Control’s Master Edit panel starts from plugin defaults/templates and only *pushes* Master SysEx on edit — so the UI can disagree with the synth (e.g. plugin shows Unison OFF while the panel reads `Uon`).

**Approach:** After a Matrix-1000 is detected and Master Edit outbound is allowed, silently request Master Parameter Data from the synth, load it into `MasterModel` + APVTS, and refresh Master Edit — without inventing patch Keyboard Mode ↔ Master Unison sync.

## Boundaries & Constraints

**Always:**
- Use the existing request path (`MidiManager::requestMasterData` / request type Master Parameters) and decode into `MasterModel`, then `ApvtsMasterMapper` buffer→APVTS.
- Matrix-1000 only for Master pull UI impact (Master Edit already hidden on Matrix-6/6R).
- Auto-pull on every successful Matrix-1000 detection, including reconnects (not once-per-session only).
- Pull silently — no confirmation modal.
- Always overwrite plugin Master from the device dump on auto-pull (device wins; no Master dirty-tracker).
- When a Matrix-1000 is connected after host/session APVTS restore, still pull Master so device Master wins for Master Edit.
- Suppress outbound Master SysEx while applying the pulled dump (same quiet pattern as other editorial/bulk loads).
- Keep patch Keyboard Mode and Master Unison independent in the plugin model — this story only aligns Master Edit with device Master memory.
- Cancel any pending Master debounce before applying an inbound pull so a stale push cannot fight the dump.

**Never:**
- Cross-write Keyboard Mode (param 48) when Master Unison changes, or the reverse.
- Replace patch edit-buffer sync with Master pull.
- Assume Tauntek-only behaviour for the pull itself (stock and Tauntek both expose Master dumps).
- Show a confirmation modal before Master pull-on-connect.
- Invent a Master dirty-tracker for this story.
- Add an explicit Master Edit “Refresh from synth” control in this story (deferred).
- Commit/PR unless asked.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh detect M-1000 | Device detected, Master Edit allowed | Silent Master request; on valid dump, MasterModel+APVTS+Master Edit match device (incl. Unison Enable) | Timeout/invalid dump: keep prior Master state; footer/log error; no partial corrupt apply |
| Reconnect | Device lost then detected again | Silent pull again; overwrite plugin Master from device | Same as above |
| User mid-edit | Pending Master debounce | Cancel debounce; overwrite from device dump | Must not send stale push that fights the pull |
| Matrix-6/6R | Device is 6/6R | No Master pull / no Master Edit change | N/A |
| Session restore + connected | Host restores APVTS then M-1000 detects | Silent pull; device Master overwrites restored plugin Master for Master Edit | Same failure handling |

</frozen-after-approval>

## Code Map

- `MidiManager::requestMasterData()` (`MidiManager.h` / `MidiManager.cpp`) — implemented; **no callers today**.
- `MidiManager::requestSysExData` / `decodeSysExPackedData` (`MidiManagerAsyncPatch.cpp`) — Master branch already decodes via `SysExDecoder::decodeMasterSysEx`; timeout/decode failures use `updateErrorState`.
- Hook point: `MidiManager::finishAsyncDeviceInquirySuccess` (`MidiManagerDeviceInquiry.cpp`) — currently only updates device status/delay; **add Master pull trigger here** (or immediately after) when type is Matrix-1000 and Master Edit outbound is allowed.
- Apply path: `MasterModel::loadFrom` → `ApvtsMasterMapper::bufferToApvts` — reuse; wrap with `setSuppressMasterSysEx` / `suppressMasterParameterSysEx_` (see `PluginProcessorConstruction.cpp`, module INIT quiet, ValueTree Master outbound gates).
- Gates: `MasterEditGate` / `PluginProcessor::isMasterEditOutboundAllowed` / `MidiManager::isMasterEditOutboundAllowed` — skip pull on Matrix-6/6R and when outbound not allowed.
- Failure UX: keep prior Master state; surface via existing `updateErrorState` + footer patterns (analog: device dump fail sticky warning — do not invent a new modal for failure).
- Do not touch: `UnisonKeyboardModePolicy` / Keyboard Mode param 48 ↔ Master Unison byte 169 independence.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` (+ processor/MIDI glue as needed) -- On every Matrix-1000 detect success, silently trigger Master pull and overwrite plugin Master -- Wire unused `requestMasterData`
- [x] Apply path (processor / mapper glue) -- Decode → `MasterModel::loadFrom` → `bufferToApvts` under Master SysEx suppress; cancel pending Master debounce -- Quiet inbound apply
- [x] Failure path -- Timeout/invalid: leave Master unchanged; log + footer via existing patterns -- No corrupt partial state
- [x] `Tests/Unit/` (+ `Tests/CMakeLists.txt` if new) -- Decode→model→APVTS Unison Enable (+ one other Master field); suppress outbound during apply; 6/6R skip -- Lock behaviour
- [ ] Manual -- Cold start with synth Master Unison ON → Master Edit shows ON after detect without user toggle or modal -- Hardware truth

**Acceptance Criteria:**
- Given Matrix-1000 detected and Master Edit allowed, when pull succeeds, then Master Edit Unison (and other Master params) match the device Master dump with no confirmation modal.
- Given device lost then detected again, when detect succeeds, then Master is pulled again and plugin Master is overwritten from the device.
- Given host/session restore then Matrix-1000 detect, when pull succeeds, then device Master wins for Master Edit over restored plugin Master.
- Given pull fails, when timeout/invalid SysEx, then plugin Master state is unchanged and the failure is observable (log/footer).
- Given Matrix-6/6R, when device is detected, then no Master pull is performed for Master Edit.
- Given Keyboard Mode changes, when Master pull runs or completes, then patch Keyboard Mode is not rewritten by this feature.

### Review Findings

- [x] [Review][Patch] Cancel/abort of in-flight Master connect-pull must not sticky-warn as Master read failure [`Source/Core/MIDI/MidiManagerAsyncPatch.cpp:8` / `MidiManagerAsyncMaster.cpp:207`]
- [x] [Review][Patch] Downgrade heartbeat "skipped by policy" from Info (spam every ~5s) [`Source/Core/MIDI/MidiManagerAsyncMaster.cpp:238`]
- [x] [Review][Patch] Cover Device Inquiry finish + port-pair force flag into Master pull args in tests [`Source/Core/MIDI/MidiManagerDeviceInquiry.cpp:202`]
- [x] [Review][Patch] Add Master async idle-timeout unit test mirroring patch path [`Source/Core/MIDI/MidiManagerAsyncMaster.cpp:135`]
- [x] [Review][Defer] Sticky-footer inventory missing `kMasterPullFailed` [`PluginDisplayNames.h:201`] — deferred: sticky-footer inventory pass is out of scope for this chantier (separate dirty inventory artifact)

#### Rejected

- false — Null `masterPullApplyHandler_` clears footer without apply — `finishConstructionSetup` always registers the handler before inquiry can succeed.
- false — Null `masterModel_` / mapper drops dump silently — both are constructed before the pull handler is wired; product path always has them.
- false — `suppressMasterSysEx` stuck if apply throws — `MasterModel::loadFrom` is `noexcept`; `bufferToApvts` / APVTS set do not throw on this path.
- false — DAW session restore never re-pulls Master — `lastInquiry*` is in-memory only; after reload the empty pair forces inquiry + Master pull on first port sync (Acceptance AC path).
- false — `outboundIdleTimeoutMs == 0` causes immediate idle timeout in product — production uses profile-derived timeouts; `0` is test-only injection.
- false — Dual Timeout error state + sticky footer on real timeout — Spec requires observable failure via footer/log; both channels match that intent.
- low — Footer casing `MASTER:` vs Title Case — cosmetic inventory alignment; not worth a separate fix pass here.
- low — `maybePull` logs "requesting" then outbound gate may no-op — misleading log only; rare gate mismatch after policy allow.
- low — Production `PluginProcessor::applyInboundMasterDump` not unit-tested — shared `applyQuietInboundMasterDump` is covered; wiring drift risk is low for everyday use.

## Implementation Notes

- Wired async Master pull on Device Inquiry success for Matrix-1000 only (`requestMasterDataAsync` + `MasterPullOnConnectPolicy`: fresh detect / reconnect, not presence-heartbeat while already M-1000).
- Quiet apply: cancel Master debounce → suppress outbound → `MasterModel::loadFrom` → `bufferToApvts` → flush deferred APVTS sync; no `dispatchFull` push-back.
- Failure: prior Master unchanged; `updateErrorState` + sticky footer `kMasterPullFailed`.
- Tests: `MasterPullOnConnectTests` (policy, apply, suppress, KM independence, failure footer); `MidiManagerTests` Matrix-6 async skip.
- Manual hardware check still open (Unison ON on panel → Master Edit after detect).
- Follow-up: `deliverMasterPullResult` is public so failure-footer behaviour is unit-testable.
- Review patches: clear sticky failure footer on success; cancel invokes pending async callback with {}; shared `applyQuietInboundMasterDump`; footer MIDI FROM hint; inquiry seam `maybePullMasterAfterInquirySuccess` + expanded tests.
- Smoke fix (2026-09-18): MIDI TO port-pair change while still `deviceDetected` skipped Master pull (heartbeat policy). Force pull on port-pair inquiry success; log skip/request/apply/fail.
- Review patches (2026-09-18): cancel/abort of connect-pull does not sticky-warn; policy skip log at Debug; `consumeMasterPullInquirySnapshot` + tests; Master idle-timeout unit test.

## Spec Change Log

## Review Triage Log

- false — Edge: sysExEncoder null crash — Encoder is always constructed in MidiManager ctor (`std::make_unique<SysExEncoder>()`); null path is unreachable in product.
- false — Edge: masterPullApplyHandler_ unset discards dump — PluginProcessor::finishConstructionSetup always registers the handler before inquiry can succeed; product path always has a handler.
- false — Edge claim: heartbeat should pull on every detect success — Frozen intent is pull on detection/reconnect; presence heartbeat while already Matrix-1000 is intentionally skipped (policy + Implementation Notes).
- false — Blind/Edge: Code Map still says wire sync requestMasterData — Rejected: finding whose fix is only to edit this build's spec (living map hygiene).
- medium / patch — Successful deliverMasterPullResult never clears sticky kMasterPullFailed — Verified: success branch calls apply handler only; prior failure footer can remain.
- medium / patch — cancelPendingSysExRequest drops pendingAsyncCallback_ without invoking it — Verified: Master/patch/inquiry share the slot; connect-pull or inquiry can abort an in-flight request with no callback/footer.
- medium / patch — Apply unit tests use QuietMasterApplyHarness copy, not production applyInboundMasterDump (and omit flushDeferredApvtsParameterSync) — Verification-gap pre-verified; production can regress while suite stays green.
- medium / patch — deliverMasterPullResult success path (sized dump → handler, no failure footer) untested — Verification-gap pre-verified.
- medium / patch — Device Inquiry success never exercises triggerMasterPullOnConnectIfAllowed in tests — Verification-gap pre-verified; policy is unit-tested in isolation only.
- low / patch — Failure footer lacks MIDI FROM wiring hint present on Mutator dump-failure copy — Verified string gap vs kDeviceDumpFailedFooter pattern; direct copy fix.
- low / patch — Policy test missing Matrix-6/6R → Matrix-1000 promotion case — Title claims promotion; only kUnknown→M1000 while detected is covered.
- low / patch — No test for non-empty wrong-size buffer through deliverMasterPullResult — Empty case covered; wrong-size should also footer + skip apply.

## Design Notes

Motivation from hardware smoke (2026-09-12): Master Unison is authoritative on the synth; plugin-only Master state misled the Unison investigation. Pull-on-connect makes Master Edit a mirror of device Master memory, complementary to (not a substitute for) documenting Matrix Unison listening quirks.

Decisions (2026-09-18): auto-pull on every detect/reconnect; silent (no modal); always overwrite plugin Master (no dirty-tracker); after DAW restore, connected Matrix-1000 still pulls so device wins for Master Edit.

Related future work (out of this spec): explicit Master Edit “Refresh from synth” control; Unison listening UX workarounds / Tauntek Unison Detune CC — deferred, not this pull.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- build OK
- Targeted Master request/apply unit tests -- pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks:**
- Matrix-1000: set Ext. Funct. Unison `Uon`, launch plugin, confirm Master Edit UNISON ON after detect without touching the control and without a modal.
- Toggle Master on device, reconnect, confirm UI follows the device again.
