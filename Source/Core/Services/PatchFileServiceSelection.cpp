#include "Core/Services/PatchFileService.h"

#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace Core
{
    namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

    namespace
    {
        struct OpenListFileComparator
        {
            static int compareElements(const juce::File& first, const juce::File& second)
            {
                const int byName = MutationNaming::compareOpenListFileNames(
                    first.getFileName(), second.getFileName());

                if (byName != 0)
                    return byName;

                return first.getFullPathName().compareIgnoreCase(second.getFullPathName());
            }
        };
    }

    PatchFolderScanResult PatchFileService::mergeDroppedSelection(const juce::Array<juce::File>& selection)
    {
        juce::Array<juce::File> validFiles;
        int invalidCount = 0;

        for (const auto& path : selection)
        {
            if (path.isDirectory())
            {
                if (! isFolderReadable(path))
                    continue;

                int folderValid = 0;
                int folderInvalid = 0;
                collectValidAbsoluteFiles(findSyxFiles(path), validFiles, folderValid, folderInvalid);
                invalidCount += folderInvalid;
                continue;
            }

            if (! hasSyxExtension(path))
            {
                ++invalidCount;
                continue;
            }

            if (validateFileContents(path))
                appendUniqueAbsoluteFile(validFiles, path);
            else
                ++invalidCount;
        }

        sortOpenListFiles(validFiles);
        const auto result = makeVirtualListResult(std::move(validFiles), invalidCount);
        cacheResult(result);
        return result;
    }

    void PatchFileService::installVirtualFileList(const juce::Array<juce::File>& absoluteFiles,
                                                  int invalidCount)
    {
        juce::Array<juce::File> files = absoluteFiles;
        sortOpenListFiles(files);
        cacheResult(makeVirtualListResult(std::move(files), invalidCount));
    }

    void PatchFileService::collectValidAbsoluteFiles(const juce::Array<juce::File>& syxFiles,
                                                     juce::Array<juce::File>& validFiles,
                                                     int& validCount,
                                                     int& invalidCount) const
    {
        for (const auto& file : syxFiles)
        {
            if (! validateFileContents(file))
            {
                ++invalidCount;
                continue;
            }

            const int before = validFiles.size();
            appendUniqueAbsoluteFile(validFiles, file);

            if (validFiles.size() > before)
                ++validCount;
        }
    }

    void PatchFileService::sortOpenListFiles(juce::Array<juce::File>& files)
    {
        OpenListFileComparator comparator;
        files.sort(comparator);
    }

    void PatchFileService::appendUniqueAbsoluteFile(juce::Array<juce::File>& files, const juce::File& file)
    {
        const auto path = file.getFullPathName();

        for (const auto& existing : files)
        {
            if (existing.getFullPathName().equalsIgnoreCase(path))
                return;
        }

        files.add(file);
    }

    PatchFolderScanResult PatchFileService::makeVirtualListResult(juce::Array<juce::File> validFiles,
                                                                  int invalidCount) const
    {
        PatchFolderScanResult result;
        result.listMode = PatchScanListMode::kVirtualList;
        result.folderUsable = true;
        result.validCount = validFiles.size();
        result.invalidCount = invalidCount;
        result.sortedValidFiles = std::move(validFiles);
        result.footerSeverity = "info";

        for (const auto& file : result.sortedValidFiles)
            result.sortedValidFileNames.add(file.getFileName());

        if (result.validCount == 0 && result.invalidCount == 0)
            result.footerMessage = FooterMessages::kEmptyFolder;
        else
            result.footerMessage = FooterMessages::formatScanSummary(result.validCount, result.invalidCount);

        return result;
    }

} // namespace Core
