#pragma once

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    juce::var mappingToVar(const SkinColourKey& key, const juce::String& paletteName);
    bool mappingFromVar(const juce::var& value,
                        SkinColourKey& key,
                        juce::String& paletteName,
                        juce::String& error);
    void expandLegacyColourSets(NamedSkin& skin);
}
