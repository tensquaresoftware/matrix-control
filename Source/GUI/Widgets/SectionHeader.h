#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class SectionHeader : public juce::Component
    {
    public:
        enum class ColourVariant
        {
            Blue,
            Orange
        };

        explicit SectionHeader(int width, int height, const juce::String& text = juce::String(), ColourVariant variant = ColourVariant::Blue);
        ~SectionHeader() override = default;

        void setLook(const SectionHeaderLook& look);

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static float kContentHeight_ = 20.0f;
        inline constexpr static float kLineHeight_ = 12.0f;
        inline constexpr static float kLeftLineWidth_ = 20.0f;
        inline constexpr static float kTextSpacing_ = 8.0f;

        SectionHeaderLook look_{};
        int width_;
        int height_;
        juce::String text_;
        ColourVariant colourVariant_;
        float cachedTextWidth_ = 0.0f;

        void calculateTextWidth();

        void drawText(juce::Graphics& g, const juce::Rectangle<float>& contentArea);
        void drawLines(juce::Graphics& g, const juce::Rectangle<float>& contentArea);
        void drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea);
        void drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& contentArea);

        juce::Colour getLineColour() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionHeader)
    };
}
