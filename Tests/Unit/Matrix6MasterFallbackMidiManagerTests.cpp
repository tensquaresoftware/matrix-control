#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MasterPullOnConnectPolicy.h"
#include "Core/MIDI/Matrix6MasterFallbackPolicy.h"
#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "MidiManagerTestSupport.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
juce::MemoryBlock makeMatrix6MasterFrame(size_t length)
{
    juce::MemoryBlock block(length, true);
    auto* data = static_cast<juce::uint8*>(block.getData());
    data[0] = SysExConstants::kSysExStart;
    data[1] = SysExConstants::kManufacturerIdOberheim;
    data[2] = SysExConstants::kDeviceIdMatrix1000;
    data[3] = SysExConstants::Opcode::kMasterParameterData;
    data[length - 1] = SysExConstants::kSysExEnd;
    return block;
}

juce::MemoryBlock matrix1000IdentityFrame()
{
    const juce::uint8 bytes[] = {
        0xF0, 0x7E, 0x00, 0x06, 0x02, 0x10, 0x06, 0x00, 0x02, 0x00, 0x20, 0x31, 0x32, 0x30, 0xF7
    };
    return juce::MemoryBlock(bytes, sizeof(bytes));
}
} // namespace

class Matrix6MasterFallbackMidiManagerTests : public juce::UnitTest
{
public:
    Matrix6MasterFallbackMidiManagerTests()
        : juce::UnitTest("Matrix6MasterFallbackMidiManager")
    {
    }

