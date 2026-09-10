#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/Actions/PatchManagerActionHandlerInternal.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileServiceFooter.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    void PatchManagerActionHandler::armPendingCombinedScanLoadFooter(int validCount, int invalidCount)
    {
        pendingCombinedScanLoadFooter_ = PendingCombinedScanLoadFooter { validCount, invalidCount };
    }

    void PatchManagerActionHandler::clearPendingCombinedScanLoadFooter()
    {
        pendingCombinedScanLoadFooter_.reset();
    }

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

        const auto previous = captureComputerPatchesBrowserSnapshot();

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            folder.getFullPathName(),
            nullptr);

        const auto scan = patchFileService_->scanFolder(folder);
        bumpScanRevision();

        if (! scan.folderUsable || scan.validCount < 1)
        {
            clearPendingCombinedScanLoadFooter();
            PatchFileServiceFooter::propagateScanResult(apvts_, scan);
            clearComputerPatchesSelection();
            clearComputerNavigationFocusIfOwned();
            return;
        }

        armPendingCombinedScanLoadFooter(scan.validCount, scan.invalidCount);

        pendingBrowserRestoreOnCancel_ = previous;

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
        suppressComputerPatchesSelectLoad_ = true;
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);
        suppressComputerPatchesSelectLoad_ = false;
        rememberComputerPatchesSelection(0);
    }

    void PatchManagerActionHandler::resetComputerPatchesBrowserAfterSessionLoad()
    {
        clearPendingCombinedScanLoadFooter();
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

    void PatchManagerActionHandler::applyComputerPatchesBrowserAfterSessionLoad()
    {
        // Keep reset+rescan as one production entry so session restore cannot call only half.
        resetComputerPatchesBrowserAfterSessionLoad();
        rescanPersistedComputerPatchesFolder();
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

        if (scan.isVirtualList())
            return scan.folderUsable && scan.validCount > 0 && scan.sortedValidFiles.size() > 0;

        const auto expectedFolder = resolveRescanFolder();

        return scan.folderUsable
            && scan.folder.isDirectory()
            && expectedFolder.isDirectory()
            && scan.folder.getFullPathName() == expectedFolder.getFullPathName();
    }

    juce::File PatchManagerActionHandler::fileAtComputerPatchesIndex(int index) const
    {
        const auto& scan = patchFileService_->getLastScanResult();

        if (scan.isVirtualList())
        {
            if (index < 0 || index >= scan.sortedValidFiles.size())
                return {};

            return scan.sortedValidFiles[index];
        }

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
        ComputerPatchesBrowserSnapshot snapshot;
        snapshot.folderPath = folderPath;
        snapshot.selectedId = selectedId;
        restoreComputerPatchesBrowser(snapshot);
    }

    void PatchManagerActionHandler::restoreComputerPatchesBrowser(
        const ComputerPatchesBrowserSnapshot& snapshot)
    {
        clearPendingCombinedScanLoadFooter();
        suppressComputerPatchesSelectLoad_ = true;

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            snapshot.folderPath,
            nullptr);

        if (snapshot.isVirtualList)
        {
            if (patchFileService_ != nullptr)
            {
                patchFileService_->installVirtualFileList(snapshot.virtualFiles,
                                                         snapshot.virtualInvalidCount);
                PatchFileServiceFooter::propagateScanResult(
                    apvts_, patchFileService_->getLastScanResult());
            }
            bumpScanRevision();
        }
        else if (snapshot.folderPath.isEmpty())
        {
            clearPublishedScanCache();
        }
        else
        {
            scanAndPublishFolder(juce::File(snapshot.folderPath));
        }

        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            snapshot.selectedId,
            nullptr);

        suppressComputerPatchesSelectLoad_ = false;
        noteStableComputerPatchesSelection(snapshot.selectedId);
    }

    PatchManagerActionHandler::ComputerPatchesBrowserSnapshot
    PatchManagerActionHandler::captureComputerPatchesBrowserSnapshot() const
    {
        ComputerPatchesBrowserSnapshot snapshot;
        snapshot.folderPath = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();
        snapshot.selectedId = lastCommittedComputerPatchesSelectedId_;

        if (patchFileService_ != nullptr)
        {
            const auto& scan = patchFileService_->getLastScanResult();

            if (scan.isVirtualList())
            {
                snapshot.isVirtualList = true;
                snapshot.virtualFiles = scan.sortedValidFiles;
                snapshot.virtualInvalidCount = scan.invalidCount;
            }
        }

        return snapshot;
    }

    void PatchManagerActionHandler::abortComputerPatchesNavigation()
    {
        clearPendingCombinedScanLoadFooter();
        abandonPendingInternalNavSettle();
        patchNavDebouncer_.cancel();
        computerSelectDebouncer_.cancel();

        if (pendingBrowserRestoreOnCancel_.has_value())
        {
            const auto snapshot = *pendingBrowserRestoreOnCancel_;
            pendingBrowserRestoreOnCancel_.reset();
            restoreComputerPatchesBrowser(snapshot);
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

} // namespace Core
