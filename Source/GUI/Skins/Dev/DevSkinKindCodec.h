#pragma once

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    juce::String kindToString(SkinKind kind);
    bool kindFromString(const juce::String& text, SkinKind& out);
    bool isReservedCompiledSkinId(const juce::String& skinId) noexcept;
}
