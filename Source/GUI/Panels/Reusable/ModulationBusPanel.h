#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Panels/Reusable/ModulationBusPanelDimensions.h"

namespace tss
{
    class ISkin;
    class Label;
    class ComboBox;
    class Slider;
    class Button;
    class HorizontalSeparator;
}

class WidgetFactory;

class ModulationBusPanel : public juce::Component
{
public:
    ModulationBusPanel(tss::ISkin& skin,
                      int width,
                      int height,
                      const ModulationBusPanelDimensions& dimensions,
                      int busNumber,
                      WidgetFactory& factory,
                      juce::AudioProcessorValueTreeState& apvts,
                      const juce::String& sourceParamId,
                      const juce::String& amountParamId,
                      const juce::String& destinationParamId,
                      const juce::String& busId);
    ~ModulationBusPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    int getHeight() const { return dimensions_.panelHeight; }

private:
    ModulationBusPanelDimensions dimensions_;
    void createBusNumberLabel(int busNumber, tss::ISkin& skin);
    void createSourceComboBox(WidgetFactory& factory, tss::ISkin& skin, const juce::String& sourceParamId, juce::AudioProcessorValueTreeState& apvts);
    void createAmountSlider(WidgetFactory& factory, tss::ISkin& skin, const juce::String& amountParamId, juce::AudioProcessorValueTreeState& apvts);
    void createDestinationComboBox(int busNumber, tss::ISkin& skin, const juce::String& destinationParamId, juce::AudioProcessorValueTreeState& apvts);
    void createInitButton(tss::ISkin& skin, int busNumber);
    void createSeparator(tss::ISkin& skin);

    void layoutWidgetRow();
    void layoutSeparator(int y);

    inline constexpr static int kSpacing_ = 5;

    tss::ISkin* skin_;

    std::unique_ptr<tss::Label> busNumberLabel_;
    std::unique_ptr<tss::ComboBox> sourceComboBox_;
    std::unique_ptr<tss::Slider> amountSlider_;
    std::unique_ptr<tss::ComboBox> destinationComboBox_;
    std::unique_ptr<tss::Button> initButton_;
    std::unique_ptr<tss::HorizontalSeparator> separator_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> destinationAttachment_;

    juce::AudioProcessorValueTreeState& apvts_;
    juce::String busId_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBusPanel)
};
