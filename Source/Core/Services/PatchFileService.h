#pragma once

#include <juce_core/juce_core.h>

class SysExDecoder;
class SysExEncoder;

namespace Core
{

    class MutationHistoryStore;

    struct PatchFileSaveResult
    {
        bool success = false;
        juce::String errorMessage;
    };

    struct PatchFileLoadResult
    {
        bool success = false;
        juce::String errorMessage;
    };

    struct PatchFileExportResult
    {
        bool success = false;
        int filesWritten = 0;
        juce::String errorMessage;
    };

    enum class PatchScanListMode
    {
        kFolder,
        kVirtualList
    };

    struct PatchFolderScanResult
    {
        juce::File folder;
        bool folderUsable = false;
        PatchScanListMode listMode = PatchScanListMode::kFolder;
        int validCount = 0;
        int invalidCount = 0;
        // Display stems / basenames for the combo (folder and virtual). Homonyms allowed.
        juce::StringArray sortedValidFileNames;
        // Absolute files for virtual list; empty in folder mode (resolve via folder + name).
        juce::Array<juce::File> sortedValidFiles;
        juce::String footerMessage;
        juce::String footerSeverity;

        bool isVirtualList() const noexcept
        {
            return listMode == PatchScanListMode::kVirtualList;
        }
    };

    struct MutatorExportWriteArgs
    {
        const MutationHistoryStore& store;
        SysExEncoder& encoder;
        const juce::String& userPatchName;
    };

    struct MutatorHistorySessionExportArgs
    {
        juce::File sessionFolder;
        const MutationHistoryStore& store;
        SysExEncoder& encoder;
        bool clearExisting = false;
        juce::String userPatchName;
    };

    struct FolderScanCounts
    {
        int validCount = 0;
        int invalidCount = 0;
        int syxFileCount = 0;
    };

    // Single-file .syx gate shared by Computer Patches scan and drag-drop preview/reject.
    enum class SinglePatchSyxRejectKind
    {
        kNone,
        kNotSyx,
        kBankOrMultiMessage,
        kInvalid
    };

    struct SinglePatchSyxAssessment
    {
        bool isValidSinglePatch = false;
        SinglePatchSyxRejectKind rejectKind = SinglePatchSyxRejectKind::kNone;
        // When valid: internal Matrix name, or OS stem when empty / BNK* / unusable.
        juce::String previewPrimaryName;
    };

    class PatchFileService
    {
    public:
        static constexpr const char* kSyxExtension = ".syx";

        explicit PatchFileService(SysExDecoder& decoder) noexcept;

        // Public SSOT for drag preview and drop reject (wraps the same validate as scan).
        SinglePatchSyxAssessment assessSinglePatchSyxFile(const juce::File& file) const;
        bool isValidSinglePatchSyxFile(const juce::File& file) const;

        PatchFolderScanResult scanFolder(const juce::File& folder);
        // Non-recursive per selected folder + direct .syx files → absolute valid paths, sorted.
        // Caches as virtual-list mode (does not invent a parent folder).
        PatchFolderScanResult mergeDroppedSelection(const juce::Array<juce::File>& selection);
        // Re-install a previously validated virtual list (cancel restore) without re-scanning disk.
        // invalidCount is preserved for scan-summary footers (cancel restore / in-list SAVE rewrite).
        void installVirtualFileList(const juce::Array<juce::File>& absoluteFiles, int invalidCount = 0);
        // patchNumber is retained for call-site compatibility (bank export passes slot 0-99) but
        // does not affect the SysEx header: all on-disk .syx use opcode 0x0D with header byte 0.
        // Slot semantics live in the filename (e.g. bank export Pxx stems) and Bank Import, not
        // in the SysEx slot byte.
        PatchFileSaveResult savePatchSysExFile(const juce::File& targetFile,
                                               const juce::uint8* packedData,
                                               SysExEncoder& encoder,
                                               int patchNumber = 0);
        PatchFileLoadResult loadPatchSysExFile(const juce::File& file, juce::uint8* packedOut);
        // userPatchName is the live SSOT musical name (model + APVTS), stamped onto bytes
        // 0-7 of every exported .syx before encoding. Filenames stay flat INITIAL.syx /
        // Mxx.syx / Mxx-Ryy.syx via MutationNaming's format helpers, independent of the
        // packed buffer's name bytes.
        PatchFileExportResult exportMutatorHistory(const juce::File& folder,
                                                   const MutationHistoryStore& store,
                                                   SysExEncoder& encoder,
                                                   const juce::String& userPatchName);
        // Session-folder export: writes the flat mutator layout inside sessionFolder.
        // When clearExisting is true the folder contents are removed first (Overwrite);
        // otherwise the folder must be created fresh.
        PatchFileExportResult exportMutatorHistorySession(const MutatorHistorySessionExportArgs& args);
        // First non-existing session folder among basename, basename-2, basename-3, … (Keep).
        static juce::File resolveKeepSessionFolder(const juce::File& parentFolder,
                                                   const juce::String& basename);
        const PatchFolderScanResult& getLastScanResult() const noexcept { return lastScan_; }
        bool hasCachedScanResult() const noexcept;
        void clearLastScan() noexcept;

