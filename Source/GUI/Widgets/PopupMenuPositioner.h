#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ComboBox;

    enum class PopupVerticalPlacement
    {
        Auto,
        Above,
        Below
    };

    struct PopupMenuDimensions
    {
        int width = 0;
        int height = 0;
        int x = 0;
        int y = 0;
        bool opensAbove = false;
    };

    class PopupMenuPositioner
    {
    public:
        static PopupMenuDimensions calculateDimensions(
            const ComboBox& comboBox,
            int popupWidth,
            int popupHeight,
            int borderThickness);

        static PopupMenuDimensions calculateDimensions(
            const juce::Component& anchor,
            int popupWidth,
            int popupHeight,
            int verticalMargin,
            PopupVerticalPlacement placement = PopupVerticalPlacement::Auto);

    private:
        static juce::Point<int> positionBelow(
            const juce::Component& anchor,
            int verticalMargin);

        static juce::Point<int> positionAbove(
            const juce::Component& anchor,
            int popupHeight,
            int verticalMargin);

        static int adjustXPosition(
            const juce::Component& anchor,
            int desiredX,
            int popupWidth,
            const juce::Rectangle<int>& screenBounds);

        static int resolveYPosition(
            const juce::Component& anchor,
            int desiredYBelow,
            int popupHeight,
            const juce::Rectangle<int>& screenBounds,
            int verticalMargin,
            PopupVerticalPlacement placement,
            bool& opensAbove);
    };
}
