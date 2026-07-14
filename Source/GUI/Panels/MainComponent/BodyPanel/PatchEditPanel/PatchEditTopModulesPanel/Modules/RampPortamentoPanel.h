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

class RampPortamentoPanel : public BaseModulePanel,
                            private juce::AudioProcessorValueTreeState::Listener
{
public:
    RampPortamentoPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts,
          const ModuleHeaderDimensions& moduleHeaderDims, const ParameterCellDimensions& parameterCellDims);
    ~RampPortamentoPanel() override;

    static ModulePanelLayout createLayout();

private:
    static constexpr int kUnisonKeyboardModeIndex = 2;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void refreshLegatoPortaGraying();

    bool legatoPortaGrayed_ = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RampPortamentoPanel)
};
