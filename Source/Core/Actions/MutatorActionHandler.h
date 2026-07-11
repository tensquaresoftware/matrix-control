#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/IActionHandler.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/Util/ComboboxPatchSendDebouncer.h"

namespace Core
{

    // Routes Mutator panel action properties to PatchMutatorEngine (Story 7.4).
    // Extension points: Settings manual Defrag (Story 7.7 Phase B) via engine->defragHistory().
    class MutatorActionHandler final : public IActionHandler
    {
    public:
        using ExportFolderPicker = std::function<juce::File()>;
        using DefragLimitModalGate = std::function<void(std::function<void()>)>;

        MutatorActionHandler(juce::AudioProcessorValueTreeState& apvts,
                             PatchMutatorEnginePort* engine,
                             ExportFolderPicker pickExportFolder = {},
                             DefragLimitModalGate showDefragLimitModal = {},
                             int historySelectionDebounceMs = kComboboxPatchSendDebounceMs);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

        void onHistorySelectionChanged();

        // Unit-test seam — see ComboboxPatchSendDebouncer::flushPendingSynchronouslyForTests().
        void flushHistorySelectionDebouncerForTests()
        {
            historySelectionDebouncer_.flushPendingSynchronouslyForTests();
        }

    private:
        void handleMutate();
        void handleRetry();
        void handleCompare();
        void handleDelete();
        void handleClear();
        void handleExport();
        void propagateFooterMessage(const juce::String& message, const juce::String& severity);
        void handleEngineResult(const MutatorActionResult& result);

        juce::AudioProcessorValueTreeState& apvts_;
        PatchMutatorEnginePort* engine_;
        ExportFolderPicker pickExportFolder_;
        DefragLimitModalGate showDefragLimitModal_;
        ComboboxPatchSendDebouncer historySelectionDebouncer_;
    };

} // namespace Core
