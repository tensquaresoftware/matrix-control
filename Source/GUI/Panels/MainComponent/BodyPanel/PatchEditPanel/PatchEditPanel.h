#pragma once

#include <array>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
    class SectionHeader;
}

class WidgetFactory;
class PatchEditTopModulesPanel;
class PatchEditDisplaysPanel;
class PatchEditBottomModulesPanel;

class PatchEditPanel : public juce::Component,
                       public juce::Slider::Listener
{
public:
    PatchEditPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    
    void sliderValueChanged(juce::Slider* slider) override;

private:
    inline constexpr static int kTrackPointSliderStartIndex_ = 3;
    inline constexpr static int kTrackPointSliderCount_ = 5;
    inline constexpr static int kEnvParamCount_ = 5;
    inline constexpr static int kEnvCount_ = 3;
    
    int width_;
    int height_;
    int topPanelHeight_;
    int middlePanelHeight_;
    int bottomPanelHeight_;
    tss::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<tss::SectionHeader> sectionHeader_;
    std::unique_ptr<PatchEditTopModulesPanel> patchEditTopModulesPanel_;
    std::unique_ptr<PatchEditDisplaysPanel> patchEditDisplaysPanel_;
    std::unique_ptr<PatchEditBottomModulesPanel> patchEditBottomModulesPanel_;
    
    std::array<juce::Slider*, kTrackPointSliderCount_> trackPointSliders_ {nullptr, nullptr, nullptr, nullptr, nullptr};
    std::array<std::array<juce::Slider*, kEnvParamCount_>, kEnvCount_> envSliders_ 
    {
        std::array<juce::Slider*, kEnvParamCount_> {nullptr, nullptr, nullptr, nullptr, nullptr},
        std::array<juce::Slider*, kEnvParamCount_> {nullptr, nullptr, nullptr, nullptr, nullptr},
        std::array<juce::Slider*, kEnvParamCount_> {nullptr, nullptr, nullptr, nullptr, nullptr}
    };
    
    void setupTrackPointSliderConnections();
    void setupEnvelopeSliderConnections();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditPanel)
};

