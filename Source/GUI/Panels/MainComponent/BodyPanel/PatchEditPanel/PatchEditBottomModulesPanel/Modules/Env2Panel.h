#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

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
    Env2Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
          const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims);
    ~Env2Panel() override = default;

private:
    static ModulePanelConfig createConfig();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Env2Panel)
};
