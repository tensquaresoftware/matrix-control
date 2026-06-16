#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace Core
{
    void MidiOutboundQueue::setWakeConsumerCallback(WakeConsumerFn callback)
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        wakeConsumer_ = std::move(callback);
    }

    void MidiOutboundQueue::wakeConsumerIfNeeded()
    {
        WakeConsumerFn callback;
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            callback = wakeConsumer_;
        }

        if (callback != nullptr)
            callback();
    }

    void MidiOutboundQueue::enqueueRealtime(juce::MidiMessage message)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            realtimeQueue_.push(std::move(message));
        }

        wakeConsumerIfNeeded();
    }

    void MidiOutboundQueue::enqueueSysEx(juce::MemoryBlock sysEx)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            sysExQueue_.push(std::move(sysEx));
        }

        wakeConsumerIfNeeded();
    }

    std::optional<MidiOutboundQueue::Message> MidiOutboundQueue::dequeue()
    {
        std::lock_guard<std::mutex> lock(queueMutex_);

        if (!realtimeQueue_.empty())
        {
            Message msg { MessageCategory::kRealtime, realtimeQueue_.front(), {} };
            realtimeQueue_.pop();
            return msg;
        }

        if (!sysExQueue_.empty())
        {
            Message msg { MessageCategory::kSysEx, {}, sysExQueue_.front() };
            sysExQueue_.pop();
            return msg;
        }

        return std::nullopt;
    }

    bool MidiOutboundQueue::isEmpty() const noexcept
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return realtimeQueue_.empty() && sysExQueue_.empty();
    }
}
