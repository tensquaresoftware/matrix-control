#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestSliders : public juce::Component
{
public:
    explicit TestSliders(TSS::ISkin& skin);
    ~TestSliders() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class SliderScalePanel;

    inline constexpr static int kGap_ = 12;
    inline constexpr static int kScaleLabelHeight_ = 20;

    inline constexpr static int kTopSliderWidth_ = 60;
    inline constexpr static int kTopSliderHeight_ = 16;
    inline constexpr static int kBottomSliderWidth_ = 132;
    inline constexpr static int kBottomSliderHeight_ = 48;
    inline constexpr static float kBoldLargeSliderFontHeight_ = 32.0f;

    TSS::ISkin* skin_ = nullptr;
    std::vector<std::unique_ptr<SliderScalePanel>> columnPanels_;

    void createColumnPanels(TSS::ISkin& skin);
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestSliders)
};
