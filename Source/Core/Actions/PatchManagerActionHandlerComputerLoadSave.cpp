#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Actions/PatchManagerActionHandlerInternal.h"

#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileServiceFooter.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

#include <cstring>

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

namespace Core
{

    void PatchManagerActionHandler::handleSavePatchAs()
    {
        if (! pickSaveFile_)
            return;

        if (refuseSaveIfInitSentinelActive())
            return;

        const auto target = pickSaveFile_(resolveDefaultSaveFolder(), resolveSuggestedSaveStem());

        if (target.getFullPathName().isEmpty())
            return;

        saveCurrentPatchToFile(target);
    }

    void PatchManagerActionHandler::handleSavePatchFile()
    {
        if (patchFileService_ == nullptr || refuseSaveIfInitSentinelActive())
            return;

        const auto target = resolveSelectedComputerPatchFileForSave();
        if (target.getFullPathName().isEmpty())
            return;

        saveCurrentPatchToFile(target);
    }

    juce::File PatchManagerActionHandler::resolveSelectedComputerPatchFileForSave() const
    {
        const int selectedId = readComputerPatchesSelectedId();
        if (selectedId < 1 || ! isComputerPatchesScanCurrent())
            return {};

        return fileAtComputerPatchesIndex(selectedId - 1);
    }

