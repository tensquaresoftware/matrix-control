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
    class ComboBox;
}

class WidgetFactory;

class ComputerPatchesPanel : public juce::Component
{
public:
    ComputerPatchesPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~ComputerPatchesPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);

    static int getWidth() { return PluginDimensions::Panels::Body::PatchManager::ComputerPatches::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchManager::ComputerPatches::kHeight; }

private:
    inline constexpr static int kGroupLabelSpacing_ = 10;
    inline constexpr static int kSpacing_ = 5;
    inline constexpr static int kSelectPatchFileEmptyId_ = 1;

    tss::Skin* skin_;
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

    void setupModuleHeader(tss::Skin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBrowserGroupLabel(tss::Skin& skin);
    void setupLoadPreviousPatchFileButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupLoadNextPatchFileButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupSelectPatchFileComboBox(tss::Skin& skin);
    void setupStorageGroupLabel(tss::Skin& skin);
    void setupOpenPatchFolderButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupSavePatchFileAsButton(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupSavePatchFileButton(tss::Skin& skin, WidgetFactory& widgetFactory);

    void layoutModuleHeader(int x, int y);
    void layoutBrowserGroupLabel(int x, int y);
    void layoutLoadPreviousPatchFileButton(int x, int y);
    void layoutLoadNextPatchFileButton(int x, int y);
    void layoutSelectPatchFileComboBox(int x, int y);
    void layoutStorageGroupLabel(int x, int y);
    void layoutOpenPatchFolderButton(int x, int y);
    void layoutSavePatchFileAsButton(int x, int y);
    void layoutSavePatchFileButton(int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComputerPatchesPanel)
};
