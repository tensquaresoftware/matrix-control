#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    class GroupLabel : public juce::Component
    {
    public:
        explicit GroupLabel(int width, int height, const GroupLabelLook& look, const juce::String& text = juce::String());
        ~GroupLabel() override = default;

        void setLook(const GroupLabelLook& look);
        void setUiScale(float uiScale);

        void setText(const juce::String& text);
        juce::String getText() const { return labelText_; }

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kTextSpacing_ = 8;
        inline constexpr static int kLineThickness_ = 1;

        GroupLabelLook look_{};
        int width_;
        int height_;
        float uiScale_ = 1.0f;
        juce::String labelText_;
        float cachedTextWidth_ {0.0f};

        void calculateTextWidth();

        void drawText(juce::Graphics& g, const juce::Rectangle<float>& area);
        void drawLines(juce::Graphics& g, const juce::Rectangle<float>& area, float textWidth);
        void drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY, float lineThickness, float textSpacing);
        void drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY, float lineThickness, float textSpacing);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupLabel)
    };
}
