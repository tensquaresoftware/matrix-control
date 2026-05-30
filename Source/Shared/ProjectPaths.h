#pragma once

#include <juce_core/juce_core.h>

namespace ProjectPaths
{
    enum class LogCategory
    {
        kMidi,
        kApvts
    };

    juce::File getProjectRoot();
    juce::File getLogsDirectory(LogCategory category);
    bool isUsingFallbackRoot();
    juce::String getFallbackRootWarning();
}
