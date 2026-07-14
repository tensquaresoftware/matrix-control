#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    class Button : public juce::Button
    {
    public:
        explicit Button(int width, int height, const ButtonLook& look, const juce::String& text = {});
        ~Button() override = default;

        void setLook(const ButtonLook& look);
        void setUiScale(float uiScale);
        void setInactiveAppearance(bool inactive);

        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kBorderThickness_ = 2;

        ButtonLook look_{};
        int width_;
        int height_;
        float uiScale_ = 1.0f;
        bool inactiveAppearance_ = false;

        juce::Colour getBackgroundColour(bool enabled, bool isHighlighted, bool isDown) const;
        juce::Colour getBorderColour(bool enabled) const;
        juce::Colour getTextColour(bool enabled, bool isHighlighted, bool isDown) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Button)
    };
}
