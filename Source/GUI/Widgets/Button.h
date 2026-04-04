#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class Button : public juce::Button
    {
    public:
        explicit Button(int width, int height, const juce::String& text = {});
        ~Button() override = default;

        void setLook(const ButtonLook& look);
        void setDisplayScale(float displayScale);
        
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        
        static int getBaseWidth() { return kDefaultWidth_; }
        static int getBaseHeight() { return kDefaultHeight_; }

    private:
        inline constexpr static int kDefaultWidth_ = 50;
        inline constexpr static int kDefaultHeight_ = 20;
        inline constexpr static int kBorderThickness_ = 2;
        inline constexpr static float kFontSize_ = 14.0f;

        ButtonLook look_{};
        int width_;
        int height_;
        float displayScale_ = 1.0f;

        juce::Colour getBackgroundColour(bool enabled, bool isHighlighted, bool isDown) const;
        juce::Colour getBorderColour(bool enabled) const;
        juce::Colour getTextColour(bool enabled, bool isHighlighted, bool isDown) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Button)
    };
}
