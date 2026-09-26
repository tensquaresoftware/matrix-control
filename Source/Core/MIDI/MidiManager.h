#pragma once

#include <memory>
#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Shared/Definitions/MatrixDeviceTypes.h"

#include "Ports/MidiInputPort.h"
#include "Ports/MidiOutputPort.h"
#include "Transport/MidiSender.h"
#include "Transport/MidiReceiver.h"
#include "SysEx/SysExParser.h"
#include "SysEx/SysExDecoder.h"
#include "SysEx/SysExEncoder.h"
#include "Exceptions/Exceptions.h"
#include "SysEx/SysExConstants.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/Queue/SysExInterMessageDelay.h"
#include "Core/MIDI/Queue/MidiRequestTiming.h"

class MidiManager : public juce::Thread
{
public:
    explicit MidiManager(juce::AudioProcessorValueTreeState& apvtsRef,
                         Core::MidiOutboundQueue& outboundQueueRef,
                         Core::MidiActivityTracker& activityTrackerRef);
    ~MidiManager() override;

    bool setMidiInputPort(const juce::String& deviceId, bool reportOpenFailure = true);
    bool setMidiOutputPort(const juce::String& deviceId, bool reportOpenFailure = true);

    /** Close then reopen so powered-off-but-still-enumerated devices fail and clear. */
    bool forceReopenInputPort(const juce::String& deviceId, bool reportOpenFailure = false);
    bool forceReopenOutputPort(const juce::String& deviceId, bool reportOpenFailure = false);

    bool isInputPortOpenWithDevice(const juce::String& deviceId) const;
    bool isOutputPortOpenWithDevice(const juce::String& deviceId) const;
    juce::String getOpenInputDeviceId() const;
    juce::String getOpenOutputDeviceId() const;
    int getRequiredSysExDelayMs() const noexcept;

    /** Rebuild inter-SysEx delay from Settings EPROM TYPE + current device family. */
    void refreshSysExDelayFromSettings();

    void sendPatch(juce::uint8 patchNumber, const juce::uint8* packedData);
    void sendPatchToEditBuffer(const juce::uint8* packedData);
    // Matrix-1000: commit the current edit buffer to bank/patch (opcode 0x0E).
    void sendStoreEditBuffer(juce::uint8 patchNumber, juce::uint8 bank, juce::uint8 unitId = 0);
    // Full-patch audition to the synth:
    // - Matrix-6/6R: 0x01 to patchNumber
    // - Matrix-1000: 0x0D edit buffer (header F0 10 06 0D 00 … per Oberheim manual)
    void sendFullPatchForAudition(const juce::uint8* packedData,
                                  juce::uint8 patchNumber,
                                  bool deviceHasBankConcept);
    void sendMaster(juce::uint8 version, const juce::uint8* packedData);
    void sendProgramChange(int programNumber, int channel = 1);
    void sendSetBank(int bank);
    void sendUnlockBank();
    SysExEncoder& getSysExEncoder() noexcept { return *sysExEncoder; }
    const SysExEncoder& getSysExEncoder() const noexcept { return *sysExEncoder; }
    SysExDecoder& getSysExDecoder() noexcept { return *sysExDecoder; }
    const SysExDecoder& getSysExDecoder() const noexcept { return *sysExDecoder; }
    void enqueueRemoteParameterEdit(int parameterNumber, juce::uint8 packedValue);
    void enqueueMatrixModBusEdit(juce::uint8 bus,
                                 juce::uint8 source,
                                 juce::uint8 amount,
                                 juce::uint8 destination);

    // All Notes Off (CC 123) then Reset All Controllers (CC 121) on the active midiChannel,
    // via the normal realtime outbound path (never bypasses the queue).
    void sendPanic();

    // Unison Detune (CC 94) when Settings EPROM TYPE is optimised; suppressed otherwise.
    // Channel resolution matches sendPanic (midiChannel 1-16, or all channels for Omni/Mono).
    void sendUnisonDetune(int value);

    size_t getRealtimeOutboundDepth() const noexcept;

    std::vector<juce::uint8> requestCurrentPatch();
    // Request a single patch from the current bank by number (Request Data type=1).
    std::vector<juce::uint8> requestSinglePatch(juce::uint8 patchNumber);
    std::vector<juce::uint8> requestMasterData();