    void PatchManagerActionHandler::commitLoadedComputerPatchFile(const DeviceMemoryLimits& limits,
                                                                  int requestedId,
                                                                  const juce::File& file,
                                                                  const PatchNameReconciliationResult& reconciliation)
    {
        pendingBrowserRestoreOnCancel_.reset();
        abandonPendingDeviceLoad();
        clearLastDeviceDumpRawName();

        noteComputerPatchOrigin(file);
        dropAttemptCommitted_ = true;

        applyLoadedPatchToApvtsAndSynth(limits);
        markPatchCoordinatesEstablished();
        setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kComputer);
        rememberComputerPatchesSelection(requestedId);
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kSelectPatchCancelBaseline,
            0,
            nullptr);
        publishLoadFooters(file, reconciliation);
    }

    void PatchManagerActionHandler::handleLoadSelectedPatchFile(const DeviceMemoryLimits& limits)
    {
        if (suppressComputerPatchesSelectLoad_)
            return;

        const auto resolution = resolveSelectedPatchFileForLoad();

        if (resolution.kind == SelectedPatchFileResolution::Kind::kSilentNoOp)
            return;

        if (resolution.kind == SelectedPatchFileResolution::Kind::kFailed)
        {
            publishLoadFailureFooter(resolution.failureMessage);
            abortComputerPatchesNavigation();
            return;
        }

        const int requestedId = readComputerPatchesSelectedId();

        const int cancelBaseline = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kSelectPatchCancelBaseline,
            0));
        if (lastCommittedComputerPatchesSelectedId_ < 1 && cancelBaseline >= 1)
            noteStableComputerPatchesSelection(cancelBaseline);

        if (! confirmPatchContextChange(true))
        {
            abortComputerPatchesNavigation();
            return;
        }

        const auto reconciliation = decodeAndReconcilePatchFile(resolution.file);
        if (! reconciliation.has_value())
        {
            abortComputerPatchesNavigation();
            return;
        }

        commitLoadedComputerPatchFile(limits, requestedId, resolution.file, *reconciliation);
    }

    void PatchManagerActionHandler::loadSelectedPatchFileImmediately(const DeviceMemoryLimits& limits)
    {
        abandonPendingInternalNavSettle();
        patchNavDebouncer_.cancel();
        computerSelectDebouncer_.cancel();
        handleLoadSelectedPatchFile(limits);
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

    namespace
    {
        // Strip a matching Bank Utility export prefix ("Pxx. {Name}") before FILE NAMES reconcile.
        // Non-matching stems (artistic "P99 - DJ", slot-only "P76", …) stay unchanged.
        juce::String stemForFilenameReconcile(const juce::File& file)
        {
            const auto rawStem = file.getFileNameWithoutExtension();
            const auto fromExport = PatchFileNameSanitizer::nameFromBankExportStem(rawStem);
            return fromExport.isNotEmpty() ? fromExport : rawStem;
        }
    }

    PatchNameReconciliationResult PatchManagerActionHandler::reconcileLoadedPatchName(const juce::File& file)
    {
        const auto policy = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            PluginIDs::Settings::ComputerPatchesNamesPolicy::kDefault));

        return PatchFileNameReconciler::reconcile(
            *patchModel_,
            stemForFilenameReconcile(file),
            policy,
            pickNameReconciliation_);
    }

    void PatchManagerActionHandler::reapplyComputerPatchDisplayedName()
    {
        if (! editorPatchFromComputerFile_ || ! canExecutePatchLoad() || ! hasUsableKnownSyxPath())
            return;

        using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;
        const int policy = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, kDefault));

        // ASK ONCE PER LOAD is load-time only — never prompt from a Settings change.
        if (policy != kDisplaySysexNames && policy != kDisplayFileNames)
            return;

        const juce::File file(knownSyxFullPath_);
        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! patchFileService_->loadPatchSysExFile(file, packed).success)
            return;

        PatchModel nameSource;
        nameSource.loadFrom(packed);
        nameSource.normalizeNameEncoding();

        const bool wasDirty = dirtyPatchTracker_ != nullptr
            && dirtyPatchTracker_->isDirty(*patchModel_);

        // Seed from the .syx internal name, then apply the current DISPLAY policy (no picker).
        patchModel_->setName(nameSource.getName());
        PatchFileNameReconciler::reconcile(
            *patchModel_,
            stemForFilenameReconcile(file),
            policy,
            {});

        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);

        if (dirtyPatchTracker_ != nullptr && ! wasDirty)
            dirtyPatchTracker_->captureSnapshot(*patchModel_);
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
        patchModel_->normalizeNameEncoding();

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
        using namespace PatchManagerActionHandlerInternal;
        pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);
    }

    void PatchManagerActionHandler::applyLoadedPatchToApvtsAndSynth(const DeviceMemoryLimits& limits)
    {
        abandonPendingDeviceLoad();
        syncLoadedPatchToApvts();
        establishEditorialCheckpoint();

        if (hooks_.onPatchLoaded)
            hooks_.onPatchLoaded();

        if (midiManager_ != nullptr)
            midiManager_->sendFullPatchForAudition(patchModel_->data(),
                                                   static_cast<juce::uint8>(getCurrentPatch(limits)),
                                                   limits.hasBankConcept());
    }

    void PatchManagerActionHandler::publishLoadFooters(const juce::File& file,
                                                       const PatchNameReconciliationResult& reconciliation)
    {
        const auto location = FooterMessages::formatReadablePatchLocation(file);
        const auto message = reconciliation.hadMismatch
            ? FooterMessages::formatReconciliationNotice(
                  reconciliation.resolvedName,
                  reconciliation.usedFilename)
            : FooterMessages::formatLoadSuccess(location);

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

        if (refuseSaveIfInitSentinelActive())
            return;

        apvtsPatchMapper_->apvtsToBuffer();
        patchNameSyncer_->apvtsToBuffer();

        const auto targetWithExt = targetFile.withFileExtension(PatchFileService::kSyxExtension);
        const auto matrixStem = PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty(
            targetWithExt.getFileNameWithoutExtension());

        // Refuse illegal stems at the write boundary (no silent sanitizeFileStem rewrite).
        if (matrixStem.isEmpty())
        {
            publishSaveFailureFooter(FooterMessages::kInvalidSaveStem);
            return;
        }

        writeValidatedPatchSyx(targetWithExt, matrixStem);
    }

    void PatchManagerActionHandler::writeValidatedPatchSyx(const juce::File& targetWithExt,
                                                           const juce::String& matrixStem)
    {
        const auto writeTarget =
            targetWithExt.getSiblingFile(PatchFileNameSanitizer::ensureSyxExtension(matrixStem));
        const auto previousName = patchModel_->getName();
        patchModel_->setName(matrixStem);

        const auto result = patchFileService_->savePatchSysExFile(
            writeTarget, patchModel_->data(), *sysExEncoder_);

        if (! result.success)
        {
            patchModel_->setName(previousName);
            publishSaveFailureFooter(result.errorMessage);
            return;
        }

        using namespace PatchManagerActionHandlerInternal;
        removeCaseFoldTwinSyxFiles(writeTarget);

        // Device / ROM / INIT: Save As is a disk copy only — keep the live Patch Name honest
        // until the user Opens the .syx. Computer-file origin keeps the injected stem.
        if (! editorPatchFromComputerFile_)
            patchModel_->setName(previousName);

        completeSuccessfulSave(writeTarget);
    }

    void PatchManagerActionHandler::completeSuccessfulSave(const juce::File& savedFile)
    {
        // Device / INIT: keep RAM risk only when it already applied (e.g. INIT) or the user had
        // edits before this export. A clean ROM/device load + Save As must not invent at-risk
        // state — otherwise OPEN after export falsely shows Unsaved Patch.
        const bool hadUnsavedEdits = dirtyPatchTracker_ != nullptr
            && dirtyPatchTracker_->isDirty(*patchModel_);
        const bool retainRamRisk = ! editorPatchFromComputerFile_
            && (patchNotStoredInRam_ || hadUnsavedEdits);

        patchNameSyncer_->bufferToApvts();
        captureCleanSnapshot();

        if (retainRamRisk)
            markPatchNotStoredInRam();

        if (editorPatchFromComputerFile_)
        {
            knownSyxFullPath_ = savedFile.getFullPathName();

            if (hooks_.setPatchLoadContext)
            {
                hooks_.setPatchLoadContext(PatchLoadContext::computerFile(
                    savedFile.getFileNameWithoutExtension(), knownSyxFullPath_));
            }
        }

        publishSaveSuccessFooter(savedFile);
        rescanAndSelectSavedFile(savedFile);
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
        {
            const auto& scan = patchFileService_->getLastScanResult();
            if (! scan.isVirtualList())
                return scan.folder;
        }

        return {};
    }

    juce::File PatchManagerActionHandler::resolveDefaultSaveFolder() const
    {
        const auto folder = resolveRescanFolder();
        return folder.isDirectory() ? folder : juce::File();
    }

    void PatchManagerActionHandler::publishSaveSuccessFooter(const juce::File& savedFile)
    {
        apvts_.state.setProperty(
            "uiMessageText",
            FooterMessages::formatSaveSuccess(FooterMessages::formatReadablePatchLocation(savedFile)),
            nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("info"), nullptr);
    }

    void PatchManagerActionHandler::publishSaveFailureFooter(const juce::String& message)
    {
        apvts_.state.setProperty("uiMessageText", message, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", juce::String("warning"), nullptr);
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

} // namespace Core
