#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Helpers/StrigUnisonGateHelper.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;

class Env2Panel : public BaseModulePanel
{
public:
    struct Config
    {
        TSS::ISkin& skin;
        int width = 0;
        int height = 0;
        WidgetFactory& widgetFactory;
        juce::AudioProcessorValueTreeState& apvts;
        const ModuleHeaderDimensions& moduleHeaderDims;
        const ParameterCellDimensions& parameterCellDims;
    };

    explicit Env2Panel(const Config& config);
    ~Env2Panel() override = default;

    static ModulePanelLayout createLayout();

private:
    static constexpr int kTriggerModeCellIndex = 7;
    static constexpr int kTriggerStrigIndex = 0;

    void refreshStrigGate(bool clearIfCurrentStrig);

    std::unique_ptr<TSS::StrigUnisonGateHelper::KeyboardModeChangeListener> keyboardModeListener_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Env2Panel)
};
