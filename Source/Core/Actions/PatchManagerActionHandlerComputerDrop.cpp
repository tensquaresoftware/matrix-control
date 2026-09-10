#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/Actions/PatchManagerActionHandlerInternal.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileServiceFooter.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

namespace Core
{

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

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::rejectDroppedComputerPatchWithMessage(const char* message)
    {
        publishLoadFailureFooter(message);
        return DroppedComputerPatchLoadResult::kRejected;
    }

    bool PatchManagerActionHandler::prepareDroppedComputerPatchSelection(
        const juce::File& file,
        const DeviceMemoryLimits& limits,
        int& outTargetId)
    {
        seedCommittedComputerPatchesSelectionIfNeeded();
        const auto previous = captureComputerPatchesBrowserSnapshot();

        const auto parent = file.getParentDirectory();
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            parent.getFullPathName(),
            nullptr);

        const auto scan = patchFileService_->scanFolder(parent);
        bumpScanRevision();

        using namespace PatchManagerActionHandlerInternal;
        const int index = indexOfFileNameIgnoreCase(scan.sortedValidFileNames, file.getFileName());

        if (! scan.folderUsable || index < 0)
        {
            clearPendingCombinedScanLoadFooter();
            restoreComputerPatchesBrowser(previous);
            return false;
        }

