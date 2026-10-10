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

/** Index of the area whose closest point is nearest to bounds centre; -1 if areas empty. */
inline int indexOfNearestUserAreaByCentreDistance (
    const juce::Array<juce::Rectangle<int>>& userAreas,
    juce::Rectangle<int> bounds,
    int fallbackIndex = 0) noexcept
{
    if (userAreas.isEmpty())
        return -1;

    const auto centre = bounds.getCentre();
    auto nearestIndex = juce::jlimit (0, userAreas.size() - 1, fallbackIndex);
    auto bestDistanceSquared = std::numeric_limits<double>::max();

    for (int i = 0; i < userAreas.size(); ++i)
    {
        const auto nearestPoint = userAreas.getReference (i).getConstrainedPoint (centre);
        const auto dx = static_cast<double> (centre.x - nearestPoint.x);
        const auto dy = static_cast<double> (centre.y - nearestPoint.y);
        const auto distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < bestDistanceSquared)
        {
            bestDistanceSquared = distanceSquared;
            nearestIndex = i;
        }
    }

    return nearestIndex;
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

    juce::Array<juce::Rectangle<int>> userAreas;
    userAreas.ensureStorageAllocated (displays.displays.size());

    int primaryIndex = 0;

    for (int i = 0; i < displays.displays.size(); ++i)
    {
        const auto& display = displays.displays.getReference (i);
        userAreas.add (display.userBounds.toNearestInt());

        if (&display == displays.getPrimaryDisplay())
            primaryIndex = i;
    }

    const auto nearestIndex = indexOfNearestUserAreaByCentreDistance (userAreas, bounds, primaryIndex);

    if (nearestIndex < 0 || nearestIndex >= displays.displays.size())
        return displays.getPrimaryDisplay();

    return &displays.displays.getReference (nearestIndex);
}
} // namespace MatrixStandalone
