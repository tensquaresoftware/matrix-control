#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
    class ModuleHeader;
    class Label;
    class Button;
}

class WidgetFactory;

class BankUtilityPanel : public juce::Component
{
public:
    BankUtilityPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BankUtilityPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);

private:
    inline constexpr static int kTopPadding_ = 5;
    inline constexpr static int kSpacing_ = 5;

    int width_;
    int height_;
    tss::ISkin* skin_;
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

    void setupModuleHeader(tss::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBankSelectorLabel(tss::ISkin& skin);
    void setupSelectBankButtons(tss::ISkin& skin, WidgetFactory& widgetFactory);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BankUtilityPanel)
};
