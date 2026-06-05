#include <juce_core/juce_core.h>

#include "Core/MIDI/KeyboardFromMidiInput.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

class KeyboardFromMidiInputTests : public juce::UnitTest
{
public:
    KeyboardFromMidiInputTests() : juce::UnitTest("KeyboardFromMidiInput") {}

    void runTest() override
    {
        testClosedPortDoesNotOpenWithoutDevice();
        testNoteCcPitchBendEnqueuedWhenProcessed();
        testProgramChangeAndSysExStrippedWhenProcessed();
        testChannelPressureAndAftertouchStrippedWhenProcessed();
    }

private:
    void testClosedPortDoesNotOpenWithoutDevice()
    {
        beginTest("No port open after empty setPort");

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput input(queue);

        expect(!input.isPortOpen());
        expect(input.setPort({}));
        expect(!input.isPortOpen());
        expect(queue.isEmpty());
    }

    void testNoteCcPitchBendEnqueuedWhenProcessed()
    {
        beginTest("Note, CC, and pitch bend enqueued when processed");

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput input(queue);

        input.processIncomingMessage(juce::MidiMessage::noteOn(1, 60, 0.8f));
        input.processIncomingMessage(juce::MidiMessage::controllerEvent(1, 7, 100));
        input.processIncomingMessage(juce::MidiMessage::pitchWheel(1, 8192));

        auto note = queue.dequeue();
        expect(note.has_value());
        expect(note->midiMessage.isNoteOn());

        auto cc = queue.dequeue();
        expect(cc.has_value());
        expect(cc->midiMessage.isController());

        auto pitch = queue.dequeue();
        expect(pitch.has_value());
        expect(pitch->midiMessage.isPitchWheel());

        expect(queue.isEmpty());
    }

    void testProgramChangeAndSysExStrippedWhenProcessed()
    {
        beginTest("Program change and SysEx stripped — only note dequeued");

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput input(queue);

        input.processIncomingMessage(juce::MidiMessage::programChange(1, 42));
        input.processIncomingMessage(juce::MidiMessage::noteOn(1, 60, 0.8f));

        const juce::uint8 sysExData[] { 0xF0, 0x41, 0x10, 0x06, 0x12, 0x00, 0xF7 };
        input.processIncomingMessage(
            juce::MidiMessage::createSysExMessage(sysExData, static_cast<int>(sizeof(sysExData))));

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(msg->midiMessage.isNoteOn());
        expectEquals(msg->midiMessage.getNoteNumber(), 60);
        expect(!queue.dequeue().has_value());
        expect(queue.isEmpty());
    }

    void testChannelPressureAndAftertouchStrippedWhenProcessed()
    {
        beginTest("Channel pressure and aftertouch stripped — only note dequeued");

        Core::MidiOutboundQueue queue;
        Core::KeyboardFromMidiInput input(queue);

        input.processIncomingMessage(juce::MidiMessage::channelPressureChange(1, 80));
        input.processIncomingMessage(juce::MidiMessage::aftertouchChange(1, 60, 90));
        input.processIncomingMessage(juce::MidiMessage::noteOn(1, 60, 0.8f));

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(msg->midiMessage.isNoteOn());
        expectEquals(msg->midiMessage.getNoteNumber(), 60);
        expect(!queue.dequeue().has_value());
        expect(queue.isEmpty());
    }
};

static KeyboardFromMidiInputTests keyboardFromMidiInputTests;
