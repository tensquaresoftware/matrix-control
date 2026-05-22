#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class VerticalSeparator : public juce::Component
    {
    public:
        explicit VerticalSeparator(int width, int height, const VerticalSeparatorLook& look);
        ~VerticalSeparator() override = default;

        void setLook(const VerticalSeparatorLook& look);
        void setUiScale(float uiScale);

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static float kLineWidth_ = 4.0f;
        inline constexpr static float kTopPadding_ = 4.0f;
        inline constexpr static float kBottomPadding_ = 2.0f;

        VerticalSeparatorLook look_{};
        int width_;
        int height_;
        float uiScale_ = 1.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VerticalSeparator)
    };
}
