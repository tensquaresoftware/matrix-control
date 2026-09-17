#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MasterPullOnConnectPolicy.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "MidiManagerTestSupport.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using MidiManagerTestSupport::MinimalAudioProcessor;

class MidiManagerTests : public juce::UnitTest
{
public:
    MidiManagerTests() : juce::UnitTest("MidiManager Tests") {}

    void runTest() override
    {
        testDeviceDumpUnavailableWithoutDevice();
        testWaitUntilOutboundQueueIdleReturnsTrueWhenEmpty();
        testRefreshInquiryClearsDetectionWithoutPorts();
        testEditorOutboundGateBlocksProgramChangeWhenUndetected();
        testEditorOutboundGateBlocksSysExWhenUndetected();
        testEditorOutboundGateAllowsSendWhenDetected();
        testSendMasterFailClosedForNonMatrix1000();
        testWeakReferenceClearsAfterMidiManagerDestroy();
        testRequestSinglePatchAsyncIdleTimeoutFailsVisibly();
        testRequestMasterDataAsyncIdleTimeoutFailsVisibly();
        testRequestMasterDataAsyncSkippedForMatrix6();
        testMasterPullInquirySnapshotConsumesPortPairForce();
        testCancelConnectPullDoesNotPublishFailureFooter();
    }

private:
    void testDeviceDumpUnavailableWithoutDevice()
    {
        beginTest("Device dump unavailable when no output/device detected");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        expect(!manager.isDeviceDumpAvailable(),
               "Dump path requires open MIDI ports and a detected device");
    }

