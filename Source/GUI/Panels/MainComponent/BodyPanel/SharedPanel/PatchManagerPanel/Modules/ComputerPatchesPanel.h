#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace TSS
{
    class ISkin;
    class ModuleHeader;
    class GroupLabel;
    class Button;
    class ComboBox;
}

class WidgetFactory;

class ComputerPatchesPanel : public juce::Component
{
public:
    ComputerPatchesPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~ComputerPatchesPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    inline constexpr static int kGroupLabelGap_ = 10;
    inline constexpr static int kGap_ = 5;
    inline constexpr static int kSelectPatchFileEmptyId_ = 1;

    int width_;
    int height_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComputerPatchesPanel)
};
