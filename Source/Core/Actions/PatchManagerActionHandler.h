#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/BankTransferProgressPresenter.h"
#include "Core/Actions/IActionHandler.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchNameOverlayStore.h"
#include "Core/Services/UnsavedEditWarningPolicy.h"
#include "Core/Util/ComboboxPatchSendDebouncer.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

class MidiManager;
class SysExEncoder;

namespace Core
{

    class ApvtsPatchMapper;
    class ClipboardService;
    class DirtyPatchTracker;
    class PatchFileService;
    class PatchInitService;
    class PatchNameSyncer;
    class PatchSelectionMidiSync;

    class PatchManagerActionHandler final : public IActionHandler
    {
    public:
        using DeviceMemoryLimitsSupplier = std::function<DeviceMemoryLimits()>;
        using PatchFolderPicker = std::function<juce::File()>;
        using PatchSaveFilePicker = std::function<juce::File(juce::File suggestedFolder,
                                                             juce::String suggestedStem)>;
        using PatchNameReconciliationPicker = PatchFileNameReconciler::Picker;

        // Bank Utility IMPORT/EXPORT: OS folder pickers (reuse the PatchFolderPicker shape) and
        // an ordered Cancel/Continue confirm gate for IMPORT. See BankTransferProgressPresenter.h
        // for the progress-modal presenter type shared with PluginProcessor.
        using BankImportConfirmGate = std::function<bool()>;
        using BankPasteConfirmGate = std::function<bool(int sourceBank, int targetBank)>;

        struct Dependencies
        {
            juce::AudioProcessorValueTreeState& apvts;
            DeviceMemoryLimitsSupplier deviceMemoryLimits;
            PatchModel* patchModel = nullptr;
            ApvtsPatchMapper* apvtsPatchMapper = nullptr;
            ClipboardService* clipboardService = nullptr;
            PatchInitService* patchInitService = nullptr;
            PatchSelectionMidiSync* patchSelectionMidiSync = nullptr;
            MidiManager* midiManager = nullptr;
            PatchFileService* patchFileService = nullptr;
            PatchNameSyncer* patchNameSyncer = nullptr;
            DirtyPatchTracker* dirtyPatchTracker = nullptr;
            SysExEncoder* sysExEncoder = nullptr;
            PatchFolderPicker pickFolder;
            PatchSaveFilePicker pickSaveFile;
            PatchNameReconciliationPicker pickNameReconciliation;
            int patchNavButtonDebounceMs = kPatchNavButtonDebounceMs;
            int computerSelectDebounceMs = kComboboxPatchSendDebounceMs;
        };

        struct InternalCoordinatesSnapshot
        {
            int bank = 0;
            int patch = 0;
            int selectedBank = 0;
            bool coordinatesEstablished = false;
            int navigationFocus = PluginIDs::PatchManagerSection::NavigationFocus::kDefault;
        };

        PatchManagerActionHandler(Dependencies dependencies, ActionExecutionHooks hooks);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

        // Bank Utility IMPORT/EXPORT wiring — optional; empty pickers/gates make the buttons no-op.
        void setBankExportFolderPicker(PatchFolderPicker picker);
        void setBankImportFolderPicker(PatchFolderPicker picker);
        void setBankImportConfirmGate(BankImportConfirmGate gate);
        void setBankExportOverwriteConfirmGate(BankImportConfirmGate gate);
        void setBankPasteConfirmGate(BankPasteConfirmGate gate);
        void setBankTransferProgressPresenter(BankTransferProgressPresenter presenter);

        void rescanPersistedComputerPatchesFolder();
        void resetComputerPatchesBrowserAfterSessionLoad();
        // Drops in-memory scan before APVTS replaceState so valueTreeRedirected cannot flash a stale list.
        void discardComputerPatchesScanCacheQuietly();

        // Re-resolve the editor Patch Name from the last device dump (or a synthesized BNK
        // placeholder on banked devices) using the current Patch Name Display Settings mode.
        void reapplyDisplayedPatchName();