    void testRefreshInquiryClearsDetectionWithoutPorts()
    {
        beginTest("refreshDeviceInquiryAfterPortSync clears stale detection without ports");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        proc.apvts.state.setProperty("deviceVersion", "1.20", nullptr);

        manager.refreshDeviceInquiryAfterPortSync();

        expect(! static_cast<bool>(proc.apvts.state.getProperty("deviceDetected")),
               "Incomplete port pair must clear deviceDetected");
        expectEquals(proc.apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString(),
                     MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kUnknown));
    }

    void testWaitUntilOutboundQueueIdleReturnsTrueWhenEmpty()
    {
        beginTest("waitUntilOutboundQueueIdle returns true when queue already empty");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        expect(manager.waitUntilOutboundQueueIdle(50),
               "Empty outbound queue must report idle immediately");
    }

    void testEditorOutboundGateBlocksProgramChangeWhenUndetected()
    {
        beginTest("FR-2 — Program Change not enqueued when deviceDetected=false");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.sendProgramChange(12, 1);
        expect(queue.isEmpty(), "Program Change must not enqueue while locked");
    }

    void testEditorOutboundGateBlocksSysExWhenUndetected()
    {
        beginTest("FR-2 — editor SysEx not enqueued when deviceDetected=false");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.enqueueRemoteParameterEdit(10, 64);
        expect(queue.isEmpty(), "Remote parameter SysEx must not enqueue while locked");
    }

    void testEditorOutboundGateAllowsSendWhenDetected()
    {
        beginTest("FR-2 — Program Change and SysEx enqueue when deviceDetected=true");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

        manager.sendProgramChange(12, 1);
        manager.enqueueRemoteParameterEdit(10, 64);
        expect(!queue.isEmpty(), "Editor outbound must enqueue when unlocked");
    }

    void testSendMasterFailClosedForNonMatrix1000()
    {
        beginTest("FR-46 — sendMaster is a no-op for Matrix-6 / Matrix-6R / Unknown / undetected");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        std::array<juce::uint8, SysExConstants::kMasterPackedDataSize> packed {};
        const auto trySend = [&](bool detected, MatrixDeviceTypes::Type type)
        {
            while (queue.dequeue().has_value())
            {
            }

            proc.apvts.state.setProperty("deviceDetected", detected, nullptr);
            proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                          MatrixDeviceTypes::toApvtsString(type),
                                          nullptr);
            manager.sendMaster(1, packed.data());
            expect(queue.isEmpty(), "sendMaster must fail closed for non-Matrix-1000");
        };

        trySend(false, MatrixDeviceTypes::Type::kUnknown);
        trySend(true, MatrixDeviceTypes::Type::kUnknown);
        trySend(true, MatrixDeviceTypes::Type::kMatrix6);
        trySend(true, MatrixDeviceTypes::Type::kMatrix6R);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        manager.sendMaster(1, packed.data());
        expect(!queue.isEmpty(), "sendMaster must enqueue for detected Matrix-1000");
    }

    void testWeakReferenceClearsAfterMidiManagerDestroy()
    {
        beginTest("WeakReference — deferred MidiManager capture clears after destroy");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        juce::WeakReference<MidiManager> weak;

        {
            MidiManager manager(proc.apvts, queue, tracker);
            weak = &manager;
            expect(weak.get() != nullptr, "WeakReference must resolve while MidiManager is alive");
        }

        expect(weak.get() == nullptr,
               "WeakReference must clear after MidiManager destruction (UAF guard primitive)");
    }

    void testRequestSinglePatchAsyncIdleTimeoutFailsVisibly()
    {
        beginTest("requestSinglePatchAsync — injectable idle timeout fails with empty callback");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

        const juce::uint8 stuckBytes[] = { 0xF0, 0x10, 0x06, 0x06, 0x00, 0x40, 0xF7 };
        juce::MemoryBlock stuckSysEx(stuckBytes, sizeof(stuckBytes));
        queue.enqueueSysEx(stuckSysEx);

        bool callbackRan = false;
        bool callbackEmpty = false;
        manager.requestSinglePatchAsync(
            0,
            [&](std::vector<juce::uint8> packed)
            {
                callbackRan = true;
                callbackEmpty = packed.empty();
            },
            0,
            0);

        expect(callbackRan, "Idle timeout must invoke the dump callback");
        expect(callbackEmpty, "Timed-out dump must fail with an empty packed buffer");
        expect(proc.apvts.state.getProperty("lastError").toString().contains("Timeout"),
               "Idle timeout must surface a visible Timeout error state");
        expectEquals(proc.apvts.state.getProperty("errorType").toString(), juce::String("Timeout"));
    }

    void testRequestMasterDataAsyncIdleTimeoutFailsVisibly()
    {
        beginTest("requestMasterDataAsync — injectable idle timeout fails with empty callback");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

        const juce::uint8 stuckBytes[] = { 0xF0, 0x10, 0x06, 0x06, 0x00, 0x40, 0xF7 };
        juce::MemoryBlock stuckSysEx(stuckBytes, sizeof(stuckBytes));
        queue.enqueueSysEx(stuckSysEx);

        bool callbackRan = false;
        bool callbackEmpty = false;
        manager.requestMasterDataAsync(
            [&](std::vector<juce::uint8> packed)
            {
                callbackRan = true;
                callbackEmpty = packed.empty();
            },
            0,
            0);

        expect(callbackRan, "Idle timeout must invoke the Master pull callback");
        expect(callbackEmpty, "Timed-out Master pull must fail with an empty packed buffer");
        expect(proc.apvts.state.getProperty("lastError").toString().contains("Timeout"),
               "Idle timeout must surface a visible Timeout error state");
        expectEquals(proc.apvts.state.getProperty("errorType").toString(), juce::String("Timeout"));
    }

    void testRequestMasterDataAsyncSkippedForMatrix6()
    {
        beginTest("requestMasterDataAsync — Matrix-6 skips without starting a pull");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);

        bool callbackRan = false;
        bool callbackEmpty = false;
        manager.requestMasterDataAsync(
            [&](std::vector<juce::uint8> packed)
            {
                callbackRan = true;
                callbackEmpty = packed.empty();
            });

        expect(callbackRan, "Gate skip must invoke callback immediately");
        expect(callbackEmpty, "Matrix-6 Master pull must not return packed data");
    }

    void testMasterPullInquirySnapshotConsumesPortPairForce()
    {
        beginTest("Master pull inquiry snapshot — port-pair force consumed into pull args");

        bool forceFlag = true;
        const auto snap = Core::consumeMasterPullInquirySnapshot(
            true, MatrixDeviceTypes::Type::kMatrix1000, forceFlag);

        expect(snap.forceBecausePortPairChanged && ! forceFlag);
        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
                   snap, MatrixDeviceTypes::Type::kMatrix1000));

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);
        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

        expect(manager.maybePullMasterAfterInquirySuccess(
                   snap.wasDetectedBeforeSuccess,
                   snap.previousType,
                   MatrixDeviceTypes::Type::kMatrix1000,
                   snap.forceBecausePortPairChanged));
        manager.cancelPendingSysExRequest();

        bool noForce = false;
        const auto heartbeat = Core::consumeMasterPullInquirySnapshot(
            true, MatrixDeviceTypes::Type::kMatrix1000, noForce);
        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
                    heartbeat, MatrixDeviceTypes::Type::kMatrix1000));
    }

    void testCancelConnectPullDoesNotPublishFailureFooter()
    {
        beginTest("cancel Master connect-pull — abort must not sticky-warn as read failure");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        proc.apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
        proc.apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);

        bool applyCalled = false;
        manager.setMasterPullApplyHandler(
            [&](std::vector<juce::uint8>)
            {
                applyCalled = true;
            });

        manager.requestMasterDataAsync(
            [&](std::vector<juce::uint8> packed)
            {
                manager.deliverMasterPullResult(std::move(packed));
            },
            5000,
            5000);

        manager.cancelPendingSysExRequest();

        expect(! applyCalled, "Cancelled Master pull must not apply");
        expect(proc.apvts.state.getProperty("uiMessageText").toString()
                   != PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed,
               "Cancel/abort must not publish sticky Master pull failure footer");
    }
};

static MidiManagerTests midiManagerTests;
