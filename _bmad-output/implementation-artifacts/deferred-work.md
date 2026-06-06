# Deferred Work

## Deferred from: code review of 2-10-matrix-mod-bus-reorder-sysex (2026-06-06)

- **`suppressMatrixModParameterSysEx_` bool vs nested counter** (`PluginProcessor.h:185`) — story spec recommends bool; nested multi-swap reentrancy out of scope for message-thread v1.

## Deferred from: code review of r-3-midi-port-open-error-feedback (2026-06-06)

- **`MidiPortOpenResult` types colocated in `MidiPortOpenFeedback.h`** — pulls `juce_audio_processors` into port headers; extract to `MidiPortOpenResult.h` if port layer grows.
- **No end-to-end `kOpenRejected` propagation test** — formatter branch covered; OS-level open rejection not mockable without harness.

## Deferred from: code review of r-2-hardware-latency-gui (2026-06-06)

- **Standalone header layout overlap (HW LATENCY + SCALE/SKIN vs UI ELEMENTS)** (`HeaderPanel.cpp`) — superseded by planned SETTINGS window story (Epic 7.7+); header interim layout accepted until then.
- **Redundant `setHardwareLatencyMs` + `updateHostDisplay` on editor open** (`PluginEditor.cpp:130-132`) — idempotent host refresh; no functional bug.
- **Sample-rate fallback 44100 before first `prepareToPlay`** (`PluginProcessor.cpp:512-518`) — corrected on prepare; same pattern as audio passthrough.
- **No automated APVTS round-trip test for `hardwareLatencyMs`** — mirrors existing `inputGainDb` test gap.

## Deferred from: code review of 2-11-header-panel-layout-and-widget-styling (2026-06-05)

- **Slider `-inf` display keyed on `unit_ == "dB"`** (`Slider.cpp:127`) — only header gain slider uses dB unit today; flag or per-slider opt-in when a second dB slider is added.
- **Optional `MidiManager::dispatchOutboundMessage` outbound-notify test** (`MidiManagerTests.cpp`) — AC 7.2 optional; producer + `MidiActivityTrackerTests` cover main paths.
- **Clock step-back guard in `computeLevelFromTimestamp`** (`MidiActivityTracker.cpp:11`) — inherited from Story 2.8; extremely rare on monotonic hi-res counter.

## Deferred from: code review of 2-8-activity-leds-on-queue-traffic (2026-06-05)

- **`memory_order_relaxed` on activity atomics** — matches `AudioPassthroughProcessor` peak pattern (Story 2.7); acquire/release optional hardening deferred.
- **`ActivityLed` unused `width_`/`height_`/`uiScale_`** — mirrors `PeakIndicator` prior art; cleanup deferred to Story 7.8 header polish.
- **No unit test for `MidiManager::dispatchOutboundMessage` tracker notify** — optional coverage; producer + EditorPath tests cover main paths.
- **Clock step-backward guard in `computeLevelFromTimestamp`** — extremely rare on monotonic hi-res counter; low priority.

## Deferred from: code review of 2-7-audio-passthrough-and-peak-indicator (2026-06-05)

- **Standalone audio-from combo not refreshed on device change** (`PluginEditor.cpp:73-77`) — follow-up UX; spec lists channels at editor open only.
- **Weak symbols (`__attribute__((weak))`) not portable to MSVC** (`StandaloneAudioInputRouterStubs.cpp:5-15`) — Windows build follow-up; macOS verified.
- **Standalone `audioFromSourceId` uses unstable numeric channel indices** (`StandaloneAudioInputRouterStandalone.cpp:35-36`) — device-change edge case; channel names as IDs optional hardening.
- **`getInstrumentPathEnabled` reads `apvts.state` on audio thread (standalone)** (`PluginProcessor.cpp:321-324`) — pre-existing pattern extracted from `processBlock`; atomic cache deferred to Epic 7/8.
- **Redundant `setAudioFromSourceId` / `setInputGainDb` on startup** (`PluginProcessor.cpp:386-388`, `PluginEditor.cpp:97-112`) — harmless duplication between ctor init and editor restore.

## Deferred from: code review of 2-9b-header-routing-controls-uat-slice (2026-06-05)

- **Combo/backend mismatch si `MidiManager::set*Port` échoue** (`PluginProcessor.cpp:249-268`) — pattern brownfield pre-existing ; combo avance, APVTS inchangé jusqu’au prochain succès.
- **Double forwarding host buffer en standalone** (`PluginProcessor.cpp:210-218`) — AC #7 assume buffer hôte vide ; edge case host injectant MIDI en standalone.
- **MIDI FROM + KEYBOARD FROM même device** (`HeaderPanel` / `PluginProcessor`) — deux `juce::MidiInput` sur un identifiant ; pas de garde UI dans scope UAT slice.
- **`setStateInformation` sans resync ports/combos** (`PluginProcessor.cpp:233-246`) — restore ports uniquement au ctor `PluginEditor` ; reload session complète OK.
- **Thread safety `keyboardFromEnabled` message/audio** (`PluginProcessor.cpp:213-214`) — `juce::var` lu audio thread, écrit message thread ; pre-existing story 2.3.

