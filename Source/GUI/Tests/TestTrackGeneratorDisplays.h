#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestTrackGeneratorDisplays : public juce::Component
{
public:
    explicit TestTrackGeneratorDisplays(TSS::ISkin& skin, const DisplayBandDimensions& dimensions);
    ~TestTrackGeneratorDisplays() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class TrackGeneratorDisplayScalePanel;

    TSS::ISkin* skin_ = nullptr;
    DisplayBandDimensions dimensions_;
    std::vector<std::unique_ptr<TrackGeneratorDisplayScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestTrackGeneratorDisplays)
};
