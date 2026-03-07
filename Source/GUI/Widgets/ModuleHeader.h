#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class ModuleHeader : public juce::Component
    {
    public:
        enum class ColourVariant
        {
            Blue,
            Orange
        };

        explicit ModuleHeader(const juce::String& text, int width, int height, ColourVariant variant = ColourVariant::Blue);
        ~ModuleHeader() override = default;

        void setLook(const ModuleHeaderLook& look);
        void setScalingFactor(float scalingFactor);

        void setText(const juce::String& text);
        juce::String getText() const { return text_; }

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kTextLeftPadding_ = 2;
        inline constexpr static int kTextAreaHeight_ = 20;
        inline constexpr static int kLineThickness_ = 4;

        ModuleHeaderLook look_{};
        int width_;
        int height_;
        juce::String text_;
        ColourVariant colourVariant_;
        float scalingFactor_ = 1.0f;

        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds);

        juce::Colour getLineColour() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleHeader)
    };
}