        // Re-apply COMPUTER PATCHES DISPLAY SYSEX / FILE NAMES to the loaded .syx without prompting.
        void reapplyComputerPatchDisplayedName();

        // Drag-drop one .syx onto the editor: parent folder → scan → select → existing immediate load.
        enum class DroppedComputerPatchLoadResult
        {
            kLoaded,
            kCancelled,
            kRejected
        };

        DroppedComputerPatchLoadResult loadDroppedComputerPatchFile(const juce::File& file,
                                                                    const DeviceMemoryLimits& limits);

        // Persist a musical overlay for the current Internal bank/patch when the name is usable
        // and not an Oberheim BNK placeholder (inline rename / STORE paths).
        void rememberCurrentOverlayFromModel();

        // Requests the current patch from the synth (async dump) and mirrors it into the editor
        // (PatchModel + APVTS) as a patch load. Rolls back Internal coordinates on failure.
        // Prefer the prior-coordinates overload when APVTS / lock state were already advanced
        // (NumberBox) so failure can restore the true pre-navigation values.
        void loadCurrentPatchFromDevice(const DeviceMemoryLimits& limits);
        void loadCurrentPatchFromDevice(const DeviceMemoryLimits& limits,
                                        const InternalCoordinatesSnapshot& priorCoordinates);

        // Chantier 1: INIT (and similar) can be clean vs snapshot yet not STORED in RAM.
        bool isPatchNotStoredInRam() const noexcept { return patchNotStoredInRam_; }
        bool isCurrentBankStoreAllowed() const;
        bool hasUsableKnownSyxPath() const;
        // File-origin dirty → Save / Save As; otherwise Store (RAM) or Save As (ROM).
        UnsavedEditPersistKind resolveUnsavedEditPersistKind(bool isDirty) const;
        // Runs Store / Save / Save As for the unsaved-edit Persist choice. False = stay.
        bool tryPersistCurrentPatchFromUnsavedGate(UnsavedEditPersistKind persistKind);

        // Unit-test seams — see ComboboxPatchSendDebouncer::flushPendingSynchronouslyForTests().
        void flushPatchNavDebouncerForTests() { patchNavDebouncer_.flushPendingSynchronouslyForTests(); }
        void flushComputerSelectDebouncerForTests()
        {
            computerSelectDebouncer_.flushPendingSynchronouslyForTests();
        }

        // APVTS-synced dirty baseline without clearing editorial undo (Processor checkpoint leg).
        void syncDirtySnapshotFromApvts();

    private:
        enum class PatchNameResolvePurpose
        {
            kDisplay,
            kExportMusical
        };

        struct PendingDeviceLoad
        {
            std::uint64_t generation = 0;
            InternalCoordinatesSnapshot priorCoordinates;
            std::array<juce::uint8, PatchModel::kBufferSize> bufferAtRequest {};
        };

