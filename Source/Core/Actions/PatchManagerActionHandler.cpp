#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Actions/PatchManagerActionHandlerInternal.h"

#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/Matrix1000FactoryPatchNames.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchMutator/PatchLoadContext.h"
#include "Core/Services/PatchNameDisplayMode.h"
#include "Core/Services/PatchNameOverlayStore.h"
#include "Core/Services/PatchNameResolver.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace MutatorMessages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

namespace Core
{

    PatchManagerActionHandler::PatchManagerActionHandler(Dependencies dependencies, ActionExecutionHooks hooks)
        : apvts_(dependencies.apvts)
        , deviceMemoryLimits_(std::move(dependencies.deviceMemoryLimits))
        , patchModel_(dependencies.patchModel)
        , apvtsPatchMapper_(dependencies.apvtsPatchMapper)
        , clipboardService_(dependencies.clipboardService)
        , patchInitService_(dependencies.patchInitService)
        , patchSelectionMidiSync_(dependencies.patchSelectionMidiSync)
        , midiManager_(dependencies.midiManager)
        , patchFileService_(dependencies.patchFileService)
        , patchNameSyncer_(dependencies.patchNameSyncer)
        , dirtyPatchTracker_(dependencies.dirtyPatchTracker)
        , sysExEncoder_(dependencies.sysExEncoder)
        , pickFolder_(std::move(dependencies.pickFolder))
        , pickSaveFile_(std::move(dependencies.pickSaveFile))
        , pickNameReconciliation_(std::move(dependencies.pickNameReconciliation))
        , hooks_(std::move(hooks))
        , patchNavDebouncer_(dependencies.patchNavButtonDebounceMs)
        , computerSelectDebouncer_(dependencies.computerSelectDebounceMs)
    {
        loadPatchNameOverlayFromApvts();
    }

    void PatchManagerActionHandler::loadPatchNameOverlayFromApvts()
    {
        if (patchNameOverlayLoaded_)
            return;

        reloadPatchNameOverlayFromApvts();
    }

    void PatchManagerActionHandler::reloadPatchNameOverlayFromApvts()
    {
        using namespace PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;

        patchNameOverlay_.replaceFromValueTree(apvts_.state.getChildWithName(kPatchNameOverlay));
        patchNameOverlayLoaded_ = true;
    }

    void PatchManagerActionHandler::persistPatchNameOverlayToApvts()
    {
        using namespace PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;

        const auto existing = apvts_.state.getChildWithName(kPatchNameOverlay);
        if (existing.isValid())
            apvts_.state.removeChild(existing, nullptr);

        juce::ValueTree named { kPatchNameOverlay };
        const auto entries = patchNameOverlay_.toValueTree();
        for (int i = 0; i < entries.getNumChildren(); ++i)
            named.appendChild(entries.getChild(i).createCopy(), nullptr);

        apvts_.state.appendChild(named, nullptr);
    }

    void PatchManagerActionHandler::rememberOverlayName(int bank, int patch, const juce::String& name)
    {
        if (! PatchFileNameSanitizer::isUsablePatchName(name)
            || PatchFileNameSanitizer::isOberheimBankPlaceholderName(name))
            return;

        loadPatchNameOverlayFromApvts();
        patchNameOverlay_.remember(bank, patch, name);
        persistPatchNameOverlayToApvts();
    }

    void PatchManagerActionHandler::clearLastDeviceDumpRawName()
    {
        hasLastDeviceDumpRawName_ = false;
        lastDeviceDumpRawName_.clear();
        lastDeviceDumpBank_ = -1;
        lastDeviceDumpPatch_ = -1;
    }

    void PatchManagerActionHandler::rememberCurrentOverlayFromModel()
    {
        if (patchModel_ == nullptr)
            return;

        const auto limits = deviceMemoryLimits_();
        rememberOverlayName(getCurrentBank(limits), getCurrentPatch(limits), patchModel_->getName());
    }