    // Non-blocking single-patch request. Waits for the outbound queue to go idle (without
    // blocking the message thread), settles, sends the request, then invokes callback on the
    // message thread with packed patch bytes — or an empty vector on timeout / decode / cancel.
    using PackedPatchCallback = std::function<void(std::vector<juce::uint8>)>;
    void requestSinglePatchAsync(juce::uint8 patchNumber,
                                 PackedPatchCallback callback,
                                 int settleMs = Core::MidiRequestTiming::kMinDeviceSettleMs,
                                 int outboundIdleTimeoutMs = Core::MidiRequestTiming::kMinOutboundIdleTimeoutMs);
    // Non-blocking Master Parameter Data request (type=3). Same idle/settle/timeout pattern as
    // requestSinglePatchAsync; gated by isMasterEditOutboundAllowed (Matrix-1000 only).
    void requestMasterDataAsync(PackedPatchCallback callback,
                                int settleMs = Core::MidiRequestTiming::kMinDeviceSettleMs,
                                int outboundIdleTimeoutMs = Core::MidiRequestTiming::kMinOutboundIdleTimeoutMs);
    // Processor registers apply path (MasterModel + APVTS under suppress). Empty dump = failure.
    void setMasterPullApplyHandler(PackedPatchCallback handler);
    // Apply a pulled Master dump, or publish the failure footer when empty/wrong size (no apply).
    void deliverMasterPullResult(std::vector<juce::uint8> packed);
    // Policy + trigger seam used after Device Inquiry success (and by unit tests).
    // Returns true when a Master pull was armed (async capture pending).
    bool maybePullMasterAfterInquirySuccess(bool wasDetectedBeforeSuccess,
                                            MatrixDeviceTypes::Type previousType,
                                            MatrixDeviceTypes::Type newType,
                                            bool forceBecausePortPairChanged = false);
    void cancelPendingSysExRequest();

    // True when MIDI output and input ports are open so a dump / inquiry can be attempted.
    // Ports alone do not satisfy FR-2 / V1.2: editor Program Change and SysEx still require a
    // supported Matrix device (see isEditorOutboundAllowed). Device Inquiry is the unlock path
    // and bypasses that gate.
    bool isDeviceDumpAvailable() const;

    // FR-2 / V1.2: true when a supported Matrix is detected — editor PC / SysEx may be enqueued
    // (inquiry excepted). Unknown Matrix-family members stay locked.
    bool isEditorOutboundAllowed() const;

    // Blocks (message thread) until the outbound queue has drained and no SysEx is pending,
    // or until timeoutMs elapses. Returns true if the queue reached idle. Wakes the consumer.
    bool waitUntilOutboundQueueIdle(int timeoutMs);

    // Non-blocking Universal Device Inquiry (armOneShotSysExCapture + timeout timer).
    // Safe to call from the message thread after ports are configured — never blocks MidiManager::run.
    void performDeviceInquiry();

    // After MIDI From/To change or sync: start inquiry when both ports are open (debounced by
    // pair), or clear detection when the pair is incomplete. Call from PluginProcessor after
    // setMidi*Port and from syncMidiPortsFromStateImpl.
    void refreshDeviceInquiryAfterPortSync();

    void run() override;

private:
    struct OutboundIdlePollArgs
    {
        std::uint64_t token = 0;
        int settleMs = 0;
        juce::uint32 idleStartMs = 0;
        int outboundIdleTimeoutMs = 0;
    };

    struct DevicePresenceTimer final : juce::Timer
    {
        explicit DevicePresenceTimer(MidiManager& ownerIn) noexcept
            : weakOwner(&ownerIn)
        {
        }

        void timerCallback() override
        {
            if (auto* self = weakOwner.get())
                self->onDevicePresenceTimer();
        }

        juce::WeakReference<MidiManager> weakOwner;
    };

    juce::AudioProcessorValueTreeState& apvts;

    std::unique_ptr<MidiInputPort> inputMidiPort;
    std::unique_ptr<MidiOutputPort> outputMidiPort;
    std::unique_ptr<MidiSender> midiSender;
    std::unique_ptr<MidiReceiver> midiReceiver;
    std::unique_ptr<SysExParser> sysExParser;
    std::unique_ptr<SysExDecoder> sysExDecoder;
    std::unique_ptr<SysExEncoder> sysExEncoder;
    Core::MidiOutboundQueue& outboundQueue_;
    Core::MidiActivityTracker& activityTracker_;
    Core::EditorPath editorPath_;
    Core::SysExInterMessageDelay sysExDelay_;

    void updateErrorState(const juce::String& errorMessage, const juce::String& errorType);
    void updateDeviceStatus(bool detected,
                            const juce::String& version = {},
                            MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown);
    void handleIncomingSysEx(const juce::MemoryBlock& sysEx);

    // FR-46: MASTER SysEx only for detected Matrix-1000 (mirrors isEditorOutboundAllowed).
    bool isMasterEditOutboundAllowed() const;