        // Returns true when the pending patch-context change may proceed.
        // `includeUnsavedEditWarning` selects FR-51 risk + history vs history-only.
        bool confirmPatchContextChange(bool includeUnsavedEditWarning = true);
        void captureCleanSnapshot();
        void establishEditorialCheckpoint();
        void markPatchNotStoredInRam() noexcept { patchNotStoredInRam_ = true; }
        void revertComputerPatchesSelectionIfNeeded(int previousSelectedId);
        void rememberComputerPatchesSelection(int selectedId);
        void clearComputerPatchesSelection();
        void seedCommittedComputerPatchesSelectionIfNeeded();
        void restoreComputerPatchesBrowser(const juce::String& folderPath, int selectedId);
        void abortComputerPatchesNavigation();
        // Returns the id written to APVTS, or nullopt when navigation was a no-op.
        std::optional<int> advanceComputerPatchesSelection(bool isNext);
        void applyPatchCoordinates(const PatchCoordinates& coordinates,
                                   const DeviceMemoryLimits& limits,
                                   bool sendMidi = true);
        InternalCoordinatesSnapshot captureInternalCoordinates(const DeviceMemoryLimits& limits) const;
        void restoreInternalCoordinates(const InternalCoordinatesSnapshot& snapshot,
                                        const DeviceMemoryLimits& limits);
        void beginPendingDeviceLoad(const InternalCoordinatesSnapshot& priorCoordinates);
        void clearPendingDeviceLoad();
        void abandonPendingDeviceLoad();
        void failPendingDeviceLoad(const DeviceMemoryLimits& limits, const juce::String& footerMessage);
        int resolveComputerPatchesCancelRevertId() const;
        void noteStableComputerPatchesSelection(int selectedId);
        bool isDeviceDumpAvailable() const;
        void requestDeviceDump(juce::uint8 patchNumber, ActionExecutionHooks::DeviceDumpCallback onResult);
        bool arePatchCoordinatesEstablished() const;
        void markPatchCoordinatesEstablished();
        void setNavigationFocus(int focusOwner);
        void clearComputerNavigationFocusIfOwned();
        // First Internal Prev/Next on undefined coordinates lands on the lowest slot instead
        // of stepping, because there is nothing to step from yet.
        PatchCoordinates resolveInternalNavigationTarget(bool isNext,
                                                         const DeviceMemoryLimits& limits) const;
        // OPEN on undefined coordinates: claim bank/patch 0 as the .syx destination and send
        // Set Bank only — the device patch must not be dumped into the editor.
        void establishCoordinatesForComputerOpen(const DeviceMemoryLimits& limits);
        // Bank Utility EXPORT/IMPORT/COPY/PASTE orchestration (live MIDI dump/write, cancellable).
        using PackedPatchBuffer = std::array<juce::uint8, PatchModel::kBufferSize>;

        void handleInternalPatchInit();
        void handleInternalPatchPaste(const DeviceMemoryLimits& limits);
        void handleInternalPatchStore(const DeviceMemoryLimits& limits);
        void handleOpenPatchFolder(const DeviceMemoryLimits& limits);
        void handleSavePatchAs();
        void handleSavePatchFile();
        void handleLoadSelectedPatchFile(const DeviceMemoryLimits& limits);
        void loadSelectedPatchFileImmediately(const DeviceMemoryLimits& limits);
        void scheduleComputerSelectPatchLoad();
        void scheduleComputerNavPatchLoad();
        void settleInternalPatchNavigation();
        void settleComputerPatchLoad();
        // Restores UI coords and clears baseline when an Internal settle is dropped/superseded.
        void abandonPendingInternalNavSettle();
        // If a Computer-select settle is pending, cancel it and revert selection to committed.
        void abandonPendingComputerSelectSettle();
        bool tryHandleInternalPatchNavigation(const juce::String& propertyId, const DeviceMemoryLimits& limits);
        bool tryHandleComputerPatchFileNavigation(const juce::String& propertyId,
                                                  const DeviceMemoryLimits& limits);
        bool tryHandleModuleHeaderClicks(const juce::String& propertyId, const DeviceMemoryLimits& limits);
        bool tryHandleBankButtonSelection(const juce::String& propertyId, const DeviceMemoryLimits& limits);
        bool tryHandleInitPasteStoreActions(const juce::String& propertyId, const DeviceMemoryLimits& limits);
        bool tryHandleComputerFileActions(const juce::String& propertyId, const DeviceMemoryLimits& limits);
        bool tryHandleBankTransferActions(const juce::String& propertyId, const DeviceMemoryLimits& limits);
        void publishPasteNothingFooter();
        void publishPasteFailedFooter(const juce::String& sourceLabel, const juce::String& targetLabel);
        void publishPasteSuccessFooter(const juce::String& sourceLabel, const juce::String& targetLabel);
        void applyPastedPatchToEditorAndSynth(const DeviceMemoryLimits& limits, int currentBank);
        bool validateBankExportPrerequisites(const DeviceMemoryLimits& limits);
        juce::String resolveExportChildFolderName(const DeviceMemoryLimits& limits) const;
        bool ensureExportChildFolder(const juce::File& parentFolder,
                                     const juce::String& childName,
                                     juce::File& outFolder);
        void initializeBankExportState(const DeviceMemoryLimits& limits,
                                       const juce::File& folder,
                                       const juce::String& childName,
                                       bool createdTargetFolderThisRun);
        void showBankExportProgress(std::uint64_t generation,
                                    const juce::File& folder,
                                    bool hasBankConcept,
                                    int bank);
        void beginBankExportDumpLoop(std::uint64_t generation,
                                     const DeviceMemoryLimits& limits,
                                     bool hasBankConcept,
                                     int bank);
        bool validateExportDumpSlot(int slot,
                                    std::uint64_t generation,
                                    const std::vector<juce::uint8>& dump);
        bool saveExportedDumpToFile(int slot, PatchModel& dumpedPatch);
        bool prepareBankExportDestination(const DeviceMemoryLimits& limits,
                                          juce::File& outFolder,
                                          juce::String& outChildName,
                                          bool& outCreatedThisRun);
        void startBankExportTransfer(const DeviceMemoryLimits& limits,
                                     const juce::File& folder,
                                     const juce::String& childName,
                                     bool createdTargetFolderThisRun);
        bool processExportDumpSlot(int slot, std::uint64_t generation, std::vector<juce::uint8> dump);
        void applySuccessfulDeviceDump(const DeviceMemoryLimits& limits,
                                       int bank,
                                       int patch,
                                       const std::vector<juce::uint8>& dump);

