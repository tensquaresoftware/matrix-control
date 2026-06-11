#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestActivityLeds : public juce::Component
{
public:
    explicit TestActivityLeds(TSS::ISkin& skin, int ledSize);
    ~TestActivityLeds() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ActivityLedScalePanel;

    TSS::ISkin* skin_ = nullptr;
    int ledSize_ = 0;
    std::vector<std::unique_ptr<ActivityLedScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestActivityLeds)
};
