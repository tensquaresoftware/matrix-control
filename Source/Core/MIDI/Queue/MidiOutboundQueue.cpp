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

    void MidiOutboundQueue::enqueueRealtimeFront(juce::MidiMessage message)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            // std::queue has no push_front — rebuild with new message first.
            std::queue<juce::MidiMessage> rebuilt;
            rebuilt.push(std::move(message));
            while (! realtimeQueue_.empty())
            {
                rebuilt.push(std::move(realtimeQueue_.front()));
                realtimeQueue_.pop();
            }
            realtimeQueue_ = std::move(rebuilt);
        }

        wakeConsumerIfNeeded();
    }

    void MidiOutboundQueue::enqueueRealtimeFrontMany(std::vector<juce::MidiMessage> messages)
    {
        if (messages.empty())
            return;

        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            // std::queue has no push_front — rebuild with the burst first (dequeue order preserved).
            std::queue<juce::MidiMessage> rebuilt;
            for (auto& message : messages)
                rebuilt.push(std::move(message));
            while (! realtimeQueue_.empty())
            {
                rebuilt.push(std::move(realtimeQueue_.front()));
                realtimeQueue_.pop();
            }
            realtimeQueue_ = std::move(rebuilt);
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

    std::optional<juce::MidiMessage> MidiOutboundQueue::tryDequeueRealtime()
    {
        std::lock_guard<std::mutex> lock(queueMutex_);

        if (realtimeQueue_.empty())
            return std::nullopt;

        auto message = realtimeQueue_.front();
        realtimeQueue_.pop();
        return message;
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

    size_t MidiOutboundQueue::realtimeDepth() const noexcept
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return realtimeQueue_.size();
    }

    size_t MidiOutboundQueue::sysExDepth() const noexcept
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return sysExQueue_.size();
    }
}
