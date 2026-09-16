#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;

class MidiPanel : public BaseModulePanel
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

    explicit MidiPanel(const Config& config);
    ~MidiPanel() override = default;

    static ModulePanelLayout createLayout();

private:
    void registerContextualHelp();

    std::unique_ptr<TSS::ContextualHelpBinder> contextualHelpBinder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPanel)
};
