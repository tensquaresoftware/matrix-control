// Extracted from MidiManager.cpp for modular maintenance.
// Outbound queue consumer thread and SysEx pacing.

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace
{
    Core::MidiActivityTracker::Path pathForOutboundMessage(
        const Core::MidiOutboundQueue::Message& msg) noexcept
    {
        if (msg.category == Core::MidiOutboundQueue::MessageCategory::kSysEx)
            return Core::MidiActivityTracker::Path::kEditor;

        if (msg.midiMessage.isProgramChange())
            return Core::MidiActivityTracker::Path::kEditor;

        return Core::MidiActivityTracker::Path::kInstrument;
    }
}

void MidiManager::wakeConsumer() noexcept
{
    notify();
}

bool MidiManager::canSendSysExNow() const noexcept
{
    const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
    return sysExDelay_.millisUntilNextAllowed(nowMs) == 0;
}

bool MidiManager::isOutboundQueueIdle() const noexcept
{
    return outboundQueue_.isEmpty() && ! hasPendingSysEx_.load(std::memory_order_acquire);
}

bool MidiManager::hasOutboundIdleTimedOut(const OutboundIdlePollArgs& args) const noexcept
{
    return juce::Time::getMillisecondCounter() - args.idleStartMs
           >= static_cast<juce::uint32>(args.outboundIdleTimeoutMs);
}

void MidiManager::sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    sysExDelay_.waitUntilReady();
    midiSender->sendSysEx(sysExMessage);
    // Direct SysEx (Inquiry, Master fallback, async dumps) bypasses the outbound queue.
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kEditor);
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
    MidiLogger::getInstance().logSysExSent(sysExMessage, description);
    sysExDelay_.recordSysExSent(static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes()));
}

void MidiManager::sendQueuedSysEx(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    // MidiSender::sendSysEx already logs once — do not double-log (and flush) on the consumer.
    juce::ignoreUnused(description);
    midiSender->sendSysEx(sysExMessage);
    sysExDelay_.recordSysExSent(static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes()));
}

void MidiManager::dispatchRealtimeMessage(const Core::MidiOutboundQueue::Message& msg)
{
    // Program Change is editor outbound — drop if freeze/gate flipped after enqueue.
    // Panic CC / notes remain ungated (instrument path).
    if (msg.midiMessage.isProgramChange() && ! isEditorOutboundAllowed())
        return;

    midiSender->sendMidiMessage(msg.midiMessage);
    activityTracker_.notifyActivity(pathForOutboundMessage(msg));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
}

void MidiManager::notifyOutboundActivity(const Core::MidiOutboundQueue::Message& msg)
{
    activityTracker_.notifyActivity(pathForOutboundMessage(msg));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
}

bool MidiManager::isEditorSysExAllowed(const juce::MemoryBlock& sysEx) const
{
    const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const bool deviceMidiUnresponsive = static_cast<bool>(
        apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::maySendEditorSysEx(deviceDetected, deviceType, sysEx, deviceMidiUnresponsive);
}

bool MidiManager::handleOutboundMessage(Core::MidiOutboundQueue::Message& msg)
{
    if (msg.category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
    {
        dispatchRealtimeMessage(msg);
        return true;
    }

    if (msg.sysExData.getSize() == 0)
        return true;

    if (! isEditorSysExAllowed(msg.sysExData))
        return true;

    if (pendingSysEx_.has_value())
    {
        outboundQueue_.enqueueSysEx(std::move(msg.sysExData));
        return false;
    }

    if (canSendSysExNow())
    {
        sendQueuedSysEx(msg.sysExData, "QUEUED");
        notifyOutboundActivity(msg);
        return true;
    }

    pendingSysEx_ = std::move(msg);
    return false;
}

bool MidiManager::tryDispatchPendingSysEx()
{
    if (! pendingSysEx_.has_value() || ! canSendSysExNow())
        return false;

    try
    {
        if (! isEditorSysExAllowed(pendingSysEx_->sysExData))
        {
            pendingSysEx_.reset();
            return false;
        }

        sendQueuedSysEx(pendingSysEx_->sysExData, "QUEUED");
        notifyOutboundActivity(*pendingSysEx_);
        pendingSysEx_.reset();
        return true;
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "MIDI");
    }

    return false;
}

void MidiManager::drainQueuedRealtimeOnly()
{
    while (auto message = outboundQueue_.tryDequeueRealtime())
        dispatchRealtimeMessage({ Core::MidiOutboundQueue::MessageCategory::kRealtime,
                                  std::move(*message),
                                  {} });
}

bool MidiManager::processOutboundQueue()
{
    bool didWork = false;

    while (true)
    {
        auto msg = outboundQueue_.dequeue();
        if (! msg.has_value())
            break;

        didWork = true;

        try
        {
            if (! handleOutboundMessage(*msg))
            {
                // Pending SysEx parked — flush any realtime already behind it before gate sleep.
                drainQueuedRealtimeOnly();
                break;
            }
        }
        catch (const MidiConnectionException& e)
        {
            updateErrorState(e.getMessage(), "Connection");
            break;
        }
        catch (const std::exception& e)
        {
            updateErrorState(e.what(), "MIDI");
            break;
        }
    }

    if (tryDispatchPendingSysEx())
        didWork = true;

    hasPendingSysEx_.store(pendingSysEx_.has_value(), std::memory_order_release);
    return didWork;
}

void MidiManager::run()
{
    while (! threadShouldExit())
    {
        if (! midiSender->isOutputAvailable())
        {
            wait(1);
            continue;
        }

        if (processOutboundQueue())
            continue;

        int sleepMs = 1;
        if (pendingSysEx_.has_value())
        {
            const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
            sleepMs = juce::jmax(1, sysExDelay_.millisUntilNextAllowed(nowMs));
        }

        wait(sleepMs);
    }
}
