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

class Lfo1Panel : public BaseModulePanel
{
public:
    Lfo1Panel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
          const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims);
    ~Lfo1Panel() override = default;

private:
    static ModulePanelLayout createLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lfo1Panel)
};