    void stopMidiInputCallbacks();
    bool openMidiInputPort(const juce::String& deviceId, bool reportOpenFailure);
    bool openMidiOutputPort(const juce::String& deviceId, bool reportOpenFailure);
    void wakeConsumer() noexcept;
    bool processOutboundQueue();
    void drainQueuedRealtimeOnly();
    bool handleOutboundMessage(Core::MidiOutboundQueue::Message& msg);
    bool tryDispatchPendingSysEx();
    bool isEditorSysExAllowed(const juce::MemoryBlock& sysEx) const;
    void notifyOutboundActivity(const Core::MidiOutboundQueue::Message& msg);
    bool isOutboundQueueIdle() const noexcept;
    bool hasOutboundIdleTimedOut(const OutboundIdlePollArgs& args) const noexcept;
    bool canSendSysExNow() const noexcept;
    void sendQueuedSysEx(const juce::MemoryBlock& sysExMessage, const juce::String& description);
    void dispatchRealtimeMessage(const Core::MidiOutboundQueue::Message& msg);
    void sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description);

    std::optional<Core::MidiOutboundQueue::Message> pendingSysEx_;
    std::atomic<bool> hasPendingSysEx_{ false };
    std::atomic<std::uint64_t> asyncRequestToken_{ 0 };
    std::atomic<bool> asyncSysExCaptureActive_{ false };
    PackedPatchCallback pendingAsyncCallback_;
    PackedPatchCallback masterPullApplyHandler_;
    juce::String lastInquiryInputId_;
    juce::String lastInquiryOutputId_;
    /** Set when Device Inquiry starts because MIDI From/To pair changed (not presence heartbeat). */
    bool forceMasterPullOnNextInquirySuccess_ { false };
    /** True while cancelPendingSysExRequest invokes the pending callback — empty Master
        deliver is abort, not sticky failure. */
    bool suppressMasterPullFailureFooterForEmptyResult_ { false };
    std::unique_ptr<DevicePresenceTimer> devicePresenceTimer_;

    std::vector<juce::uint8> requestSysExData(juce::uint8 requestType,
                                              size_t expectedPackedSize,
                                              const juce::String& requestDescription,
                                              juce::uint8 patchNumber = 0);
    std::vector<juce::uint8> decodeSysExPackedData(juce::uint8 requestType,
                                                   const juce::MemoryBlock& response,
                                                   size_t expectedPackedSize,
                                                   const juce::String& requestDescription);
    void sendArmedSinglePatchRequest(juce::uint8 patchNumber, std::uint64_t token);
    void armAsyncSinglePatchCapture(std::uint64_t token);
    void scheduleAsyncPatchTimeout(std::uint64_t token);
    void finishAsyncPackedPatch(std::uint64_t token, std::vector<juce::uint8> packed);
    void pollOutboundIdleThenRequest(juce::uint8 patchNumber, OutboundIdlePollArgs args);
    void scheduleOrSendArmedPatchRequest(juce::uint8 patchNumber, const OutboundIdlePollArgs& args);
    std::vector<juce::uint8> decodePackedPatchResponse(const juce::MemoryBlock& response,
                                                       const juce::String& requestDescription);
    // Quiet decode for async capture: returns empty for non-patch / corrupt SysEx without
    // treating that as request failure (caller may keep listening until timeout).
    std::vector<juce::uint8> tryDecodeAsyncPatchResponse(const juce::MemoryBlock& response);

    void sendArmedMasterRequest(std::uint64_t token);
    void armAsyncMasterCapture(std::uint64_t token);
    void scheduleAsyncMasterTimeout(std::uint64_t token);
    void pollOutboundIdleThenMasterRequest(OutboundIdlePollArgs args);
    void scheduleOrSendArmedMasterRequest(const OutboundIdlePollArgs& args);
    std::vector<juce::uint8> tryDecodeAsyncMasterResponse(const juce::MemoryBlock& response);
    void triggerMasterPullOnConnectIfAllowed();
    void publishMasterPullFailureFooter();
    void clearMasterPullFailureFooterIfPresent();

    void clearDeviceDetectionAfterPortLoss();
    void clearLastInquiryPortPair() noexcept;
    void updateDevicePresenceMonitoring();
    void onDevicePresenceTimer();
    bool armAsyncDeviceInquiryCapture(std::uint64_t token);
    void handleAsyncDeviceInquiryResponse(std::uint64_t token, const juce::MemoryBlock& response);
    void sendArmedDeviceInquiry(std::uint64_t token);
    void scheduleDeviceInquiryTimeout(std::uint64_t token);
    void pollOutboundIdleThenDeviceInquiry(OutboundIdlePollArgs args);
    void scheduleOrSendArmedDeviceInquiry(const OutboundIdlePollArgs& args);
    void finishAsyncDeviceInquirySuccess(std::uint64_t token,
                                         const DeviceIdInfo& info,
                                         MatrixDeviceTypes::Type deviceType);
    void finishAsyncDeviceInquiryFailure(std::uint64_t token,
                                         const juce::String& errorMessage,
                                         const juce::String& errorType);
    /** Abort inquiry start because the outbound queue stayed busy — keep detection state. */
    void softAbortDeviceInquiryOutboundBusy(std::uint64_t token);
    /** Presence timeout while already detected — keep detection; freeze editor SysEx traffic. */
    void softAbortDeviceInquiryUnresponsive(std::uint64_t token);
    void setDeviceMidiUnresponsive(bool unresponsive);

    JUCE_DECLARE_WEAK_REFERENCEABLE(MidiManager)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiManager)
};