    void PatchManagerActionHandler::applyResolvedPatchName(PatchModel& model,
                                                           const PatchCoordinates& coordinates,
                                                           const DeviceMemoryLimits& limits,
                                                           PatchNameResolvePurpose purpose)
    {
        loadPatchNameOverlayFromApvts();

        const int bank = coordinates.bank;
        const int patch = coordinates.patch;
        const bool isRom = limits.isRomBank(bank);
        const auto factoryName = isRom ? Matrix1000FactoryPatchNames::nameFor(bank, patch)
                                       : juce::String();
        const auto overlayName = patchNameOverlay_.lookup(bank, patch);

        const auto mode = [&]()
        {
            if (purpose == PatchNameResolvePurpose::kExportMusical)
                return PatchNameResolver::Mode::kMusical;

            const int modeId = PatchNameDisplay::normalize(static_cast<int>(
                apvts_.state.getProperty(PluginIDs::Settings::kMatrix1000PatchesNamesMode,
                                         PluginIDs::Settings::Matrix1000PatchesNamesMode::kDefault)));

            return PatchNameDisplay::isHardwareNames(modeId) ? PatchNameResolver::Mode::kHardware
                                                             : PatchNameResolver::Mode::kMusical;
        }();

        model.setName(PatchNameResolver::resolve({
            .deviceName = model.getName(),
            .bank = bank,
            .patchNumber = patch,
            .isRomBank = isRom,
            .factoryName = factoryName,
            .overlayName = overlayName,
            .mode = mode,
        }));
    }

    juce::String PatchManagerActionHandler::resolveDisplayedPatchNameSeed(int bank,
                                                                         int patch,
                                                                         const DeviceMemoryLimits& limits) const
    {
        if (hasLastDeviceDumpRawName_
            && bank == lastDeviceDumpBank_
            && patch == lastDeviceDumpPatch_)
        {
            return lastDeviceDumpRawName_;
        }

        if (limits.hasBankConcept())
        {
            // Spec live-toggle fallback when no raw dump is cached for this slot.
            return PatchFileNameSanitizer::formatOberheimBankPlaceholderName(bank, patch);
        }

        return patchModel_ != nullptr ? patchModel_->getName() : juce::String();
    }

    void PatchManagerActionHandler::reapplyDisplayedPatchName()
    {
        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        if (pendingDeviceLoad_.has_value() || editorPatchFromComputerFile_)
            return;

        const auto limits = deviceMemoryLimits_();
        const int bank = getCurrentBank(limits);
        const int patch = getCurrentPatch(limits);
        const auto seedName = resolveDisplayedPatchNameSeed(bank, patch, limits);

        const bool wasDirty = dirtyPatchTracker_ != nullptr
            && dirtyPatchTracker_->isDirty(*patchModel_);

        patchModel_->setName(seedName);
        applyResolvedPatchName(*patchModel_,
                               PatchCoordinates { bank, patch },
                               limits,
                               PatchNameResolvePurpose::kDisplay);

        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);

