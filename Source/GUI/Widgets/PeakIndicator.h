#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    class ISkin;

    /// Solid vertical peak fill (D-071) — not a continuous VU meter.
    class PeakIndicator : public juce::Component,
                          private juce::Timer
    {
    public:
        PeakIndicator(int width, int height);
        ~PeakIndicator() override = default;

        void paint(juce::Graphics& g) override;
        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);
        void setLevel(float normalisedLevel);

    private:
        inline constexpr static float kBorderThicknessDesign_ = 2.0f;
        inline constexpr static juce::int64 kReleaseTimeMs_ = 600;
        inline constexpr static int kAnimationHz_ = 60;

        ISkin* skin_ = nullptr;
        ButtonLook buttonLook_{};
        SliderLook sliderLook_{};
        int width_;
        int height_;
        float uiScale_ = 1.0f;
        float displayedLevel_ = 0.0f;
        float lastInputLevel_ = 0.0f;
        juce::int64 lastUpdateMs_ = 0;

        void timerCallback() override;
        void advanceRelease(juce::int64 nowMs);
        void updateBallistics(juce::int64 nowMs, float normalisedLevel);
        void ensureAnimationTimerRunning();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PeakIndicator)
    };
}
