#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class Toggle : public juce::ToggleButton
    {
    public:
        explicit Toggle(ISkin& skin, int width, const juce::String& text = {});

        void setSkin(ISkin& skin);
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
        inline constexpr static int kBorderThickness_ = 2;

        ISkin* skin_ = nullptr;
        int width_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Toggle)
    };
}
