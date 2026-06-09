#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"

namespace TSS
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
    BankUtilityPanel(TSS::ISkin& skin, const BankUtilityPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BankUtilityPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    BankUtilityPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<TSS::ModuleHeader> bankUtilityModuleHeader_;
    std::unique_ptr<TSS::Label> bankSelectorLabel_;
    std::unique_ptr<TSS::Button> lockBankButton_;
    std::unique_ptr<TSS::Button> selectBank0Button_;
    std::unique_ptr<TSS::Button> selectBank1Button_;
    std::unique_ptr<TSS::Button> selectBank2Button_;
    std::unique_ptr<TSS::Button> selectBank3Button_;
    std::unique_ptr<TSS::Button> selectBank4Button_;
    std::unique_ptr<TSS::Button> selectBank5Button_;
    std::unique_ptr<TSS::Button> selectBank6Button_;
    std::unique_ptr<TSS::Button> selectBank7Button_;
    std::unique_ptr<TSS::Button> selectBank8Button_;
    std::unique_ptr<TSS::Button> selectBank9Button_;

    void setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId);
    void setupBankSelectorLabel(TSS::ISkin& skin);
    void setupSelectBankButtons(TSS::ISkin& skin, WidgetFactory& widgetFactory);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BankUtilityPanel)
};
