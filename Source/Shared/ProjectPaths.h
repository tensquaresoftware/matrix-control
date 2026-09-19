#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

namespace ProjectPaths
{
    enum class LogCategory
    {
        kMidi,
        kApvts
    };

    juce::File getProjectRoot();
    juce::File getLogsDirectory(LogCategory category);
    // Per-user product root shared with PropertiesFile stores (Standalone + Device Connection + Init):
    // macOS ~/Library/Application Support/Ten Square Software/Matrix-Control/, Windows %APPDATA%/…,
    // Linux ~/Ten Square Software/Matrix-Control/. Independent of getProjectRoot() repo discovery.
    juce::File getApplicationDataDirectory();
    // PropertiesFile Options::folderName ("Ten Square Software/Matrix-Control").
    juce::String getPropertiesFileFolderName();
    // Shared Options for product PropertiesFile stores (folderName, Application Support, per-user).
    juce::PropertiesFile::Options makeProductPropertiesFileOptions(const juce::String& applicationName);
    // Fixed Init/ under application data; creates the directory on demand. Empty File on failure.
    juce::File getInitTemplatesDirectory();
    bool isUsingFallbackRoot();
    juce::String getFallbackRootWarning();
}
