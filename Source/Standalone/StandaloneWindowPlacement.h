#pragma once

#include <limits>

#include <juce_gui_basics/juce_gui_basics.h>

namespace MatrixStandalone
{
inline juce::Rectangle<int> titleBarDragStrip (juce::Rectangle<int> clientBounds,
                                               juce::BorderSize<int> frame) noexcept
{
    const int dragStripHeight = juce::jmax (1, frame.getTop());

    return { clientBounds.getX() - frame.getLeft(),
             clientBounds.getY() - frame.getTop(),
             clientBounds.getWidth() + frame.getLeft() + frame.getRight(),
             dragStripHeight };
}

inline bool titleBarStripIntersectsAnyUserArea (const juce::Rectangle<int>& titleBarStrip,
                                                const juce::Array<juce::Rectangle<int>>& userAreas) noexcept
{
    for (const auto& area : userAreas)
        if (titleBarStrip.intersects (area))
            return true;

    return false;
}

inline juce::Rectangle<int> clampBoundsIntoDisplayUserArea (juce::Rectangle<int> bounds,
                                                            const juce::Rectangle<int>& screenLimits) noexcept
{
    return { juce::jlimit (screenLimits.getX(),
                           juce::jmax (screenLimits.getX(), screenLimits.getRight() - bounds.getWidth()),
                           bounds.getX()),
             juce::jlimit (screenLimits.getY(),
                           juce::jmax (screenLimits.getY(), screenLimits.getBottom() - bounds.getHeight()),
                           bounds.getY()),
             bounds.getWidth(),
             bounds.getHeight() };
}

/** Returns client bounds with title-bar strip intersecting a user area, or unchanged if already valid / no areas.
    When the native frame top is unknown (<= 0), leaves bounds unchanged so a 1 px client-top strip cannot
    falsely accept an off-screen native title bar, and so we do not clamp before the peer reports chrome. */
inline juce::Rectangle<int> ensureClientBoundsTitleBarOnScreen (
    juce::Rectangle<int> clientBounds,
    juce::BorderSize<int> frame,
    const juce::Array<juce::Rectangle<int>>& userAreas,
    juce::Rectangle<int> preferredLimits) noexcept
{
    if (userAreas.isEmpty() || frame.getTop() <= 0)
        return clientBounds;

    if (titleBarStripIntersectsAnyUserArea (titleBarDragStrip (clientBounds, frame), userAreas))
        return clientBounds;

    const auto fullBounds = frame.addedTo (clientBounds);
    const auto clampedClient = frame.subtractedFrom (
        clampBoundsIntoDisplayUserArea (fullBounds, preferredLimits));

    if (titleBarStripIntersectsAnyUserArea (titleBarDragStrip (clampedClient, frame), userAreas))
        return clampedClient;

    return frame.subtractedFrom (fullBounds.withCentre (preferredLimits.getCentre()));
}

inline const juce::Displays::Display* findNearestDisplayForBounds (
    const juce::Displays& displays,
    juce::Rectangle<int> bounds) noexcept
{
    if (displays.displays.isEmpty())
        return nullptr;

    if (const auto* hit = displays.getDisplayForRect (bounds))
    {
        const auto user = hit->userBounds.toNearestInt();
        const auto total = hit->logicalBounds.toNearestInt();

        if (bounds.intersects (user) || bounds.intersects (total))
            return hit;
    }

    const auto centre = bounds.getCentre();
    const juce::Displays::Display* nearest = displays.getPrimaryDisplay();
    auto bestDistanceSquared = std::numeric_limits<double>::max();

    for (const auto& display : displays.displays)
    {
        const auto nearestPoint = display.userBounds.toNearestInt().getConstrainedPoint (centre);
        const auto dx = static_cast<double> (centre.x - nearestPoint.x);
        const auto dy = static_cast<double> (centre.y - nearestPoint.y);
        const auto distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < bestDistanceSquared)
        {
            bestDistanceSquared = distanceSquared;
            nearest = &display;
        }
    }

    return nearest;
}
} // namespace MatrixStandalone
