#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Actions/PatchManagerActionHandlerInternal.h"

#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiRequestTiming.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

#include <cstring>

namespace MutatorMessages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

namespace Core
{

    PatchManagerActionHandler::InternalCoordinatesSnapshot
    PatchManagerActionHandler::captureInternalCoordinates(const DeviceMemoryLimits& limits) const
    {
        InternalCoordinatesSnapshot snapshot;
        snapshot.bank = getCurrentBank(limits);
        snapshot.patch = getCurrentPatch(limits);
        snapshot.selectedBank = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
            snapshot.bank));
        snapshot.coordinatesEstablished = arePatchCoordinatesEstablished();
        snapshot.navigationFocus = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
            PluginIDs::PatchManagerSection::NavigationFocus::kDefault));
        return snapshot;
    }

    void PatchManagerActionHandler::restoreInternalCoordinates(const InternalCoordinatesSnapshot& snapshot,
                                                               const DeviceMemoryLimits& limits)
    {
        applyPatchCoordinates({ snapshot.bank, snapshot.patch }, limits);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
            snapshot.selectedBank,
            nullptr);
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
            snapshot.coordinatesEstablished,
            nullptr);
        setNavigationFocus(snapshot.navigationFocus);
    }

    void PatchManagerActionHandler::beginPendingDeviceLoad(const InternalCoordinatesSnapshot& priorCoordinates)
    {
        if (midiManager_ != nullptr)
            midiManager_->cancelPendingSysExRequest();

        PendingDeviceLoad pending;
        pending.generation = ++deviceLoadGeneration_;
        pending.priorCoordinates = priorCoordinates;

        if (patchModel_ != nullptr)
        {
            if (apvtsPatchMapper_ != nullptr)
                apvtsPatchMapper_->apvtsToBuffer();
            if (patchNameSyncer_ != nullptr)
                patchNameSyncer_->apvtsToBuffer();

            std::memcpy(pending.bufferAtRequest.data(),
                        patchModel_->data(),
                        PatchModel::kBufferSize);
        }

        pendingDeviceLoad_ = pending;
    }

    void PatchManagerActionHandler::clearPendingDeviceLoad()
    {
        pendingDeviceLoad_.reset();
    }

    void PatchManagerActionHandler::abandonPendingDeviceLoad()
    {
        if (midiManager_ != nullptr)
            midiManager_->cancelPendingSysExRequest();

        clearPendingDeviceLoad();
    }

    void PatchManagerActionHandler::failPendingDeviceLoad(const DeviceMemoryLimits& limits,
                                                          const juce::String& footerMessage)
    {
        // KEEP_DISPLAYED: a late/failed dump must not yank NumberBoxes backward while the user
        // is still stepping Internal Prev/Next (or after bank/NumberBox already advanced).
        juce::ignoreUnused(limits);
        clearPendingDeviceLoad();
        apvts_.state.setProperty("uiMessageText", footerMessage, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
    }

    bool PatchManagerActionHandler::isDeviceDumpAvailable() const
    {
        if (hooks_.isDeviceDumpAvailable)
            return hooks_.isDeviceDumpAvailable();

        return midiManager_ != nullptr && midiManager_->isDeviceDumpAvailable();
    }

    void PatchManagerActionHandler::requestDeviceDump(juce::uint8 patchNumber,
                                                      ActionExecutionHooks::DeviceDumpCallback onResult)
    {
        if (hooks_.requestDeviceDump)
        {
            hooks_.requestDeviceDump(patchNumber, std::move(onResult));
            return;
        }

        if (midiManager_ == nullptr)
        {
            if (onResult)
                onResult({});
            return;
        }

        const int profileDelayMs = midiManager_->getRequiredSysExDelayMs();
        midiManager_->requestSinglePatchAsync(
            patchNumber,
            std::move(onResult),
            MidiRequestTiming::deviceSettleMs(profileDelayMs),
            MidiRequestTiming::outboundIdleTimeoutMs(profileDelayMs));
    }

    void PatchManagerActionHandler::loadCurrentPatchFromDevice(const DeviceMemoryLimits& limits,
                                                                 const InternalCoordinatesSnapshot& priorCoordinates)
    {
        beginPendingDeviceLoad(priorCoordinates);
        loadCurrentPatchFromDevice(limits);
    }

    void PatchManagerActionHandler::applySuccessfulDeviceDump(const DeviceMemoryLimits& limits,
                                                              int bank,
                                                              int patch,
                                                              const std::vector<juce::uint8>& dump)
    {
        patchModel_->loadFrom(dump.data());
        patchModel_->normalizeNameEncoding();
        lastDeviceDumpRawName_ = patchModel_->getName();
        lastDeviceDumpBank_ = bank;
        lastDeviceDumpPatch_ = patch;
        hasLastDeviceDumpRawName_ = true;
        applyResolvedPatchName(*patchModel_,
                               PatchCoordinates { bank, patch },
                               limits,
                               PatchNameResolvePurpose::kDisplay);

        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);
        establishEditorialCheckpoint();
        clearPendingDeviceLoad();

        noteDevicePatchOrigin(bank, patch);

        if (hooks_.onPatchLoaded)
            hooks_.onPatchLoaded();
    }

    void PatchManagerActionHandler::handlePendingDeviceDumpResult(const PendingDeviceDumpResultContext& context,
                                                                  std::vector<juce::uint8> dump)
    {
        if (! pendingDeviceLoad_.has_value() || pendingDeviceLoad_->generation != context.generation)
            return;

        if (dump.size() != SysExConstants::kPatchPackedDataSize)
        {
            failPendingDeviceLoad(context.limits, context.failFooter);
            return;
        }

        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr)
        {
            failPendingDeviceLoad(context.limits, context.failFooter);
            return;
        }

        if (apvtsPatchMapper_ != nullptr)
            apvtsPatchMapper_->apvtsToBuffer();
        if (patchNameSyncer_ != nullptr)
            patchNameSyncer_->apvtsToBuffer();

        if (std::memcmp(patchModel_->data(),
                        pendingDeviceLoad_->bufferAtRequest.data(),
                        PatchModel::kBufferSize)
            != 0)
        {
            failPendingDeviceLoad(context.limits, juce::String(MutatorMessages::kDeviceDumpAbortedEditedFooter));
            return;
        }

        applySuccessfulDeviceDump(context.limits, context.bank, context.patch, dump);
    }

    void PatchManagerActionHandler::loadCurrentPatchFromDevice(const DeviceMemoryLimits& limits)
    {
        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        if (! pendingDeviceLoad_.has_value())
            beginPendingDeviceLoad(captureInternalCoordinates(limits));

        PendingDeviceDumpResultContext context;
        context.generation = pendingDeviceLoad_->generation;
        context.failFooter = juce::String(MutatorMessages::kDeviceDumpFailedFooter);
        context.limits = limits;

        if (! isDeviceDumpAvailable())
        {
            failPendingDeviceLoad(limits, context.failFooter);
            return;
        }

        context.bank = getCurrentBank(limits);
        context.patch = getCurrentPatch(limits);

        requestDeviceDump(
            static_cast<juce::uint8>(context.patch),
            [this, context](std::vector<juce::uint8> dump) mutable
            {
                handlePendingDeviceDumpResult(context, std::move(dump));
            });
    }

} // namespace Core
