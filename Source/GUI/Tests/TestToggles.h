#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestToggles : public juce::Component
{
public:
    explicit TestToggles(TSS::ISkin& skin, const ToggleDimensions& dimensions);
    ~TestToggles() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ToggleScalePanel;

    TSS::ISkin* skin_ = nullptr;
    ToggleDimensions dimensions_;
    std::vector<std::unique_ptr<ToggleScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestToggles)
};