    private:
        static bool hasSyxExtension(const juce::File& file) noexcept;
        static bool isFolderReadable(const juce::File& folder) noexcept;
        static juce::Array<juce::File> findSyxFiles(const juce::File& folder);
        static juce::File withSyxExtension(const juce::File& file);
        static PatchFileSaveResult makeSaveFailure(const char* message);
        bool validateTempSyxContents(const juce::File& tempFile) const;
        static bool replaceFileWithTemp(const juce::File& target, juce::File& tempFile);
        PatchFileSaveResult finalizeTempSyxWrite(const juce::File& target, juce::File tempFile);
        bool validateFileContents(const juce::File& file) const;
        static bool looksLikeBankOrMultiMessageDump(const juce::MemoryBlock& sysEx) noexcept;
        juce::String resolveDragPreviewPrimaryName(const juce::File& file,
                                                  const juce::uint8* packedData) const;
        void appendValidFileName(juce::StringArray& names, const juce::File& file) const;
        void collectSyxScanResults(const juce::Array<juce::File>& syxFiles,
                                   juce::StringArray& validNames,
                                   int& validCount,
                                   int& invalidCount) const;
        void collectValidAbsoluteFiles(const juce::Array<juce::File>& syxFiles,
                                       juce::Array<juce::File>& validFiles,
                                       int& validCount,
                                       int& invalidCount) const;
        static void sortOpenListFileNames(juce::StringArray& validNames);
        static void sortOpenListFiles(juce::Array<juce::File>& files);
        static void appendUniqueAbsoluteFile(juce::Array<juce::File>& files, const juce::File& file);
        PatchFolderScanResult scanReadableFolder(const juce::File& folder) const;
        PatchFolderScanResult makeUnusableFolderResult(const juce::File& folder) const;
        PatchFolderScanResult makeScanResult(const juce::File& folder,
                                             juce::StringArray validNames,
                                             const FolderScanCounts& counts) const;
        PatchFolderScanResult makeVirtualListResult(juce::Array<juce::File> validFiles,
                                                    int invalidCount) const;
        void cacheResult(PatchFolderScanResult result);
        PatchFileExportResult validateMutatorExport(const juce::File& folder,
                                                    const MutationHistoryStore& store);
        PatchFileExportResult writeInitialSnapshot(const juce::File& folder,
                                                   const MutatorExportWriteArgs& args);
        PatchFileExportResult writeExportPatchFile(const juce::File& file,
                                                   const juce::uint8* packedData,
                                                   const MutatorExportWriteArgs& args);
        PatchFileExportResult writeRootEntry(const juce::File& folder,
                                             int rootIndex,
                                             const MutatorExportWriteArgs& args);
        PatchFileExportResult writeRetryEntries(const juce::File& folder,
                                                int rootIndex,
                                                const MutatorExportWriteArgs& args);
        PatchFileExportResult writeRootAndRetries(const juce::File& folder,
                                                  int rootIndex,
                                                  const MutatorExportWriteArgs& args);
        PatchFileExportResult writeAllRoots(const juce::File& folder,
                                            const MutatorExportWriteArgs& args);
        PatchFileExportResult writeHistoryLayout(const juce::File& folder,
                                                 const MutatorExportWriteArgs& args);

        SysExDecoder& decoder_;
        PatchFolderScanResult lastScan_;
    };

} // namespace Core
