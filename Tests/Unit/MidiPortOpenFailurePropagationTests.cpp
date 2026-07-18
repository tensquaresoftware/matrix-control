#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Shared/Definitions/PluginDisplayNames.h"

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

    bool setMidiOutputPortLikeProcessor(juce::AudioProcessorValueTreeState& apvts,
                                        MidiManager& midiManager,
                                        const juce::String& deviceId)
    {
        if (midiManager.setMidiOutputPort(deviceId))
        {
            apvts.state.setProperty("midiOutputPortId", deviceId, nullptr);
            return true;
        }

        return false;
    }

    bool setMidiInputPortLikeProcessor(juce::AudioProcessorValueTreeState& apvts,
                                       MidiManager& midiManager,
                                       const juce::String& deviceId)
    {
        if (midiManager.setMidiInputPort(deviceId))
        {
            apvts.state.setProperty("midiInputPortId", deviceId, nullptr);
            return true;
        }

        return false;
    }
}

class MidiPortOpenFailurePropagationTests : public juce::UnitTest
{
public:
    MidiPortOpenFailurePropagationTests()
        : juce::UnitTest("MidiPortOpenFailurePropagation")
    {
    }

    void runTest() override
    {
        testOutputPortNotFoundPropagatesFooterMessage();
        testInputPortNotFoundPropagatesFooterMessage();
        testFailedOpenDoesNotUpdateApvtsPortId();
        testEmptyDeviceIdClearDoesNotSetFooterMessage();
        testPortFailureDoesNotUpdateLastErrorState();
    }

private:
    void testOutputPortNotFoundPropagatesFooterMessage()
    {
        beginTest("setMidiOutputPort invalid id — footer uiMessageText populated");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager(processor.apvts, queue, tracker);

        const juce::String staleId { "matrix-control-stale-output-id" };
        processor.apvts.state.setProperty("midiOutputPortId", staleId, nullptr);

        expect(!setMidiOutputPortLikeProcessor(processor.apvts,
                                              midiManager,
                                              "matrix-control-nonexistent-output-port"));

        const auto footerMessage = processor.apvts.state.getProperty("uiMessageText").toString();
        expect(footerMessage.isNotEmpty());
        expect(footerMessage.contains("MIDI To"));
        expect(footerMessage.contains("port not found"));
        expectEquals(processor.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String("error"));
        expectEquals(processor.apvts.state.getProperty("midiOutputPortId").toString(), staleId);
    }

    void testInputPortNotFoundPropagatesFooterMessage()
    {
        beginTest("setMidiInputPort invalid id — footer uiMessageText populated");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager(processor.apvts, queue, tracker);

        expect(!setMidiInputPortLikeProcessor(processor.apvts,
                                              midiManager,
                                              "matrix-control-nonexistent-input-port"));

        const auto footerMessage = processor.apvts.state.getProperty("uiMessageText").toString();
        expect(footerMessage.isNotEmpty());
        expect(footerMessage.contains("MIDI From"));
        expect(footerMessage.contains("port not found"));
        expectEquals(processor.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String("error"));
    }

    void testFailedOpenDoesNotUpdateApvtsPortId()
    {
        beginTest("failed open — midiInputPortId unchanged");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager(processor.apvts, queue, tracker);

        const juce::String staleId { "matrix-control-stale-input-id" };
        processor.apvts.state.setProperty("midiInputPortId", staleId, nullptr);

        expect(!setMidiInputPortLikeProcessor(processor.apvts,
                                              midiManager,
                                              "matrix-control-nonexistent-input-port-2"));

        expectEquals(processor.apvts.state.getProperty("midiInputPortId").toString(), staleId);
    }

    void testEmptyDeviceIdClearDoesNotSetFooterMessage()
    {
        beginTest("empty device id clear — re-asserts device-lock guidance while undetected");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager(processor.apvts, queue, tracker);

        // MidiManager ctor leaves deviceDetected=false (FR-2 locked).
        expect(midiManager.setMidiInputPort(juce::String()));
        expect(midiManager.setMidiOutputPort(juce::String()));

        expectEquals(processor.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(PluginDisplayNames::FooterPanel::kDeviceLockGuidance));
        expectEquals(processor.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String("info"));

        processor.apvts.state.setProperty("deviceDetected", true, nullptr);
        expect(midiManager.setMidiInputPort(juce::String()));
        expect(midiManager.setMidiOutputPort(juce::String()));
        expect(processor.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(processor.apvts.state.getProperty("uiMessageSeverity").toString().isEmpty());
    }

    void testPortFailureDoesNotUpdateLastErrorState()
    {
        beginTest("port open failure — lastError and errorType unchanged");

        MinimalAudioProcessor processor;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager(processor.apvts, queue, tracker);

        const juce::String priorError { "Prior SysEx error" };
        const juce::String priorType { "SysEx" };
        processor.apvts.state.setProperty("lastError", priorError, nullptr);
        processor.apvts.state.setProperty("errorType", priorType, nullptr);

        expect(!midiManager.setMidiOutputPort("matrix-control-nonexistent-output-port-3"));

        expectEquals(processor.apvts.state.getProperty("lastError").toString(), priorError);
        expectEquals(processor.apvts.state.getProperty("errorType").toString(), priorType);
    }
};

static MidiPortOpenFailurePropagationTests midiPortOpenFailurePropagationTests;
