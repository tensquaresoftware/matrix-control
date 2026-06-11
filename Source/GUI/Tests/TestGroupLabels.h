#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestGroupLabels : public juce::Component
{
public:
    explicit TestGroupLabels(TSS::ISkin& skin, const GroupLabelDimensions& dimensions);
    ~TestGroupLabels() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class GroupLabelScalePanel;

    TSS::ISkin* skin_ = nullptr;
    GroupLabelDimensions dimensions_;
    std::vector<std::unique_ptr<GroupLabelScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestGroupLabels)
};
