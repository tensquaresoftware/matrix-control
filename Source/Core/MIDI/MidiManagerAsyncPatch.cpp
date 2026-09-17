// Extracted from MidiManager.cpp for modular maintenance.
// Sync and async single-patch / master SysEx request paths.

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"

void MidiManager::cancelPendingSysExRequest()
{
    const auto token = asyncRequestToken_.fetch_add(1, std::memory_order_acq_rel) + 1;
    juce::ignoreUnused(token);

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    auto callback = std::move(pendingAsyncCallback_);
    pendingAsyncCallback_ = nullptr;
    asyncSysExCaptureActive_.store(false, std::memory_order_release);

    // Same contract as finishAsyncPackedPatch: invoke after clearing capture state.
    // Callbacks must not throw (neither path catches). Empty to Master connect-pull means
    // abort, not a sticky failure footer (suppress while invoking).
    suppressMasterPullFailureFooterForEmptyResult_ = true;
    if (callback)
        callback({});
    suppressMasterPullFailureFooterForEmptyResult_ = false;
}

void MidiManager::finishAsyncPackedPatch(std::uint64_t token, std::vector<juce::uint8> packed)
{
    // First finisher for this token wins (success or timeout); invalidates the other.
    auto expected = token;
    if (! asyncRequestToken_.compare_exchange_strong(expected, token + 1, std::memory_order_acq_rel))
        return;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    auto callback = std::move(pendingAsyncCallback_);
    pendingAsyncCallback_ = nullptr;
    asyncSysExCaptureActive_.store(false, std::memory_order_release);

    if (callback)
        callback(std::move(packed));
}

std::vector<juce::uint8> MidiManager::decodePackedPatchResponse(const juce::MemoryBlock& response,
                                                                const juce::String& requestDescription)
{
    if (response.getSize() == 0)
        return {};

    MidiLogger::getInstance().logSysExReceived(response, requestDescription + " response");

    std::vector<juce::uint8> packedData(SysExConstants::kPatchPackedDataSize);
    if (sysExDecoder->decodePatchSysEx(response, packedData.data()))
        return packedData;

    updateErrorState("Failed to decode " + requestDescription + " response", "SysEx");
    return {};
}

std::vector<juce::uint8> MidiManager::tryDecodeAsyncPatchResponse(const juce::MemoryBlock& response)
{
    if (response.getSize() == 0 || sysExParser == nullptr || sysExDecoder == nullptr)
        return {};

    // Ignore non-patch SysEx (Device ID, Master, noise) without failing the pending request.
    const auto validation = sysExParser->validateSysEx(response);
    if (! validation.isValid || validation.messageType != SysExParser::MessageType::kPatch)
        return {};

    std::vector<juce::uint8> packedData(SysExConstants::kPatchPackedDataSize);
    if (! sysExDecoder->decodePatchSysEx(response, packedData.data()))
        return {};

    MidiLogger::getInstance().logSysExReceived(response, "single patch response");
    return packedData;
}