        armPendingCombinedScanLoadFooter(scan.validCount, scan.invalidCount);
        pendingBrowserRestoreOnCancel_ = previous;
        establishCoordinatesForComputerOpen(limits);
        outTargetId = index + 1;
        return true;
    }

    bool PatchManagerActionHandler::prepareDroppedComputerFolderSelection(
        const juce::File& folder,
        const DeviceMemoryLimits& limits,
        int& outTargetId)
    {
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
            // Keep empty/unusable folder published (OPEN-like footers); no load.
            clearPendingCombinedScanLoadFooter();
            PatchFileServiceFooter::propagateScanResult(apvts_, scan);
            clearComputerPatchesSelection();
            clearComputerNavigationFocusIfOwned();
            pendingBrowserRestoreOnCancel_.reset();
            return false;
        }

        armPendingCombinedScanLoadFooter(scan.validCount, scan.invalidCount);
        pendingBrowserRestoreOnCancel_ = previous;
        establishCoordinatesForComputerOpen(limits);
        outTargetId = 1;
        return true;
    }

    bool PatchManagerActionHandler::prepareDroppedComputerVirtualSelection(
        const juce::Array<juce::File>& selection,
        const DeviceMemoryLimits& limits,
        int& outTargetId)
    {
        seedCommittedComputerPatchesSelectionIfNeeded();
        const auto previous = captureComputerPatchesBrowserSnapshot();

        const auto result = patchFileService_->mergeDroppedSelection(selection);
        bumpScanRevision();

        if (result.validCount < 1)
        {
            clearPendingCombinedScanLoadFooter();
            restoreComputerPatchesBrowser(previous);
            return false;
        }

        armPendingCombinedScanLoadFooter(result.validCount, result.invalidCount);

        // Virtual list must not overwrite the last real folder remembered for OPEN / Save As.
        pendingBrowserRestoreOnCancel_ = previous;
        establishCoordinatesForComputerOpen(limits);
        outTargetId = 1;
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
    PatchManagerActionHandler::selectAndLoadComputerPatchAtId(int targetId,
                                                              const DeviceMemoryLimits& limits)
    {
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

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::loadDroppedComputerPatchFile(const juce::File& file,
                                                            const DeviceMemoryLimits& limits)
    {
        juce::StringArray paths;
        paths.add(file.getFullPathName());
        return loadDroppedComputerPatchSelection(paths, limits);
    }

    namespace
    {
        struct DroppedPathShape
        {
            juce::Array<juce::File> files;
            int directoryCount = 0;
            int syxFileCount = 0;
        };

        DroppedPathShape classifyDroppedPaths(const juce::StringArray& paths)
        {
            DroppedPathShape shape;

            for (const auto& path : paths)
            {
                const juce::File file(path);
                shape.files.add(file);

                if (file.isDirectory())
                    ++shape.directoryCount;
                else if (file.getFileExtension().equalsIgnoreCase(PatchFileService::kSyxExtension))
                    ++shape.syxFileCount;
            }

            return shape;
        }
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::loadDroppedSingleFolder(const juce::File& folder,
                                                       const DeviceMemoryLimits& limits)
    {
        int targetId = 0;
        if (! prepareDroppedComputerFolderSelection(folder, limits, targetId))
            return DroppedComputerPatchLoadResult::kRejected;

        return selectAndLoadComputerPatchAtId(targetId, limits);
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::loadDroppedSingleSyxFile(const juce::File& file,
                                                        const DeviceMemoryLimits& limits)
    {
        const auto assessment = patchFileService_->assessSinglePatchSyxFile(file);
        if (! assessment.isValidSinglePatch)
            return rejectDroppedComputerPatch(assessment.rejectKind);

        int targetId = 0;
        if (! prepareDroppedComputerPatchSelection(file, limits, targetId))
            return rejectDroppedComputerPatch(SinglePatchSyxRejectKind::kInvalid);

        return selectAndLoadComputerPatchAtId(targetId, limits);
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::loadDroppedVirtualSelection(const juce::Array<juce::File>& files,
                                                           const DeviceMemoryLimits& limits)
    {
        int targetId = 0;
        if (! prepareDroppedComputerVirtualSelection(files, limits, targetId))
            return rejectDroppedComputerPatchWithMessage(FooterMessages::kDropRejectedNoValid);

        return selectAndLoadComputerPatchAtId(targetId, limits);
    }

    PatchManagerActionHandler::DroppedComputerPatchLoadResult
    PatchManagerActionHandler::loadDroppedComputerPatchSelection(const juce::StringArray& paths,
                                                                 const DeviceMemoryLimits& limits)
    {
        if (patchFileService_ == nullptr || paths.isEmpty())
            return rejectDroppedComputerPatch(SinglePatchSyxRejectKind::kInvalid);

        const auto shape = classifyDroppedPaths(paths);
        if (shape.directoryCount == 0 && shape.syxFileCount == 0)
            return rejectDroppedComputerPatch(SinglePatchSyxRejectKind::kNotSyx);

        if (shape.files.size() == 1 && shape.directoryCount == 1)
            return loadDroppedSingleFolder(shape.files[0], limits);

        if (shape.files.size() == 1 && shape.directoryCount == 0 && shape.syxFileCount == 1)
            return loadDroppedSingleSyxFile(shape.files[0], limits);

        return loadDroppedVirtualSelection(shape.files, limits);
    }

    void PatchManagerActionHandler::rescanAndSelectSavedFile(const juce::File& savedFile)
    {
        if (patchFileService_ == nullptr)
            return;

        if (patchFileService_->getLastScanResult().isVirtualList())
        {
            // In-list SAVE (same slot / case-fold rewrite): keep browsing that entry.
            // Save As outside the drop list: leave the list intact and clear selection
            // (<SELECT>), matching OPEN + Save As outside the opened folder.
            if (rewriteVirtualListEntryForSavedFile(savedFile))
                selectSavedFileInCurrentScan(savedFile);
            else
                clearComputerPatchesSelection();

            return;
        }

        const auto folder = resolveRescanFolder();
        if (! folder.isDirectory())
            return;

        patchFileService_->scanFolder(folder);
        selectSavedFileInCurrentScan(savedFile);
    }

    bool PatchManagerActionHandler::rewriteVirtualListEntryForSavedFile(const juce::File& savedFile)
    {
        const auto& scan = patchFileService_->getLastScanResult();
        juce::Array<juce::File> files = scan.sortedValidFiles;
        const auto savedParent = savedFile.getParentDirectory().getFullPathName();
        const auto savedName = savedFile.getFileName();
        const int invalidCount = scan.invalidCount;

        for (int i = 0; i < files.size(); ++i)
        {
            if (! files[i].getParentDirectory().getFullPathName().equalsIgnoreCase(savedParent))
                continue;

            if (! files[i].getFileName().equalsIgnoreCase(savedName))
                continue;

            files.set(i, savedFile);
            patchFileService_->installVirtualFileList(files, invalidCount);
            return true;
        }

        return false;
    }

    void PatchManagerActionHandler::selectSavedFileInCurrentScan(const juce::File& savedFile)
    {
        const auto& scan = patchFileService_->getLastScanResult();
        int index = -1;

        if (scan.isVirtualList())
        {
            const auto savedPath = savedFile.getFullPathName();

            for (int i = 0; i < scan.sortedValidFiles.size(); ++i)
            {
                if (scan.sortedValidFiles[i].getFullPathName() == savedPath)
                {
                    index = i;
                    break;
                }
            }
        }
        else
        {
            using namespace PatchManagerActionHandlerInternal;
            index = indexOfFileNameIgnoreCase(scan.sortedValidFileNames, savedFile.getFileName());
        }

        const int selectedId = index >= 0 ? index + 1 : 0;
        suppressComputerPatchesSelectLoad_ = true;
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
            selectedId,
            nullptr);
        suppressComputerPatchesSelectLoad_ = false;
        rememberComputerPatchesSelection(selectedId);
        bumpScanRevision();
    }


} // namespace Core
