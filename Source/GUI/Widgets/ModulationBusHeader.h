#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ISkin;

    class ModulationBusHeader : public juce::Component
    {
    public:
        enum class ColourVariant
        {
            Blue,
            Orange
        };

        explicit ModulationBusHeader(ISkin& skin, int width, int height, ColourVariant variant = ColourVariant::Blue);
        ~ModulationBusHeader() override = default;

        void setSkin(ISkin& skin);

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        inline constexpr static int kBusNumberTextWidth_ = 15;
        inline constexpr static int kBusSourceTextWidth_ = 65;
        inline constexpr static int kBusAmountTextWidth_ = 65;
        inline constexpr static int kBusDestinationTextWidth_ = 110;
                
        inline constexpr static int kTextLeftPadding_ = 2;
        inline constexpr static int kTextAreaHeight_ = 20;
        inline constexpr static int kLineThickness_ = 4;

        ISkin* skin_ = nullptr;
        juce::String busNumberText_;
        juce::String busSourceText_;
        juce::String busAmountText_;
        juce::String busDestinationText_;
        ColourVariant colourVariant_;

        // Image cache
        juce::Image cachedImage_;
        bool cacheValid_ = false;
        juce::Colour cachedTextColour_;
        juce::Colour cachedLineColour_;
        juce::Font cachedFont_;

        void regenerateCache();
        void invalidateCache();
        float getPixelScale() const;

        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        
        void drawBusNumberText(juce::Graphics& g, float x, float y);
        void drawBusSourceText(juce::Graphics& g, float x, float y);
        void drawBusAmountText(juce::Graphics& g, float x, float y);
        void drawBusDestinationText(juce::Graphics& g, float x, float y);
        
        juce::Colour getLineColour() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBusHeader)
    };
}

