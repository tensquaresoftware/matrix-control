// Matrix-6/6R Master-parameter fallback when Universal Device Inquiry is ignored.

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/Matrix6MasterFallbackPolicy.h"
#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

bool MidiManager::tryStartMasterOnlyPresenceProofIfEligible()
{
    const bool detected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const auto detectedType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));

    if (! Core::shouldSendMasterOnlyPresenceProof(
            detected, detectedType, forceMasterPullOnNextInquirySuccess_))
        return false;

    performMatrix6MasterPresenceProof();
    return true;
}

void MidiManager::performMatrix6MasterPresenceProof()
{
    cancelPendingSysExRequest();

    if (! isDeviceDumpAvailable() || midiSender == nullptr || ! midiSender->isOutputAvailable())
    {
        clearLastInquiryPortPair();
        sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
        updateDeviceStatus(false);
        updateErrorState("MIDI ports not available for Matrix-6/6R Master presence", "Connection");
        return;
    }

    // Inquiry settles on outbound idle; Master-only must not slam a busy wire —
    // next presence tick retries (avoid false unresponsive during editor SysEx).
    if (! isOutboundQueueIdle())
    {
        MidiLogger::getInstance().logInfo(
            "Matrix-6/6R Master presence deferred: outbound MIDI queue still busy");
        return;
    }

    asyncSysExCaptureActive_.store(true, std::memory_order_release);

    const auto token = asyncRequestToken_.load(std::memory_order_acquire);
    MidiLogger::getInstance().logInfo(
        "Matrix-6/6R presence — Master proof (skip Device Inquiry)");
    sendArmedMatrix6MasterFallback(token);
}

void MidiManager::finishMatrix6MasterFallbackFailure(std::uint64_t token,
                                                     const juce::String& errorMessage,
                                                     const juce::String& errorType)
{
    const bool detected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const auto detectedType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));

    if (Core::shouldSoftAbortOnMatrix6MasterFallbackFailure(
            detected, detectedType, forceMasterPullOnNextInquirySuccess_))
    {
        softAbortDeviceInquiryUnresponsive(token);
        return;
    }

    finishAsyncDeviceInquiryFailure(token, errorMessage, errorType);
}

void MidiManager::sendArmedMatrix6MasterFallback(std::uint64_t inquiryToken)
{
    // Invalidate late Inquiry identity callbacks that still hold the Inquiry token.
    auto expected = inquiryToken;
    if (! asyncRequestToken_.compare_exchange_strong(expected,
                                                    inquiryToken + 1,
                                                    std::memory_order_acq_rel))
        return;

    const std::uint64_t token = inquiryToken + 1;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    if (! isDeviceDumpAvailable() || midiSender == nullptr || ! midiSender->isOutputAvailable()
        || sysExEncoder == nullptr)
    {
        finishMatrix6MasterFallbackFailure(token,
                                           "MIDI ports unavailable for Matrix-6/6R Master fallback",
                                           "Connection");
        return;
    }

    try
    {
        if (! armAsyncMatrix6MasterFallbackCapture(token))
            return;

        const auto request = sysExEncoder->encodeRequestMessage(
            SysExConstants::RequestType::kRequestMasterParameters, 0);
        sendSysExWithDelay(request, "Matrix-6/6R Master fallback: request Master parameters");
        scheduleMatrix6MasterFallbackTimeout(token);
    }
    catch (const MidiConnectionException& e)
    {
        finishMatrix6MasterFallbackFailure(token, e.getMessage(), "Connection");
    }
    catch (const std::exception& e)
    {
        finishMatrix6MasterFallbackFailure(token, e.what(), "SysEx");
    }
}

bool MidiManager::armAsyncMatrix6MasterFallbackCapture(std::uint64_t token)
{
    if (! Core::isMatrix6MasterFallbackTokenCurrent(
            token, asyncRequestToken_.load(std::memory_order_acquire)))
        return false;

    if (midiReceiver == nullptr)
    {
        finishMatrix6MasterFallbackFailure(token,
                                           "MIDI input unavailable for Matrix-6/6R Master fallback",
                                           "Connection");
        return false;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    midiReceiver->armOneShotSysExCapture(
        [weakThis, token](const juce::MemoryBlock& response)
        {
            // Claim immediately so a queued Master-fallback timeout cannot finishFailure
            // after a valid dump was accepted (filter already matched this frame).
            auto* selfSync = weakThis.get();
            if (selfSync == nullptr)
                return;

            auto expected = token;
            if (! selfSync->asyncRequestToken_.compare_exchange_strong(
                    expected, token + 1, std::memory_order_acq_rel))
                return;

            const std::uint64_t claimed = token + 1;
            juce::MessageManager::callAsync(
                [weakThis, claimed, response]
                {
                    if (auto* self = weakThis.get())
                        self->handleAsyncMatrix6MasterFallbackResponse(claimed, response);
                });
        },
        [](const juce::MemoryBlock& sysex)
        {
            return Core::isMatrix6MasterParameterDump(sysex);
        });

    return true;
}

void MidiManager::handleAsyncMatrix6MasterFallbackResponse(std::uint64_t token,
                                                           const juce::MemoryBlock& response)
{
    if (! Core::isMatrix6MasterFallbackTokenCurrent(
            token, asyncRequestToken_.load(std::memory_order_acquire)))
        return;

    if (! Core::isMatrix6MasterParameterDump(response))
    {
        // Capture filter matched, then frame failed a defensive re-check — re-arm and
        // reschedule timeout under the claimed token (old timeout was invalidated).
        if (! armAsyncMatrix6MasterFallbackCapture(token))
            return;
        scheduleMatrix6MasterFallbackTimeout(token);
        return;
    }

    MidiLogger::getInstance().logSysExReceived(response, "Matrix-6/6R Master fallback response");

    // Family proof only — empty version, shared Matrix-6 display (no distinct 6R from Master).
    DeviceIdInfo unusedInfo {};
    finishAsyncDeviceInquirySuccess(token,
                                    unusedInfo,
                                    Core::matrix6MasterFallbackDeviceType(),
                                    Core::matrix6MasterFallbackPreservesStoredEpromType());
}

void MidiManager::onMatrix6MasterFallbackTimeout(std::uint64_t token)
{
    if (! Core::isMatrix6MasterFallbackTokenCurrent(
            token, asyncRequestToken_.load(std::memory_order_acquire)))
        return;

    MidiLogger::getInstance().logWarning(
        "Timeout waiting for Matrix-6/6R Master fallback ("
        + juce::String(SysExConstants::kDefaultTimeoutMs) + "ms)");

    finishMatrix6MasterFallbackFailure(token,
                                       Core::matrix6MasterFallbackTimeoutFailureMessage(),
                                       "Timeout");
}

void MidiManager::scheduleMatrix6MasterFallbackTimeout(std::uint64_t token)
{
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(
        SysExConstants::kDefaultTimeoutMs,
        [weakThis, token]
        {
            if (auto* self = weakThis.get())
                self->onMatrix6MasterFallbackTimeout(token);
        });
}
