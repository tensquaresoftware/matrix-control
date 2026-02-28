#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDimensions.h"

namespace tss
{
    class Skin;
    class ModuleHeader;
    class Label;
    class Button;
}

class WidgetFactory;

class BankUtilityPanel : public juce::Component
{
public:
    BankUtilityPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BankUtilityPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);

    static int getWidth() { return PluginDimensions::Panels::Body::PatchManagerSection::BankUtilityModule::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchManagerSection::BankUtilityModule::kHeight; }

private:
    inline constexpr static int kTopPadding_ = 5;
    inline constexpr static int kSpacing_ = 5;

    tss::Skin* skin_;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<tss::ModuleHeader> bankUtilityModuleHeader_;
    std::unique_ptr<tss::Label> bankSelectorLabel_;
    std::unique_ptr<tss::Button> unlockBankButton_;
    std::unique_ptr<tss::Button> selectBank0Button_;
    std::unique_ptr<tss::Button> selectBank1Button_;
    std::unique_ptr<tss::Button> selectBank2Button_;
    std::unique_ptr<tss::Button> selectBank3Button_;
    std::unique_ptr<tss::Button> selectBank4Button_;
    std::unique_ptr<tss::Button> selectBank5Button_;
    std::unique_ptr<tss::Button> selectBank6Button_;
    std::unique_ptr<tss::Button> selectBank7Button_;
    std::unique_ptr<tss::Button> selectBank8Button_;
    std::unique_ptr<tss::Button> selectBank9Button_;

    void setupModuleHeader(tss::Skin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBankSelectorLabel(tss::Skin& skin);
    void setupSelectBankButtons(tss::Skin& skin, WidgetFactory& widgetFactory);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BankUtilityPanel)
};
