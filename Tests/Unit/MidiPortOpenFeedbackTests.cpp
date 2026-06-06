#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiPortOpenFeedback.h"

class MidiPortOpenFeedbackTests : public juce::UnitTest
{
public:
    MidiPortOpenFeedbackTests() : juce::UnitTest("MidiPortOpenFeedback") {}

    void runTest() override
    {
        testFailureReasonCodes();
        testFormatFooterMessageNotFound();
        testFormatFooterMessageOpenRejected();
    }

private:
    void testFailureReasonCodes()
    {
        beginTest("failureReasonCode maps enum values");

        expectEquals(Core::MidiPortOpenFeedback::failureReasonCode(Core::MidiPortOpenFailureReason::kNotFound),
                     juce::String("not_found"));
        expectEquals(Core::MidiPortOpenFeedback::failureReasonCode(Core::MidiPortOpenFailureReason::kOpenRejected),
                     juce::String("open_rejected"));
        expectEquals(Core::MidiPortOpenFeedback::failureReasonCode(Core::MidiPortOpenFailureReason::kNone),
                     juce::String("none"));
    }

    void testFormatFooterMessageNotFound()
    {
        beginTest("formatFooterMessage — not found");

        const auto inputMessage = Core::MidiPortOpenFeedback::formatFooterMessage(
            true,
            "IAC Driver Bus 1",
            Core::MidiPortOpenFailureReason::kNotFound);
        expect(inputMessage.contains("MIDI From"));
        expect(inputMessage.contains("IAC Driver Bus 1"));
        expect(inputMessage.contains("port not found"));

        const auto outputMessage = Core::MidiPortOpenFeedback::formatFooterMessage(
            false,
            "loopMIDI Port",
            Core::MidiPortOpenFailureReason::kNotFound);
        expect(outputMessage.contains("MIDI To"));
        expect(outputMessage.contains("loopMIDI Port"));
    }

    void testFormatFooterMessageOpenRejected()
    {
        beginTest("formatFooterMessage — open rejected");

        const auto message = Core::MidiPortOpenFeedback::formatFooterMessage(
            false,
            "loopMIDI Port 1",
            Core::MidiPortOpenFailureReason::kOpenRejected);

        expect(message.contains("MIDI To"));
        expect(message.contains("loopMIDI Port 1"));
        expect(message.contains("could not open"));

#if JUCE_WINDOWS
        expect(message.contains("loopMIDI"));
        expect(message.contains("Documentation/windows-midi-multi-client.md"));
#else
        expect(message.contains("Try closing other applications"));
        expect(!message.contains("Documentation/windows-midi-multi-client.md"));
#endif
    }
};

static MidiPortOpenFeedbackTests midiPortOpenFeedbackTests;
