#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"

namespace tss
{
    class ISkin;
    class Label;
    class ComboBox;
}

class HeaderPanel : public juce::Component
{
public:
    HeaderPanel(tss::ISkin& skin, int width, int height);
    ~HeaderPanel() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);

    tss::ComboBox& getSkinComboBox() { return skinComboBox_; }
    tss::ComboBox& getUiScaleComboBox() { return uiScaleComboBox_; }
    tss::Button& getUiElementsButton() { return uiElementsButton_; }

    static int getGap() { return kGap_; }

private:
    int width_;
    int height_;
    inline constexpr static int kGap_ = 5;
    inline constexpr static int kSkinLabelWidth_ = 30;
    inline constexpr static int kUiScaleLabelWidth_ = 35;
    inline constexpr static int kComboBoxWidth_ = 50;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLeftPadding_ = 15;
    inline constexpr static int kRightPadding_ = 15;
    inline constexpr static int kUiElementsButtonWidth_ = 88;

    tss::ISkin* skin_;
    float uiScale_ = 1.0f;

    tss::Label skinLabel_;
    tss::ComboBox skinComboBox_;
    tss::Label uiScaleLabel_;
    tss::ComboBox uiScaleComboBox_;
    tss::Button uiElementsButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
