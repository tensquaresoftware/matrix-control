#pragma once

#include <functional>
#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
    class ModuleHeader;
    class PatchNameDisplay;
}

class PatchNameDisplayPanel : public juce::Component,
                              public juce::ValueTree::Listener
{
public:
    PatchNameDisplayPanel(TSS::ISkin& skin,
                          int width,
                          int height,
                          const PatchNameDisplayDimensions& patchNameDims,
                          const ModuleHeaderDimensions& moduleHeaderDims,
                          juce::AudioProcessorValueTreeState& apvts);
    ~PatchNameDisplayPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    TSS::PatchNameDisplay& getPatchNameDisplay();

    // Drag-drop overlay on PATCH NAME — display-only; does not write APVTS patch name.
    enum class DragOverlayKind
    {
        kInvalid,
        kValidSingle,
        kValidSelection
    };

    void applyDragOverlay(DragOverlayKind kind, const juce::String& previewPrimaryName = {});
    void clearDragOverlay();

    // Name-required presentation (STORE-after-INIT gate UI). Drag overlay still wins while active.
    void armNameRequired();
    void clearNameRequired();
    bool isNameRequiredArmed() const;

    // True when the current origin/bank allows the inline rename (ROM banks are not editable).
    using CanEditProvider = std::function<bool()>;
    void setCanEditProvider(CanEditProvider provider);

    // Applies a committed rename (model + APVTS + dirty + live MIDI push) — owned by PluginProcessor.
    using RenameCommitHandler = std::function<void(const juce::String& newName)>;
    void setRenameCommitHandler(RenameCommitHandler handler);

    // Name-required session ended: success = trim-non-empty commit; false = cancel or empty Return.
    // PluginEditor uses this for pending STORE complete/abort.
    using NameRequiredOutcomeHandler = std::function<void(bool success)>;
    void setNameRequiredOutcomeHandler(NameRequiredOutcomeHandler handler);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

private:
    void syncFromApvtsState();
    void ensureNameRequiredOutcomeForwarder();
    juce::String computeSecondaryLabel() const;
    void clearInvalidCharacterFooterIfPresent();
    static bool isTrackedProperty(const juce::String& propertyName);

    int width_;
    int height_;
    PatchNameDisplayDimensions patchNameDims_;
    ModuleHeaderDimensions moduleHeaderDims_;
    float uiScale_ = 1.0f;

    juce::AudioProcessorValueTreeState& apvts_;
    CanEditProvider canEditProvider_;
    RenameCommitHandler renameCommitHandler_;
    NameRequiredOutcomeHandler nameRequiredOutcomeHandler_;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader_;
    std::unique_ptr<TSS::PatchNameDisplay> patchNameDisplay_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchNameDisplayPanel)
};
