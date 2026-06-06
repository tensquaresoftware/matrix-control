#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class ISkin;
}

class MatrixModulationPanel;
class PatchManagerPanel;
class WidgetFactory;

class SharedPanel : public juce::Component
{
public:
    SharedPanel(tss::ISkin& skin, int width, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~SharedPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

private:
    int width_;
    int matrixModulationPanelHeight_;
    int patchManagerPanelHeight_;
    float uiScale_ = 1.0f;

    std::unique_ptr<MatrixModulationPanel> matrixModulationPanel_;
    std::unique_ptr<PatchManagerPanel> patchManagerPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SharedPanel)
};