        struct PendingDeviceDumpResultContext
        {
            int bank = 0;
            int patch = 0;
            std::uint64_t generation = 0;
            DeviceMemoryLimits limits { DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kUnknown) };
            juce::String failFooter;
        };

        void handlePendingDeviceDumpResult(const PendingDeviceDumpResultContext& context,
                                           std::vector<juce::uint8> dump);
        bool loadImportPatchesFromScan(const PatchFolderScanResult& scan, int cappedValidCount);

        struct BankImportFolderSelection
        {
            juce::File folder;
            int foundCount = 0;
            int cappedValidCount = 0;
        };

        std::optional<BankImportFolderSelection> resolveBankImportFolderSelection(const DeviceMemoryLimits& limits);
        bool validateBankImportGate(const DeviceMemoryLimits& limits);
        std::optional<juce::File> pickBankImportFolder();
        std::optional<BankImportFolderSelection> scanBankImportSelection(const juce::File& folder);
        void initializeBankImportState(const DeviceMemoryLimits& limits, int foundCount);
        void showBankImportProgress(std::uint64_t generation, const juce::File& folder);
        void scheduleBankImportAfterSetBank(std::uint64_t generation,
                                            const DeviceMemoryLimits& limits,
                                            int bank);
        bool processImportSnapshotDump(int slot, std::uint64_t generation, std::vector<juce::uint8> dump);
        std::optional<PackedPatchBuffer> takeWrittenCurrentImportSlot(bool importSucceeded,
                                                                      int currentPatch) const;
        void startBankImportAfterConfirm(const DeviceMemoryLimits& limits,
                                         const juce::File& folder,
                                         int foundCount,
                                         int cappedValidCount);
        void applyWrittenImportSlotToEditor(const DeviceMemoryLimits& limits,
                                            int importedBank,
                                            int currentPatch,
                                            const PackedPatchBuffer& writtenCurrentSlot);
        void schedulePostImportDeviceReload(const DeviceMemoryLimits& limits);
        void rememberOverlayFromPackedSlot(int bank, int slot, const juce::uint8* packed);
        void rememberPasteOverlayForSlot(int destinationBank, int slot, const juce::uint8* packed);
        void restoreOverlayFromPackedSlot(int bank, int slot, const juce::uint8* packed);

