#pragma once

#include <mutex>
#include <optional>
#include <queue>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

namespace Core
{
    class MidiOutboundQueue
    {
    public:
        enum class MessageCategory { kRealtime, kSysEx };

        struct Message
        {
            MessageCategory category;
            juce::MidiMessage midiMessage;
            juce::MemoryBlock sysExData;
        };

        MidiOutboundQueue() = default;

        void enqueueRealtime(juce::MidiMessage message);
        void enqueueSysEx(juce::MemoryBlock sysEx);

        std::optional<Message> dequeue();
        bool isEmpty() const noexcept;

    private:
        std::queue<juce::MidiMessage> realtimeQueue_;
        std::queue<juce::MemoryBlock> sysExQueue_;
        mutable std::mutex queueMutex_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiOutboundQueue)
    };
}
