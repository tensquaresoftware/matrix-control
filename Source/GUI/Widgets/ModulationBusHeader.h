#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    class ModulationBusHeader : public juce::Component
    {
    public:
        enum class ColourVariant
        {
            Blue,
            Orange
        };

        explicit ModulationBusHeader(int width, int height, const ModulationBusHeaderDimensions& dimensions,
                                     const ModulationBusHeaderLook& look,
                                     ColourVariant variant = ColourVariant::Blue);
        ~ModulationBusHeader() override = default;

        void setLook(const ModulationBusHeaderLook& look);
        void setUiScale(float uiScale);

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getHeight() const { return height_; }

    private:
        static constexpr int kTextLeftPadding = 0;
        static constexpr int kTextAreaHeight = 20;
        static constexpr int kLineThickness = 4;

        ModulationBusHeaderDimensions dimensions_;
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

        void drawBusNumberText(juce::Graphics& g, float x, float y, float width, float height);
        void drawBusSourceText(juce::Graphics& g, float x, float y, float width, float height);
        void drawBusAmountText(juce::Graphics& g, float x, float y, float width, float height);
        void drawBusDestinationText(juce::Graphics& g, float x, float y, float width, float height);

        juce::Colour getLineColour() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBusHeader)
    };
}
