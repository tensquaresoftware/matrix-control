#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDimensions.h"

namespace tss
{
    class ISkin;
    class ModuleHeader;
    class Button;
}

class WidgetFactory;

class ModuleHeaderPanel : public juce::Component
{
public:
    enum class ButtonSet { InitCopyPaste, InitOnly };
    enum class ModuleType { PatchEdit, MasterEdit };

    ModuleHeaderPanel(tss::ISkin& skin,
                     WidgetFactory& factory,
                     const juce::String& moduleId,
                     ButtonSet buttonSet,
                     ModuleType moduleType,
                     juce::AudioProcessorValueTreeState& apvts,
                     const juce::String& initWidgetId,
                     const juce::String& copyWidgetId = juce::String(),
                     const juce::String& pasteWidgetId = juce::String());
    ~ModuleHeaderPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setDisplayScale(float displayScale);
    static int getHeight() { return PluginDimensions::Widgets::Heights::kModuleHeader; }

private:
    void createModuleHeader(tss::ISkin& skin, WidgetFactory& factory, const juce::String& moduleId);
    void createInitButton(tss::ISkin& skin, WidgetFactory& factory, const juce::String& initWidgetId);
    void createCopyPasteButtons(tss::ISkin& skin, WidgetFactory& factory, const juce::String& copyWidgetId, const juce::String& pasteWidgetId);

    void layoutModuleHeader();
    void layoutInitOnlyButtons();
    void layoutInitCopyPasteButtons();

    tss::ISkin* skin_;
    ButtonSet buttonSet_;
    ModuleType moduleType_;
    float displayScale_ = 1.0f;

    std::unique_ptr<tss::ModuleHeader> moduleHeader_;
    std::unique_ptr<tss::Button> initButton_;
    std::unique_ptr<tss::Button> copyButton_;
    std::unique_ptr<tss::Button> pasteButton_;

    juce::AudioProcessorValueTreeState& apvts_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleHeaderPanel)
};
