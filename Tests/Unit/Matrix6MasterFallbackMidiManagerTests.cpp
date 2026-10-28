#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/Matrix6MasterFallbackPolicy.h"
#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
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
        testSoftAbortPresenceSkipsFallback();
        testPortPairForceStartsFallbackWithoutPorts();
        testFallbackSuccessPreservesEpromAndEmptyVersion();
        testLateInquiryTokenIgnoredAfterFallbackStarts();
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

    void testSoftAbortPresenceSkipsFallback()
    {
        beginTest("Already detected on same pair — soft-abort, no Master fallback");

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

    void testPortPairForceStartsFallbackWithoutPorts()
    {
        beginTest("Port-pair force + Inquiry timeout — Master fallback runs");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManagerTestSupport::MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        manager.forceMasterPullOnNextInquirySuccess_ = true;
        manager.asyncRequestToken_.store(9, std::memory_order_release);

        manager.onDeviceInquiryIdentityTimeout(9);

        expect(proc.apvts.state.getProperty("lastError").toString().containsIgnoreCase("Master"),
               "Port-pair force must enter Master fallback when Inquiry times out");
        expect(! static_cast<bool>(
                   proc.apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
               "Port-pair fallback path must not soft-abort as unresponsive");
    }

    void testFallbackSuccessPreservesEpromAndEmptyVersion()
    {
        beginTest("Master fallback success — Matrix-6, empty version, EPROM preserved");

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
};

static Matrix6MasterFallbackMidiManagerTests matrix6MasterFallbackMidiManagerTests;
