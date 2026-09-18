#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "MidiManagerTestSupport.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

using MidiManagerTestSupport::MinimalAudioProcessor;

namespace
{
    class MidiChannelAudioProcessor : public juce::AudioProcessor
    {
    public:
        explicit MidiChannelAudioProcessor(int channelChoiceIndex)
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", makeLayout(channelChoiceIndex))
        {
        }

        juce::AudioProcessorValueTreeState apvts;

        const juce::String getName() const override { return "Test"; }
        void prepareToPlay(double, int) override {}
        void releaseResources() override {}
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
        juce::AudioProcessorEditor* createEditor() override { return nullptr; }
        bool hasEditor() const override { return false; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
        bool isMidiEffect() const override { return false; }
        double getTailLengthSeconds() const override { return 0.0; }
        int getNumPrograms() override { return 1; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int) override {}
        const juce::String getProgramName(int) override { return {}; }
        void changeProgramName(int, const juce::String&) override {}
        void getStateInformation(juce::MemoryBlock&) override {}
        void setStateInformation(const void*, int) override {}

    private:
        static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(int channelChoiceIndex)
        {
            juce::AudioProcessorValueTreeState::ParameterLayout layout;

            for (const auto& d : PluginDescriptors::MasterEditSection::MidiModule::kChoiceParameters)
            {
                if (d.parameterId
                    != PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel)
                    continue;

                layout.add(std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(d.parameterId, 1),
                    d.displayName,
                    d.choices,
                    channelChoiceIndex));
            }

            return layout;
        }
    };
}

class MidiManagerUnisonDetuneTests : public juce::UnitTest
{
public:
    MidiManagerUnisonDetuneTests() : juce::UnitTest("MidiManager UnisonDetune") {}

    void runTest() override
    {
        testSendUnisonDetuneCc94WhenOptimised();
        testSendUnisonDetuneCc94OnBasicChannel();
        testSendUnisonDetuneSuppressedWhenStock();
        testSendUnisonDetuneSuppressedWhenOutboundNotAllowed();
    }

private:
    void testSendUnisonDetuneCc94WhenOptimised()
    {
        beginTest("sendUnisonDetune — CC 94 Omni spray when EPROM optimised");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                      PluginIDs::Settings::EpromType::kTauntek,
                                      nullptr);

        // No midiChannel AudioParameter → Panic-style Omni sprays all 16 channels.
        manager.sendUnisonDetune(64);

        int ccCount = 0;
        while (auto msg = queue.tryDequeueRealtime())
        {
            expect(msg->isController());
            expectEquals(msg->getControllerNumber(), 94);
            expectEquals(msg->getControllerValue(), 64);
            ++ccCount;
        }
        expectEquals(ccCount, 16);
    }

    void testSendUnisonDetuneCc94OnBasicChannel()
    {
        beginTest("sendUnisonDetune — single CC 94 on Basic Channel 5");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        // Choice index 5 = CHANNEL 5 (0 = Omni, 1..16 = Basic Channels).
        MidiChannelAudioProcessor proc(5);
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                      PluginIDs::Settings::EpromType::kGligli,
                                      nullptr);

        manager.sendUnisonDetune(77);

        auto msg = queue.tryDequeueRealtime();
        expect(msg.has_value(), "Expected one CC 94");
        if (msg.has_value())
        {
            expect(msg->isController());
            expectEquals(msg->getControllerNumber(), 94);
            expectEquals(msg->getControllerValue(), 77);
            expectEquals(msg->getChannel(), 5);
        }
        expect(queue.isEmpty(), "Basic Channel must enqueue a single CC, not Omni spray");
    }

    void testSendUnisonDetuneSuppressedWhenStock()
    {
        beginTest("sendUnisonDetune — suppressed for FACTORY/UNKNOWN EPROM");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                      PluginIDs::Settings::EpromType::kFactory,
                                      nullptr);

        manager.sendUnisonDetune(100);
        expect(queue.isEmpty(), "CC 94 must not enqueue while EPROM gate is closed");

        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                      PluginIDs::Settings::EpromType::kUnknown,
                                      nullptr);
        manager.sendUnisonDetune(100);
        expect(queue.isEmpty(), "CC 94 must not enqueue for UNKNOWN EPROM");
    }

    void testSendUnisonDetuneSuppressedWhenOutboundNotAllowed()
    {
        beginTest("sendUnisonDetune — suppressed when Master Edit outbound not allowed");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                      PluginIDs::Settings::EpromType::kTauntek,
                                      nullptr);

        manager.sendUnisonDetune(40);
        expect(queue.isEmpty(), "CC 94 must not enqueue without Master Edit outbound");
    }
};

static MidiManagerUnisonDetuneTests midiManagerUnisonDetuneTests;
