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
        return calculateDimensions(static_cast<const juce::Component&>(comboBox),
                                   popupWidth,
                                   popupHeight,
                                   comboBox.getScaledVerticalMargin());
    }

    PopupMenuDimensions PopupMenuPositioner::calculateDimensions(
        const juce::Component& anchor,
        int popupWidth,
        int popupHeight,
        int verticalMargin)
    {
        auto* topLevelComponent = anchor.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return { popupWidth, popupHeight, 0, 0 };

        const auto topLevelScreenBounds = topLevelComponent->getScreenBounds();
        const auto initialPosition = calculateInitialPosition(anchor, verticalMargin);
        const auto adjustedPosition = adjustPositionToFitScreen(
            anchor, initialPosition, popupWidth, popupHeight, topLevelScreenBounds, verticalMargin);
        const auto relativePosition = adjustedPosition - topLevelScreenBounds.getPosition();

        return { popupWidth, popupHeight, relativePosition.getX(), relativePosition.getY() };
    }

    juce::Point<int> PopupMenuPositioner::calculateInitialPosition(
        const juce::Component& anchor,
        int verticalMargin)
    {
        return anchor.localPointToGlobal(juce::Point<int>(0, anchor.getHeight() + verticalMargin));
    }

    juce::Point<int> PopupMenuPositioner::adjustPositionToFitScreen(
        const juce::Component& anchor,
        juce::Point<int> desiredPosition,
        int popupWidth,
        int popupHeight,
        const juce::Rectangle<int>& screenBounds,
        int verticalMargin)
    {
        const auto adjustedX = adjustXPosition(anchor, desiredPosition.getX(), popupWidth, screenBounds);
        const auto adjustedY = adjustYPosition(anchor, desiredPosition.getY(), popupHeight, screenBounds, verticalMargin);

        return { adjustedX, adjustedY };
    }

    int PopupMenuPositioner::adjustXPosition(
        const juce::Component& anchor,
        int desiredX,
        int popupWidth,
        const juce::Rectangle<int>& screenBounds)
    {
        if (desiredX + popupWidth > screenBounds.getRight())
        {
            const auto anchorRight = anchor.localPointToGlobal(juce::Point<int>(anchor.getWidth(), 0)).getX();
            return anchorRight - popupWidth;
        }

        return desiredX;
    }

    int PopupMenuPositioner::adjustYPosition(
        const juce::Component& anchor,
        int desiredY,
        int popupHeight,
        const juce::Rectangle<int>& screenBounds,
        int verticalMargin)
    {
        if (desiredY + popupHeight > screenBounds.getBottom())
            return anchor.localPointToGlobal(juce::Point<int>(0, -popupHeight - verticalMargin)).getY();

        return desiredY;
    }
}
