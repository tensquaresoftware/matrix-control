#include <juce_core/juce_core.h>

#include "Core/Audio/InstrumentMidiForwarder.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

class EditorPathTests : public juce::UnitTest
{
public:
    EditorPathTests() : juce::UnitTest("EditorPath") {}

    void runTest() override
    {
        testEnqueueSysExCategory();
        testEnqueueProgramChangeRealtime();
        testEditorEnqueueWhileInstrumentPathDisabled();
    }

private:
    void testEnqueueSysExCategory()
    {
        beginTest("EditorPath SysEx — dequeue returns kSysEx");

        Core::MidiOutboundQueue queue;
        Core::EditorPath editorPath(queue);
        juce::MemoryBlock sysEx { "\xf0\x41\xf7", 3 };

        editorPath.enqueueSysEx(sysEx);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expectEquals(static_cast<int>(msg->category),
                     static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kSysEx));
        expect(queue.isEmpty());
    }

    void testEnqueueProgramChangeRealtime()
    {
        beginTest("EditorPath program change — dequeue returns kRealtime");

        Core::MidiOutboundQueue queue;
        Core::EditorPath editorPath(queue);

        editorPath.enqueueProgramChange(42, 1);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expectEquals(static_cast<int>(msg->category),
                     static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kRealtime));
        expect(msg->midiMessage.isProgramChange());
        expectEquals(msg->midiMessage.getProgramChangeNumber(), 42);
        expect(queue.isEmpty());
    }

    void testEditorEnqueueWhileInstrumentPathDisabled()
    {
        beginTest("Editor enqueue works while instrument path disabled");

        Core::MidiOutboundQueue queue;
        Core::InstrumentMidiForwarder forwarder;
        Core::EditorPath editorPath(queue);

        juce::MidiBuffer buffer;
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
        forwarder.forward(buffer, false, queue);
        expect(queue.isEmpty());

        editorPath.enqueueProgramChange(7, 1);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(msg->midiMessage.isProgramChange());
        expect(queue.isEmpty());
    }
};

static EditorPathTests editorPathTests;
