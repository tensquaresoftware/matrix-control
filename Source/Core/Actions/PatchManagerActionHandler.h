#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/IActionHandler.h"
#include "Core/Services/DeviceMemoryLimits.h"

class MidiManager;
class SysExEncoder;

namespace Core
{

    class ApvtsPatchMapper;
    class ClipboardService;
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
                                  SysExEncoder* sysExEncoder,
                                  PatchFolderPicker pickFolder,
                                  PatchSaveFilePicker pickSaveFile,
                                  ActionExecutionHooks hooks);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

        void rescanPersistedComputerPatchesFolder();

    private:
        void applyPatchCoordinates(const PatchCoordinates& coordinates, const DeviceMemoryLimits& limits);
        void handleUnlockBank(const DeviceMemoryLimits& limits);
        void markBanksLockedInApvts();
        void handleInternalPatchInit();
        void handleInternalPatchPaste(const DeviceMemoryLimits& limits);
        void handleInternalPatchStore(const DeviceMemoryLimits& limits);
        void handleOpenPatchFolder();
        void handleSavePatchAs();
        void handleSavePatchFile();
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
        SysExEncoder* sysExEncoder_;
        PatchFolderPicker pickFolder_;
        PatchSaveFilePicker pickSaveFile_;
        ActionExecutionHooks hooks_;
    };

} // namespace Core
