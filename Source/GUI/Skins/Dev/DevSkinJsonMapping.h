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

    /** Editor-only colourSetLabels object, or void when all labels empty. */
    juce::var colourSetLabelsToVar(const NamedSkin& skin);
    /** Reads optional colourSetLabels; unknown / invalid keys ignored. */
    void colourSetLabelsFromVar(const juce::var& labelsVar, NamedSkin& out);
}
