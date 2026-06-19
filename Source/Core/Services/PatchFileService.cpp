#include "Core/Services/PatchFileService.h"

#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
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
