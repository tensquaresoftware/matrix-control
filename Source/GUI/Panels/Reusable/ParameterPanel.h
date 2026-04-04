#pragma once

#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
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

    ParameterPanel(tss::ISkin& skin,
                   WidgetFactory& factory,
                   const juce::String& parameterId,
                   ParameterType type,
                   ModuleType moduleType,
                   juce::AudioProcessorValueTreeState& apvts);
    ~ParameterPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setDisplayScale(float displayScale);
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

    void createParameterLabel(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId);
    void createParameterWidget(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createSliderWidget(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createComboBoxWidget(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts);
    void createSeparator(tss::ISkin& skin);

    void layoutParameterLabel(int y);
    void layoutParameterWidget(int y);
    void layoutSeparator(int yTop, int separatorHeight);
    void applyChildDisplayScales();

    tss::ISkin* skin_;
    ParameterType parameterType_;
    ModuleType moduleType_;
    float displayScale_ = 1.0f;

    std::unique_ptr<tss::Label> label_;
    std::unique_ptr<tss::Slider> slider_;
    std::unique_ptr<tss::ComboBox> comboBox_;
    std::unique_ptr<tss::HorizontalSeparator> separator_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterPanel)
};
