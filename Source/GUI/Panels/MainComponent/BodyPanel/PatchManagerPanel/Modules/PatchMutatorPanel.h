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
    class Slider;
    class Button;
    class ComboBox;
    class Toggle;
}

class WidgetFactory;

class PatchMutatorPanel : public juce::Component
{
public:
    PatchMutatorPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchMutatorPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);

    static int getWidth() { return PluginDimensions::Panels::Body::PatchManager::PatchMutator::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchManager::PatchMutator::kHeight; }

private:
    inline constexpr static int kSpacing_ = 5;

    tss::Skin* skin_;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<tss::ModuleHeader> moduleHeader_;

    std::unique_ptr<tss::Label> amountLabel_;
    std::unique_ptr<tss::Slider> amountSlider_;
    std::unique_ptr<tss::Button> mutateButton_;
    std::unique_ptr<tss::Toggle> dco1Toggle_;
    std::unique_ptr<tss::Toggle> dco2Toggle_;
    std::unique_ptr<tss::Toggle> vcfVcaToggle_;
    std::unique_ptr<tss::Toggle> fmTrackToggle_;
    std::unique_ptr<tss::Toggle> rampPortamentoToggle_;

    std::unique_ptr<tss::Label> randomLabel_;
    std::unique_ptr<tss::Slider> randomSlider_;
    std::unique_ptr<tss::Button> retryButton_;
    std::unique_ptr<tss::Toggle> env1Toggle_;
    std::unique_ptr<tss::Toggle> env2Toggle_;
    std::unique_ptr<tss::Toggle> env3Toggle_;
    std::unique_ptr<tss::Toggle> lfo1Toggle_;
    std::unique_ptr<tss::Toggle> lfo2Toggle_;

    std::unique_ptr<tss::Label> historyLabel_;
    std::unique_ptr<tss::ComboBox> historyComboBox_;
    std::unique_ptr<tss::Button> compareButton_;
    std::unique_ptr<tss::Button> deleteButton_;
    std::unique_ptr<tss::Button> clearButton_;
    std::unique_ptr<tss::Button> exportButton_;

    void setupModuleHeader(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupAmountLine(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupRandomLine(tss::Skin& skin, WidgetFactory& widgetFactory);
    void setupHistoryLine(tss::Skin& skin, WidgetFactory& widgetFactory);
    void connectButtonToApvts(tss::Button* button, const char* widgetId);
    void connectToggleToApvts(tss::Toggle* toggle, const char* widgetId);

    void layoutModuleHeader(int x, int y);
    void layoutAmountLine(int x, int& y);
    void layoutRandomLine(int x, int& y);
    void layoutHistoryLine(int x, int& y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchMutatorPanel)
};
