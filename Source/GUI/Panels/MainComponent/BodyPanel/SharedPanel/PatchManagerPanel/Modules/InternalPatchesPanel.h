#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
    class ModuleHeader;
    class GroupLabel;
    class Button;
    class NumberBox;
}

class WidgetFactory;

class InternalPatchesPanel : public juce::Component,
                             public juce::ValueTree::Listener
{
public:
    InternalPatchesPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~InternalPatchesPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    
    // juce::ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                 const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override {}

private:
    inline constexpr static int kGroupLabelGap_ = 10;
    inline constexpr static int kGap_ = 5;

    int width_;
    int height_;
    tss::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<tss::ModuleHeader> moduleHeader;

    std::unique_ptr<tss::GroupLabel> browserGroupLabel;
    std::unique_ptr<tss::GroupLabel> memoryGroupLabel;

    std::unique_ptr<tss::Button> loadPreviousPatchButton_;
    std::unique_ptr<tss::Button> loadNextPatchButton_;

    std::unique_ptr<tss::NumberBox> currentBankNumber;
    std::unique_ptr<tss::NumberBox> currentPatchNumber;

    std::unique_ptr<tss::Button> initPatchButton_;
    std::unique_ptr<tss::Button> copyPatchButton_;
    std::unique_ptr<tss::Button> pastePatchButton_;
    std::unique_ptr<tss::Button> storePatchButton_;

    void setupModuleHeader(tss::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBrowserGroupLabel(tss::ISkin& skin);
    void setupLoadPreviousPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupLoadNextPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupCurrentBankNumberBox(tss::ISkin& skin);
    void setupCurrentPatchNumberBox(tss::ISkin& skin);
    void setupMemoryGroupLabel(tss::ISkin& skin);
    void setupInitPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupCopyPatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupPastePatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupStorePatchButton(tss::ISkin& skin, WidgetFactory& widgetFactory);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InternalPatchesPanel)
};
