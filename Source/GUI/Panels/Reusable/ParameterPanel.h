#pragma once

#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class Skin;
    class Label;
    class Slider;
    class ComboBox;
    class HorizontalSeparator;
}

class WidgetFactory;

class ParameterPanel : public juce::Component
{
public:
    enum class ParameterType { Slider, ComboBox, None };
    enum class ModuleType { PatchEdit, MasterEdit };

    ParameterPanel(tss::Skin& skin,
                   WidgetFactory& factory,
                   const juce::String& parameterId,
                   ParameterType type,
                   ModuleType moduleType,
                   juce::AudioProcessorValueTreeState& apvts);
    ~ParameterPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);
    int getTotalHeight() const;

    tss::Slider* getSlider() const { return slider_.get(); }

private:
    struct ParameterPanelDimensions
    {
        int labelWidth;
        int comboBoxWidth;
        int separatorWidth;
    };

    ParameterPanelDimensions getDimensionsForModuleType(ModuleType moduleType) const;

    void createParameterLabel(tss::Skin& skin, WidgetFactory& factory, const juce::String& parameterId);
    void createParameterWidget(tss::Skin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createSliderWidget(tss::Skin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createComboBoxWidget(tss::Skin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createSeparator(tss::Skin& skin);

    void layoutParameterLabel(int y);
    void layoutParameterWidget(int y);
    void layoutSeparator(int y);

    tss::Skin* skin_;
    ParameterType parameterType_;
    ModuleType moduleType_;

    std::unique_ptr<tss::Label> label_;
    std::unique_ptr<tss::Slider> slider_;
    std::unique_ptr<tss::ComboBox> comboBox_;
    std::unique_ptr<tss::HorizontalSeparator> separator_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterPanel)
};
