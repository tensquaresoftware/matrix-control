#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"

namespace TSS
{
    class ISkin;
}

class SettingsPanel : public juce::Component
{
public:
    static constexpr int kDesignWidth = 240;
    static constexpr int kDesignHeight = 220;

    SettingsPanel(TSS::ISkin& skin, bool isPluginMode);
    ~SettingsPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPluginMode);

    TSS::Slider& getHardwareLatencySlider() { return hardwareLatencySlider_; }
    TSS::Button& getUiElementsButton() { return uiElementsButton_; }

private:
    void updateModeSpecificVisibility();
    void layoutContent(juce::Rectangle<int> bounds);

    inline constexpr static int kPadding_ = 16;
    inline constexpr static int kGap_ = 8;
    inline constexpr static int kRowGap_ = 12;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLabelWidth_ = 96;
    inline constexpr static int kSliderWidth_ = 72;
    inline constexpr static int kContentWidth_ = kDesignWidth - kPadding_ * 2;

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;

    TSS::Label hardwareLatencyLabel_;
    TSS::Slider hardwareLatencySlider_;
    TSS::Label masterOpsLabel_;
    TSS::Label masterOpsPlaceholder_;
    TSS::Label policiesLabel_;
    TSS::Label policiesPlaceholder_;
    TSS::Label defragLabel_;
    TSS::Label defragPlaceholder_;
    TSS::Label loggingLabel_;
    TSS::Label loggingPlaceholder_;
    TSS::Button uiElementsButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};
