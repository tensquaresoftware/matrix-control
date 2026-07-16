#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"

namespace Core
{
    class PatchFileService;
}

namespace TSS
{
    class ISkin;
    class CompareLockBinder;
}

class MatrixModulationPanel;
class PatchManagerPanel;
class WidgetFactory;

class SharedPanel : public juce::Component
{
public:
    SharedPanel(TSS::ISkin& skin,
                const SharedPanelDimensions& dims,
                WidgetFactory& widgetFactory,
                juce::AudioProcessorValueTreeState& apvts,
                const Core::PatchFileService& patchFileService);
    ~SharedPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

private:
    SharedPanelDimensions dims_;
    float uiScale_ = 1.0f;

    std::unique_ptr<MatrixModulationPanel> matrixModulationPanel_;
    std::unique_ptr<PatchManagerPanel> patchManagerPanel_;
    std::unique_ptr<TSS::CompareLockBinder> compareLockBinder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SharedPanel)
};
