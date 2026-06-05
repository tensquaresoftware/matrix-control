#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/KeyboardFromMidiInput.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace
{
    class MinimalAudioProcessor : public juce::AudioProcessor
    {
    public:
        MinimalAudioProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", {})
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
    };

    // Mirrors PluginProcessor::setMidiInputPort success-only APVTS write.
    void setMidiInputPortLikeProcessor(juce::AudioProcessorValueTreeState& apvts,
                                       MidiManager& midiManager,
                                       const juce::String& deviceId)
    {
        if (midiManager.setMidiInputPort(deviceId))
            apvts.state.setProperty("midiInputPortId", deviceId, nullptr);
    }

    // Mirrors PluginProcessor::setMidiOutputPort success-only APVTS write.
    void setMidiOutputPortLikeProcessor(juce::AudioProcessorValueTreeState& apvts,
                                        MidiManager& midiManager,
                                        const juce::String& deviceId)
    {
        if (midiManager.setMidiOutputPort(deviceId))
            apvts.state.setProperty("midiOutputPortId", deviceId, nullptr);
    }

    // Mirrors PluginProcessor::setKeyboardFromPort (post-review) without standalone guard.
    bool setKeyboardFromPortLikeProcessor(juce::ValueTree& state,
                                        Core::KeyboardFromMidiInput& keyboardFrom,
                                        const juce::String& deviceId)
    {
        if (deviceId.isEmpty())
        {
            keyboardFrom.closePort();
            state.setProperty("keyboardFromEnabled", false, nullptr);
            state.setProperty("keyboardFromPortId", juce::String(), nullptr);
            return true;
        }

        if (!keyboardFrom.setPort(deviceId))
        {
            state.setProperty("keyboardFromEnabled", false, nullptr);
            state.setProperty("keyboardFromPortId", juce::String(), nullptr);
            return false;
        }

        state.setProperty("keyboardFromEnabled", true, nullptr);
        state.setProperty("keyboardFromPortId", deviceId, nullptr);
        return true;
    }
}

class MidiPortRoutingPropertyTests : public juce::UnitTest
{
public:
    MidiPortRoutingPropertyTests() : juce::UnitTest("MidiPortRoutingProperty") {}

    void runTest() override
    {
        testMidiInputPortEmptyClearsApvtsProperty();
        testMidiInputPortInvalidIdLeavesApvtsUnchanged();
        testMidiOutputPortEmptyClearsApvtsProperty();
        testMidiOutputPortInvalidIdLeavesApvtsUnchanged();
        testKeyboardFromPortEmptyClearsProperties();
        testKeyboardFromPortInvalidIdClearsProperties();
        testKeyboardFromPortValidIdWouldSetProperties();
    }

private:
    void testMidiInputPortEmptyClearsApvtsProperty()
    {
        beginTest("setMidiInputPort empty — clears midiInputPortId on success");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        MidiManager midiManager(processor.apvts, queue);

        processor.apvts.state.setProperty("midiInputPortId", "stale-input-id", nullptr);
        setMidiInputPortLikeProcessor(processor.apvts, midiManager, {});

        expectEquals(processor.apvts.state.getProperty("midiInputPortId").toString(), juce::String());
    }

    void testMidiInputPortInvalidIdLeavesApvtsUnchanged()
    {
        beginTest("setMidiInputPort invalid id — midiInputPortId unchanged");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        MidiManager midiManager(processor.apvts, queue);

        const juce::String staleId { "matrix-control-nonexistent-input-port" };
        processor.apvts.state.setProperty("midiInputPortId", staleId, nullptr);
        setMidiInputPortLikeProcessor(processor.apvts, midiManager, "matrix-control-nonexistent-input-port-2");

        expectEquals(processor.apvts.state.getProperty("midiInputPortId").toString(), staleId);
    }

    void testMidiOutputPortEmptyClearsApvtsProperty()
    {
        beginTest("setMidiOutputPort empty — clears midiOutputPortId on success");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        MidiManager midiManager(processor.apvts, queue);

        processor.apvts.state.setProperty("midiOutputPortId", "stale-output-id", nullptr);
        setMidiOutputPortLikeProcessor(processor.apvts, midiManager, {});

        expectEquals(processor.apvts.state.getProperty("midiOutputPortId").toString(), juce::String());
    }

    void testMidiOutputPortInvalidIdLeavesApvtsUnchanged()
    {
        beginTest("setMidiOutputPort invalid id — midiOutputPortId unchanged");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        MidiManager midiManager(processor.apvts, queue);

        const juce::String staleId { "matrix-control-nonexistent-output-port" };
        processor.apvts.state.setProperty("midiOutputPortId", staleId, nullptr);
        setMidiOutputPortLikeProcessor(processor.apvts, midiManager, "matrix-control-nonexistent-output-port-2");

        expectEquals(processor.apvts.state.getProperty("midiOutputPortId").toString(), staleId);
    }

    void testKeyboardFromPortEmptyClearsProperties()
    {
        beginTest("setKeyboardFromPort empty — clears keyboardFromEnabled and keyboardFromPortId");

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput keyboardFrom(queue);
        juce::ValueTree state { "TEST" };

        state.setProperty("keyboardFromEnabled", true, nullptr);
        state.setProperty("keyboardFromPortId", "stale-keyboard-id", nullptr);

        expect(setKeyboardFromPortLikeProcessor(state, keyboardFrom, {}));
        expect(!keyboardFrom.isPortOpen());
        expect(!static_cast<bool>(state.getProperty("keyboardFromEnabled")));
        expectEquals(state.getProperty("keyboardFromPortId").toString(), juce::String());
    }

    void testKeyboardFromPortInvalidIdClearsProperties()
    {
        beginTest("setKeyboardFromPort invalid id — clears properties and returns false");

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput keyboardFrom(queue);
        juce::ValueTree state { "TEST" };

        state.setProperty("keyboardFromEnabled", true, nullptr);
        state.setProperty("keyboardFromPortId", "stale-keyboard-id", nullptr);

        expect(!setKeyboardFromPortLikeProcessor(state, keyboardFrom, "matrix-control-nonexistent-keyboard-port"));
        expect(!keyboardFrom.isPortOpen());
        expect(!static_cast<bool>(state.getProperty("keyboardFromEnabled")));
        expectEquals(state.getProperty("keyboardFromPortId").toString(), juce::String());
    }

    void testKeyboardFromPortValidIdWouldSetProperties()
    {
        beginTest("setKeyboardFromPort valid id — sets keyboardFromEnabled and keyboardFromPortId");

        const auto devices = juce::MidiInput::getAvailableDevices();
        if (devices.isEmpty())
        {
            logMessage("Skipped — no MIDI input devices available");
            return;
        }

        const auto deviceId = devices.getReference(0).identifier;

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput keyboardFrom(queue);
        juce::ValueTree state { "TEST" };

        expect(setKeyboardFromPortLikeProcessor(state, keyboardFrom, deviceId));
        expect(keyboardFrom.isPortOpen());
        expect(static_cast<bool>(state.getProperty("keyboardFromEnabled")));
        expectEquals(state.getProperty("keyboardFromPortId").toString(), deviceId);

        keyboardFrom.closePort();
    }
};

static MidiPortRoutingPropertyTests midiPortRoutingPropertyTests;
