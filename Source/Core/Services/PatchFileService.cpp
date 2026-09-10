#include "Core/Services/PatchFileService.h"

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace Core
{
    namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

    namespace
    {
        struct OpenListFileNameComparator
        {
            static int compareElements(const juce::String& first, const juce::String& second)
            {
                return MutationNaming::compareOpenListFileNames(first, second);
            }
        };

    }

    PatchFileService::PatchFileService(SysExDecoder& decoder) noexcept
        : decoder_(decoder)
    {
    }

    bool PatchFileService::looksLikeBankOrMultiMessageDump(const juce::MemoryBlock& sysEx) noexcept
    {
        if (sysEx.getSize() > SysExConstants::kPatchMessageLength)
            return true;

        int startCount = 0;
        const auto* bytes = static_cast<const juce::uint8*>(sysEx.getData());
        for (size_t i = 0; i < sysEx.getSize(); ++i)
        {
            if (bytes[i] == SysExConstants::kSysExStart)
                ++startCount;
        }

        return startCount > 1;
    }

    juce::String PatchFileService::resolveDragPreviewPrimaryName(const juce::File& file,
                                                                 const juce::uint8* packedData) const
    {
        PatchModel model;
        model.loadFrom(packedData);
        model.normalizeNameEncoding();

        const auto name = model.getName();
        if (! PatchFileNameSanitizer::isUsablePatchName(name)
            || PatchFileNameSanitizer::isOberheimBankPlaceholderName(name))
        {
            return file.getFileNameWithoutExtension();
        }

        return name;
    }

    SinglePatchSyxAssessment PatchFileService::assessSinglePatchSyxFile(const juce::File& file) const
    {
        SinglePatchSyxAssessment assessment;

        if (! file.existsAsFile())
        {
            assessment.rejectKind = SinglePatchSyxRejectKind::kInvalid;
            return assessment;
        }

        if (! hasSyxExtension(file))
        {
            assessment.rejectKind = SinglePatchSyxRejectKind::kNotSyx;
            return assessment;
        }

        if (! validateFileContents(file))
        {
            juce::MemoryBlock sysEx;
            if (! file.loadFileAsData(sysEx))
            {
                assessment.rejectKind = SinglePatchSyxRejectKind::kInvalid;
                return assessment;
            }

            assessment.rejectKind = looksLikeBankOrMultiMessageDump(sysEx)
                ? SinglePatchSyxRejectKind::kBankOrMultiMessage
                : SinglePatchSyxRejectKind::kInvalid;
            return assessment;
        }

        juce::MemoryBlock sysEx;
        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! file.loadFileAsData(sysEx) || ! decoder_.decodePatchSysEx(sysEx, packed))
        {
            assessment.rejectKind = SinglePatchSyxRejectKind::kInvalid;
            return assessment;
        }

        assessment.isValidSinglePatch = true;
        assessment.rejectKind = SinglePatchSyxRejectKind::kNone;
        assessment.previewPrimaryName = resolveDragPreviewPrimaryName(file, packed);
        return assessment;
    }

    bool PatchFileService::isValidSinglePatchSyxFile(const juce::File& file) const
    {
        return assessSinglePatchSyxFile(file).isValidSinglePatch;
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
            || scan.folder.getFullPathName().isNotEmpty()
            || scan.isVirtualList();
    }

    void PatchFileService::clearLastScan() noexcept
    {
        lastScan_ = {};
    }

    juce::File PatchFileService::withSyxExtension(const juce::File& file)
    {
        return file.hasFileExtension(kSyxExtension)
            ? file
            : file.withFileExtension(kSyxExtension);
    }

    PatchFileSaveResult PatchFileService::makeSaveFailure(const char* message)
    {
        PatchFileSaveResult result;
        result.errorMessage = message;
        return result;
    }

    bool PatchFileService::validateTempSyxContents(const juce::File& tempFile) const
    {
        juce::MemoryBlock readBack;
        return tempFile.loadFileAsData(readBack) && decoder_.validatePatchSysExMessage(readBack);
    }

    bool PatchFileService::replaceFileWithTemp(const juce::File& target, juce::File& tempFile)
    {
        if (target.existsAsFile() && ! target.deleteFile())
            return false;

        return tempFile.moveFileTo(target);
    }

    PatchFileSaveResult PatchFileService::finalizeTempSyxWrite(const juce::File& target,
                                                               juce::File tempFile)
    {
        if (! validateTempSyxContents(tempFile))
        {
            tempFile.deleteFile();
            return makeSaveFailure("Validation failed");
        }

        if (! replaceFileWithTemp(target, tempFile))
        {
            tempFile.deleteFile();
            return makeSaveFailure("Write failed");
        }

        PatchFileSaveResult result;
        result.success = true;
        return result;
    }

    PatchFileSaveResult PatchFileService::savePatchSysExFile(const juce::File& targetFile,
                                                             const juce::uint8* packedData,
                                                             SysExEncoder& encoder,
                                                             int patchNumber)
    {
        if (packedData == nullptr)
            return makeSaveFailure("Invalid patch data");

        const auto target = withSyxExtension(targetFile);
        const auto parent = target.getParentDirectory();
        if (! parent.isDirectory() || ! parent.hasWriteAccess())
            return makeSaveFailure("Folder not writable");

        (void) patchNumber;
        const auto encoded = encoder.encodePatchToEditBufferSysEx(packedData);
        if (encoded.getSize() == 0)
            return makeSaveFailure("Encode failed");

        const auto tempFile = parent.getNonexistentChildFile(
            target.getFileNameWithoutExtension() + "_write",
            ".tmp");

        if (! tempFile.replaceWithData(encoded.getData(), encoded.getSize()))
            return makeSaveFailure("Write failed");

        return finalizeTempSyxWrite(target, tempFile);
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
        FolderScanCounts counts;
        counts.syxFileCount = syxFiles.size();

        collectSyxScanResults(syxFiles, validNames, counts.validCount, counts.invalidCount);
        sortOpenListFileNames(validNames);

        return makeScanResult(folder, std::move(validNames), counts);
    }

    void PatchFileService::sortOpenListFileNames(juce::StringArray& validNames)
    {
        juce::Array<juce::String> ordered;

        for (const auto& name : validNames)
            ordered.add(name);

        OpenListFileNameComparator comparator;
        ordered.sort(comparator);
        validNames.clearQuick();

        for (const auto& name : ordered)
            validNames.add(name);
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
                                                           const FolderScanCounts& counts) const
    {
        PatchFolderScanResult result;
        result.folder = folder;
        result.folderUsable = true;
        result.listMode = PatchScanListMode::kFolder;
        result.validCount = counts.validCount;
        result.invalidCount = counts.invalidCount;
        result.sortedValidFileNames = std::move(validNames);
        result.footerSeverity = "info";

        if (counts.syxFileCount == 0)
            result.footerMessage = FooterMessages::kEmptyFolder;
        else
            result.footerMessage = FooterMessages::formatScanSummary(counts.validCount, counts.invalidCount);

        return result;
    }


    void PatchFileService::cacheResult(PatchFolderScanResult result)
    {
        lastScan_ = std::move(result);
    }

} // namespace Core
