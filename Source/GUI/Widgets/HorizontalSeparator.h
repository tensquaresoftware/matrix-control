#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class HorizontalSeparator : public juce::Component
    {
    public:
        HorizontalSeparator(ISkin& skin, int width, int height);
        ~HorizontalSeparator() override = default;

        void setSkin(ISkin& skin);

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        inline constexpr static int kLineThickness_ = 1;

        ISkin* skin_ = nullptr;
        int width_;
        int height_;

        // Image cache
        juce::Image cachedImage_;
        bool cacheValid_ = false;
        juce::Colour cachedLineColour_;

        void regenerateCache();
        void invalidateCache();
        float getPixelScale() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HorizontalSeparator)
    };
}

