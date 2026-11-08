#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "GUI/Skins/SkinValues.h"

class SkinColourWiringTests;

namespace TSS
{
    class ISkin;

    /// Solid vertical peak fill (D-071). Visual release is governed solely by kReleaseTimeMs_.
    class PeakIndicator : public juce::Component,
                          private juce::Timer
    {
    public:
        PeakIndicator(int width, int height, ColourSet colourSet = ColourSet::k1);
        ~PeakIndicator() override = default;

        void paint(juce::Graphics& g) override;
        void setColourSet(ColourSet colourSet);
        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);
        void setLevel(float normalisedLevel);
        ColourSet getColourSet() const noexcept { return colourSet_; }

    private:
        friend class ::SkinColourWiringTests;

        inline constexpr static float kBorderThicknessDesign_ = 2.0f;
        inline constexpr static juce::int64 kReleaseTimeMs_ = 300;
        inline constexpr static float kReleaseCurveExponent_ = 3.0f;
        inline constexpr static int kAnimationHz_ = 60;

        ISkin* skin_ = nullptr;
        ColourSet colourSet_ = ColourSet::k1;
        LedLook ledLook_{};
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
