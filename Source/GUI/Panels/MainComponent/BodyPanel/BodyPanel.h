#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace TSS
{
    class ISkin;
    class VerticalSeparator;
}

class PatchEditPanel;
class SharedPanel;
class MasterEditPanel;
class WidgetFactory;

class BodyPanel : public juce::Component
{
public:
    BodyPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BodyPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

private:
    int width_;
    int height_;
    int padding_;
    int patchEditPanelWidth_;
    int patchEditPanelHeight_;
    int masterEditPanelWidth_;
    int masterEditPanelHeight_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<PatchEditPanel> patchEditPanel_;
    std::unique_ptr<TSS::VerticalSeparator> verticalSeparator1_;
    std::unique_ptr<SharedPanel> sharedPanel_;
    std::unique_ptr<TSS::VerticalSeparator> verticalSeparator2_;
    std::unique_ptr<MasterEditPanel> masterEditPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BodyPanel)
};

