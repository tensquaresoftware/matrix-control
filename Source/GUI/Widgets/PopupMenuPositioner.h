#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    class ComboBox;

    struct PopupMenuDimensions
    {
        int width = 0;
        int height = 0;
        int x = 0;
        int y = 0;
    };

    class PopupMenuPositioner
    {
    public:
        static PopupMenuDimensions calculateDimensions(
            const ComboBox& comboBox,
            int popupWidth,
            int popupHeight,
            int borderThickness);

    private:
        static juce::Point<int> calculateInitialPosition(
            const ComboBox& comboBox,
            int verticalMargin);

        static juce::Point<int> adjustPositionToFitScreen(
            const ComboBox& comboBox,
            juce::Point<int> desiredPosition,
            int popupWidth,
            int popupHeight,
            const juce::Rectangle<int>& screenBounds,
            int verticalMargin);

        static int adjustXPosition(
            const ComboBox& comboBox,
            int desiredX,
            int popupWidth,
            const juce::Rectangle<int>& screenBounds);

        static int adjustYPosition(
            const ComboBox& comboBox,
            int desiredY,
            int popupHeight,
            const juce::Rectangle<int>& screenBounds,
            int verticalMargin);
    };
}
