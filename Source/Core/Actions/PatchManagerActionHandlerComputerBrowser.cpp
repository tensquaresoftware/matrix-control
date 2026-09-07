#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/Actions/PatchManagerActionHandlerInternal.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/Services/PatchFileService.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

namespace Core
{

    void PatchManagerActionHandler::establishCoordinatesForComputerOpen(const DeviceMemoryLimits& limits)
    {
        if (arePatchCoordinatesEstablished())
            return;

        const PatchCoordinates destination { limits.minBankNumber(), limits.minPatchNumber() };
        applyPatchCoordinates(destination, limits, false);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
            destination.bank,
            nullptr);

        // Set Bank only: a Program Change here would pull device patch 00 into the edit buffer
        // before the .syx is applied. Do not mark established yet — cancel / failed load must
        // leave the UI in the undefined state until a .syx actually commits.
        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->sendSetBank(destination.bank, limits);
    }

    void PatchManagerActionHandler::handleOpenPatchFolder(const DeviceMemoryLimits& limits)
    {
        if (patchFileService_ == nullptr || ! pickFolder_)
            return;

        const juce::File folder = pickFolder_();

        if (! folder.isDirectory())
            return;

        seedCommittedComputerPatchesSelectionIfNeeded();

        const juce::String previousFolderPath = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();
        const int previousSelectedId = lastCommittedComputerPatchesSelectedId_;

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            folder.getFullPathName(),
            nullptr);
        scanAndPublishFolder(folder);

        const auto& scan = patchFileService_->getLastScanResult();
        if (! scan.folderUsable || scan.validCount < 1)
        {
            clearComputerPatchesSelection();
            clearComputerNavigationFocusIfOwned();
            return;
        }

        pendingBrowserRestoreOnCancel_ = ComputerPatchesBrowserSnapshot {
            previousFolderPath,
            previousSelectedId
        };

        // The folder holds usable patches, so this OPEN counts as the moment that pins the
        // destination slot. Runs before the .syx apply so Set Bank precedes the dump.
        establishCoordinatesForComputerOpen(limits);

        constexpr int kFirstPatchFileId = 1;
        const int beforeId = readComputerPatchesSelectedId();
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            kFirstPatchFileId,
            nullptr);

        if (readComputerPatchesSelectedId() == beforeId && beforeId == kFirstPatchFileId)
            loadSelectedPatchFileImmediately(limits);
    }

    void PatchManagerActionHandler::clearComputerPatchesSelection()
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);
        rememberComputerPatchesSelection(0);
    }

    void PatchManagerActionHandler::resetComputerPatchesBrowserAfterSessionLoad()
    {
        clearComputerPatchesSelection();
        lastStableComputerPatchesSelectedId_ = 0;
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kSelectPatchCancelBaseline,
            0,
            nullptr);

        if (patchFileService_ != nullptr && patchFileService_->hasCachedScanResult())
            clearPublishedScanCache();
        else
            bumpScanRevision();

        pendingBrowserRestoreOnCancel_.reset();
        reloadPatchNameOverlayFromApvts();
    }

    void PatchManagerActionHandler::discardComputerPatchesScanCacheQuietly()
    {
        if (patchFileService_ != nullptr && patchFileService_->hasCachedScanResult())
            patchFileService_->clearLastScan();
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

    std::optional<int> PatchManagerActionHandler::advanceComputerPatchesSelection(bool isNext)
    {
        const int currentId = readComputerPatchesSelectedId();
        if (currentId < 1 || ! isComputerPatchesScanCurrent())
            return std::nullopt;

        const int count = patchFileService_->getLastScanResult().sortedValidFileNames.size();
        if (count < 1 || currentId > count)
            return std::nullopt;

        seedCommittedComputerPatchesSelectionIfNeeded();

        const int nextId = isNext
            ? (currentId >= count ? 1 : currentId + 1)
            : (currentId <= 1 ? count : currentId - 1);

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            nextId,
            nullptr);
        return nextId;
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

    void PatchManagerActionHandler::noteStableComputerPatchesSelection(int selectedId)
    {
        if (selectedId >= 1)
            lastStableComputerPatchesSelectedId_ = selectedId;
    }

    int PatchManagerActionHandler::resolveComputerPatchesCancelRevertId() const
    {
        if (lastCommittedComputerPatchesSelectedId_ >= 1)
            return lastCommittedComputerPatchesSelectedId_;

        if (lastStableComputerPatchesSelectedId_ >= 1)
            return lastStableComputerPatchesSelectedId_;

        return static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kSelectPatchCancelBaseline,
            0));
    }

    void PatchManagerActionHandler::revertComputerPatchesSelectionIfNeeded(int previousSelectedId)
    {
        if (readComputerPatchesSelectedId() == previousSelectedId)
            return;

        suppressComputerPatchesSelectLoad_ = true;
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            previousSelectedId,
            nullptr);
        suppressComputerPatchesSelectLoad_ = false;
        noteStableComputerPatchesSelection(previousSelectedId);
    }

    void PatchManagerActionHandler::rememberComputerPatchesSelection(int selectedId)
    {
        lastCommittedComputerPatchesSelectedId_ = selectedId;
        noteStableComputerPatchesSelection(selectedId);
    }

    void PatchManagerActionHandler::seedCommittedComputerPatchesSelectionIfNeeded()
    {
        if (lastCommittedComputerPatchesSelectedId_ >= 1)
            return;

        const int currentId = readComputerPatchesSelectedId();
        if (currentId >= 1)
        {
            lastCommittedComputerPatchesSelectedId_ = currentId;
            noteStableComputerPatchesSelection(currentId);
        }
    }

    void PatchManagerActionHandler::restoreComputerPatchesBrowser(const juce::String& folderPath,
                                                                  int selectedId)
    {
        suppressComputerPatchesSelectLoad_ = true;

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            folderPath,
            nullptr);

        if (folderPath.isEmpty())
            clearPublishedScanCache();
        else
            scanAndPublishFolder(juce::File(folderPath));

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            selectedId,
            nullptr);

        suppressComputerPatchesSelectLoad_ = false;
        noteStableComputerPatchesSelection(selectedId);
    }

    void PatchManagerActionHandler::abortComputerPatchesNavigation()
    {
        abandonPendingInternalNavSettle();
        patchNavDebouncer_.cancel();
        computerSelectDebouncer_.cancel();

        if (pendingBrowserRestoreOnCancel_.has_value())
        {
            const auto snapshot = *pendingBrowserRestoreOnCancel_;
            pendingBrowserRestoreOnCancel_.reset();
            restoreComputerPatchesBrowser(snapshot.folderPath, snapshot.selectedId);
            noteStableComputerPatchesSelection(snapshot.selectedId);
            apvts_.state.setProperty(
                PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kSelectPatchCancelBaseline,
                0,
                nullptr);
            return;
        }

        revertComputerPatchesSelectionIfNeeded(resolveComputerPatchesCancelRevertId());
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kSelectPatchCancelBaseline,
            0,
            nullptr);
    }

    void PatchManagerActionHandler::publishDropRejectFooter(SinglePatchSyxRejectKind rejectKind)
    {
        const char* message = FooterMessages::kDropRejectedInvalid;

        switch (rejectKind)
        {
            case SinglePatchSyxRejectKind::kNotSyx:
                message = FooterMessages::kDropRejectedNotSyx;
                break;
            case SinglePatchSyxRejectKind::kBankOrMultiMessage:
                message = FooterMessages::kDropRejectedBankOrMulti;
                break;
            case SinglePatchSyxRejectKind::kInvalid:
            case SinglePatchSyxRejectKind::kNone:
                break;
        }

        publishLoadFailureFooter(message);
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::rejectDroppedComputerPatch(SinglePatchSyxRejectKind rejectKind)
    {
        publishDropRejectFooter(rejectKind);
        return DroppedComputerPatchLoadResult::kRejected;
    }

    bool PatchManagerActionHandler::prepareDroppedComputerPatchSelection(
        const juce::File& file,
        const DeviceMemoryLimits& limits,
        int& outTargetId)
    {
        seedCommittedComputerPatchesSelectionIfNeeded();

        const juce::String previousFolderPath = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();
        const int previousSelectedId = lastCommittedComputerPatchesSelectedId_;

        const auto parent = file.getParentDirectory();
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            parent.getFullPathName(),
            nullptr);
        scanAndPublishFolder(parent);

        const auto& scan = patchFileService_->getLastScanResult();
        using namespace PatchManagerActionHandlerInternal;
        const int index = indexOfFileNameIgnoreCase(scan.sortedValidFileNames, file.getFileName());

        if (! scan.folderUsable || index < 0)
        {
            restoreComputerPatchesBrowser(previousFolderPath, previousSelectedId);
            return false;
        }

        pendingBrowserRestoreOnCancel_ = ComputerPatchesBrowserSnapshot {
            previousFolderPath,
            previousSelectedId
        };
        establishCoordinatesForComputerOpen(limits);
        outTargetId = index + 1;
        return true;
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::finalizeDroppedComputerPatchLoad()
    {
        return dropAttemptCommitted_
            ? DroppedComputerPatchLoadResult::kLoaded
            : DroppedComputerPatchLoadResult::kCancelled;
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::loadDroppedComputerPatchFile(const juce::File& file,
                                                            const DeviceMemoryLimits& limits)
    {
        if (patchFileService_ == nullptr)
            return rejectDroppedComputerPatch(SinglePatchSyxRejectKind::kInvalid);

        const auto assessment = patchFileService_->assessSinglePatchSyxFile(file);
        if (! assessment.isValidSinglePatch)
            return rejectDroppedComputerPatch(assessment.rejectKind);

        int targetId = 0;
        if (! prepareDroppedComputerPatchSelection(file, limits, targetId))
            return rejectDroppedComputerPatch(SinglePatchSyxRejectKind::kInvalid);

        suppressComputerPatchesSelectLoad_ = true;
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            targetId,
            nullptr);
        suppressComputerPatchesSelectLoad_ = false;

        dropAttemptCommitted_ = false;
        loadSelectedPatchFileImmediately(limits);
        return finalizeDroppedComputerPatchLoad();
    }

} // namespace Core
