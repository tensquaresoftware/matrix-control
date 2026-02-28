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

class Dco1Panel : public BaseModulePanel
{
public:
    Dco1Panel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~Dco1Panel() override = default;

    static int getWidth() { return PluginDimensions::Panels::Body::PatchEditSection::TopModules::ChildModules::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::PatchEditSection::TopModules::ChildModules::kHeight; }

private:
    static ModulePanelConfig createConfig();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dco1Panel)
};

