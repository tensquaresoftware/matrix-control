#pragma once

#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
    class Label;
    class Slider;
    class ComboBox;
    class HorizontalSeparator;
}

class WidgetFactory;

class ParameterCell : public juce::Component
{
public:
    enum class ParameterType { Slider, ComboBox, None };
    enum class ModuleType { PatchEdit, MasterEdit };

    ParameterCell(TSS::ISkin& skin,
                  WidgetFactory& factory,
                  const juce::String& parameterId,
                  ParameterType type,
                  ModuleType moduleType,
                  juce::AudioProcessorValueTreeState& apvts,
                  const ParameterCellDimensions& dimensions);
    ~ParameterCell() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    int getDesignRowHeight() const;

    TSS::Slider* getSlider() const { return slider_.get(); }

private:
    void createParameterLabel(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId);
    void createParameterWidget(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createSliderWidget(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createComboBoxWidget(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createSeparator(TSS::ISkin& skin);

    void layoutParameterLabel(int y);
    void layoutParameterWidget(int y);
    void layoutSeparator(int yTop, int separatorHeight);
    void applyChildUiScales();

    TSS::ISkin* skin_;
    ParameterType parameterType_;
    ModuleType moduleType_;
    ParameterCellDimensions dimensions_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::Label> label_;
    std::unique_ptr<TSS::Slider> slider_;
    std::unique_ptr<TSS::ComboBox> comboBox_;
    std::unique_ptr<TSS::HorizontalSeparator> separator_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterCell)
};
