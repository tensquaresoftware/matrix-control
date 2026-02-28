#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"

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

    tss::ComboBox& getSkinComboBox() { return skinComboBox_; }
    tss::ComboBox& getGuiScaleComboBox() { return guiScaleComboBox_; }

    static int getSpacing() { return kSpacing_; }

private:
    int width_;
    int height_;
    inline constexpr static int kSpacing_ = 5;
    inline constexpr static int kSkinLabelWidth_ = 30;
    inline constexpr static int kGuiScaleLabelWidth_ = 25;
    inline constexpr static int kComboBoxWidth_ = 50;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLeftPadding_ = 15;

    tss::ISkin* skin_;

    tss::Label skinLabel_;
    tss::ComboBox skinComboBox_;
    tss::Label guiScaleLabel_;
    tss::ComboBox guiScaleComboBox_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};