## Deferred from: code review of 2-9-wire-midimanager-queue-consumer (2026-06-05)

- **RPC + consumer concurrent `sysExDelay_` / `MidiSender` access** (`MidiManager.cpp:223,275,351`) — brownfield threading; story completion notes accept timestamp-only gate; `MidiSender` not thread-safe if inquiry runs concurrent with consumer; no call-site refactor in scope.
- **AC #8c strict `millisUntilNextAllowed` assertion absent** (`MidiManagerTests.cpp:113-133`) — gate-sharing test verifies drain-without-hang only; strict timing deferred to `SysExInterMessageDelayTests` per dev notes.
- **`stopThread` during blocking `waitUntilReady`** (`MidiManager.cpp:351`) — consumer may block on inter-SysEx sleep; teardown races possible; same pattern as existing `PluginProcessor` lifecycle.
- **No test for `MidiConnectionException` → `updateErrorState` in consumer** (`MidiManagerTests.cpp`) — catch branch unverified; no-output guard prevents throw in current tests.
- **Drain tests assert queue empty, not bytes on wire** (`MidiManagerTests.cpp`) — no spy on `sendMidiMessage`/`sendSysEx`; hardware validation deferred to Story 2.9b smoke.

## Deferred from: code review of 2-6-matrix-mod-bus-parameter-sysex (2026-06-05)

- **`readPackedByte` silent zero on invalid offset** (`MatrixModBusParameterSysExDispatcher.cpp:44-47`) — same `PackedFieldCodec::safeOffset` pattern as `PatchParameterSysExDispatcher`; descriptor offsets are stable.
- **`syncIntToBuffer` / `syncChoiceToBuffer` skip when `rawValue == nullptr`** (`ApvtsPatchMapper.cpp:38-47`) — pre-existing mapper behaviour; Matrix Mod extension inherits it.
- **Unknown `parameterId` silent return** (`MatrixModBusParameterSysExDispatcher.cpp:25-27`) — matches patch dispatcher; typo in ID fails silently in release.
- **Dual routing ID sets** (`PluginProcessor.cpp:632-643`) — `patchParameterIds_`, `matrixModParameterIds_`, and dispatcher map built separately; established 2.4/2.5 branching pattern.
- **Matrix Mod mapper test one-way only** (`ApvtsPatchMapperTests.cpp:175-211`) — `apvtsToBuffer` spot-check bus 0 satisfies story task; full `bufferToApvts` round-trip optional.
- **No SysEx coalescing on rapid Matrix Mod edits** (`PluginProcessor.cpp:518-525`) — one 0x0B per property change by design (FR-14); debounce deferred to future perf work.

## Deferred from: code review of 2-5-apvts-master-parameter-to-full-master-sysex-0x03 (2026-06-05)

- **Choice unit test omits full payload byte-compare** (`MasterParameterSysExDispatcherTests.cpp:91-123`) — int test compares `msg->sysExData == expected`; choice test stops at header/frame checks. AC #5 satisfied; parity optional.
- **Full test-suite pass not evidenced in changeset** — no CI log or local `Matrix-Control_Tests` output in artifact; run before merge.

## Deferred from: code review of 2-4-apvts-patch-parameter-to-sysex-0x06 (2026-06-04)

- **`MidiManager::enqueueRemoteParameterEdit` sans tests** (`MidiManager.cpp:172`) — garde silencieuse + enqueue EditorPath ; `MidiManagerTests` reste stub TODO, hors périmètre 2.4.
- **Pas de test E2E `PluginProcessor` → `MidiManager` → queue** — câblage prod validé par revue ; test d’intégration reporté à story consommateur / harness MIDI.
- **`apvtsToBuffer()` O(n) par changement patch** (`PluginProcessor.cpp:501`) — aligné dev notes 2.4 ; batching perf = story future.

## Deferred from: code review of 2-2-sysexdelayprofile-and-inter-message-delay (2026-06-04)

- **Inquiry / RPC SysEx bypass inter-SysEx gate** (`MidiManager.cpp:204,252`) — only `sendSysExWithDelay` uses `SysExInterMessageDelay` per AC #5; global gate for all outbound SysEx deferred to Story 2.9 consumer.
- **4-char Device Inquiry version limits optimised EPROM detection** (`SysExDecoder.cpp:181-191`) — `extractDeviceVersion` fills at most 4 chars; full `TAUNTEK`/`GLIGLI`/`NORDCORE` tokens cannot match on real replies until decoder/SM-1 extends version extraction.

## Deferred from: code review of 2-1-midioutboundqueue-core (2026-06-04)

