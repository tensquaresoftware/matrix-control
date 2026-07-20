#pragma once

#include <functional>
#include <optional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/IActionHandler.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/PatchFileNameReconciler.h"

class MidiManager;
class SysExEncoder;

namespace Core
{

    class ApvtsPatchMapper;
    class ClipboardService;
    class DirtyPatchTracker;
    class PatchFileService;
    class PatchInitService;
    class PatchModel;
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

        PatchManagerActionHandler(juce::AudioProcessorValueTreeState& apvts,
                                  DeviceMemoryLimitsSupplier deviceMemoryLimits,
                                  PatchModel* patchModel,
                                  ApvtsPatchMapper* apvtsPatchMapper,
                                  ClipboardService* clipboardService,
                                  PatchInitService* patchInitService,
                                  PatchSelectionMidiSync* patchSelectionMidiSync,
                                  MidiManager* midiManager,
                                  PatchFileService* patchFileService,
                                  PatchNameSyncer* patchNameSyncer,
                                  DirtyPatchTracker* dirtyPatchTracker,
                                  SysExEncoder* sysExEncoder,
                                  PatchFolderPicker pickFolder,
                                  PatchSaveFilePicker pickSaveFile,
                                  PatchNameReconciliationPicker pickNameReconciliation,
                                  ActionExecutionHooks hooks);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

        void rescanPersistedComputerPatchesFolder();
        void resetComputerPatchesBrowserAfterSessionLoad();
        // Drops in-memory scan before APVTS replaceState so valueTreeRedirected cannot flash a stale list.
        void discardComputerPatchesScanCacheQuietly();

        // Requests the current patch from the synth (async dump) and mirrors it into the editor
        // (PatchModel + APVTS) as a patch load. No-op when no device is available. Used after
        // Bank/Internal navigation and after direct patch-number edits.
        void loadCurrentPatchFromDevice(const DeviceMemoryLimits& limits);

    private:
        // Returns true when the pending patch-context change may proceed.
        // `includeUnsavedEditWarning` selects FR-51 + history (navigation) vs history-only (INIT/PASTE).
        bool confirmPatchContextChange(bool includeUnsavedEditWarning = true);
        void captureCleanSnapshot();
        void revertComputerPatchesSelectionIfNeeded(int previousSelectedId);
        void rememberComputerPatchesSelection(int selectedId);
        void restoreComputerPatchesBrowser(const juce::String& folderPath, int selectedId);
        void abortComputerPatchesNavigation();
        // Returns the id written to APVTS, or nullopt when navigation was a no-op.
        std::optional<int> advanceComputerPatchesSelection(bool isNext);
        void applyPatchCoordinates(const PatchCoordinates& coordinates, const DeviceMemoryLimits& limits);
        void handleUnlockBank(const DeviceMemoryLimits& limits);
        void markBanksLockedInApvts();
        void handleInternalPatchInit();
        void handleInternalPatchPaste(const DeviceMemoryLimits& limits);
        void handleInternalPatchStore(const DeviceMemoryLimits& limits);
        void handleOpenPatchFolder(const DeviceMemoryLimits& limits);
        void handleSavePatchAs();
        void handleSavePatchFile();
        void handleLoadSelectedPatchFile(const DeviceMemoryLimits& limits);
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
        void syncLoadedPatchToApvts();
        void applyLoadedPatchToApvtsAndSynth(const DeviceMemoryLimits& limits);
        void publishLoadFooters(const juce::String& fileName,
                                  const PatchNameReconciliationResult& reconciliation);
        void publishLoadFailureFooter(const juce::String& message);
        void publishDeviceDumpFailureFooter();
        void saveCurrentPatchToFile(const juce::File& targetFile);
        void completeSuccessfulSave(const juce::String& savedFileName);
        void rescanAndSelectSavedFile(const juce::String& savedFileName);
        juce::File resolveRescanFolder() const;
        juce::File resolveDefaultSaveFolder() const;
        juce::String resolveSuggestedSaveStem() const;
        void scanAndPublishFolder(const juce::File& folder);
        void clearPublishedScanCache();
        void bumpScanRevision();
        void publishSaveSuccessFooter(const juce::String& fileName);
        void publishSaveFailureFooter(const juce::String& message);
        void propagateRomBlockedFooter();
        int getCurrentBank(const DeviceMemoryLimits& limits) const;
        int getCurrentPatch(const DeviceMemoryLimits& limits) const;
        int parseBankButtonIndex(const juce::String& propertyId) const;

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
        bool suppressComputerPatchesSelectLoad_ = false;

        // When OPEN replaces the browser then Cancel/fails the auto-load, restore this snapshot.
        struct ComputerPatchesBrowserSnapshot
        {
            juce::String folderPath;
            int selectedId = 0;
        };

        std::optional<ComputerPatchesBrowserSnapshot> pendingBrowserRestoreOnCancel_;
    };

} // namespace Core
