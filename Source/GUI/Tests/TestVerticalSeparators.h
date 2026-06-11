#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestVerticalSeparators : public juce::Component
{
public:
    explicit TestVerticalSeparators(TSS::ISkin& skin, const SeparatorDimensions& dimensions);
    ~TestVerticalSeparators() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class VerticalSeparatorScalePanel;

    TSS::ISkin* skin_ = nullptr;
    SeparatorDimensions dimensions_;
    std::vector<std::unique_ptr<VerticalSeparatorScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestVerticalSeparators)
};
