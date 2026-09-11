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

class Lfo1Panel : public BaseModulePanel
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

    explicit Lfo1Panel(const Config& config);
    ~Lfo1Panel() override = default;

    static ModulePanelLayout createLayout();

private:
    static constexpr int kTriggerModeCellIndex = 6;
    static constexpr int kTriggerStrigIndex = 1;

    void refreshStrigGate(bool clearIfCurrentStrig);

    std::unique_ptr<TSS::StrigUnisonGateHelper::KeyboardModeChangeListener> keyboardModeListener_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lfo1Panel)
};
