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
    class ComboBox;
}

class WidgetFactory;

class ComputerPatchesPanel : public juce::Component
{
public:
    ComputerPatchesPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~ComputerPatchesPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setScalingFactor(float scalingFactor);

private:
    inline constexpr static int kGroupLabelSpacing_ = 10;
    inline constexpr static int kSpacing_ = 5;
    inline constexpr static int kSelectPatchFileEmptyId_ = 1;

    int width_;
    int height_;
    tss::ISkin* skin_;
    float scalingFactor_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<tss::ModuleHeader> moduleHeader_;

    std::unique_ptr<tss::GroupLabel> browserGroupLabel;
    std::unique_ptr<tss::GroupLabel> storageGroupLabel;

    std::unique_ptr<tss::Button> loadPreviousPatchFileButton_;
    std::unique_ptr<tss::Button> loadNextPatchFileButton_;
    std::unique_ptr<tss::ComboBox> selectPatchFileComboBox_;

    std::unique_ptr<tss::Button> openPatchFolderButton_;
    std::unique_ptr<tss::Button> savePatchFileAsButton_;
    std::unique_ptr<tss::Button> savePatchFileButton_;

    void setupModuleHeader(tss::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBrowserGroupLabel(tss::ISkin& skin);
    void setupLoadPreviousPatchFileButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupLoadNextPatchFileButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupSelectPatchFileComboBox(tss::ISkin& skin);
    void setupStorageGroupLabel(tss::ISkin& skin);
    void setupOpenPatchFolderButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupSavePatchFileAsButton(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupSavePatchFileButton(tss::ISkin& skin, WidgetFactory& widgetFactory);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComputerPatchesPanel)
};