        struct SelectedPatchFileResolution
        {
            enum class Kind
            {
                kSilentNoOp,
                kFailed,
                kOk
            };

            Kind kind = Kind::kSilentNoOp;
            juce::File file;
            juce::String failureMessage;
        };

        int readComputerPatchesSelectedId() const;
        bool isComputerPatchesScanCurrent() const;
        juce::File fileAtComputerPatchesIndex(int index) const;
        SelectedPatchFileResolution resolveSelectedPatchFileForLoad() const;
        SelectedPatchFileResolution makeLoadFailedResolution(const juce::String& message) const;
        bool canExecutePatchLoad() const;
        bool loadPackedPatchFromFile(const juce::File& file, juce::uint8* packedOut);
        PatchNameReconciliationResult reconcileLoadedPatchName(const juce::File& file);
        std::optional<PatchNameReconciliationResult> decodeAndReconcilePatchFile(const juce::File& file);
        void commitLoadedComputerPatchFile(const DeviceMemoryLimits& limits,
                                             int requestedId,
                                             const juce::File& file,
                                             const PatchNameReconciliationResult& reconciliation);
        void syncLoadedPatchToApvts();
        void applyLoadedPatchToApvtsAndSynth(const DeviceMemoryLimits& limits);
        void publishLoadFooters(const juce::String& fileName,
                                  const PatchNameReconciliationResult& reconciliation);
        void publishLoadFailureFooter(const juce::String& message);
        void publishDropRejectFooter(SinglePatchSyxRejectKind rejectKind);
        DroppedComputerPatchLoadResult rejectDroppedComputerPatch(SinglePatchSyxRejectKind rejectKind);
        bool prepareDroppedComputerPatchSelection(const juce::File& file,
                                                  const DeviceMemoryLimits& limits,
                                                  int& outTargetId);
        DroppedComputerPatchLoadResult finalizeDroppedComputerPatchLoad();
        void noteDevicePatchOrigin(int bank, int patch);
        void noteComputerPatchOrigin(const juce::File& file);
        bool performUnsavedGatePersistAction(UnsavedEditPersistKind persistKind);
        bool didUnsavedGatePersistSucceed(UnsavedEditPersistKind persistKind) const;
        void saveCurrentPatchToFile(const juce::File& targetFile);
        void writeValidatedPatchSyx(const juce::File& targetWithExt, const juce::String& matrixStem);
        void completeSuccessfulSave(const juce::File& savedFile);
        void rescanAndSelectSavedFile(const juce::String& savedFileName);
        juce::File resolveRescanFolder() const;
        juce::File resolveDefaultSaveFolder() const;
        juce::String resolveSuggestedSaveStem() const;
        bool isInitPatchNameSentinelActive() const;
        bool refuseSaveIfInitSentinelActive();
        // True when STORE was deferred for name-required (sentinel active).
        bool tryDeferStoreForInitNameRequired();
        void scanAndPublishFolder(const juce::File& folder);
        void clearPublishedScanCache();
        void bumpScanRevision();
        void publishSaveSuccessFooter(const juce::String& fileName);
        void publishSaveFailureFooter(const juce::String& message);
        void propagateRomBlockedFooter();
        int getCurrentBank(const DeviceMemoryLimits& limits) const;
        int getCurrentPatch(const DeviceMemoryLimits& limits) const;
        int parseBankButtonIndex(const juce::String& propertyId) const;

        struct BankTransferState
        {
            enum class Kind
            {
                kNone,
                kExport,
                kImport,
                kCopy,
                kPaste
            };

            Kind kind = Kind::kNone;
            std::uint64_t generation = 0;
            bool cancelRequested = false;
            int totalSlots = 0;
            int completedSlots = 0;
            DeviceMemoryLimits limits { DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kUnknown) };
            int bank = 0;
            int pasteSourceBank = -1;
            bool hasBankConcept = false;

