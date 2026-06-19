#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/Init/InitTemplateFooter.h"
#include "Core/Init/PatchInitService.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Core/Services/PatchFileServiceFooter.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

#include <cstring>

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

namespace
{
    void setPatchLoadSuppressHooks(Core::ActionExecutionHooks& hooks, bool suppress)
    {
        if (hooks.setSuppressPatchSysEx)
            hooks.setSuppressPatchSysEx(suppress);

        if (hooks.setSuppressMatrixModSysEx)
            hooks.setSuppressMatrixModSysEx(suppress);
    }

    void flushDeferredApvtsParameterSync(juce::AudioProcessorValueTreeState& apvts)
    {
        // setValueNotifyingHost queues ValueTree sync on APVTS's internal timer; copyState() forces flush (public API).
        (void) apvts.copyState();
    }

    void pushPatchModelToApvtsWithSuppress(juce::AudioProcessorValueTreeState& apvts,
                                           Core::ActionExecutionHooks& hooks,
                                           Core::ApvtsPatchMapper& mapper,
                                           Core::PatchNameSyncer* patchNameSyncer)
    {
        setPatchLoadSuppressHooks(hooks, true);
        mapper.bufferToApvts();

        if (patchNameSyncer != nullptr)
            patchNameSyncer->bufferToApvts();

        flushDeferredApvtsParameterSync(apvts);
        setPatchLoadSuppressHooks(hooks, false);
    }
}

namespace Core
{

    namespace
    {
        int indexOfFileNameIgnoreCase(const juce::StringArray& names, const juce::String& fileName)
        {
            for (int i = 0; i < names.size(); ++i)
            {
                if (names[i].equalsIgnoreCase(fileName))
                    return i;
            }

            return -1;
        }

        juce::String savedSyxFileName(const juce::File& targetFile)
        {
            return targetFile.hasFileExtension(PatchFileService::kSyxExtension)
                ? targetFile.getFileName()
                : targetFile.withFileExtension(PatchFileService::kSyxExtension).getFileName();
        }
    }

    PatchManagerActionHandler::PatchManagerActionHandler(
        juce::AudioProcessorValueTreeState& apvts,
        DeviceMemoryLimitsSupplier deviceMemoryLimits,
        PatchModel* patchModel,
        ApvtsPatchMapper* apvtsPatchMapper,
        ClipboardService* clipboardService,
        PatchInitService* patchInitService,
        PatchSelectionMidiSync* patchSelectionMidiSync,
        MidiManager* midiManager,
        PatchFileService* patchFileService,
        PatchNameSyncer* patchNameSyncer,
        SysExEncoder* sysExEncoder,
        PatchFolderPicker pickFolder,
        PatchSaveFilePicker pickSaveFile,
        PatchNameReconciliationPicker pickNameReconciliation,
        ActionExecutionHooks hooks)
        : apvts_(apvts)
        , deviceMemoryLimits_(std::move(deviceMemoryLimits))
        , patchModel_(patchModel)
        , apvtsPatchMapper_(apvtsPatchMapper)
        , clipboardService_(clipboardService)
        , patchInitService_(patchInitService)
        , patchSelectionMidiSync_(patchSelectionMidiSync)
        , midiManager_(midiManager)
        , patchFileService_(patchFileService)
        , patchNameSyncer_(patchNameSyncer)
        , sysExEncoder_(sysExEncoder)
        , pickFolder_(std::move(pickFolder))
        , pickSaveFile_(std::move(pickSaveFile))
        , pickNameReconciliation_(std::move(pickNameReconciliation))
        , hooks_(std::move(hooks))
    {
    }