void MidiManager::armAsyncSinglePatchCapture(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (midiReceiver == nullptr)
    {
        finishAsyncPackedPatch(token, {});
        return;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    midiReceiver->armOneShotSysExCapture(
        [weakThis, token](const juce::MemoryBlock& response)
        {
            // MIDI input thread: marshal decode + callback to the message thread.
            juce::MessageManager::callAsync(
                [weakThis, token, response]
                {
                    if (auto* self = weakThis.get())
                    {
                        if (token != self->asyncRequestToken_.load(std::memory_order_acquire))
                            return;

                        auto packed = self->tryDecodeAsyncPatchResponse(response);
                        if (! packed.empty())
                        {
                            self->finishAsyncPackedPatch(token, std::move(packed));
                            return;
                        }

                        // Parasitic / non-patch SysEx consumed the one-shot — keep listening.
                        self->armAsyncSinglePatchCapture(token);
                    }
                });
        });
}

void MidiManager::scheduleAsyncPatchTimeout(std::uint64_t token)
{
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(
        SysExConstants::kDefaultTimeoutMs,
        [weakThis, token]
        {
            if (auto* self = weakThis.get())
            {
                if (token != self->asyncRequestToken_.load(std::memory_order_acquire))
                    return;

                MidiLogger::getInstance().logWarning(
                    "Timeout waiting for SysEx response ("
                    + juce::String(SysExConstants::kDefaultTimeoutMs) + "ms)");
                self->updateErrorState("Timeout waiting for single patch response", "Timeout");
                self->finishAsyncPackedPatch(token, {});
            }
        });
}

void MidiManager::sendArmedSinglePatchRequest(juce::uint8 patchNumber, std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (! isEditorOutboundAllowed()
        || midiReceiver == nullptr
        || midiSender == nullptr
        || ! midiSender->isOutputAvailable())
    {
        finishAsyncPackedPatch(token, {});
        return;
    }

    try
    {
        auto requestMessage = sysExEncoder->encodeRequestMessage(
            SysExConstants::RequestType::kRequestSinglePatch, patchNumber);

        armAsyncSinglePatchCapture(token);
        sendSysExWithDelay(requestMessage, "single patch request");
        scheduleAsyncPatchTimeout(token);
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
        finishAsyncPackedPatch(token, {});
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
        finishAsyncPackedPatch(token, {});
    }
}

void MidiManager::requestSinglePatchAsync(juce::uint8 patchNumber,
                                          PackedPatchCallback callback,
                                          int settleMs,
                                          int outboundIdleTimeoutMs)
{
    if (! isEditorOutboundAllowed())
    {
        if (callback)
            callback({});
        return;
    }

    cancelPendingSysExRequest();

    const auto token = asyncRequestToken_.load(std::memory_order_acquire);
    pendingAsyncCallback_ = std::move(callback);
    asyncSysExCaptureActive_.store(true, std::memory_order_release);

    wakeConsumer();
    pollOutboundIdleThenRequest(patchNumber,
                                { token,
                                  juce::jmax(0, settleMs),
                                  juce::Time::getMillisecondCounter(),
                                  juce::jmax(0, outboundIdleTimeoutMs) });
}

void MidiManager::scheduleOrSendArmedPatchRequest(juce::uint8 patchNumber,
                                                  const OutboundIdlePollArgs& args)
{
    if (args.settleMs <= 0)
    {
        sendArmedSinglePatchRequest(patchNumber, args.token);
        return;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(args.settleMs,
                                [weakThis, patchNumber, token = args.token]
                                {
                                    if (auto* self = weakThis.get())
                                        self->sendArmedSinglePatchRequest(patchNumber, token);
                                });
}

void MidiManager::pollOutboundIdleThenRequest(juce::uint8 patchNumber, OutboundIdlePollArgs args)
{
    if (args.token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (isOutboundQueueIdle())
    {
        scheduleOrSendArmedPatchRequest(patchNumber, args);
        return;
    }

    if (hasOutboundIdleTimedOut(args))
    {
        MidiLogger::getInstance().logWarning("Timeout waiting for outbound MIDI queue to go idle");
        updateErrorState("Timeout waiting for outbound MIDI queue to go idle", "Timeout");
        finishAsyncPackedPatch(args.token, {});
        return;
    }

    wakeConsumer();
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(1,
                                [weakThis, patchNumber, args]
                                {
                                    if (auto* self = weakThis.get())
                                        self->pollOutboundIdleThenRequest(patchNumber, args);
                                });
}

std::vector<juce::uint8> MidiManager::decodeSysExPackedData(juce::uint8 requestType,
                                                            const juce::MemoryBlock& response,
                                                            size_t expectedPackedSize,
                                                            const juce::String& requestDescription)
{
    std::vector<juce::uint8> packedData(expectedPackedSize);
    bool decodeSuccess = false;

    if (requestType == SysExConstants::RequestType::kRequestEditBuffer
        || requestType == SysExConstants::RequestType::kRequestSinglePatch)
    {
        decodeSuccess = sysExDecoder->decodePatchSysEx(response, packedData.data());
    }
    else if (requestType == SysExConstants::RequestType::kRequestMasterParameters)
    {
        decodeSuccess = sysExDecoder->decodeMasterSysEx(response, packedData.data());
    }

    if (decodeSuccess)
        return packedData;

    updateErrorState("Failed to decode " + requestDescription + " response", "SysEx");
    return {};
}

std::vector<juce::uint8> MidiManager::requestSysExData(juce::uint8 requestType,
                                                       size_t expectedPackedSize,
                                                       const juce::String& requestDescription,
                                                       juce::uint8 patchNumber)
{
    if (! isEditorOutboundAllowed())
        return {};

    try
    {
        auto requestMessage = sysExEncoder->encodeRequestMessage(requestType, patchNumber);
        sendSysExWithDelay(requestMessage, requestDescription + " request");

        auto response = midiReceiver->waitForSysExResponse(SysExConstants::kDefaultTimeoutMs);
        if (response.getSize() == 0)
        {
            updateErrorState("Timeout waiting for " + requestDescription + " response", "Timeout");
            return {};
        }

        MidiLogger::getInstance().logSysExReceived(response, requestDescription + " response");
        return decodeSysExPackedData(requestType, response, expectedPackedSize, requestDescription);
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
        return {};
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
        return {};
    }
}
