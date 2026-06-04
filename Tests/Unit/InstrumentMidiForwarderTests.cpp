#include <juce_core/juce_core.h>

#include "Core/Audio/InstrumentMidiForwarder.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

class InstrumentMidiForwarderTests : public juce::UnitTest
{
public:
    InstrumentMidiForwarderTests() : juce::UnitTest("InstrumentMidiForwarder") {}

    void runTest() override
    {
        testDisabledPathLeavesQueueEmpty();
        testNoteCcPitchBendForwardedWhenEnabled();
        testProgramChangeAndSysExStrippedWhenEnabled();
        testChannelPressureAndAftertouchStrippedWhenEnabled();
    }

private:
    void testDisabledPathLeavesQueueEmpty()
    {
        beginTest("Disabled instrument path — queue empty after forward");

        Core::MidiOutboundQueue queue;
        Core::InstrumentMidiForwarder forwarder;
        juce::MidiBuffer buffer;
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);

        forwarder.forward(buffer, false, queue);

        expect(queue.isEmpty());
        expect(!queue.dequeue().has_value());
    }

    void testNoteCcPitchBendForwardedWhenEnabled()
    {
        beginTest("Note, CC, and pitch bend forwarded when enabled");

        Core::MidiOutboundQueue queue;
        Core::InstrumentMidiForwarder forwarder;
        juce::MidiBuffer buffer;
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
        buffer.addEvent(juce::MidiMessage::controllerEvent(1, 7, 100), 0);
        buffer.addEvent(juce::MidiMessage::pitchWheel(1, 8192), 0);

        forwarder.forward(buffer, true, queue);

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

    void testProgramChangeAndSysExStrippedWhenEnabled()
    {
        beginTest("Program change and SysEx stripped — only note dequeued");

        Core::MidiOutboundQueue queue;
        Core::InstrumentMidiForwarder forwarder;
        juce::MidiBuffer buffer;
        buffer.addEvent(juce::MidiMessage::programChange(1, 42), 0);
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);

        const juce::uint8 sysExData[] { 0xF0, 0x41, 0x10, 0x06, 0x12, 0x00, 0xF7 };
        buffer.addEvent(juce::MidiMessage::createSysExMessage(sysExData, static_cast<int>(sizeof(sysExData))), 0);

        forwarder.forward(buffer, true, queue);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(msg->midiMessage.isNoteOn());
        expectEquals(msg->midiMessage.getNoteNumber(), 60);
        expect(!queue.dequeue().has_value());
        expect(queue.isEmpty());
    }

    void testChannelPressureAndAftertouchStrippedWhenEnabled()
    {
        beginTest("Channel pressure and aftertouch stripped — only note dequeued");

        Core::MidiOutboundQueue queue;
        Core::InstrumentMidiForwarder forwarder;
        juce::MidiBuffer buffer;
        buffer.addEvent(juce::MidiMessage::channelPressureChange(1, 80), 0);
        buffer.addEvent(juce::MidiMessage::aftertouchChange(1, 60, 90), 0);
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);

        forwarder.forward(buffer, true, queue);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(msg->midiMessage.isNoteOn());
        expectEquals(msg->midiMessage.getNoteNumber(), 60);
        expect(!queue.dequeue().has_value());
        expect(queue.isEmpty());
    }
};

static InstrumentMidiForwarderTests instrumentMidiForwarderTests;
