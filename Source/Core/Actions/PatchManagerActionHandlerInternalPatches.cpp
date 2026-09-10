#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Actions/PatchManagerActionHandlerInternal.h"

#include "Core/Init/InitTemplateFooter.h"
#include "Core/Init/PatchInitService.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    namespace
    {
        void assignInitPatchNameSentinel(PatchModel* patchModel)
        {
            if (patchModel == nullptr) return;

            using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
            patchModel->setName(kInitPatchName);
        }
    }

    bool PatchManagerActionHandler::tryHandleInitPasteStoreActions(const juce::String& propertyId,
                                                                   const DeviceMemoryLimits& limits)
    {
        using namespace PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

        if (propertyId == kInitPatch)
        {
            handleInternalPatchInit();
            return true;
        }

        if (propertyId == kPastePatch)
        {
            handleInternalPatchPaste(limits);
            return true;
        }

        if (propertyId == kStorePatch)
        {
            handleInternalPatchStore(limits);
            return true;
        }

        return false;
    }

    bool PatchManagerActionHandler::tryHandleComputerFileActions(const juce::String& propertyId,
                                                                 const DeviceMemoryLimits& limits)
    {
        using namespace PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets;

        if (propertyId == kOpenPatchFolder)
        {
            handleOpenPatchFolder(limits);
            return true;
        }

        if (propertyId == kSavePatchAs)
        {
            handleSavePatchAs();
            return true;
        }

        if (propertyId == kSavePatchFile)
        {
            handleSavePatchFile();
            return true;
        }

        if (propertyId == kSelectPatchFile)
        {
            scheduleComputerSelectPatchLoad();
            return true;
        }

        return false;
    }

    bool PatchManagerActionHandler::tryHandleBankTransferActions(const juce::String& propertyId,
                                                                 const DeviceMemoryLimits& limits)
    {
        using namespace PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;

        if (propertyId == kExportBank)
        {
            handleBankExport(limits);
            return true;
        }

        if (propertyId == kImportBank)
        {
            handleBankImport(limits);
            return true;
        }

        if (propertyId == kCopyBank)
        {
            handleBankCopy(limits);
            return true;
        }

        if (propertyId == kPasteBank)
        {
            handleBankPaste(limits);
            return true;
        }

        return false;
    }

    bool PatchManagerActionHandler::tryHandleInternalPatchNavigation(const juce::String& propertyId,
                                                                     const DeviceMemoryLimits& limits)
    {
        using namespace PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

        if (propertyId != kLoadPreviousPatch && propertyId != kLoadNextPatch)
            return false;

        if (! pendingInternalNavBaseline_.has_value())
            pendingInternalNavBaseline_ = captureInternalCoordinates(limits);

        const bool isNext = propertyId == kLoadNextPatch;

        const auto target = resolveInternalNavigationTarget(isNext, limits);
        applyPatchCoordinates(target, limits, false);
        if (limits.hasBankConcept())
        {
            apvts_.state.setProperty(
                PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
                target.bank,
                nullptr);
        }
        markPatchCoordinatesEstablished();
        setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kInternal);

        abandonPendingDeviceLoad();
        abandonPendingComputerSelectSettle();

        patchNavDebouncer_.schedule([this]() { settleInternalPatchNavigation(); });
        return true;
    }

    bool PatchManagerActionHandler::arePatchCoordinatesEstablished() const
    {
        return static_cast<bool>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
            false));
    }

    void PatchManagerActionHandler::markPatchCoordinatesEstablished()
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::StateProperties::kPatchCoordinatesEstablished,
            true,
            nullptr);
    }

    void PatchManagerActionHandler::setNavigationFocus(int focusOwner)
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
            focusOwner,
            nullptr);
    }

    void PatchManagerActionHandler::clearComputerNavigationFocusIfOwned()
    {
        // An empty Computer pick is not Internal navigation — only drop Computer focus.
        const int focus = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
            PluginIDs::PatchManagerSection::NavigationFocus::kDefault));
        if (focus == PluginIDs::PatchManagerSection::NavigationFocus::kComputer)
            setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kNone);
    }

    PatchCoordinates PatchManagerActionHandler::resolveInternalNavigationTarget(
        bool isNext,
        const DeviceMemoryLimits& limits) const
    {
        if (! arePatchCoordinatesEstablished())
            return { limits.minBankNumber(), limits.minPatchNumber() };

        const PatchCoordinates current { getCurrentBank(limits), getCurrentPatch(limits) };
        return limits.advancePatch(current, isNext ? 1 : -1);
    }

    void PatchManagerActionHandler::abandonPendingInternalNavSettle()
    {
        if (! pendingInternalNavBaseline_.has_value())
            return;

        const auto baseline = *pendingInternalNavBaseline_;
        pendingInternalNavBaseline_.reset();
        restoreInternalCoordinates(baseline, deviceMemoryLimits_());
        abandonPendingDeviceLoad();
    }

    void PatchManagerActionHandler::abandonPendingComputerSelectSettle()
    {
        if (! computerSelectDebouncer_.isPending())
            return;

        computerSelectDebouncer_.cancel();
        clearPendingCombinedScanLoadFooter();
        revertComputerPatchesSelectionIfNeeded(resolveComputerPatchesCancelRevertId());
    }

    void PatchManagerActionHandler::settleInternalPatchNavigation()
    {
        const auto limits = deviceMemoryLimits_();
        if (! pendingInternalNavBaseline_.has_value())
            return;

        const auto baseline = *pendingInternalNavBaseline_;

        if (! confirmPatchContextChange())
        {
            pendingInternalNavBaseline_.reset();
            restoreInternalCoordinates(baseline, limits);
            abandonPendingDeviceLoad();
            return;
        }

        pendingInternalNavBaseline_.reset();

        const PatchCoordinates finalCoords { getCurrentBank(limits), getCurrentPatch(limits) };
        applyPatchCoordinates(finalCoords, limits, true);
        beginPendingDeviceLoad(baseline);
        loadCurrentPatchFromDevice(limits);
    }

    bool PatchManagerActionHandler::tryHandleComputerPatchFileNavigation(const juce::String& propertyId,
                                                                         const DeviceMemoryLimits& limits)
    {
        using namespace PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets;
        juce::ignoreUnused(limits);

        if (propertyId != kLoadPreviousPatchFile && propertyId != kLoadNextPatchFile)
            return false;

        const bool isNext = propertyId == kLoadNextPatchFile;

        suppressComputerPatchesSelectLoad_ = true;
        const auto nextId = advanceComputerPatchesSelection(isNext);
        suppressComputerPatchesSelectLoad_ = false;

        if (! nextId.has_value())
            return true;

        scheduleComputerNavPatchLoad();
        return true;
    }

    void PatchManagerActionHandler::scheduleComputerSelectPatchLoad()
    {
        if (suppressComputerPatchesSelectLoad_)
            return;

        abandonPendingDeviceLoad();
        // Drop Internal settle if the shared button debouncer was armed for Internal nav.
        abandonPendingInternalNavSettle();
        patchNavDebouncer_.cancel();

        computerSelectDebouncer_.schedule([this]() { settleComputerPatchLoad(); });
    }

    void PatchManagerActionHandler::scheduleComputerNavPatchLoad()
    {
        abandonPendingDeviceLoad();
        abandonPendingInternalNavSettle();
        computerSelectDebouncer_.cancel();

        patchNavDebouncer_.schedule([this]() { settleComputerPatchLoad(); });
    }

    void PatchManagerActionHandler::settleComputerPatchLoad()
    {
        handleLoadSelectedPatchFile(deviceMemoryLimits_());
    }

    bool PatchManagerActionHandler::tryHandleBankButtonSelection(const juce::String& propertyId,
                                                                 const DeviceMemoryLimits& limits)
    {
        using namespace PluginIDs::PatchManagerSection;

        if (! limits.hasBankConcept())
        {
            if (parseBankButtonIndex(propertyId) >= 0)
                return true;

            return false;
        }

        const int bankIndex = parseBankButtonIndex(propertyId);
        if (bankIndex < 0)
            return false;

        if (isBankTransferBusy())
        {
            publishBankTransferFooter(
                PluginDisplayNames::PatchManagerSection::BankUtilityModule::kBankTransferBusyFooterMessage,
                "warning");
            return true;
        }

        const int clampedBank = juce::jlimit(limits.minBankNumber(), limits.maxBankNumber(), bankIndex);

        // Re-clicking the already-current bank must not reset to patch 00 or re-fire load.
        if (arePatchCoordinatesEstablished() && clampedBank == getCurrentBank(limits))
            return true;

        if (! confirmPatchContextChange())
            return true;

        const auto priorCoordinates = captureInternalCoordinates(limits);
        apvts_.state.setProperty(BankUtilityModule::StateProperties::kSelectedBank, clampedBank, nullptr);

        // Picking a bank lands on its first slot.
        const PatchCoordinates destination { clampedBank, limits.minPatchNumber() };
        applyPatchCoordinates(destination, limits, false);

        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->syncSelection(destination.bank, destination.patch, limits, true);

        markPatchCoordinatesEstablished();
        setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kInternal);
        beginPendingDeviceLoad(priorCoordinates);
        loadCurrentPatchFromDevice(limits);
        return true;
    }

    void PatchManagerActionHandler::publishPasteNothingFooter()
    {
        apvts_.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::ShortLabels::kNothingToPasteFooter),
            nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
    }

    void PatchManagerActionHandler::publishPasteFailedFooter(const juce::String& sourceLabel,
                                                             const juce::String& targetLabel)
    {
        apvts_.state.setProperty(
            "uiMessageText",
            PluginDisplayNames::ClipboardMessages::formatPasteFailed(sourceLabel, targetLabel),
            nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("error"), nullptr);
    }

    void PatchManagerActionHandler::publishPasteSuccessFooter(const juce::String& sourceLabel,
                                                              const juce::String& targetLabel)
    {
        apvts_.state.setProperty(
            "uiMessageText",
            PluginDisplayNames::ClipboardMessages::formatPatchPasted(sourceLabel, targetLabel),
            nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("info"), nullptr);
    }

    void PatchManagerActionHandler::applyPastedPatchToEditorAndSynth(const DeviceMemoryLimits& limits,
                                                                     int currentBank)
    {
        noteDevicePatchOrigin(currentBank, getCurrentPatch(limits));

        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);

        establishEditorialCheckpoint();

        if (hooks_.onPatchLoaded)
            hooks_.onPatchLoaded();

        if (midiManager_ != nullptr)
            midiManager_->sendFullPatchForAudition(patchModel_->data(),
                                                   static_cast<juce::uint8>(getCurrentPatch(limits)),
                                                   limits.hasBankConcept());
    }

    void PatchManagerActionHandler::handleInternalPatchInit()
    {
        if (patchInitService_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        const auto limits = deviceMemoryLimits_();

        if (! limits.isPasteStoreAllowed(getCurrentBank(limits)))
        {
            propagateRomBlockedFooter();
            return;
        }

        if (static_cast<bool>(apvts_.state.getProperty(
                PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties::kCompareActive,
                false)))
            return;

        if (! confirmPatchContextChange(true))
            return;

        abandonPendingDeviceLoad();
        clearLastDeviceDumpRawName();

        const auto result = patchInitService_->initFullPatch();
        assignInitPatchNameSentinel(patchModel_);

        noteDevicePatchOrigin(getCurrentBank(limits), getCurrentPatch(limits));

        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);
        establishEditorialCheckpoint();
        markPatchNotStoredInRam();

        if (hooks_.onPatchLoaded)
            hooks_.onPatchLoaded();

        InitTemplateFooter::propagateMessage(apvts_, result);

        if (patchModel_ == nullptr || midiManager_ == nullptr)
            return;

        apvtsPatchMapper_->apvtsToBuffer();
        midiManager_->sendFullPatchForAudition(patchModel_->data(),
                                               static_cast<juce::uint8>(getCurrentPatch(limits)),
                                               limits.hasBankConcept());
    }

    void PatchManagerActionHandler::handleInternalPatchPaste(const DeviceMemoryLimits& limits)
    {
        const int currentBank = getCurrentBank(limits);

        if (! limits.isPasteStoreAllowed(currentBank))
        {
            propagateRomBlockedFooter();
            return;
        }

        if (clipboardService_ == nullptr || ! clipboardService_->canPasteFullPatch())
        {
            publishPasteNothingFooter();
            return;
        }

        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        // PASTE entry: silent like module C/P — no risk modal and no Mutator history gate.
        abandonPendingDeviceLoad();
        clearLastDeviceDumpRawName();

        const auto sourceLabel = clipboardService_->getFullPatchSourceLabel().isNotEmpty()
            ? clipboardService_->getFullPatchSourceLabel()
            : juce::String("Patch");
        const auto targetLabel = PluginDisplayNames::ClipboardMessages::formatInternalPatchLocation(
            limits.hasBankConcept(), currentBank, getCurrentPatch(limits));

        apvtsPatchMapper_->apvtsToBuffer();
        if (! clipboardService_->pasteFullPatch(*patchModel_))
        {
            publishPasteFailedFooter(sourceLabel, targetLabel);
            return;
        }

        if (hooks_.disarmClipboardFeedback)
            hooks_.disarmClipboardFeedback();

        applyPastedPatchToEditorAndSynth(limits, currentBank);
        publishPasteSuccessFooter(sourceLabel, targetLabel);
    }

    void PatchManagerActionHandler::handleInternalPatchStore(const DeviceMemoryLimits& limits)
    {
        const int currentBank = getCurrentBank(limits);

        if (! limits.isPasteStoreAllowed(currentBank))
        {
            propagateRomBlockedFooter();
            return;
        }

        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr || midiManager_ == nullptr)
            return;

        if (tryDeferStoreForInitNameRequired())
            return;

        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->syncSelection(currentBank, getCurrentPatch(limits), limits, true);

        // STORE writes to a concrete slot, so the coordinates are no longer a guess.
        markPatchCoordinatesEstablished();
        setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kInternal);

        apvtsPatchMapper_->apvtsToBuffer();
        if (patchNameSyncer_ != nullptr)
            patchNameSyncer_->apvtsToBuffer();

        midiManager_->sendPatch(static_cast<juce::uint8>(getCurrentPatch(limits)), patchModel_->data());
        rememberOverlayName(currentBank, getCurrentPatch(limits), patchModel_->getName());

        if (midiManager_->isEditorOutboundAllowed())
            captureCleanSnapshot();
    }

} // namespace Core
