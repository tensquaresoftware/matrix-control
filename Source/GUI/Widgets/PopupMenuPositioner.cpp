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
                                   comboBox.getScaledVerticalMargin(),
                                   comboBox.getPopupVerticalPlacement());
    }

    PopupMenuDimensions PopupMenuPositioner::calculateDimensions(
        const juce::Component& anchor,
        int popupWidth,
        int popupHeight,
        int verticalMargin,
        PopupVerticalPlacement placement)
    {
        auto* topLevelComponent = anchor.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return { popupWidth, popupHeight, 0, 0, false };

        const auto topLevelScreenBounds = topLevelComponent->getScreenBounds();
        const auto belowPosition = positionBelow(anchor, verticalMargin);
        bool opensAbove = false;
        const int resolvedY = resolveYPosition(anchor,
                                               belowPosition.getY(),
                                               popupHeight,
                                               topLevelScreenBounds,
                                               verticalMargin,
                                               placement,
                                               opensAbove);
        const int resolvedX = adjustXPosition(anchor,
                                              belowPosition.getX(),
                                              popupWidth,
                                              topLevelScreenBounds);
        const juce::Point<int> adjustedPosition { resolvedX, resolvedY };
        const auto relativePosition = adjustedPosition - topLevelScreenBounds.getPosition();

        return { popupWidth, popupHeight, relativePosition.getX(), relativePosition.getY(), opensAbove };
    }

    juce::Point<int> PopupMenuPositioner::positionBelow(
        const juce::Component& anchor,
        int verticalMargin)
    {
        return anchor.localPointToGlobal(juce::Point<int>(0, anchor.getHeight() + verticalMargin));
    }

    juce::Point<int> PopupMenuPositioner::positionAbove(
        const juce::Component& anchor,
        int popupHeight,
        int verticalMargin)
    {
        return anchor.localPointToGlobal(juce::Point<int>(0, -popupHeight - verticalMargin));
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

    int PopupMenuPositioner::resolveYPosition(
        const juce::Component& anchor,
        int desiredYBelow,
        int popupHeight,
        const juce::Rectangle<int>& screenBounds,
        int verticalMargin,
        PopupVerticalPlacement placement,
        bool& opensAbove)
    {
        switch (placement)
        {
            case PopupVerticalPlacement::Above:
                opensAbove = true;
                return positionAbove(anchor, popupHeight, verticalMargin).getY();

            case PopupVerticalPlacement::Below:
                opensAbove = false;
                return desiredYBelow;

            case PopupVerticalPlacement::Auto:
            default:
                if (desiredYBelow + popupHeight > screenBounds.getBottom())
                {
                    opensAbove = true;
                    return positionAbove(anchor, popupHeight, verticalMargin).getY();
                }

                opensAbove = false;
                return desiredYBelow;
        }
    }
}
