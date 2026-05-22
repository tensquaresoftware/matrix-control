#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace tss
{
    class ModulationBusHeader : public juce::Component
    {
    public:
        enum class ColourVariant
        {
            Blue,
            Orange
        };

        explicit ModulationBusHeader(int width, int height, const ModulationBusHeaderLook& look,
                                     ColourVariant variant = ColourVariant::Blue);
        ~ModulationBusHeader() override = default;

        void setLook(const ModulationBusHeaderLook& look);
        void setUiScale(float uiScale);

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getHeight() const { return height_; }

    private:
        inline constexpr static int kBusNumberTextWidth_ = 15;
        inline constexpr static int kBusSourceTextWidth_ = 65;
        inline constexpr static int kBusAmountTextWidth_ = 65;
        inline constexpr static int kBusDestinationTextWidth_ = 110;

        inline constexpr static int kTextLeftPadding_ = 2;
        inline constexpr static int kTextAreaHeight_ = 20;
        inline constexpr static int kLineThickness_ = 4;

        ModulationBusHeaderLook look_{};
        juce::String busNumberText_;
        juce::String busSourceText_;
        juce::String busAmountText_;
        juce::String busDestinationText_;
        ColourVariant colourVariant_;
        int height_;
        float uiScale_ = 1.0f;

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
