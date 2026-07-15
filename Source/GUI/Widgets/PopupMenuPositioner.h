#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ComboBox;

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
            int verticalMargin);

    private:
        static juce::Point<int> calculateInitialPosition(
            const juce::Component& anchor,
            int verticalMargin);

        static juce::Point<int> adjustPositionToFitScreen(
            const juce::Component& anchor,
            juce::Point<int> desiredPosition,
            int popupWidth,
            int popupHeight,
            const juce::Rectangle<int>& screenBounds,
            int verticalMargin);

        static int adjustXPosition(
            const juce::Component& anchor,
            int desiredX,
            int popupWidth,
            const juce::Rectangle<int>& screenBounds);

        static int adjustYPosition(
            const juce::Component& anchor,
            int desiredY,
            int popupHeight,
            const juce::Rectangle<int>& screenBounds,
            int verticalMargin);
    };
}
