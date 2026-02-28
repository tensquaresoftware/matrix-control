#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Panels/Reusable/BaseModulePanel.h"

namespace tss
{
    class ISkin;
}

class WidgetFactory;

class VcfVcaPanel : public BaseModulePanel
{
public:
    VcfVcaPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~VcfVcaPanel() override = default;

private:
    static ModulePanelConfig createConfig();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VcfVcaPanel)
};
