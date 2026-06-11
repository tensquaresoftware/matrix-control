#pragma once

#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;

class TestModulationBusCells : public juce::Component
{
public:
    TestModulationBusCells(TSS::ISkin& skin,
                           WidgetFactory& widgetFactory,
                           juce::AudioProcessorValueTreeState& apvts,
                           const ModulationBusCellDimensions& dimensions,
                           int panelWidth,
                           int panelHeight);
    ~TestModulationBusCells() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ModulationBusCellScalePanel;

    TSS::ISkin* skin_ = nullptr;
    WidgetFactory* widgetFactory_ = nullptr;
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    ModulationBusCellDimensions dimensions_;
    int panelWidth_ = 0;
    int panelHeight_ = 0;
    std::vector<std::unique_ptr<ModulationBusCellScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestModulationBusCells)
};
