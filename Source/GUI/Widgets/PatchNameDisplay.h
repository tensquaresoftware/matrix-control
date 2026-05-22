#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class PatchNameDisplay : public juce::Component
    {
    public:
        explicit PatchNameDisplay(int width, int height, const PatchNameDisplayLook& look);
        ~PatchNameDisplay() override = default;

        void setLook(const PatchNameDisplayLook& look);
        void setUiScale(float uiScale);
        void setPatchName(const juce::String& patchName);

        void paint(juce::Graphics& g) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kBorderThickness_ = 2;

        PatchNameDisplayLook look_{};
        int width_;
        int height_;
        juce::String patchName_;
        float uiScale_ = 1.0f;

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    };
}
