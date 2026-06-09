#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"

namespace TSS
{
    class Skin;
}

class WidgetFactory;

class MainComponent : public juce::Component
{
public:
    MainComponent(TSS::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::Skin& skin);
    void setUiScale(float uiScale);

    HeaderPanel& getHeaderPanel() { return headerPanel; }
    BodyPanel& getBodyPanel() { return bodyPanel; }
    FooterPanel& getFooterPanel() { return footerPanel; }

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

    void setUiElementsTestVisible(bool visible);
    juce::Rectangle<int> getUiElementsTestAreaBounds() const;

private:
    TSS::Skin* skin_;
    float uiScale_ = 1.0f;
    bool uiElementsTestVisible_ = false;
    int uiElementsTestAreaY_ = 0;

    HeaderPanel headerPanel;
    BodyPanel bodyPanel;
    FooterPanel footerPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

