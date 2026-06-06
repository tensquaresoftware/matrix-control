#include <juce_core/juce_core.h>

#include <atomic>
#include <mutex>
#include <set>
#include <thread>

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

    void testDualProducerConsumerStress()
    {
        beginTest("Dual producer + single consumer stress — no loss under contention");

        Core::MidiOutboundQueue queue;
        constexpr int kRealtimeCount { 1000 };
        constexpr int kSysExCount { 1000 };

        std::atomic<int> realtimeEnqueued { 0 };
        std::atomic<int> sysExEnqueued { 0 };
        std::atomic<int> realtimeDequeued { 0 };
        std::atomic<int> sysExDequeued { 0 };
        std::atomic<bool> producersDone { false };

        std::mutex seenMutex;
        std::set<int> seenNotes;
        std::set<int> seenSysExIds;

        auto realtimeProducer = [&]
        {
            for (int i = 0; i < kRealtimeCount; ++i)
            {
                const int channel = (i / 128) + 1;
                const int note = i % 128;
                queue.enqueueRealtime(juce::MidiMessage::noteOn(channel, note, 0.5f));
                ++realtimeEnqueued;
            }
        };

        auto sysExProducer = [&]
        {
            for (int i = 0; i < kSysExCount; ++i)
            {
                juce::MemoryBlock block { "\xf0\x00\x00\xf7", 4 };
                block[1] = static_cast<char>((i >> 7) & 0x7f);
                block[2] = static_cast<char>(i & 0x7f);
                queue.enqueueSysEx(std::move(block));
                ++sysExEnqueued;
            }
        };

        auto consumer = [&]
        {
            const auto deadline = juce::Time::getMillisecondCounter() + 5000;

            while (juce::Time::getMillisecondCounter() < deadline)
            {
                if (auto msg = queue.dequeue())
                {
                    if (msg->category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
                    {
                        const int id = (msg->midiMessage.getChannel() - 1) * 128
                                       + msg->midiMessage.getNoteNumber();
                        {
                            const std::lock_guard<std::mutex> lock(seenMutex);
                            seenNotes.insert(id);
                        }
                        ++realtimeDequeued;
                    }
                    else
                    {
                        expect(msg->sysExData.getSize() >= 3);
                        const int id = (static_cast<int>(static_cast<juce::uint8>(msg->sysExData[1])) << 7)
                                       | static_cast<int>(static_cast<juce::uint8>(msg->sysExData[2]));
                        {
                            const std::lock_guard<std::mutex> lock(seenMutex);
                            seenSysExIds.insert(id);
                        }
                        ++sysExDequeued;
                    }

                    continue;
                }

                if (producersDone.load()
                    && realtimeDequeued.load() >= kRealtimeCount
                    && sysExDequeued.load() >= kSysExCount)
                    break;

                juce::Thread::sleep(1);
            }
        };

        std::thread consumerThread(consumer);
        std::thread realtimeThread(realtimeProducer);
        std::thread sysExThread(sysExProducer);

        realtimeThread.join();
        sysExThread.join();
        producersDone.store(true);
        consumerThread.join();

        expectEquals(realtimeEnqueued.load(), kRealtimeCount);
        expectEquals(sysExEnqueued.load(), kSysExCount);
        expectEquals(realtimeDequeued.load(), kRealtimeCount);
        expectEquals(sysExDequeued.load(), kSysExCount);
        expectEquals(static_cast<int>(seenNotes.size()), kRealtimeCount);
        expectEquals(static_cast<int>(seenSysExIds.size()), kSysExCount);
        expect(queue.isEmpty());
    }
};

static MidiOutboundQueueTests midiOutboundQueueTests;