    void runTest() override
    {
        testUndetectedTimeoutStartsFallbackWithoutPorts();
        testMatrix1000PresenceSoftAborts();
        testMatrix6FamilyPresenceStartsFallbackWithoutPorts();
        testMatrix6FamilyPresenceMasterFailureSoftAborts();
        testMatrix6FamilyPresenceMasterSuccessClearsUnresponsive();
        testMasterOnlyPresenceProofWithoutPortsClearsDetect();
        testMasterOnlyPresenceEligibilitySkipsForceAndMatrix1000();
        testPortPairForceStartsFallbackWithoutPorts();
        testFallbackSuccessPreservesEpromAndEmptyVersion();
        testLateInquiryTokenIgnoredAfterFallbackStarts();
        testStaleMasterArmClearsAsyncCaptureFlag();
        testMasterFallbackTimeoutFailureMessage();
        testMatrix1000InquirySuccessSkipsFallback();
        testPortPairForceFallbackSuccess();
        testAcceptedMasterClaimBeatsStaleTimeout();
    }

private:
    void testUndetectedTimeoutStartsFallbackWithoutPorts()
    {
        beginTest("Inquiry timeout while undetected — Master fallback runs (ports unavailable)");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.asyncRequestToken_.store(3, std::memory_order_release);

        manager.onDeviceInquiryIdentityTimeout(3);

        expect(proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Fallback without ports must fail visibly after Inquiry timeout");
        expectEquals(proc.apvts.state.getProperty("errorType").toString(), juce::String("Connection"));
        expect(! static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", true)),
               "Failed fallback must leave device undetected");
    }

    void testMatrix1000PresenceSoftAborts()
    {
        beginTest("Detected Matrix-1000 on same pair — soft-abort, no Master fallback");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = false;
        manager.asyncRequestToken_.store(5, std::memory_order_release);

        manager.onDeviceInquiryIdentityTimeout(5);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)),
               "Soft-abort must keep detection");
        expect(static_cast<bool>(
                   proc.apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
               "Soft-abort must mark MIDI unresponsive");
        expect(! proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Soft-abort must not publish Master-fallback failure");
    }

    void testMatrix6FamilyPresenceStartsFallbackWithoutPorts()
    {
        beginTest("Detected Matrix-6 family + Inquiry timeout — Master presence re-check runs");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = false;
        manager.asyncRequestToken_.store(6, std::memory_order_release);

        manager.onDeviceInquiryIdentityTimeout(6);

        // Inquiry soft-abort alone would leave token at 7; Master arm bumps then soft-abort -> 8.
        expectEquals(static_cast<int>(manager.asyncRequestToken_.load(std::memory_order_acquire)),
                     8);
        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)),
               "Presence Master failure must keep detection");
        expect(static_cast<bool>(
                   proc.apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
               "Presence Master failure (no ports) must soft-abort unresponsive");
        expect(! proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Presence Master failure must not clear-detect with Master error copy");
    }

    void testMatrix6FamilyPresenceMasterFailureSoftAborts()
    {
        beginTest("Detected Matrix-6 + Master timeout — soft-abort, keep detection");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = false;
        manager.asyncRequestToken_.store(7, std::memory_order_release);

        manager.onMatrix6MasterFallbackTimeout(7);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)),
               "Presence Master timeout must keep detection");
        expectEquals(proc.apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString(),
                     juce::String(MatrixDeviceTypes::kMatrix6Id));
        expect(static_cast<bool>(
                   proc.apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
               "Presence Master timeout must soft-abort unresponsive");
        expect(! proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Presence Master timeout must not publish clear-detect failure");
    }

    void testMasterOnlyPresenceProofWithoutPortsClearsDetect()
    {
        beginTest("Master-only presence proof without ports — clear detect (same as Inquiry start)");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        manager.asyncRequestToken_.store(12, std::memory_order_release);

        manager.performMatrix6MasterPresenceProof();

        expect(! static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", true)),
               "Defensive no-ports Master presence start must clear detect");
        expect(proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Defensive no-ports path must report Master presence connection failure");
        expectEquals(proc.apvts.state.getProperty("errorType").toString(), juce::String("Connection"));
    }

    void testMasterOnlyPresenceEligibilitySkipsForceAndMatrix1000()
    {
        beginTest("Master-only eligibility — Matrix-6 yes; Matrix-1000 / port-pair force no");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = false;

        // No ports → Master start clears detect; eligibility must still return true first.
        expect(manager.tryStartMasterOnlyPresenceProofIfEligible(),
               "Detected Matrix-6 without force must take Master-only presence");

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        expect(! manager.tryStartMasterOnlyPresenceProofIfEligible(),
               "Detected Matrix-1000 must keep Inquiry-first presence");

        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = true;
        expect(! manager.tryStartMasterOnlyPresenceProofIfEligible(),
               "Port-pair force must keep Inquiry-first even when Matrix-6 detected");
    }

    void testMatrix6FamilyPresenceMasterSuccessClearsUnresponsive()
    {
        beginTest("Detected Matrix-6 + valid Master — stay unlocked, clear unresponsive");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        constexpr int kStoredEprom = PluginIDs::Settings::EpromType::kTauntek;
        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType, kStoredEprom, nullptr);
        proc.apvts.state.setProperty(Core::kDeviceMidiUnresponsiveProperty, true, nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = false;
        manager.asyncRequestToken_.store(8, std::memory_order_release);

        const auto frame = makeMatrix6MasterFrame(SysExConstants::kMatrix6MasterMessageLengthMin);
        manager.handleAsyncMatrix6MasterFallbackResponse(8, frame);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)));
        expectEquals(proc.apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString(),
                     MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kMatrix6));
        expect(! static_cast<bool>(
                   proc.apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, true)),
               "Presence Master success must clear unresponsive");
        expect(proc.apvts.state.getProperty("deviceVersion").toString().isEmpty(),
               "Presence Master success must not invent firmware");
        expectEquals(static_cast<int>(proc.apvts.state.getProperty(PluginIDs::Settings::kEpromType)),
                     kStoredEprom);
        expect(! Core::isSectionLocked(
                   true, MatrixDeviceTypes::Type::kMatrix6, false, false));
    }

    void testPortPairForceStartsFallbackWithoutPorts()
    {
        beginTest("Port-pair force + Inquiry timeout — Master fallback runs");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix6Id,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = true;
        manager.asyncRequestToken_.store(9, std::memory_order_release);

        manager.onDeviceInquiryIdentityTimeout(9);

        expect(proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Port-pair force must enter Master fallback when Inquiry times out");
        expect(! static_cast<bool>(
                   proc.apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
               "Port-pair fallback path must not soft-abort as unresponsive");
        expect(! static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", true)),
               "Port-pair Master failure must clear detection");
    }

    void testFallbackSuccessPreservesEpromAndEmptyVersion()
    {
        beginTest("Master fallback success — Matrix-6, empty version, EPROM preserved, delay");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        constexpr int kStoredEprom = PluginIDs::Settings::EpromType::kTauntek;
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType, kStoredEprom, nullptr);
        manager.asyncRequestToken_.store(11, std::memory_order_release);

        const auto frame = makeMatrix6MasterFrame(SysExConstants::kMatrix6MasterMessageLengthMin);
        expect(Core::isMatrix6MasterParameterDump(frame));
        manager.handleAsyncMatrix6MasterFallbackResponse(11, frame);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)));
        expectEquals(proc.apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString(),
                     MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kMatrix6));
        expect(proc.apvts.state.getProperty("deviceVersion").toString().isEmpty(),
               "Fallback must not invent a firmware version");
        expectEquals(static_cast<int>(proc.apvts.state.getProperty(PluginIDs::Settings::kEpromType)),
                     kStoredEprom);
        expectEquals(manager.getRequiredSysExDelayMs(),
                     Core::SysExDelayProfile::kOptimisedDelayMsMatrix6);
    }

    void testLateInquiryTokenIgnoredAfterFallbackStarts()
    {
        beginTest("Late Inquiry token — ignored after Master fallback invalidates it");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.asyncRequestToken_.store(20, std::memory_order_release);

        manager.sendArmedMatrix6MasterFallback(20);

        expect(! Core::isMatrix6MasterFallbackTokenCurrent(
                   20, manager.asyncRequestToken_.load(std::memory_order_acquire)),
               "Fallback must bump the async token so late Inquiry callbacks no-op");
    }

    void testStaleMasterArmClearsAsyncCaptureFlag()
    {
        beginTest("Stale Master arm token — clears async capture so presence can retry");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        manager.asyncRequestToken_.store(30, std::memory_order_release);
        manager.asyncSysExCaptureActive_.store(true, std::memory_order_release);

        manager.sendArmedMatrix6MasterFallback(29);

        expect(! manager.asyncSysExCaptureActive_.load(std::memory_order_acquire),
               "Failed Master arm must not leave presence capture stuck");
        expectEquals(static_cast<int>(manager.asyncRequestToken_.load(std::memory_order_acquire)),
                     30);
    }

    void testMasterFallbackTimeoutFailureMessage()
    {
        beginTest("Master-fallback timeout — Inquiry-then-Master message and Timeout type");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.asyncRequestToken_.store(25, std::memory_order_release);

        manager.onMatrix6MasterFallbackTimeout(25);

        expectEquals(proc.apvts.state.getProperty("lastError").toString(),
                     juce::String(Core::matrix6MasterFallbackTimeoutFailureMessage()));
        expectEquals(proc.apvts.state.getProperty("errorType").toString(), juce::String("Timeout"));
        expect(! static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", true)),
               "Fallback timeout must clear detection");
    }

    void testMatrix1000InquirySuccessSkipsFallback()
    {
        beginTest("Matrix-1000 Inquiry success — detects M-1000, never enters Master fallback");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.asyncRequestToken_.store(15, std::memory_order_release);

        manager.handleAsyncDeviceInquiryResponse(15, matrix1000IdentityFrame());
        manager.cancelPendingSysExRequest(); // Drop M-1000 pull-on-connect armed by fresh detect.

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)),
               "Inquiry success must detect the synth");
        expectEquals(proc.apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString(),
                     MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kMatrix1000));
        expect(! proc.apvts.state.getProperty("deviceVersion").toString().isEmpty(),
               "Inquiry success must keep a firmware version string");
        expect(! proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase(
                       "Device Inquiry, then Master"),
               "Inquiry success must not publish Master-discovery-fallback timeout copy");
        expect(! Core::shouldRunMatrix6MasterFallbackOnInquiryTimeout(
                       true, false, MatrixDeviceTypes::Type::kMatrix1000),
               "After M-1000 detect, presence timeout must soft-abort, not Master-fallback");
    }

    void testPortPairForceFallbackSuccess()
    {
        beginTest("Port-pair force + valid Master — Matrix-6 detect, force consumed, no M-1000 pull");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                      PluginIDs::Settings::EpromType::kFactory,
                                      nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = true;
        manager.asyncRequestToken_.store(31, std::memory_order_release);

        const auto frame = makeMatrix6MasterFrame(SysExConstants::kMatrix6MasterMessageLengthMax);
        manager.handleAsyncMatrix6MasterFallbackResponse(31, frame);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)));
        expectEquals(proc.apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName).toString(),
                     MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kMatrix6));
        expect(! manager.forceMasterPullOnNextInquirySuccess_,
               "Success must consume the port-pair force flag");
        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
                    true,
                    MatrixDeviceTypes::Type::kMatrix1000,
                    MatrixDeviceTypes::Type::kMatrix6,
                    true),
               "Matrix-6 family must not arm M-1000 Master pull-on-connect");
        expectEquals(manager.getRequiredSysExDelayMs(),
                     Core::SysExDelayProfile::kStockDelayMsMatrix6);
    }

    void testAcceptedMasterClaimBeatsStaleTimeout()
    {
        beginTest("Capture claim + Master success — stale fallback timeout must no-op");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        manager.asyncRequestToken_.store(40, std::memory_order_release);

        // Mirror capture-callback claim before message-thread finish (invalidates timeout 40).
        auto expected = static_cast<std::uint64_t>(40);
        expect(manager.asyncRequestToken_.compare_exchange_strong(
            expected, 41, std::memory_order_acq_rel));

        const auto frame = makeMatrix6MasterFrame(SysExConstants::kMatrix6MasterMessageLengthMin);
        manager.handleAsyncMatrix6MasterFallbackResponse(41, frame);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)));

        manager.onMatrix6MasterFallbackTimeout(40);

        expect(static_cast<bool>(proc.apvts.state.getProperty("deviceDetected", false)),
               "Stale Master-fallback timeout must not clear a successful detection");
        expect(proc.apvts.state.getProperty("errorType").toString() != "Timeout",
               "Stale timeout must not publish Timeout after successful Master proof");
    }
};

static Matrix6MasterFallbackMidiManagerTests matrix6MasterFallbackMidiManagerTests;
