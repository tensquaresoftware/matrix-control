#include "PopupMenuPositioner.h"
#include "ComboBox.h"

namespace TSS
{
    PopupMenuDimensions PopupMenuPositioner::calculateDimensions(
        const ComboBox& comboBox,
        int popupWidth,
        int popupHeight,
        int /* borderThickness */)
    {
        auto* topLevelComponent = comboBox.getTopLevelComponent();
        if (topLevelComponent == nullptr)
        {
            return {popupWidth, popupHeight, 0, 0};
        }

        const auto topLevelScreenBounds = topLevelComponent->getScreenBounds();
        const auto verticalMargin = ComboBox::getVerticalMargin();

        auto initialPosition = calculateInitialPosition(comboBox, verticalMargin);
        auto adjustedPosition = adjustPositionToFitScreen(
            comboBox, initialPosition, popupWidth, popupHeight, topLevelScreenBounds, verticalMargin);

        const auto relativePosition = adjustedPosition - topLevelScreenBounds.getPosition();

        return {popupWidth, popupHeight, relativePosition.getX(), relativePosition.getY()};
    }

    juce::Point<int> PopupMenuPositioner::calculateInitialPosition(
        const ComboBox& comboBox,
        int verticalMargin)
    {
        return comboBox.localPointToGlobal(juce::Point<int>(0, comboBox.getHeight() + verticalMargin));
    }

    juce::Point<int> PopupMenuPositioner::adjustPositionToFitScreen(
        const ComboBox& comboBox,
        juce::Point<int> desiredPosition,
        int popupWidth,
        int popupHeight,
        const juce::Rectangle<int>& screenBounds,
        int verticalMargin)
    {
        const auto adjustedX = adjustXPosition(comboBox, desiredPosition.getX(), popupWidth, screenBounds);
        const auto adjustedY = adjustYPosition(comboBox, desiredPosition.getY(), popupHeight, screenBounds, verticalMargin);

        return {adjustedX, adjustedY};
    }

    int PopupMenuPositioner::adjustXPosition(
        const ComboBox& comboBox,
        int desiredX,
        int popupWidth,
        const juce::Rectangle<int>& screenBounds)
    {
        if (desiredX + popupWidth > screenBounds.getRight())
        {
            const auto comboBoxRight = comboBox.localPointToGlobal(juce::Point<int>(comboBox.getWidth(), 0)).getX();
            return comboBoxRight - popupWidth;
        }

        return desiredX;
    }

    int PopupMenuPositioner::adjustYPosition(
        const ComboBox& comboBox,
        int desiredY,
        int popupHeight,
        const juce::Rectangle<int>& screenBounds,
        int verticalMargin)
    {
        if (desiredY + popupHeight > screenBounds.getBottom())
        {
            return comboBox.localPointToGlobal(juce::Point<int>(0, -popupHeight - verticalMargin)).getY();
        }

        return desiredY;
    }
}
