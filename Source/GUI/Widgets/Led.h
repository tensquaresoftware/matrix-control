#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    class ISkin;

    /// Square activity LED with pulse + decay level (UX-DR3).
    /// Visual release is governed solely by kReleaseTimeMs_ (Core only signals activity edges).
    class Led : public juce::Component,
                private juce::Timer
    {
    public:
        Led(int width, int height);
        ~Led() override = default;

        void paint(juce::Graphics& g) override;
        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);
        void setLevel(float normalisedLevel);

    private:
        inline constexpr static float kBorderThicknessDesign_ = 2.0f;
        inline constexpr static juce::int64 kReleaseTimeMs_ = 300;
        inline constexpr static float kReleaseCurveExponent_ = 3.0f;
        inline constexpr static int kAnimationHz_ = 60;

        ISkin* skin_ = nullptr;
        ButtonLook buttonLook_{};
        SliderLook sliderLook_{};
        float uiScale_ = 1.0f;
        float displayedLevel_ = 0.0f;
        juce::int64 lastUpdateMs_ = 0;

        void timerCallback() override;
        void advanceRelease(juce::int64 nowMs);
        void ensureAnimationTimerRunning();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Led)
    };
}
