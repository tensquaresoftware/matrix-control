#pragma once

#include <juce_core/juce_core.h>

#include "GUI/Skins/SkinValues.h"

namespace TSS::DevSkin
{
    juce::String toString(SkinObjectType object);
    juce::String toString(SkinColourProperty property);
    juce::String toString(SkinColourState state);

    bool parseObjectType(const juce::String& text, SkinObjectType& out);
    bool parseProperty(const juce::String& text, SkinColourProperty& out);
    bool parseState(const juce::String& text, SkinColourState& out);

    juce::String keyLabel(const SkinColourKey& key);
}
