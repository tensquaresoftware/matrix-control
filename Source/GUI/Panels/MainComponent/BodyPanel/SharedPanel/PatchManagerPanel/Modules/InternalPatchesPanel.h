#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "Core/Services/DeviceMemoryLimits.h"

namespace TSS
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
    InternalPatchesPanel(TSS::ISkin& skin, const InternalPatchesPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~InternalPatchesPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
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
    InternalPatchesPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader;

    std::unique_ptr<TSS::GroupLabel> browserGroupLabel;
    std::unique_ptr<TSS::GroupLabel> memoryGroupLabel;

    std::unique_ptr<TSS::Button> loadPreviousPatchButton_;
    std::unique_ptr<TSS::Button> loadNextPatchButton_;

    std::unique_ptr<TSS::NumberBox> currentBankNumber;
    std::unique_ptr<TSS::NumberBox> currentPatchNumber;

    std::unique_ptr<TSS::Button> initPatchButton_;
    std::unique_ptr<TSS::Button> copyPatchButton_;
    std::unique_ptr<TSS::Button> pastePatchButton_;
    std::unique_ptr<TSS::Button> storePatchButton_;

    void setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBrowserGroupLabel(TSS::ISkin& skin);
    void setupLoadPreviousPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupLoadNextPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupCurrentBankNumberBox(TSS::ISkin& skin);
    void setupCurrentPatchNumberBox(TSS::ISkin& skin);
    void setupMemoryGroupLabel(TSS::ISkin& skin);
    void setupInitPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupCopyPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupPastePatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupStorePatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);

    void refreshDeviceLimits();
    void applyPatchNumberRange(const Core::DeviceMemoryLimits& limits);
    void updatePasteStoreEnabled(const Core::DeviceMemoryLimits& limits, int currentBank);

    bool bankNumberVisible_ = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InternalPatchesPanel)
};
