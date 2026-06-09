#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

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
        void drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds) const;
        void drawItem(juce::Graphics& g, const ComboBox& comboBox, int itemIndex, 
                     const juce::Rectangle<float>& itemBounds, int highlightedItemIndex, 
                     const juce::Font& font) const;
        void drawVerticalSeparators(juce::Graphics& g, const juce::Rectangle<float>& contentBounds,
                                   int columnCount, float actualColumnWidth, float separatorWidth) const;

    private:
        inline constexpr static int kTextLeftPadding_ = 3;
        float getHighlightGap() const { return juce::jmax(1.0f, 1.0f * uiScale_); }

        PopupMenuLook look_{};
        bool isButtonLike_;
        float uiScale_;
    };
}