    void PatchManagerActionHandler::handleAction(const juce::String& propertyId, const juce::var&)
    {
        using namespace PluginIDs::PatchManagerSection;

        const auto limits = deviceMemoryLimits_();

        if (propertyId == InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch
            || propertyId == InternalPatchesModule::StandaloneWidgets::kLoadNextPatch)
        {
            const bool isNext = propertyId == InternalPatchesModule::StandaloneWidgets::kLoadNextPatch;
            const int direction = isNext ? 1 : -1;

            PatchCoordinates current;
            current.bank = getCurrentBank(limits);
            current.patch = getCurrentPatch(limits);

            const bool banksLocked = static_cast<bool>(apvts_.state.getProperty(
                BankUtilityModule::StateProperties::kBanksLocked,
                false));

            applyPatchCoordinates(limits.advancePatch(current, direction, banksLocked), limits);
            return;
        }

        if (propertyId == InternalPatchesModule::StandaloneWidgets::kInitPatch)
        {
            handleInternalPatchInit();
            return;
        }

        if (propertyId == InternalPatchesModule::StandaloneWidgets::kPastePatch)
        {
            handleInternalPatchPaste(limits);
            return;
        }

        if (propertyId == InternalPatchesModule::StandaloneWidgets::kStorePatch)
        {
            handleInternalPatchStore(limits);
            return;
        }

        if (propertyId == ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder)
        {
            handleOpenPatchFolder();
            return;
        }

        if (propertyId == ComputerPatchesModule::StandaloneWidgets::kSavePatchAs)
        {
            handleSavePatchAs();
            return;
        }

        if (propertyId == ComputerPatchesModule::StandaloneWidgets::kSavePatchFile)
        {
            handleSavePatchFile();
            return;
        }

        if (propertyId == ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile)
        {
            handleLoadSelectedPatchFile(limits);
            return;
        }

        if (propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile
            || propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile)
        {
            const int beforeId = readComputerPatchesSelectedId();
            const bool isNext = propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile;
            advanceComputerPatchesSelection(isNext);
            if (readComputerPatchesSelectedId() == beforeId && beforeId >= 1 && isComputerPatchesScanCurrent())
            {
                const int count = patchFileService_->getLastScanResult().sortedValidFileNames.size();
                if (count >= 1)
                    handleLoadSelectedPatchFile(limits);
            }
            return;
        }

        if (!limits.hasBankConcept())
        {
            if (parseBankButtonIndex(propertyId) >= 0
                || propertyId == BankUtilityModule::StandaloneWidgets::kUnlockBank)
            {
                return;
            }
        }

        const int bankIndex = parseBankButtonIndex(propertyId);
        if (bankIndex >= 0)
        {
            const int clampedBank = juce::jlimit(limits.minBankNumber(), limits.maxBankNumber(), bankIndex);
            apvts_.state.setProperty(BankUtilityModule::StateProperties::kSelectedBank, clampedBank, nullptr);
            apvts_.state.setProperty(InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber, clampedBank, nullptr);

            if (patchSelectionMidiSync_ != nullptr)
                patchSelectionMidiSync_->syncSelection(clampedBank, getCurrentPatch(limits), limits, true);

            markBanksLockedInApvts();
            return;
        }

        if (propertyId == BankUtilityModule::StandaloneWidgets::kUnlockBank)
            handleUnlockBank(limits);
    }

