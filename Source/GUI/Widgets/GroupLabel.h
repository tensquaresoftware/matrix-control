#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class Skin;

    class GroupLabel : public juce::Component
    {
    public:
        explicit GroupLabel(Skin& skin, int width, int height, const juce::String& text = juce::String());
        ~GroupLabel() override = default;

        void setSkin(Skin& skin);

        void setText(const juce::String& text);
        juce::String getText() const { return labelText_; }

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kTextSpacing_ = 8;
        inline constexpr static int kLineThickness_ = 1;
        
        Skin* skin_ = nullptr;
        int width_;
        int height_;
        juce::String labelText_;
        
        // Cache optimization
        juce::Image cachedImage_;
        bool cacheValid_ {false};
        juce::Colour cachedTextColour_;
        juce::Colour cachedLineColour_;
        juce::Font cachedFont_;
        float cachedTextWidth_ {0.0f};
        
        void regenerateCache();
        void invalidateCache();
        void updateSkinCache();
        float getPixelScale() const;
        
        void drawText(juce::Graphics& g, const juce::Rectangle<float>& area);
        void drawLines(juce::Graphics& g, const juce::Rectangle<float>& area, float textWidth);
        void drawLeftLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY);
        void drawRightLine(juce::Graphics& g, const juce::Rectangle<float>& area, float centreX, float halfTextWidth, float centreY);

        void calculateTextWidth();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupLabel)
    };
}

