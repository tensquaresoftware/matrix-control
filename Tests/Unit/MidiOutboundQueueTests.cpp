#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/MidiOutboundQueue.h"

class MidiOutboundQueueTests : public juce::UnitTest
{
public:
    MidiOutboundQueueTests() : juce::UnitTest("MidiOutboundQueue") {}

    void runTest() override
    {
        testRealtimeRoundTrip();
        testRealtimePriorityOverSysEx();
        testInterleavedPriority();
        testEmptyQueue();
    }

private:
    void testRealtimeRoundTrip()
    {
        beginTest("Realtime round-trip");

        Core::MidiOutboundQueue queue;
        auto noteOn = juce::MidiMessage::noteOn(1, 60, 0.8f);
        queue.enqueueRealtime(noteOn);

        auto result = queue.dequeue();
        expect(result.has_value());
        expectEquals(static_cast<int>(result->category),
                     static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kRealtime));
        expect(result->midiMessage.isNoteOn());
        expectEquals(result->midiMessage.getNoteNumber(), 60);
        expect(queue.isEmpty());
    }

    void testRealtimePriorityOverSysEx()
    {
        beginTest("SysEx enqueued before realtime — realtime dequeued first");

        Core::MidiOutboundQueue queue;
        juce::MemoryBlock sysEx { "\xf0\x41\xf7", 3 };
        queue.enqueueSysEx(sysEx);
        queue.enqueueRealtime(juce::MidiMessage::noteOff(1, 60));

        auto first = queue.dequeue();
        expect(first.has_value());
        expectEquals(static_cast<int>(first->category),
                     static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kRealtime));
        expect(first->midiMessage.isNoteOff());

        auto second = queue.dequeue();
        expect(second.has_value());
        expectEquals(static_cast<int>(second->category),
                     static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kSysEx));

        expect(queue.isEmpty());
    }

    void testInterleavedPriority()
    {
        beginTest("Interleaved N realtime + M SysEx — all realtime before all SysEx");

        Core::MidiOutboundQueue queue;
        constexpr int kRealtimeCount { 3 };
        constexpr int kSysExCount { 2 };

        queue.enqueueSysEx(juce::MemoryBlock { "\xf0\x01\xf7", 3 });
        queue.enqueueRealtime(juce::MidiMessage::controllerEvent(1, 7, 100));
        queue.enqueueSysEx(juce::MemoryBlock { "\xf0\x02\xf7", 3 });
        queue.enqueueRealtime(juce::MidiMessage::noteOn(1, 64, 0.5f));
        queue.enqueueRealtime(juce::MidiMessage::pitchWheel(1, 8192));

        for (int i = 0; i < kRealtimeCount; ++i)
        {
            auto msg = queue.dequeue();
            expect(msg.has_value());
            expectEquals(static_cast<int>(msg->category),
                         static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kRealtime));
        }

        for (int i = 0; i < kSysExCount; ++i)
        {
            auto msg = queue.dequeue();
            expect(msg.has_value());
            expectEquals(static_cast<int>(msg->category),
                         static_cast<int>(Core::MidiOutboundQueue::MessageCategory::kSysEx));
        }

        expect(queue.isEmpty());
    }

    void testEmptyQueue()
    {
        beginTest("Empty queue — dequeue returns nullopt and isEmpty is true");

        Core::MidiOutboundQueue queue;
        expect(queue.isEmpty());
        expect(!queue.dequeue().has_value());
    }
};

static MidiOutboundQueueTests midiOutboundQueueTests;
