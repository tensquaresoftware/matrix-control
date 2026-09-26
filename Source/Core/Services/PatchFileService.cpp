#include "Core/Services/PatchFileService.h"

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchM1kpCodec.h"
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

    juce::String PatchFileService::formatOpenListDisplayName(const juce::File& file)
    {
        const auto stem = file.getFileNameWithoutExtension();

        if (hasM1kpExtension(file))
            return stem + " (m1kp)";

        return stem;
    }

    juce::String PatchFileService::formatOpenListDisplayName(const juce::String& fileName)
    {
        return formatOpenListDisplayName(juce::File::createFileWithoutCheckingPath(fileName));
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
        // .m1kp drop always forces the sanitized filename stem — hover must match.
        if (hasM1kpExtension(file))
            return PatchFileNameSanitizer::sanitizeFileStem(file.getFileNameWithoutExtension());

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

        if (! hasSupportedPatchExtension(file))
        {
            assessment.rejectKind = SinglePatchSyxRejectKind::kNotSyx;
            return assessment;
        }

        if (hasM1kpExtension(file)
            && file.getSize() != static_cast<juce::int64>(PatchM1kpCodec::kFileByteSize))
        {
            assessment.rejectKind = SinglePatchSyxRejectKind::kInvalid;
            return assessment;
        }

        juce::MemoryBlock data;
        if (! file.loadFileAsData(data))
        {
            assessment.rejectKind = SinglePatchSyxRejectKind::kInvalid;
            return assessment;
        }

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! decodePackedFromFile(file, data, packed))
        {
            assessment.rejectKind = hasSyxExtension(file) && looksLikeBankOrMultiMessageDump(data)
                ? SinglePatchSyxRejectKind::kBankOrMultiMessage
                : SinglePatchSyxRejectKind::kInvalid;
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

        if (hasM1kpExtension(file)
            && file.getSize() != static_cast<juce::int64>(PatchM1kpCodec::kFileByteSize))
        {
            result.errorMessage = "Invalid patch file";
            return result;
        }

        juce::MemoryBlock data;
        if (! file.loadFileAsData(data))
        {
            result.errorMessage = "Read failed";
            return result;
        }

        if (! decodePackedFromFile(file, data, packedOut))
        {
            result.errorMessage = "Invalid patch file";
            return result;
        }

        result.success = true;
        return result;
    }

    bool PatchFileService::hasSyxExtension(const juce::File& file) noexcept
    {
        return file.getFileExtension().equalsIgnoreCase(kSyxExtension);
    }

    bool PatchFileService::hasM1kpExtension(const juce::File& file) noexcept
    {
        return PatchM1kpCodec::hasExtension(file);
    }

    bool PatchFileService::hasSupportedPatchExtension(const juce::File& file) noexcept
    {
        return hasSyxExtension(file) || hasM1kpExtension(file);
    }

    bool PatchFileService::isFolderReadable(const juce::File& folder) noexcept
    {
        return folder.hasReadAccess();
    }

    juce::Array<juce::File> PatchFileService::findPatchFiles(const juce::File& folder)
    {
        const auto files = folder.findChildFiles(juce::File::findFiles, false);
        juce::Array<juce::File> patchFiles;

        for (const auto& file : files)
        {
            if (hasSupportedPatchExtension(file))
                patchFiles.add(file);
        }

        return patchFiles;
    }

    void PatchFileService::collectPatchScanResults(const juce::Array<juce::File>& patchFiles,
                                                   juce::StringArray& validNames,
                                                   int& validCount,
                                                   int& invalidCount) const
    {
        for (const auto& file : patchFiles)
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
        const auto patchFiles = findPatchFiles(folder);
        juce::StringArray validNames;
        FolderScanCounts counts;
        counts.patchFileCount = patchFiles.size();

        collectPatchScanResults(patchFiles, validNames, counts.validCount, counts.invalidCount);
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



    bool PatchFileService::decodePackedFromFile(const juce::File& file,
                                                const juce::MemoryBlock& data,
                                                juce::uint8* packedOut) const
    {
        if (packedOut == nullptr)
            return false;

        if (hasM1kpExtension(file))
            return PatchM1kpCodec::decodeToPacked(data, packedOut);

        if (hasSyxExtension(file))
            return decoder_.decodePatchSysEx(data, packedOut);

        return false;
    }

    bool PatchFileService::validateFileContents(const juce::File& file) const
    {
        if (hasM1kpExtension(file)
            && file.getSize() != static_cast<juce::int64>(PatchM1kpCodec::kFileByteSize))
        {
            return false;
        }

        juce::MemoryBlock data;

        if (! file.loadFileAsData(data))
            return false;

        if (hasM1kpExtension(file))
            return PatchM1kpCodec::isValidContents(data);

        if (hasSyxExtension(file))
            return decoder_.validatePatchSysExMessage(data);

        return false;
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

        if (counts.patchFileCount == 0)
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
