#include <juce_core/juce_core.h>

#include <atomic>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace
{
    struct DualProducerStressState
    {
        static constexpr int kRealtimeCount { 1000 };
        static constexpr int kSysExCount { 1000 };
        static constexpr int kConsumerTimeoutMs { 5000 };

        Core::MidiOutboundQueue queue;
        std::atomic<int> realtimeEnqueued { 0 };
        std::atomic<int> sysExEnqueued { 0 };
        std::atomic<int> realtimeDequeued { 0 };
        std::atomic<int> sysExDequeued { 0 };
        std::atomic<bool> producersDone { false };
        std::mutex seenMutex;
        std::set<int> seenNotes;
        std::set<int> seenSysExIds;
    };

    void enqueueRealtimeMessages(DualProducerStressState& state)
    {
        for (int i = 0; i < DualProducerStressState::kRealtimeCount; ++i)
        {
            const int channel = (i / 128) + 1;
            const int note = i % 128;
            state.queue.enqueueRealtime(juce::MidiMessage::noteOn(channel, note, 0.5f));
            ++state.realtimeEnqueued;
        }
    }

    void enqueueSysExMessages(DualProducerStressState& state)
    {
        for (int i = 0; i < DualProducerStressState::kSysExCount; ++i)
        {
            juce::MemoryBlock block { "\xf0\x00\x00\xf7", 4 };
            block[1] = static_cast<char>((i >> 7) & 0x7f);
            block[2] = static_cast<char>(i & 0x7f);
            state.queue.enqueueSysEx(std::move(block));
            ++state.sysExEnqueued;
        }
    }

    int realtimeMessageId(const juce::MidiMessage& message)
    {
        return (message.getChannel() - 1) * 128 + message.getNoteNumber();
    }

    int sysExMessageId(const juce::MemoryBlock& data)
    {
        return (static_cast<int>(static_cast<juce::uint8>(data[1])) << 7)
               | static_cast<int>(static_cast<juce::uint8>(data[2]));
    }

    void recordSeenId(std::mutex& mutex, std::set<int>& ids, int id)
    {
        const std::lock_guard<std::mutex> lock(mutex);
        ids.insert(id);
    }

    void handleDequeuedMessage(DualProducerStressState& state,
                               const Core::MidiOutboundQueue::Message& msg,
                               juce::UnitTest& test)
    {
        if (msg.category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
        {
            recordSeenId(state.seenMutex, state.seenNotes, realtimeMessageId(msg.midiMessage));
            ++state.realtimeDequeued;
            return;
        }

        test.expect(msg.sysExData.getSize() >= 3);
        recordSeenId(state.seenMutex, state.seenSysExIds, sysExMessageId(msg.sysExData));
        ++state.sysExDequeued;
    }

    bool consumerShouldStop(const DualProducerStressState& state)
    {
        return state.producersDone.load()
               && state.realtimeDequeued.load() >= DualProducerStressState::kRealtimeCount
               && state.sysExDequeued.load() >= DualProducerStressState::kSysExCount;
    }

    void runStressConsumer(DualProducerStressState& state, juce::UnitTest& test)
    {
        const auto deadline = juce::Time::getMillisecondCounter()
                              + DualProducerStressState::kConsumerTimeoutMs;

        while (juce::Time::getMillisecondCounter() < deadline)
        {
            if (auto msg = state.queue.dequeue())
            {
                handleDequeuedMessage(state, *msg, test);
                continue;
            }

            if (consumerShouldStop(state))
                break;

            juce::Thread::sleep(1);
        }
    }

    void expectStressCounts(juce::UnitTest& test, const DualProducerStressState& state)
    {
        test.expectEquals(state.realtimeEnqueued.load(), DualProducerStressState::kRealtimeCount);
        test.expectEquals(state.sysExEnqueued.load(), DualProducerStressState::kSysExCount);
        test.expectEquals(state.realtimeDequeued.load(), DualProducerStressState::kRealtimeCount);
        test.expectEquals(state.sysExDequeued.load(), DualProducerStressState::kSysExCount);
        test.expectEquals(static_cast<int>(state.seenNotes.size()),
                          DualProducerStressState::kRealtimeCount);
        test.expectEquals(static_cast<int>(state.seenSysExIds.size()),
                          DualProducerStressState::kSysExCount);
        test.expect(state.queue.isEmpty());
    }
}

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
        testRealtimeDepth();
        testEnqueueRealtimeFrontManyPreservesOrderAndPrepends();
        testDualProducerConsumerStress();
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
        expectEquals(static_cast<int>(queue.realtimeDepth()), 0);
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
        expectEquals(static_cast<int>(queue.realtimeDepth()), 0);
        expectEquals(static_cast<int>(queue.sysExDepth()), 0);
        expect(!queue.dequeue().has_value());
    }

    void testRealtimeDepth()
    {
        beginTest("realtimeDepth / sysExDepth track enqueue and dequeue");

        Core::MidiOutboundQueue queue;
        queue.enqueueRealtime(juce::MidiMessage::noteOn(1, 60, 0.5f));
        queue.enqueueRealtime(juce::MidiMessage::noteOff(1, 60));
        queue.enqueueSysEx(juce::MemoryBlock { "\xf0\x10\xf7", 3 });

        expectEquals(static_cast<int>(queue.realtimeDepth()), 2);
        expectEquals(static_cast<int>(queue.sysExDepth()), 1);

        (void) queue.dequeue();
        expectEquals(static_cast<int>(queue.realtimeDepth()), 1);
        expectEquals(static_cast<int>(queue.sysExDepth()), 1);
    }

    void testEnqueueRealtimeFrontManyPreservesOrderAndPrepends()
    {
        beginTest("enqueueRealtimeFrontMany prepends burst in order ahead of existing realtime");

        Core::MidiOutboundQueue queue;
        queue.enqueueRealtime(juce::MidiMessage::noteOn(1, 60, 0.5f));

        std::vector<juce::MidiMessage> burst;
        burst.push_back(juce::MidiMessage::noteOff(2, 10));
        burst.push_back(juce::MidiMessage::noteOff(2, 11));
        burst.push_back(juce::MidiMessage::controllerEvent(2, 120, 0));
        queue.enqueueRealtimeFrontMany(std::move(burst));

        expectEquals(static_cast<int>(queue.realtimeDepth()), 4);

        auto first = queue.dequeue();
        expect(first.has_value());
        expect(first->midiMessage.isNoteOff());
        expectEquals(first->midiMessage.getChannel(), 2);
        expectEquals(first->midiMessage.getNoteNumber(), 10);

        auto second = queue.dequeue();
        expect(second.has_value());
        expectEquals(second->midiMessage.getNoteNumber(), 11);

        auto third = queue.dequeue();
        expect(third.has_value());
        expect(third->midiMessage.isController());
        expectEquals(third->midiMessage.getControllerNumber(), 120);

        auto fourth = queue.dequeue();
        expect(fourth.has_value());
        expect(fourth->midiMessage.isNoteOn());
        expectEquals(fourth->midiMessage.getNoteNumber(), 60);

        expect(queue.isEmpty());
    }

    void testDualProducerConsumerStress()
    {
        beginTest("Dual producer + single consumer stress — no loss under contention");

        DualProducerStressState state;

        std::thread consumerThread([&state, this] { runStressConsumer(state, *this); });
        std::thread realtimeThread([&state] { enqueueRealtimeMessages(state); });
        std::thread sysExThread([&state] { enqueueSysExMessages(state); });

        realtimeThread.join();
        sysExThread.join();
        state.producersDone.store(true);
        consumerThread.join();

        expectStressCounts(*this, state);
    }
};

static MidiOutboundQueueTests midiOutboundQueueTests;
