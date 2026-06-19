#pragma once

#include <juce_core/juce_core.h>

class SysExDecoder;
class SysExEncoder;

namespace Core
{

    struct PatchFileSaveResult
    {
        bool success = false;
        juce::String errorMessage;
    };

    struct PatchFolderScanResult
    {
        juce::File folder;
        bool folderUsable = false;
        int validCount = 0;
        int invalidCount = 0;
        juce::StringArray sortedValidFileNames;
        juce::String footerMessage;
        juce::String footerSeverity;
    };

    class PatchFileService
    {
    public:
        static constexpr const char* kSyxExtension = ".syx";

        explicit PatchFileService(SysExDecoder& decoder) noexcept;

        PatchFolderScanResult scanFolder(const juce::File& folder);
        PatchFileSaveResult savePatchSysExFile(const juce::File& targetFile,
                                               const juce::uint8* packedData,
                                               SysExEncoder& encoder);
        const PatchFolderScanResult& getLastScanResult() const noexcept { return lastScan_; }
        bool hasCachedScanResult() const noexcept;
        void clearLastScan() noexcept;

    private:
        static bool hasSyxExtension(const juce::File& file) noexcept;
        static bool isFolderReadable(const juce::File& folder) noexcept;
        static juce::Array<juce::File> findSyxFiles(const juce::File& folder);
        bool validateFileContents(const juce::File& file) const;
        void appendValidFileName(juce::StringArray& names, const juce::File& file) const;
        void collectSyxScanResults(const juce::Array<juce::File>& syxFiles,
                                   juce::StringArray& validNames,
                                   int& validCount,
                                   int& invalidCount) const;
        PatchFolderScanResult scanReadableFolder(const juce::File& folder) const;
        PatchFolderScanResult makeUnusableFolderResult(const juce::File& folder) const;
        PatchFolderScanResult makeScanResult(const juce::File& folder,
                                             juce::StringArray validNames,
                                             int validCount,
                                             int invalidCount,
                                             int syxFileCount) const;
        void cacheResult(PatchFolderScanResult result);

        SysExDecoder& decoder_;
        PatchFolderScanResult lastScan_;
    };

} // namespace Core
