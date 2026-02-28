#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    enum class LabelStyle
    {
        Default,
        HeaderPanel
    };

    class Label : public juce::Component
    {
    public:
        explicit Label(ISkin& skin, int width, int height, const juce::String& text = juce::String(),
                      LabelStyle style = LabelStyle::Default);
        ~Label() override = default;

        void setSkin(ISkin& skin);
        
        void setText(const juce::String& text);
        juce::String getText() const { return labelText_; }

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kTextLeftPadding_ = 2;

        ISkin* skin_ = nullptr;
        int width_;
        int height_;
        juce::String labelText_;
        LabelStyle style_;
        
        // Cache optimization
        juce::Image cachedImage_;
        bool cacheValid_ {false};
        juce::Colour cachedTextColour_;
        juce::Font cachedFont_;
        
        void regenerateCache();
        void invalidateCache();
        void updateSkinCache();
        float getPixelScale() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Label)
    };
}

