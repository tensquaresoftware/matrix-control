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
    // OS application-data product root (…/Ten Square Software/Matrix-Control/), independent of
    // getProjectRoot() repo discovery — used for system Init templates and similar product files.
    juce::File getApplicationDataDirectory();
    // Fixed Init/ under application data; creates the directory on demand. Empty File on failure.
    juce::File getInitTemplatesDirectory();
    bool isUsingFallbackRoot();
    juce::String getFallbackRootWarning();
}
