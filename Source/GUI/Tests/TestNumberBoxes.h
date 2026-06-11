#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestNumberBoxes : public juce::Component
{
public:
    explicit TestNumberBoxes(TSS::ISkin& skin, const NumberBoxDimensions& dimensions);
    ~TestNumberBoxes() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class NumberBoxScalePanel;

    TSS::ISkin* skin_ = nullptr;
    NumberBoxDimensions dimensions_;
    std::vector<std::unique_ptr<NumberBoxScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestNumberBoxes)
};
