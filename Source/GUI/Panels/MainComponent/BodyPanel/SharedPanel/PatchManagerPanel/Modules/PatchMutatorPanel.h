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
    class Slider;
    class Button;
    class ComboBox;
    class Toggle;
}

class WidgetFactory;

class PatchMutatorPanel : public juce::Component
{
public:
    PatchMutatorPanel(TSS::ISkin& skin, const PatchMutatorPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchMutatorPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    PatchMutatorPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader_;

    std::unique_ptr<TSS::Label> amountLabel_;
    std::unique_ptr<TSS::Slider> amountSlider_;
    std::unique_ptr<TSS::Button> mutateButton_;
    std::unique_ptr<TSS::Toggle> dco1Toggle_;
    std::unique_ptr<TSS::Toggle> dco2Toggle_;
    std::unique_ptr<TSS::Toggle> vcfVcaToggle_;
    std::unique_ptr<TSS::Toggle> fmTrackToggle_;
    std::unique_ptr<TSS::Toggle> rampPortamentoToggle_;

    std::unique_ptr<TSS::Label> randomLabel_;
    std::unique_ptr<TSS::Slider> randomSlider_;
    std::unique_ptr<TSS::Button> retryButton_;
    std::unique_ptr<TSS::Toggle> env1Toggle_;
    std::unique_ptr<TSS::Toggle> env2Toggle_;
    std::unique_ptr<TSS::Toggle> env3Toggle_;
    std::unique_ptr<TSS::Toggle> lfo1Toggle_;
    std::unique_ptr<TSS::Toggle> lfo2Toggle_;

    std::unique_ptr<TSS::Label> historyLabel_;
    std::unique_ptr<TSS::ComboBox> historyComboBox_;
    std::unique_ptr<TSS::Button> compareButton_;
    std::unique_ptr<TSS::Button> deleteButton_;
    std::unique_ptr<TSS::Button> clearButton_;
    std::unique_ptr<TSS::Button> exportButton_;

    void propagateSkinsToControlWidgets(TSS::ISkin& skin);
    void propagateSkinsToToggleWidgets(TSS::ISkin& skin);

    void setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupAmountLine(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupRandomLine(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupHistoryLine(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void connectButtonToApvts(TSS::Button* button, const char* widgetId);
    void connectToggleToApvts(TSS::Toggle* toggle, const char* widgetId);

    void layoutSliderLine(int x, int y, TSS::Label* label, TSS::Slider* slider, TSS::Button* button,
                          const std::vector<TSS::Toggle*>& toggles, int actionButtonWidth);
    void layoutHistoryLine(int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchMutatorPanel)
};