        if (dirtyPatchTracker_ != nullptr && ! wasDirty)
            dirtyPatchTracker_->captureSnapshot(*patchModel_);
    }

    void PatchManagerActionHandler::handleAction(const juce::String& propertyId, const juce::var&)
    {
        const auto limits = deviceMemoryLimits_();

        if (tryHandleInternalPatchNavigation(propertyId, limits))
            return;

        if (tryHandleComputerPatchFileNavigation(propertyId, limits))
            return;

        if (tryHandleModuleHeaderClicks(propertyId, limits))
            return;

        if (tryHandleBankButtonSelection(propertyId, limits))
            return;

        if (tryHandleInitPasteStoreActions(propertyId, limits))
            return;

        if (tryHandleComputerFileActions(propertyId, limits))
            return;

        if (tryHandleBankTransferActions(propertyId, limits))
            return;
    }

    bool PatchManagerActionHandler::confirmPatchContextChange(bool includeUnsavedEditWarning)
    {
        return ! hooks_.confirmPatchContextChange
            || hooks_.confirmPatchContextChange(includeUnsavedEditWarning);
    }

    bool PatchManagerActionHandler::isCurrentBankStoreAllowed() const
    {
        if (! deviceMemoryLimits_)
            return false;

        const auto limits = deviceMemoryLimits_();
        return limits.isPasteStoreAllowed(getCurrentBank(limits));
    }

    bool PatchManagerActionHandler::hasUsableKnownSyxPath() const
    {
        if (knownSyxFullPath_.isEmpty())
            return false;

        const juce::File file(knownSyxFullPath_);
        return file.getParentDirectory().isDirectory();
    }

    UnsavedEditPersistKind PatchManagerActionHandler::resolveUnsavedEditPersistKind(bool isDirty) const
    {
        if (editorPatchFromComputerFile_ && isDirty)
        {
            return hasUsableKnownSyxPath() ? UnsavedEditPersistKind::kSave
                                           : UnsavedEditPersistKind::kSaveAs;
        }

        return isCurrentBankStoreAllowed() ? UnsavedEditPersistKind::kStore
                                           : UnsavedEditPersistKind::kSaveAs;
    }

    void PatchManagerActionHandler::noteDevicePatchOrigin(int bank, int patch)
    {
        editorPatchFromComputerFile_ = false;
        knownSyxFullPath_.clear();

        if (hooks_.setPatchLoadContext)
            hooks_.setPatchLoadContext(PatchLoadContext::deviceMemory(bank, patch));
    }

    void PatchManagerActionHandler::noteComputerPatchOrigin(const juce::File& file)
    {
        editorPatchFromComputerFile_ = true;
        knownSyxFullPath_ = file.getFullPathName();

        if (hooks_.setPatchLoadContext)
        {
            hooks_.setPatchLoadContext(PatchLoadContext::computerFile(
                file.getFileNameWithoutExtension(), knownSyxFullPath_));
        }
    }

    bool PatchManagerActionHandler::performUnsavedGatePersistAction(UnsavedEditPersistKind persistKind)
    {
        switch (persistKind)
        {
            case UnsavedEditPersistKind::kStore:
                if (! isCurrentBankStoreAllowed())
                    return false;

                handleInternalPatchStore(deviceMemoryLimits_());
                return true;

            case UnsavedEditPersistKind::kSave:
                if (! hasUsableKnownSyxPath())
                    return false;

                // Gate path must fail closed on sentinel: perform() used to return true even when
                // saveCurrentPatchToFile refused, which could look like success if dirty was clear.
                if (refuseSaveIfInitSentinelActive())
                    return false;

                saveCurrentPatchToFile(juce::File(knownSyxFullPath_));
                return true;

            case UnsavedEditPersistKind::kSaveAs:
                handleSavePatchAs();
                return true;
        }

        return false;
    }

    bool PatchManagerActionHandler::didUnsavedGatePersistSucceed(UnsavedEditPersistKind persistKind) const
    {
        if (dirtyPatchTracker_ == nullptr || patchModel_ == nullptr || apvtsPatchMapper_ == nullptr
            || patchNameSyncer_ == nullptr)
        {
            return false;
        }

        const bool stillDirty = dirtyPatchTracker_->syncApvtsAndIsDirty(
            *apvtsPatchMapper_, *patchNameSyncer_, *patchModel_);

        if (persistKind == UnsavedEditPersistKind::kStore)
            return ! patchNotStoredInRam_ && ! stillDirty;

        // Save / Save As: dirty must clear. Device-origin Save As may intentionally keep not-STORED.
        return ! stillDirty;
    }

    bool PatchManagerActionHandler::tryPersistCurrentPatchFromUnsavedGate(
        UnsavedEditPersistKind persistKind)
    {
        if (! performUnsavedGatePersistAction(persistKind))
            return false;

        return didUnsavedGatePersistSucceed(persistKind);
    }

    int PatchManagerActionHandler::getCurrentBank(const DeviceMemoryLimits& limits) const
    {
        return static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            limits.minBankNumber()));
    }

    int PatchManagerActionHandler::getCurrentPatch(const DeviceMemoryLimits& limits) const
    {
        return static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            limits.minPatchNumber()));
    }

    void PatchManagerActionHandler::applyPatchCoordinates(const PatchCoordinates& coordinates,
                                                          const DeviceMemoryLimits& limits,
                                                          bool sendMidi)
    {
        if (hooks_.setSuppressPatchSelectionMidiSync)
            hooks_.setSuppressPatchSelectionMidiSync(true);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            coordinates.bank,
            nullptr);
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            coordinates.patch,
            nullptr);

        if (hooks_.setSuppressPatchSelectionMidiSync)
            hooks_.setSuppressPatchSelectionMidiSync(false);

        if (! sendMidi)
            return;

        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->syncSelection(coordinates.bank, coordinates.patch, limits, false);
    }

    int PatchManagerActionHandler::parseBankButtonIndex(const juce::String& propertyId) const
    {
        using namespace PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;

        static constexpr const char* kBankButtonIds[] {
            kSelectBank0, kSelectBank1, kSelectBank2, kSelectBank3, kSelectBank4,
            kSelectBank5, kSelectBank6, kSelectBank7, kSelectBank8, kSelectBank9
        };

        for (int i = 0; i < static_cast<int>(std::size(kBankButtonIds)); ++i)
        {
            if (propertyId == kBankButtonIds[static_cast<size_t>(i)])
                return i;
        }

        return -1;
    }

    void PatchManagerActionHandler::setBankExportFolderPicker(PatchFolderPicker picker)
    {
        bankExportFolderPicker_ = std::move(picker);
    }

    void PatchManagerActionHandler::setBankImportFolderPicker(PatchFolderPicker picker)
    {
        bankImportFolderPicker_ = std::move(picker);
    }

    void PatchManagerActionHandler::setBankImportConfirmGate(BankImportConfirmGate gate)
    {
        bankImportConfirmGate_ = std::move(gate);
    }

    void PatchManagerActionHandler::setBankExportOverwriteConfirmGate(BankImportConfirmGate gate)
    {
        bankExportOverwriteConfirmGate_ = std::move(gate);
    }

    void PatchManagerActionHandler::setBankTransferProgressPresenter(BankTransferProgressPresenter presenter)
    {
        bankTransferProgress_ = std::move(presenter);
    }

    bool PatchManagerActionHandler::isBankTransferBusy() const noexcept
    {
        return bankTransfer_.kind != BankTransferState::Kind::kNone;
    }

    void PatchManagerActionHandler::publishBankTransferFooter(const juce::String& message,
                                                              const juce::String& severity)
    {
        apvts_.state.setProperty("uiMessageText", message, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", severity, nullptr);
    }

    int PatchManagerActionHandler::bankTransferWriteDelayMs() const
    {
        constexpr int kPatchSysExWireMs = 100;
        constexpr int kMatrixStoreGapMs = 20;
        const int profileDelayMs = midiManager_ != nullptr ? midiManager_->getRequiredSysExDelayMs() : 0;
        return juce::jmax(kPatchSysExWireMs + kMatrixStoreGapMs,
                          profileDelayMs + kPatchSysExWireMs);
    }

    void PatchManagerActionHandler::requestBankTransferCancel(std::uint64_t generation)
    {
        using namespace PluginDisplayNames::PatchManagerSection::BankUtilityModule;

        if (bankTransfer_.kind == BankTransferState::Kind::kNone || bankTransfer_.generation != generation)
            return;

        if (bankTransfer_.isRestoring)
            return;

        bankTransfer_.cancelRequested = true;

        if (midiManager_ != nullptr)
            midiManager_->cancelPendingSysExRequest();

        if (bankTransfer_.kind == BankTransferState::Kind::kExport
            || bankTransfer_.kind == BankTransferState::Kind::kCopy)
        {
            if (bankTransfer_.kind == BankTransferState::Kind::kCopy)
                finishBankCopy(false, kCopyCancelledFooterMessage, "warning");
            else
                finishBankExport(false, kExportCancelledFooterMessage, "warning");
            return;
        }

        if (bankTransfer_.importWrittenCount > 0)
            return;

        const auto cancelMessage = bankTransfer_.kind == BankTransferState::Kind::kPaste
            ? juce::String(kPasteCancelledFooterMessage)
            : juce::String(kImportCancelledFooterMessage);
        finishBankImport(cancelMessage, "warning");
    }

    int PatchManagerActionHandler::getSelectedBankForTransfer(const DeviceMemoryLimits& limits) const
    {
        if (! limits.hasBankConcept())
            return 0;

        const int selected = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
            limits.minBankNumber()));
        return juce::jlimit(limits.minBankNumber(), limits.maxBankNumber(), selected);
    }

    void PatchManagerActionHandler::propagateRomBlockedFooter()
    {
        // ROM Init/Paste/Store are grayed in the UI; no warning footer (avoids false alarms).
    }

} // namespace Core
