#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class PatchNameDisplay : public juce::Component
    {
    public:
        explicit PatchNameDisplay(ISkin& skin, int width, int height);
        ~PatchNameDisplay() override = default;

        void setSkin(ISkin& skin);
        void setPatchName(const juce::String& patchName);

        void paint(juce::Graphics& g) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kBorderThickness_ = 2;
        inline constexpr static float kFontHeight_ = 28.0f;

        ISkin* skin_ = nullptr;
        int width_;
        int height_;
        juce::String patchName_;

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    };
}
