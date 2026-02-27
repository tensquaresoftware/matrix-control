#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDimensions.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"

namespace tss
{
    class Skin;
}

class WidgetFactory;

class Lfo1Panel : public BaseModulePanel
{
public:
    Lfo1Panel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~Lfo1Panel() override = default;

    static int getWidth() { return PluginDimensions::Panels::Body::PatchEdit::Bottom::ChildPanels::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchEdit::Bottom::ChildPanels::kHeight; }

private:
    static ModulePanelConfig createConfig();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lfo1Panel)
};
