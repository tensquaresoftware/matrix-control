#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "Shared/Definitions/PluginIDs.h"

namespace Core::GettingStartedMachineDefaults
{
    /** Machine-scoped Getting Started auto-open preference (Settings User Interface). */
    int readAutoOpenPreference(const juce::PropertiesFile& store);
    void writeAutoOpenPreference(juce::PropertiesFile& store, int autoOpenId);

    int loadAutoOpenPreference();
    void writeAutoOpenPreference(int autoOpenId);
}