- **Unbounded queue growth** (`MidiOutboundQueue.cpp:5`) — no max depth; revisit with 2.9 consumer or flood policy.
- **SysEx via `enqueueRealtime`** (`MidiOutboundQueue.cpp:5`) — no `isSysEx()` reroute; producer discipline in 2.3.
- **`isEmpty` / `dequeue` TOCTOU** (`MidiOutboundQueue.cpp:38`) — consumer must not gate on prior `isEmpty()`.
- **Large SysEx copy + mutex hold on dequeue** (`MidiOutboundQueue.cpp:30`) — audio enqueue can block; profile before optimizing.
- **No flush/clear API** (`MidiOutboundQueue.h`) — disconnect/panic handling deferred to 2.9.
- **`enqueueRealtime` by-value copy** (`MidiOutboundQueue.cpp:5`) — optional `&&` overload if profiling warrants.
- **Empty SysEx `MemoryBlock` accepted** (`MidiOutboundQueue.cpp:11`) — validate at consumer if needed.
- **Single-consumer not enforced** (`MidiOutboundQueue.cpp:17`) — document for `MidiManager::run()` in 2.9.
- **Tests beyond AC A–D** (`MidiOutboundQueueTests.cpp:10`) — SysEx byte assert, concurrency optional later.

## Deferred from: code review of 1-5-patch-name-bytes-0-7-sync (2026-06-04)

- **`apvtsToBuffer` no thread contract, allocates** (`PatchNameSyncer.cpp:15`) — `valueTreePropertyChanged` may fire off the message thread; `getProperty().toString()` + `setName` allocate. Same pre-existing pattern as `ApvtsPatchMapper`; revisit with threading-hardening story.
- **Lossy 6-bit/7-bit charset round-trip in `PatchModel`** (`PatchModel.cpp:62`) — `setName` masks 0x7F, `getName` masks 0x3F + remaps codes < 0x20; identity only in printable 0x20-0x5F band. Pre-existing `PatchModel` behavior, out of Story 1.5 scope.
- **Model name not seeded from default at startup** (`PluginProcessor.cpp:271`) — default `"--------"` set before `addListener`, so `apvtsToBuffer` never runs at construction; model name stays zero-init (decodes `"@@@@@@@@"`) until first edit/patch load. Matches existing mapper construction pattern.

## Deferred from: code review of 1-3-apvtspatchmapper-round-trip (2026-06-04)

- **`apvtsToBuffer()` O(n) full-buffer syncs per parameter change** (`PluginProcessor.cpp:459`) — intentional for Story 1.3; batching belongs in a future performance story.
- **Silent null skip without assertion** (`ApvtsPatchMapper.cpp:36-38`) — correct guard for synthetic-descriptor test pattern; add `jassert` if stricter contract needed.
- **Thread safety — `bufferToApvts()` / `apvtsToBuffer()` unguarded** (`ApvtsPatchMapper.cpp`) — explicitly scoped to future threading hardening story per spec dev notes.
- **`memset` in Test C bypasses `PatchModel` API** (`ApvtsPatchMapperTests.cpp:190`) — acceptable direct-buffer reset in test; revisit if PatchModel gains write invariants.
- **Descriptor vectors built twice at startup** (`PluginProcessor.cpp:526-534`) — mapper ctor + `buildPatchParameterIdSet()` both call static builders; startup-only, negligible cost.
- **Hardcoded `intDescs[0]` / `[2]` indices in tests** (`ApvtsPatchMapperTests.cpp:132-134`) — confirmed correct by ECH reading `PluginDescriptorsPatchEdit.cpp`; same deferred pattern as Story 1.4.
- **Public `bufferToApvts()` feedback loop risk** (`ApvtsPatchMapper.h:22`) — not called in current code; Story 2.4 caller sites must use `MessageManager::callAsync` per spec thread note.

## Deferred from: code review of 1-4-apvtsmastermapper-round-trip (2026-06-04)

- **Hardcoded vector indices in tests** (`Tests/Unit/ApvtsMasterMapperTests.cpp:130`) — `intDescs[0]` / `intDescs[2]` rely on `buildIntDescriptors()` returning a stable order; tests pass and document expected index-to-parameter mapping; revisit with `std::find_if` if descriptor order ever changes.

## Deferred from: code review of 1-2-mastermodel-packed-buffer (2026-06-03)

- **`safeOffset` no release-mode bounds guard** (`Source/Core/Models/PackedFieldCodec.cpp:22`) — debug-only jassert mirrors the original PatchModel pattern; descriptor offsets are compile-time constants so the risk is low; revisit if runtime-provided offsets are ever introduced.
- **`choices.size() - 1` size_t underflow when choices is empty** (`Source/Core/Models/MasterModel.cpp:30`, same in `PatchModel.cpp`) — pre-existing from Story 1.1; all real descriptors have ≥ 1 choice; add `jassert(!descriptor.choices.isEmpty())` if stricter contract is desired.

## Deferred from: code review of 1-1-patchmodel-packed-buffer (2026-06-02)

- **`signBitPosition` undefined for `maxValue ≤ 0` or non-`2^n−1` ranges** (`Source/Core/Models/PatchModel.cpp:53-58`) — `jlimit` acts as a safety net so no current descriptor is affected; revisit if a signed descriptor with a non-power-of-two max is ever added.
- **`getChoiceIndex` silently clamps stale/corrupt buffer bytes** (`Source/Core/Models/PatchModel.cpp:40`) — defensive clamping via `jmax`/`jlimit` is correct for the current descriptor set; add `jassert(!descriptor.choices.isEmpty())` if a stricter contract is desired.
