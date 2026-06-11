#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestPeakIndicators : public juce::Component
    , private juce::Timer
{
public:
    explicit TestPeakIndicators(TSS::ISkin& skin, int indicatorWidth, int indicatorHeight);
    ~TestPeakIndicators() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    void visibilityChanged() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class PeakIndicatorScalePanel;

    TSS::ISkin* skin_ = nullptr;
    int indicatorWidth_ = 0;
    int indicatorHeight_ = 0;
    int msUntilNextPulse_ = 0;
    int peakHoldRemainingMs_ = 0;
    std::vector<std::unique_ptr<PeakIndicatorScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();
    void triggerPeakOnAllIndicators();
    void restoreAllIndicatorLevels();
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestPeakIndicators)
};
