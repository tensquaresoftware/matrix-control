#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    // Mirrors PluginProcessor::dispatchUnisonDetuneChange (CC path only).
    struct UnisonDetuneDispatchHarness
    {
        class Proc : public juce::AudioProcessor
        {
        public:
            Proc()
                : juce::AudioProcessor(BusesProperties())
                , apvts(*this, nullptr, "P", makeLayout())
            {
            }

            juce::AudioProcessorValueTreeState apvts;

            const juce::String getName() const override { return "UnisonDetuneDispatch"; }
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
            static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout()
            {
                juce::AudioProcessorValueTreeState::ParameterLayout layout;

                for (const auto& d : PluginDescriptors::MasterEditSection::MiscModule::kIntParameters)
                {
                    if (d.parameterId
                        != PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune)
                        continue;

                    layout.add(std::make_unique<juce::AudioParameterInt>(
                        juce::ParameterID(d.parameterId, 1),
                        d.displayName,
                        d.minValue,
                        d.maxValue,
                        d.defaultValue));
                }

                for (const auto& d : PluginDescriptors::MasterEditSection::MidiModule::kChoiceParameters)
                {
                    if (d.parameterId
                        != PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel)
                        continue;

                    // Index 5 = CHANNEL 5.
                    layout.add(std::make_unique<juce::AudioParameterChoice>(
                        juce::ParameterID(d.parameterId, 1),
                        d.displayName,
                        d.choices,
                        5));
                }

                return layout;
            }
        };

        Proc proc;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager manager;
        bool suppressMasterParameterSysEx = false;
        bool editorialQuiet = false;

        UnisonDetuneDispatchHarness()
            : manager(proc.apvts, queue, tracker)
        {
            proc.apvts.state.setProperty("deviceDetected", true, nullptr);
            proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                          MatrixDeviceTypes::kMatrix1000Id,
                                          nullptr);
            proc.apvts.state.setProperty(PluginIDs::Settings::kEpromType,
                                          PluginIDs::Settings::EpromType::kTauntek,
                                          nullptr);
        }

        void dispatchUnisonDetuneChange(const juce::String& parameterId)
        {
            if (parameterId
                != PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune)
                return;

            if (suppressMasterParameterSysEx || editorialQuiet)
                return;

            const auto* raw = proc.apvts.getRawParameterValue(parameterId);
            if (raw == nullptr)
                return;

            manager.sendUnisonDetune(juce::roundToInt(raw->load()));
        }

        void setDetuneValue(int value)
        {
            auto* param = proc.apvts.getParameter(
                PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune);
            jassert(param != nullptr);
            param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(value)));
        }
    };
}

class UnisonDetuneDispatchTests : public juce::UnitTest
{
public:
    UnisonDetuneDispatchTests() : juce::UnitTest("UnisonDetuneDispatch") {}

    void runTest() override
    {
        testDispatchEnqueuesCc94();
        testDispatchSuppressedWhenMasterSysExSuppressed();
        testDispatchSuppressedWhenEditorialQuiet();
    }

private:
    void testDispatchEnqueuesCc94()
    {
        beginTest("dispatchUnisonDetuneChange — CC 94 on Basic Channel when optimised");

        UnisonDetuneDispatchHarness harness;
        harness.setDetuneValue(42);
        harness.dispatchUnisonDetuneChange(
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune);

        auto msg = harness.queue.tryDequeueRealtime();
        expect(msg.has_value());
        if (msg.has_value())
        {
            expect(msg->isController());
            expectEquals(msg->getControllerNumber(), 94);
            expectEquals(msg->getControllerValue(), 42);
            expectEquals(msg->getChannel(), 5);
        }
        expect(harness.queue.isEmpty());
    }

    void testDispatchSuppressedWhenMasterSysExSuppressed()
    {
        beginTest("dispatchUnisonDetuneChange — no CC when suppressMasterParameterSysEx");

        UnisonDetuneDispatchHarness harness;
        harness.suppressMasterParameterSysEx = true;
        harness.setDetuneValue(55);
        harness.dispatchUnisonDetuneChange(
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune);

        expect(harness.queue.isEmpty());
    }

    void testDispatchSuppressedWhenEditorialQuiet()
    {
        beginTest("dispatchUnisonDetuneChange — no CC when editorial quiet");

        UnisonDetuneDispatchHarness harness;
        harness.editorialQuiet = true;
        harness.setDetuneValue(66);
        harness.dispatchUnisonDetuneChange(
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune);

        expect(harness.queue.isEmpty());
    }
};

static UnisonDetuneDispatchTests unisonDetuneDispatchTests;