            // EXPORT-only.
            juce::File targetFolder;
            juce::String childFolderDisplayName;
            juce::StringArray filesCreatedThisRun; // only paths that did not exist before this run
            bool createdTargetFolderThisRun = false;

            // IMPORT / PASTE / COPY dump buffer.
            int importFoundCount = 0;
            int importValidCount = 0;
            int importWrittenCount = 0;
            std::vector<PackedPatchBuffer> importPatches;
            std::vector<PackedPatchBuffer> deviceSnapshot;
            bool isRestoring = false;
            juce::String pendingFooterMessage;
            juce::String pendingFooterSeverity;
        };

        static bool isImportFamilyKind(BankTransferState::Kind kind) noexcept;
        static bool isExportFamilyKind(BankTransferState::Kind kind) noexcept;
        bool isBankTransferBusy() const noexcept;
        int getSelectedBankForTransfer(const DeviceMemoryLimits& limits) const;
        void handleBankExport(const DeviceMemoryLimits& limits);
        void handleBankImport(const DeviceMemoryLimits& limits);
        void handleBankCopy(const DeviceMemoryLimits& limits);
        void restoreBankCopyFeedbackAfterConfirmCancel();
        void handleBankPaste(const DeviceMemoryLimits& limits);
        bool validateBankCopyPrerequisites(const DeviceMemoryLimits& limits);
        void initializeBankCopyState(const DeviceMemoryLimits& limits, int bank);
        bool showBankCopyProgress(std::uint64_t generation, int bank);
        void beginBankCopyDumpLoop(std::uint64_t generation, const DeviceMemoryLimits& limits, int bank);
        void commitCopiedBankToClipboard();
        bool processCopyDumpSlot(int slot, std::uint64_t generation, std::vector<juce::uint8> dump);
        bool validateBankPastePrerequisites(const DeviceMemoryLimits& limits, int targetBank);
        bool prepareBankPastePatches(int sourceBank, int targetBank, const DeviceMemoryLimits& limits);
        bool showBankPasteProgress(std::uint64_t generation, int targetBank);
        void beginBankPasteWriteLoop(std::uint64_t generation, const DeviceMemoryLimits& limits, int targetBank);
        void exportNextSlot(int slot, std::uint64_t generation);
        void finishBankExport(bool success, const juce::String& footerMessage, const juce::String& severity);
        void copyNextSlot(int slot, std::uint64_t generation);
        void refreshBankCopyFeedbackAfterFinish(bool success);
        void finishBankCopy(bool success, const juce::String& footerMessage, const juce::String& severity);
        void beginBankImportSnapshot(std::uint64_t generation);
        void snapshotNextImportSlot(int slot, std::uint64_t generation);
        void beginBankImportWrite(std::uint64_t generation);
        void showImportFamilyWriteProgress();
        void writeNextImportSlot(int slot, std::uint64_t generation);
        juce::String bankImportFamilyCancelMessage() const;
        juce::String bankImportFamilyRestoreFailedMessage() const;
        juce::String bankImportFamilyRestoringProgressMessage() const;
        void showBankImportRestoreProgress();
        void finishImportFamilyWriteSuccess();
        void beginBankImportRestore(std::uint64_t generation, const juce::String& footerMessage, const juce::String& severity);
        void restoreNextSnapshotSlot(int slot, std::uint64_t generation);
        void finishBankImport(const juce::String& footerMessage, const juce::String& severity);
        struct BankImportFinishContext
        {
            DeviceMemoryLimits limits { DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kUnknown) };
            int importedBank = 0;
            int currentPatch = 0;
            bool importSucceeded = false;
            bool wasPaste = false;
            bool deviceMayHaveChanged = false;
            std::optional<PackedPatchBuffer> writtenCurrentSlot;
        };
        void applyBankImportFinishSideEffects(const BankImportFinishContext& context);
        void requestBankTransferCancel(std::uint64_t generation);
        void publishBankTransferFooter(const juce::String& message, const juce::String& severity);
        int bankTransferWriteDelayMs() const;
        void loadPatchNameOverlayFromApvts();
        void reloadPatchNameOverlayFromApvts();
        void persistPatchNameOverlayToApvts();
        void rememberOverlayName(int bank, int patch, const juce::String& name);
        void applyResolvedPatchName(PatchModel& model,
                                    const PatchCoordinates& coordinates,
                                    const DeviceMemoryLimits& limits,
                                    PatchNameResolvePurpose purpose);

        juce::String resolveDisplayedPatchNameSeed(int bank,
                                                   int patch,
                                                   const DeviceMemoryLimits& limits) const;

        BankTransferState bankTransfer_;
        std::uint64_t bankTransferGeneration_ = 0;
        PatchFolderPicker bankExportFolderPicker_;
        PatchFolderPicker bankImportFolderPicker_;
        BankImportConfirmGate bankImportConfirmGate_;
        BankImportConfirmGate bankExportOverwriteConfirmGate_;
        BankPasteConfirmGate bankPasteConfirmGate_;
        BankTransferProgressPresenter bankTransferProgress_;
        PatchNameOverlayStore patchNameOverlay_;
        bool patchNameOverlayLoaded_ = false;
        juce::String lastDeviceDumpRawName_;
        int lastDeviceDumpBank_ = -1;
        int lastDeviceDumpPatch_ = -1;
        bool hasLastDeviceDumpRawName_ = false;

        void clearLastDeviceDumpRawName();

        juce::AudioProcessorValueTreeState& apvts_;
        DeviceMemoryLimitsSupplier deviceMemoryLimits_;
        PatchModel* patchModel_;
        ApvtsPatchMapper* apvtsPatchMapper_;
        ClipboardService* clipboardService_;
        PatchInitService* patchInitService_;
        PatchSelectionMidiSync* patchSelectionMidiSync_;
        MidiManager* midiManager_;
        PatchFileService* patchFileService_;
        PatchNameSyncer* patchNameSyncer_;
        DirtyPatchTracker* dirtyPatchTracker_;
        SysExEncoder* sysExEncoder_;
        PatchFolderPicker pickFolder_;
        PatchSaveFilePicker pickSaveFile_;
        PatchNameReconciliationPicker pickNameReconciliation_;
        ActionExecutionHooks hooks_;
        int lastCommittedComputerPatchesSelectedId_ = 0;
        int lastStableComputerPatchesSelectedId_ = 0;
        bool suppressComputerPatchesSelectLoad_ = false;
        ComboboxPatchSendDebouncer patchNavDebouncer_;
        ComboboxPatchSendDebouncer computerSelectDebouncer_;
        std::optional<InternalCoordinatesSnapshot> pendingInternalNavBaseline_;
        std::uint64_t deviceLoadGeneration_ = 0;
        std::optional<PendingDeviceLoad> pendingDeviceLoad_;
        bool patchNotStoredInRam_ = false;
        // Mirrored from PatchLoadContext for leave Persist (file Save vs Store) without a get hook.
        bool editorPatchFromComputerFile_ = false;
        juce::String knownSyxFullPath_;
        // Set only by commitLoadedComputerPatchFile during a drop attempt (see loadDropped…).
        bool dropAttemptCommitted_ = false;

        // When OPEN replaces the browser then Cancel/fails the auto-load, restore this snapshot.
        struct ComputerPatchesBrowserSnapshot
        {
            juce::String folderPath;
            int selectedId = 0;
        };

        std::optional<ComputerPatchesBrowserSnapshot> pendingBrowserRestoreOnCancel_;

        JUCE_DECLARE_WEAK_REFERENCEABLE(PatchManagerActionHandler)
    };

} // namespace Core
