#include "Core/Services/PatchFileService.h"

#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace Core
{
    namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

    PatchFileService::PatchFileService(SysExDecoder& decoder) noexcept
        : decoder_(decoder)
    {
    }

    PatchFolderScanResult PatchFileService::scanFolder(const juce::File& folder)
    {
        if (! folder.isDirectory() || ! isFolderReadable(folder))
        {
            const auto result = makeUnusableFolderResult(folder);
            cacheResult(result);
            return result;
        }

        const auto result = scanReadableFolder(folder);
        cacheResult(result);
        return result;
    }

    bool PatchFileService::hasCachedScanResult() const noexcept
    {
        const auto& scan = lastScan_;
        return scan.folderUsable
            || scan.validCount > 0
            || scan.invalidCount > 0
            || scan.footerMessage.isNotEmpty()
            || scan.folder.getFullPathName().isNotEmpty();
    }

    void PatchFileService::clearLastScan() noexcept
    {
        lastScan_ = {};
    }

    PatchFileSaveResult PatchFileService::savePatchSysExFile(const juce::File& targetFile,
                                                             const juce::uint8* packedData,
                                                             SysExEncoder& encoder)
    {
        PatchFileSaveResult result;

        if (packedData == nullptr)
        {
            result.errorMessage = "Invalid patch data";
            return result;
        }

        const auto target = targetFile.hasFileExtension(kSyxExtension)
            ? targetFile
            : targetFile.withFileExtension(kSyxExtension);

        const auto parent = target.getParentDirectory();
        if (! parent.isDirectory() || ! parent.hasWriteAccess())
        {
            result.errorMessage = "Folder not writable";
            return result;
        }

        const auto encoded = encoder.encodePatchSysEx(0, packedData);
        if (encoded.getSize() == 0)
        {
            result.errorMessage = "Encode failed";
            return result;
        }

        const auto tempFile = parent.getNonexistentChildFile(
            target.getFileNameWithoutExtension() + "_write",
            ".tmp");

        if (! tempFile.replaceWithData(encoded.getData(), encoded.getSize()))
        {
            result.errorMessage = "Write failed";
            return result;
        }

        juce::MemoryBlock readBack;
        if (! tempFile.loadFileAsData(readBack) || ! decoder_.validatePatchSysExMessage(readBack))
        {
            tempFile.deleteFile();
            result.errorMessage = "Validation failed";
            return result;
        }

        if (target.existsAsFile() && ! target.deleteFile())
        {
            tempFile.deleteFile();
            result.errorMessage = "Write failed";
            return result;
        }

        if (! tempFile.moveFileTo(target))
        {
            tempFile.deleteFile();
            result.errorMessage = "Write failed";
            return result;
        }

        result.success = true;
        return result;
    }

    PatchFileExportResult PatchFileService::validateMutatorExport(const juce::File& folder,
                                                                    const MutationHistoryStore& store)
    {
        PatchFileExportResult result;

        if (store.isEmpty())
        {
            result.errorMessage = "History empty";
            return result;
        }

        if (! folder.isDirectory() || ! folder.hasWriteAccess())
            result.errorMessage = "Folder not writable";
        else
            result.success = true;

        return result;
    }

    PatchFileExportResult PatchFileService::writeInitialSnapshot(const juce::File& folder,
                                                                 const MutationHistoryStore& store,
                                                                 SysExEncoder& encoder)
    {
        const auto initialFile = folder.getChildFile("Initial.syx");
        return writeExportPatchFile(initialFile, store.getInitialSnapshot().data(), encoder);
    }

    PatchFileExportResult PatchFileService::writeExportPatchFile(const juce::File& file,
                                                                 const juce::uint8* packedData,
                                                                 SysExEncoder& encoder)
    {
        PatchFileExportResult result;
        const auto save = savePatchSysExFile(file, packedData, encoder);

        if (! save.success)
        {
            result.errorMessage = save.errorMessage;
            return result;
        }

        result.success = true;
        result.filesWritten = 1;
        return result;
    }

    PatchFileExportResult PatchFileService::writeRootEntry(const juce::File& rootDir,
                                                           int rootIndex,
                                                           const MutationHistoryStore& store,
                                                           SysExEncoder& encoder)
    {
        PatchFileExportResult result;
        result.success = true;

        if (const auto rootEntry = store.getEntry(rootIndex, MutationHistoryStore::kRootOnly))
        {
            const auto rootLabel = MutationNaming::formatRootLabel(rootIndex);
            const auto rootFile = rootDir.getChildFile(
                PatchFileNameSanitizer::ensureSyxExtension(rootLabel));
            return writeExportPatchFile(rootFile, rootEntry->result.data(), encoder);
        }

        return result;
    }

    PatchFileExportResult PatchFileService::writeRetryEntries(const juce::File& rootDir,
                                                              int rootIndex,
                                                              const MutationHistoryStore& store,
                                                              SysExEncoder& encoder)
    {
        PatchFileExportResult result;
        result.success = true;

        for (const auto retryIndex : store.getSortedRetryIndices(rootIndex))
        {
            if (const auto retryEntry = store.getEntry(rootIndex, retryIndex))
            {
                const auto stem = MutationNaming::formatExportStem(rootIndex, retryIndex);
                const auto retryFile = rootDir.getChildFile(
                    PatchFileNameSanitizer::ensureSyxExtension(stem));
                const auto write = writeExportPatchFile(retryFile, retryEntry->result.data(), encoder);

                if (! write.success)
                    return write;

                result.filesWritten += write.filesWritten;
            }
        }

        return result;
    }

    PatchFileExportResult PatchFileService::writeRootFolder(const juce::File& folder,
                                                            int rootIndex,
                                                            const MutationHistoryStore& store,
                                                            SysExEncoder& encoder)
    {
        PatchFileExportResult result;
        const auto rootDir = folder.getChildFile(MutationNaming::formatRootLabel(rootIndex));

        if (! rootDir.createDirectory())
        {
            result.errorMessage = "Folder not writable";
            return result;
        }

        const auto rootWrite = writeRootEntry(rootDir, rootIndex, store, encoder);
        if (! rootWrite.success)
            return rootWrite;

        result.filesWritten += rootWrite.filesWritten;

        const auto retryWrite = writeRetryEntries(rootDir, rootIndex, store, encoder);
        if (! retryWrite.success)
            return retryWrite;

        result.filesWritten += retryWrite.filesWritten;
        result.success = true;
        return result;
    }

    PatchFileExportResult PatchFileService::writeAllRootFolders(const juce::File& folder,
                                                                const MutationHistoryStore& store,
                                                                SysExEncoder& encoder)
    {
        PatchFileExportResult result;

        for (const auto rootIndex : store.getSortedRootIndices())
        {
            const auto rootWrite = writeRootFolder(folder, rootIndex, store, encoder);

            if (! rootWrite.success)
                return rootWrite;

            result.filesWritten += rootWrite.filesWritten;
        }

        result.success = true;
        return result;
    }

    PatchFileExportResult PatchFileService::exportMutatorHistory(const juce::File& folder,
                                                                 const MutationHistoryStore& store,
                                                                 SysExEncoder& encoder)
    {
        const auto validation = validateMutatorExport(folder, store);
        if (! validation.success)
            return validation;

        PatchFileExportResult result;

        if (store.hasInitialSnapshot())
        {
            const auto initialWrite = writeInitialSnapshot(folder, store, encoder);
            if (! initialWrite.success)
                return initialWrite;

            result.filesWritten += initialWrite.filesWritten;
        }

        const auto rootsWrite = writeAllRootFolders(folder, store, encoder);
        if (! rootsWrite.success)
            return rootsWrite;

        result.filesWritten += rootsWrite.filesWritten;
        result.success = true;
        return result;
    }

    PatchFileLoadResult PatchFileService::loadPatchSysExFile(const juce::File& file, juce::uint8* packedOut)
    {
        PatchFileLoadResult result;

        if (packedOut == nullptr)
        {
            result.errorMessage = "Invalid patch buffer";
            return result;
        }

        if (! file.existsAsFile())
        {
            result.errorMessage = "File not found";
            return result;
        }

        juce::MemoryBlock sysEx;
        if (! file.loadFileAsData(sysEx))
        {
            result.errorMessage = "Read failed";
            return result;
        }

        if (! decoder_.validatePatchSysExMessage(sysEx))
        {
            result.errorMessage = "Invalid patch file";
            return result;
        }

        if (! decoder_.decodePatchSysEx(sysEx, packedOut))
        {
            result.errorMessage = "Decode failed";
            return result;
        }

        result.success = true;
        return result;
    }

    bool PatchFileService::hasSyxExtension(const juce::File& file) noexcept
    {
        return file.getFileExtension().toLowerCase() == kSyxExtension;
    }

    bool PatchFileService::isFolderReadable(const juce::File& folder) noexcept
    {
        return folder.hasReadAccess();
    }

    juce::Array<juce::File> PatchFileService::findSyxFiles(const juce::File& folder)
    {
        const auto files = folder.findChildFiles(juce::File::findFiles, false);
        juce::Array<juce::File> syxFiles;

        for (const auto& file : files)
        {
            if (hasSyxExtension(file))
                syxFiles.add(file);
        }

        return syxFiles;
    }

    void PatchFileService::collectSyxScanResults(const juce::Array<juce::File>& syxFiles,
                                                 juce::StringArray& validNames,
                                                 int& validCount,
                                                 int& invalidCount) const
    {
        for (const auto& file : syxFiles)
        {
            if (validateFileContents(file))
            {
                appendValidFileName(validNames, file);
                ++validCount;
            }
            else
            {
                ++invalidCount;
            }
        }
    }

    PatchFolderScanResult PatchFileService::scanReadableFolder(const juce::File& folder) const
    {
        const auto syxFiles = findSyxFiles(folder);
        juce::StringArray validNames;
        int validCount = 0;
        int invalidCount = 0;

        collectSyxScanResults(syxFiles, validNames, validCount, invalidCount);
        validNames.sort(false);

        return makeScanResult(folder, std::move(validNames), validCount, invalidCount, syxFiles.size());
    }

    bool PatchFileService::validateFileContents(const juce::File& file) const
    {
        juce::MemoryBlock sysEx;

        if (! file.loadFileAsData(sysEx))
            return false;

        return decoder_.validatePatchSysExMessage(sysEx);
    }

    void PatchFileService::appendValidFileName(juce::StringArray& names, const juce::File& file) const
    {
        names.add(file.getFileName());
    }

    PatchFolderScanResult PatchFileService::makeUnusableFolderResult(const juce::File& folder) const
    {
        PatchFolderScanResult result;
        result.folder = folder;
        result.folderUsable = false;
        result.footerMessage = FooterMessages::kFolderNotFound;
        result.footerSeverity = "warning";
        return result;
    }

    PatchFolderScanResult PatchFileService::makeScanResult(const juce::File& folder,
                                                           juce::StringArray validNames,
                                                           int validCount,
                                                           int invalidCount,
                                                           int syxFileCount) const
    {
        PatchFolderScanResult result;
        result.folder = folder;
        result.folderUsable = true;
        result.validCount = validCount;
        result.invalidCount = invalidCount;
        result.sortedValidFileNames = std::move(validNames);
        result.footerSeverity = "info";

        if (syxFileCount == 0)
            result.footerMessage = FooterMessages::kEmptyFolder;
        else
            result.footerMessage = FooterMessages::formatScanSummary(validCount, invalidCount);

        return result;
    }

    void PatchFileService::cacheResult(PatchFolderScanResult result)
    {
        lastScan_ = std::move(result);
    }

} // namespace Core
