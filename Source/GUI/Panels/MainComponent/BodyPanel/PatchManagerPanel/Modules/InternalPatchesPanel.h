#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDimensions.h"

namespace tss
{
    class Skin;
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
    InternalPatchesPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~InternalPatchesPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);
    
    // juce::ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                 const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override {}

    static int getWidth() { return PluginDimensions::Panels::Body::PatchManagerSection::InternalPatchesModule::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchManagerSection::InternalPatchesModule::kHeight; }

private:
    inline constexpr static int kGroupLabelSpacing_ = 10;
    inline constexpr static int kSpacing_ = 5;

    tss::Skin* skin_;
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

    void setupModuleHeader(tss::Skin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBrowserGroupLabel(tss::Skin& skin);
    void setupLoadPreviousPatchButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupLoadNextPatchButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupCurrentBankNumberBox(tss::Skin& skin);
    void setupCurrentPatchNumberBox(tss::Skin& skin);
    void setupMemoryGroupLabel(tss::Skin& skin);
    void setupInitPatchButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupCopyPatchButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupPastePatchButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupStorePatchButton(tss::Skin& skin, WidgetFactory& widgetFactory);

    void layoutModuleHeader(int x, int y);
    void layoutBrowserGroupLabel(int x, int y);
    void layoutLoadPreviousPatchButton(int x, int y);
    void layoutLoadNextPatchButton(int x, int y);
    void layoutCurrentBankNumberBox(int x, int y);
    void layoutCurrentPatchNumberBox(int x, int y);
    void layoutMemoryGroupLabel(int x, int y);
    void layoutInitPatchButton(int x, int y);
    void layoutCopyPatchButton(int x, int y);
    void layoutPastePatchButton(int x, int y);
    void layoutStorePatchButton(int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InternalPatchesPanel)
};
