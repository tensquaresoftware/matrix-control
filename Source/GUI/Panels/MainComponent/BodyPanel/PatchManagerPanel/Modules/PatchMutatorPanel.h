#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
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
    PatchMutatorPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchMutatorPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);

private:
    inline constexpr static int kSpacing_ = 5;

    int width_;
    int height_;
    tss::ISkin* skin_;
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

    void propagateSkinsToControlWidgets(tss::ISkin& skin);
    void propagateSkinsToToggleWidgets(tss::ISkin& skin);

    void setupModuleHeader(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupAmountLine(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupRandomLine(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void setupHistoryLine(tss::ISkin& skin, WidgetFactory& widgetFactory);
    void connectButtonToApvts(tss::Button* button, const char* widgetId);
    void connectToggleToApvts(tss::Toggle* toggle, const char* widgetId);

    void layoutModuleHeader(int x, int y);
    void layoutAmountLine(int x, int& y);
    void layoutRandomLine(int x, int& y);
    void layoutHistoryLine(int x, int& y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchMutatorPanel)
};
