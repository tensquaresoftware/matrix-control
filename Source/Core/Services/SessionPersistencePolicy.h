#pragma once

#include <juce_data_structures/juce_data_structures.h>

namespace Core::SessionPersistencePolicy
{
    bool shouldStripPatchAndMasterParameters(const juce::ValueTree& state);

    void stripPatchAndMasterParameters(juce::ValueTree& state);
}
