#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    /// Solid vertical peak fill (D-071) — not a continuous VU meter.
    class PeakIndicator : public juce::Component
    {
    public:
        PeakIndicator(int width, int height);
        ~PeakIndicator() override = default;

        void paint(juce::Graphics& g) override;
        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);
        void setLevel(float normalisedLevel);

    private:
        inline constexpr static int kWidth_ = 8;

        ISkin* skin_ = nullptr;
        int width_;
        int height_;
        float uiScale_ = 1.0f;
        float level_ = 0.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PeakIndicator)
    };
}
