#include <juce_core/juce_core.h>

#include "Core/Audio/InstrumentMidiForwarder.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
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
        testTrackerNotifiedOnEnqueue();
    }

private:
    void testEnqueueSysExCategory()
    {
        beginTest("EditorPath SysEx — dequeue returns kSysEx");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        Core::EditorPath editorPath(queue, tracker);
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
        Core::MidiActivityTracker tracker;
        Core::EditorPath editorPath(queue, tracker);

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
        Core::MidiActivityTracker tracker;
        Core::InstrumentMidiForwarder forwarder;
        Core::EditorPath editorPath(queue, tracker);

        juce::MidiBuffer buffer;
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
        forwarder.forward(buffer, false, queue, tracker);
        expect(queue.isEmpty());

        editorPath.enqueueProgramChange(7, 1);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        expect(msg->midiMessage.isProgramChange());
        expect(queue.isEmpty());
    }

    void testTrackerNotifiedOnEnqueue()
    {
        beginTest("Activity tracker notified on editor enqueue");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        Core::EditorPath editorPath(queue, tracker);

        editorPath.enqueueProgramChange(3, 1);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 0.0f);
    }
};

static EditorPathTests editorPathTests;
