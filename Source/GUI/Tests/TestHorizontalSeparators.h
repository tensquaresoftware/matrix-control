#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestHorizontalSeparators : public juce::Component
{
public:
    explicit TestHorizontalSeparators(TSS::ISkin& skin, int separatorWidth, int separatorHeight);
    ~TestHorizontalSeparators() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class HorizontalSeparatorScalePanel;

    TSS::ISkin* skin_ = nullptr;
    int separatorWidth_ = 0;
    int separatorHeight_ = 0;
    std::vector<std::unique_ptr<HorizontalSeparatorScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestHorizontalSeparators)
};
