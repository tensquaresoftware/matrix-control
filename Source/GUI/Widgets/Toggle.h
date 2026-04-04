#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class Toggle : public juce::ToggleButton
    {
    public:
        explicit Toggle(int width, const juce::String& text = {});

        void setLook(const ToggleLook& look);
        void setDisplayScale(float displayScale);
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
        inline constexpr static int kBorderThickness_ = 2;

        ToggleLook look_{};
        int width_;
        float displayScale_ = 1.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Toggle)
    };
}
