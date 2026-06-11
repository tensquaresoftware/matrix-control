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

class TestParameterCells : public juce::Component
{
public:
    TestParameterCells(TSS::ISkin& skin,
                       WidgetFactory& widgetFactory,
                       juce::AudioProcessorValueTreeState& apvts,
                       const ParameterCellDimensions& dimensions);
    ~TestParameterCells() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ParameterCellScalePanel;

    TSS::ISkin* skin_ = nullptr;
    WidgetFactory* widgetFactory_ = nullptr;
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    ParameterCellDimensions dimensions_;
    std::vector<std::unique_ptr<ParameterCellScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestParameterCells)
};
