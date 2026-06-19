#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/IActionHandler.h"
#include "Core/Services/DeviceMemoryLimits.h"

class MidiManager;

namespace Core
{

    class ApvtsPatchMapper;
    class ClipboardService;
    class PatchInitService;
    class PatchModel;
    class PatchSelectionMidiSync;

    class PatchManagerActionHandler final : public IActionHandler
    {
    public:
        using DeviceMemoryLimitsSupplier = std::function<DeviceMemoryLimits()>;

        PatchManagerActionHandler(juce::AudioProcessorValueTreeState& apvts,
                                  DeviceMemoryLimitsSupplier deviceMemoryLimits,
                                  PatchModel* patchModel,
                                  ApvtsPatchMapper* apvtsPatchMapper,
                                  ClipboardService* clipboardService,
                                  PatchInitService* patchInitService,
                                  PatchSelectionMidiSync* patchSelectionMidiSync,
                                  MidiManager* midiManager,
                                  ActionExecutionHooks hooks);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

    private:
        void applyPatchCoordinates(const PatchCoordinates& coordinates, const DeviceMemoryLimits& limits);
        void handleUnlockBank(const DeviceMemoryLimits& limits);
        void markBanksLockedInApvts();
        void handleInternalPatchInit();
        void handleInternalPatchPaste(const DeviceMemoryLimits& limits);
        void handleInternalPatchStore(const DeviceMemoryLimits& limits);
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
        ActionExecutionHooks hooks_;
    };

} // namespace Core
