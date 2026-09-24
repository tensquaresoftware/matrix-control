#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    class ComboBox;

    class PopupMenuRenderer
    {
    public:
        PopupMenuRenderer(bool isButtonLike, float uiScale);

        void setLook(const PopupMenuLook& look);

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds) const;
        void drawBorder(juce::Graphics& g,
                        const juce::Rectangle<float>& bounds,
                        float systemDisplayScale,
                        ScaledDrawing::StrokeSnapPolicy borderSnapPolicy = ScaledDrawing::StrokeSnapPolicy::kRound) const;

        struct DrawItemArgs
        {
            const ComboBox& comboBox;
            int itemIndex = 0;
            juce::Rectangle<float> itemBounds;
            int highlightedItemIndex = -1;
            const juce::Font& font;
            float systemDisplayScale = 1.0f;
            /** Extra bottom trim on the hover fill (e.g. air above a sentinel rule). */
            float highlightBottomExtraTrim = 0.0f;
        };

        void drawItem(juce::Graphics& g, const DrawItemArgs& args) const;

        struct DrawLabelArgs
        {
            juce::String text;
            juce::Rectangle<float> itemBounds;
            bool isHighlighted = false;
            bool isEnabled = true;
            const juce::Font* font = nullptr;
            float systemDisplayScale = 1.0f;
            float highlightBottomExtraTrim = 0.0f;
        };

        void drawLabelItem(juce::Graphics& g, const DrawLabelArgs& args) const;
        void drawSubMenuChevron(juce::Graphics& g,
                                const juce::Rectangle<float>& itemBounds,
                                const juce::Font& font) const;
        void drawVerticalSeparators(juce::Graphics& g, const juce::Rectangle<float>& contentBounds,
                                   int columnCount, float actualColumnWidth, float separatorWidth) const;
        /** Full-width rule on the bottom edge of a sentinel row (No Input / No Output). */
        void drawSentinelBottomRule(juce::Graphics& g,
                                    const juce::Rectangle<float>& itemBounds,
                                    float systemDisplayScale) const;

    private:
        float getHighlightGap(float systemDisplayScale) const;

        PopupMenuLook look_{};
        bool isButtonLike_;
        float uiScale_;
    };
}
