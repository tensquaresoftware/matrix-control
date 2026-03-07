#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class HorizontalSeparator : public juce::Component
    {
    public:
        HorizontalSeparator(int width, int height);
        ~HorizontalSeparator() override = default;

        void setLook(const HorizontalSeparatorLook& look);
        void setScalingFactor(float scalingFactor);
        
        void paint(juce::Graphics& g) override;
        
        static int getBaseWidth() { return kDefaultWidth_; }
        static int getBaseHeight() { return kDefaultHeight_; }

    private:
        static constexpr int kDefaultWidth_ = 50;
        static constexpr int kDefaultHeight_ = 5;
        static constexpr int kLineThickness_ = 1;

        HorizontalSeparatorLook look_{};
        int width_;
        int height_;
        float scalingFactor_ = 1.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HorizontalSeparator)
    };
}
