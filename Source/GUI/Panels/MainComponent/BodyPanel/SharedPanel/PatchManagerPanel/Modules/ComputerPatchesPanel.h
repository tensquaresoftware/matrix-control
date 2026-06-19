#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"

namespace Core
{
    class PatchFileService;
}

namespace TSS
{
    class ISkin;
    class ModuleHeader;
    class GroupLabel;
    class Button;
    class ComboBox;
}

class WidgetFactory;

class ComputerPatchesPanel : public juce::Component,
                             public juce::ValueTree::Listener
{
public:
    ComputerPatchesPanel(TSS::ISkin& skin,
                         const ComputerPatchesPanelDimensions& dims,
                         WidgetFactory& widgetFactory,
                         juce::AudioProcessorValueTreeState& apvts,
                         const Core::PatchFileService& patchFileService);
    ~ComputerPatchesPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override;

private:
    ComputerPatchesPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;
    const Core::PatchFileService& patchFileService_;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader_;

    std::unique_ptr<TSS::GroupLabel> browserGroupLabel;
    std::unique_ptr<TSS::GroupLabel> storageGroupLabel;

    std::unique_ptr<TSS::Button> loadPreviousPatchFileButton_;
    std::unique_ptr<TSS::Button> loadNextPatchFileButton_;
    std::unique_ptr<TSS::ComboBox> selectPatchFileComboBox_;

    std::unique_ptr<TSS::Button> openPatchFolderButton_;
    std::unique_ptr<TSS::Button> savePatchFileAsButton_;
    std::unique_ptr<TSS::Button> savePatchFileButton_;

    void setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBrowserGroupLabel(TSS::ISkin& skin);
    void setupLoadPreviousPatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupLoadNextPatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupSelectPatchFileComboBox(TSS::ISkin& skin);
    void setupStorageGroupLabel(TSS::ISkin& skin);
    void setupOpenPatchFolderButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupSavePatchFileAsButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupSavePatchFileButton(TSS::ISkin& skin, WidgetFactory& widgetFactory);

    void refreshPatchFileComboBox();
    void applyEmptySentinel();
    void applySelectSentinel(const juce::StringArray& sortedValidFileNames);
    void setNavigationButtonsEnabled(bool enabled);
    void clearPatchFileSelectionProperty();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComputerPatchesPanel)
};
