#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"

namespace tss
{
    class Skin;
}

class WidgetFactory;

class MainComponent : public juce::Component
{
public:
    MainComponent(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(tss::Skin& skin);
    void setScalingFactor(float scalingFactor);

    HeaderPanel& getHeaderPanel() { return headerPanel; }
    BodyPanel& getBodyPanel() { return bodyPanel; }
    FooterPanel& getFooterPanel() { return footerPanel; }

private:
    void layoutHeaderPanel(juce::Rectangle<int> bounds, float y);
    void layoutBodyPanel(juce::Rectangle<int> bounds, float y);
    void layoutFooterPanel(juce::Rectangle<int> bounds, float y);

    tss::Skin* skin_;
    float scalingFactor_ = 1.0f;

    HeaderPanel headerPanel;
    BodyPanel bodyPanel;
    FooterPanel footerPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

