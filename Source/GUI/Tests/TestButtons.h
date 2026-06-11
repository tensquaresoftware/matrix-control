#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestButtons : public juce::Component
{
public:
    explicit TestButtons(TSS::ISkin& skin);
    ~TestButtons() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ButtonScalePanel;

    inline constexpr static int kGap_ = 12;
    inline constexpr static int kMainButtonWidth_ = 48;
    inline constexpr static int kMainButtonHeight_ = 20;
    inline constexpr static int kSmallButtonSize_ = 20;
    inline constexpr static int kBigButtonWidth_ = 120;
    inline constexpr static int kBigButtonHeight_ = 48;
    inline constexpr static int kScaleLabelHeight_ = 20;
    inline constexpr static int kPanelGapMultiplier_ = 2;
    inline constexpr static float kLargeTextFontHeight_ = 28.0f;

    TSS::ISkin* skin_ = nullptr;
    std::vector<std::unique_ptr<ButtonScalePanel>> columnPanels_;

    void createColumnPanels(TSS::ISkin& skin);
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestButtons)
};