    void PatchManagerActionHandler::handleUnlockBank(const DeviceMemoryLimits& limits)
    {
        if (!limits.hasBankConcept())
            return;

        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->sendUnlockBank(limits);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kBanksLocked,
            false,
            nullptr);
    }

    void PatchManagerActionHandler::markBanksLockedInApvts()
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kBanksLocked,
            true,
            nullptr);
    }

    void PatchManagerActionHandler::handleInternalPatchInit()
    {
        if (patchInitService_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        const auto result = patchInitService_->initFullPatch();

        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, nullptr);

        InitTemplateFooter::propagateMessage(apvts_, result);
    }

    void PatchManagerActionHandler::handleInternalPatchPaste(const DeviceMemoryLimits& limits)
    {
        const int currentBank = getCurrentBank(limits);

        if (!limits.isPasteStoreAllowed(currentBank))
        {
            propagateRomBlockedFooter();
            return;
        }

        if (clipboardService_ == nullptr || !clipboardService_->canPasteFullPatch())
            return;

        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        apvtsPatchMapper_->apvtsToBuffer();
        clipboardService_->pasteFullPatch(*patchModel_);

        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, nullptr);

        if (midiManager_ != nullptr)
            midiManager_->sendPatch(static_cast<juce::uint8>(getCurrentPatch(limits)), patchModel_->data());
    }

    void PatchManagerActionHandler::handleInternalPatchStore(const DeviceMemoryLimits& limits)
    {
        const int currentBank = getCurrentBank(limits);

        if (!limits.isPasteStoreAllowed(currentBank))
        {
            propagateRomBlockedFooter();
            return;
        }

        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr || midiManager_ == nullptr)
            return;

        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->syncSelection(currentBank, getCurrentPatch(limits), limits, true);

        if (limits.hasBankConcept())
            markBanksLockedInApvts();

        apvtsPatchMapper_->apvtsToBuffer();
        midiManager_->sendPatch(static_cast<juce::uint8>(getCurrentPatch(limits)), patchModel_->data());
    }

    void PatchManagerActionHandler::handleOpenPatchFolder()
    {
        if (patchFileService_ == nullptr || ! pickFolder_)
            return;

        const juce::File folder = pickFolder_();

        if (! folder.isDirectory())
            return;

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            folder.getFullPathName(),
            nullptr);
        scanAndPublishFolder(folder);
    }

    void PatchManagerActionHandler::handleSavePatchAs()
    {
        if (! pickSaveFile_)
            return;

        const auto target = pickSaveFile_(resolveDefaultSaveFolder(), resolveSuggestedSaveStem());

        if (target.getFullPathName().isEmpty())
            return;

        saveCurrentPatchToFile(target);
    }

    void PatchManagerActionHandler::handleSavePatchFile()
    {
        if (patchFileService_ == nullptr)
            return;

        const int selectedId = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            0));

        if (selectedId < 1)
            return;

        const auto& scan = patchFileService_->getLastScanResult();
        const auto expectedFolder = resolveRescanFolder();

        if (! scan.folderUsable || ! scan.folder.isDirectory() || ! expectedFolder.isDirectory()
            || scan.folder.getFullPathName() != expectedFolder.getFullPathName())
            return;

        const int index = selectedId - 1;
        if (index < 0 || index >= scan.sortedValidFileNames.size())
            return;

        saveCurrentPatchToFile(scan.folder.getChildFile(scan.sortedValidFileNames[index]));
    }

    void PatchManagerActionHandler::handleLoadSelectedPatchFile(const DeviceMemoryLimits& limits)
    {
        const auto resolution = resolveSelectedPatchFileForLoad();

        if (resolution.kind == SelectedPatchFileResolution::Kind::kSilentNoOp)
            return;

        if (resolution.kind == SelectedPatchFileResolution::Kind::kFailed)
        {
            publishLoadFailureFooter(resolution.failureMessage);
            return;
        }

        const auto reconciliation = decodeAndReconcilePatchFile(resolution.file);
        if (! reconciliation.has_value())
            return;

        applyLoadedPatchToApvtsAndSynth(limits);
        publishLoadFooters(resolution.file.getFileName(), *reconciliation);
    }

    void PatchManagerActionHandler::advanceComputerPatchesSelection(bool isNext)
    {
        const int currentId = readComputerPatchesSelectedId();
        if (currentId < 1 || ! isComputerPatchesScanCurrent())
            return;

        const int count = patchFileService_->getLastScanResult().sortedValidFileNames.size();
        if (count < 1 || currentId > count)
            return;

        const int nextId = isNext
            ? (currentId >= count ? 1 : currentId + 1)
            : (currentId <= 1 ? count : currentId - 1);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            nextId,
            nullptr);
    }

    int PatchManagerActionHandler::readComputerPatchesSelectedId() const
    {
        return static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            0));
    }

    bool PatchManagerActionHandler::isComputerPatchesScanCurrent() const
    {
        if (patchFileService_ == nullptr)
            return false;

        const auto& scan = patchFileService_->getLastScanResult();
        const auto expectedFolder = resolveRescanFolder();

        return scan.folderUsable
            && scan.folder.isDirectory()
            && expectedFolder.isDirectory()
            && scan.folder.getFullPathName() == expectedFolder.getFullPathName();
    }

    juce::File PatchManagerActionHandler::fileAtComputerPatchesIndex(int index) const
    {
        const auto& scan = patchFileService_->getLastScanResult();

        if (index < 0 || index >= scan.sortedValidFileNames.size())
            return {};

        return scan.folder.getChildFile(scan.sortedValidFileNames[index]);
    }

    PatchManagerActionHandler::SelectedPatchFileResolution
    PatchManagerActionHandler::makeLoadFailedResolution(const juce::String& message) const
    {
        SelectedPatchFileResolution resolution;
        resolution.kind = SelectedPatchFileResolution::Kind::kFailed;
        resolution.failureMessage = message;
        return resolution;
    }

    PatchManagerActionHandler::SelectedPatchFileResolution
    PatchManagerActionHandler::resolveSelectedPatchFileForLoad() const
    {
        const int selectedId = readComputerPatchesSelectedId();
        if (selectedId < 1)
            return {};

        if (! isComputerPatchesScanCurrent())
            return makeLoadFailedResolution(FooterMessages::kLoadSelectionStale);

        const auto file = fileAtComputerPatchesIndex(selectedId - 1);
        if (file.getFullPathName().isEmpty())
            return makeLoadFailedResolution(FooterMessages::kLoadSelectionStale);

        if (! file.existsAsFile())
            return makeLoadFailedResolution(FooterMessages::kPatchFileNotFound);

        SelectedPatchFileResolution resolution;
        resolution.kind = SelectedPatchFileResolution::Kind::kOk;
        resolution.file = file;
        return resolution;
    }

    bool PatchManagerActionHandler::canExecutePatchLoad() const
    {
        return patchModel_ != nullptr
            && apvtsPatchMapper_ != nullptr
            && patchFileService_ != nullptr
            && patchNameSyncer_ != nullptr;
    }

    bool PatchManagerActionHandler::loadPackedPatchFromFile(const juce::File& file, juce::uint8* packedOut)
    {
        const auto loadResult = patchFileService_->loadPatchSysExFile(file, packedOut);
        if (loadResult.success)
            return true;

        publishLoadFailureFooter(loadResult.errorMessage);
        return false;
    }

    PatchNameReconciliationResult PatchManagerActionHandler::reconcileLoadedPatchName(const juce::File& file)
    {
        const auto policy = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy,
            PluginIDs::Settings::NameReconciliationPolicy::kDefault));

        return PatchFileNameReconciler::reconcile(
            *patchModel_,
            file.getFileNameWithoutExtension(),
            policy,
            pickNameReconciliation_);
    }

    std::optional<PatchNameReconciliationResult>
    PatchManagerActionHandler::decodeAndReconcilePatchFile(const juce::File& file)
    {
        if (! canExecutePatchLoad())
            return std::nullopt;

        juce::uint8 previousPacked[SysExConstants::kPatchPackedDataSize] = {};
        std::memcpy(previousPacked, patchModel_->data(), sizeof(previousPacked));

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! loadPackedPatchFromFile(file, packed))
            return std::nullopt;

        patchModel_->loadFrom(packed);

        const auto reconciliation = reconcileLoadedPatchName(file);
        if (reconciliation.cancelled)
        {
            patchModel_->loadFrom(previousPacked);
            return std::nullopt;
        }

        return reconciliation;
    }

    void PatchManagerActionHandler::syncLoadedPatchToApvts()
    {
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);
    }

    void PatchManagerActionHandler::applyLoadedPatchToApvtsAndSynth(const DeviceMemoryLimits& limits)
    {
        syncLoadedPatchToApvts();

        // FR-31 hook: MutationHistoryStore::clear() on patch load (Epic 6.13).

        if (midiManager_ != nullptr)
            midiManager_->sendPatch(static_cast<juce::uint8>(getCurrentPatch(limits)), patchModel_->data());
    }

    void PatchManagerActionHandler::publishLoadFooters(const juce::String& fileName,
                                                       const PatchNameReconciliationResult& reconciliation)
    {
        const auto message = reconciliation.hadMismatch
            ? PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages::formatReconciliationNotice(
                  reconciliation.resolvedName,
                  reconciliation.usedFilename)
            : PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages::formatLoadSuccess(
                  fileName);

        apvts_.state.setProperty("uiMessageText", message, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("info"), nullptr);
    }

    void PatchManagerActionHandler::publishLoadFailureFooter(const juce::String& message)
    {
        apvts_.state.setProperty("uiMessageText", message, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
    }

    void PatchManagerActionHandler::saveCurrentPatchToFile(const juce::File& targetFile)
    {
        if (patchModel_ == nullptr || apvtsPatchMapper_ == nullptr || patchFileService_ == nullptr
            || patchNameSyncer_ == nullptr || sysExEncoder_ == nullptr)
            return;

        apvtsPatchMapper_->apvtsToBuffer();

        const auto originalName = patchModel_->getName();
        patchModel_->setName(PatchFileNameSanitizer::sanitizeFileStem(
            targetFile.getFileNameWithoutExtension()));

        const auto result = patchFileService_->savePatchSysExFile(
            targetFile.withFileExtension(PatchFileService::kSyxExtension),
            patchModel_->data(),
            *sysExEncoder_);

        if (! result.success)
        {
            patchModel_->setName(originalName);
            publishSaveFailureFooter(result.errorMessage);
            return;
        }

        completeSuccessfulSave(savedSyxFileName(targetFile));
    }

    void PatchManagerActionHandler::completeSuccessfulSave(const juce::String& savedFileName)
    {
        patchNameSyncer_->bufferToApvts();
        publishSaveSuccessFooter(savedFileName);
        rescanAndSelectSavedFile(savedFileName);
    }

    void PatchManagerActionHandler::rescanAndSelectSavedFile(const juce::String& savedFileName)
    {
        if (patchFileService_ == nullptr)
            return;

        const auto folder = resolveRescanFolder();
        if (! folder.isDirectory())
            return;

        patchFileService_->scanFolder(folder);

        const auto& names = patchFileService_->getLastScanResult().sortedValidFileNames;
        const int index = indexOfFileNameIgnoreCase(names, savedFileName);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            index >= 0 ? index + 1 : 0,
            nullptr);
        bumpScanRevision();
    }

    juce::File PatchManagerActionHandler::resolveRescanFolder() const
    {
        const auto path = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();

        if (path.isNotEmpty())
        {
            const juce::File persisted(path);
            if (persisted.isDirectory())
                return persisted;
        }

        if (patchFileService_ != nullptr)
            return patchFileService_->getLastScanResult().folder;

        return {};
    }

    juce::File PatchManagerActionHandler::resolveDefaultSaveFolder() const
    {
        const auto folder = resolveRescanFolder();
        return folder.isDirectory() ? folder : juce::File();
    }

    juce::String PatchManagerActionHandler::resolveSuggestedSaveStem() const
    {
        const auto raw = apvts_.state.getProperty(
            PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
            juce::String()).toString();

        const auto sanitized = PatchFileNameSanitizer::sanitizeToMatrixName(raw.trim());
        return sanitized.isNotEmpty() ? sanitized : juce::String(PatchFileNameSanitizer::kEmptyNameFallback);
    }

    void PatchManagerActionHandler::publishSaveSuccessFooter(const juce::String& fileName)
    {
        apvts_.state.setProperty(
            "uiMessageText",
            PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages::formatSaveSuccess(
                fileName),
            nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("info"), nullptr);
    }

    void PatchManagerActionHandler::publishSaveFailureFooter(const juce::String& message)
    {
        apvts_.state.setProperty("uiMessageText", message, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
    }

    void PatchManagerActionHandler::rescanPersistedComputerPatchesFolder()
    {
        const auto path = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();

        if (path.isEmpty())
        {
            clearPublishedScanCache();
            return;
        }

        scanAndPublishFolder(juce::File(path));
    }

    void PatchManagerActionHandler::clearPublishedScanCache()
    {
        if (patchFileService_ == nullptr || ! patchFileService_->hasCachedScanResult())
            return;

        const auto footerMessage = patchFileService_->getLastScanResult().footerMessage;
        patchFileService_->clearLastScan();

        if (footerMessage.isNotEmpty()
            && apvts_.state.getProperty("uiMessageText").toString() == footerMessage)
        {
            apvts_.state.setProperty("uiMessageText", juce::String(), nullptr);
            apvts_.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
        }

        bumpScanRevision();
    }

    void PatchManagerActionHandler::scanAndPublishFolder(const juce::File& folder)
    {
        if (patchFileService_ == nullptr)
            return;

        const auto result = patchFileService_->scanFolder(folder);
        PatchFileServiceFooter::propagateScanResult(apvts_, result);
        bumpScanRevision();
    }

    void PatchManagerActionHandler::bumpScanRevision()
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kScanRevision,
            juce::Time::getMillisecondCounterHiRes(),
            nullptr);
    }

    void PatchManagerActionHandler::propagateRomBlockedFooter()
    {
        apvts_.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::PatchManagerSection::InternalPatchesModule::kRomBankPasteStoreFooterMessage),
            nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
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
                                                          const DeviceMemoryLimits& limits)
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

        bool setBankSent = false;
        if (patchSelectionMidiSync_ != nullptr)
            setBankSent = patchSelectionMidiSync_->syncSelection(coordinates.bank, coordinates.patch, limits, false);

        if (setBankSent)
            markBanksLockedInApvts();
    }

    int PatchManagerActionHandler::parseBankButtonIndex(const juce::String& propertyId) const
    {
        using namespace PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;

        if (propertyId == kSelectBank0) return 0;
        if (propertyId == kSelectBank1) return 1;
        if (propertyId == kSelectBank2) return 2;
        if (propertyId == kSelectBank3) return 3;
        if (propertyId == kSelectBank4) return 4;
        if (propertyId == kSelectBank5) return 5;
        if (propertyId == kSelectBank6) return 6;
        if (propertyId == kSelectBank7) return 7;
        if (propertyId == kSelectBank8) return 8;
        if (propertyId == kSelectBank9) return 9;

        return -1;
    }

} // namespace Core
