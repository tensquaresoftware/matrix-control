#pragma once

#include <optional>

#include <juce_core/juce_core.h>

class WidgetDimensionRegistry
{
public:
    static std::optional<int> resolveStandaloneButtonWidth(const juce::String& widgetId);

    /** Returns false when widgetId is a kButton descriptor with no resolvable width. */
    static bool isStandaloneButtonWidthResolvable(const juce::String& widgetId);
};
