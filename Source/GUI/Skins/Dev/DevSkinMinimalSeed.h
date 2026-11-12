#pragma once

#include <cstddef>
#include <cstdint>

#include "GUI/Skins/SkinValues.h"

namespace TSS::DevSkin
{
    struct MinimalColourEntry
    {
        SkinObjectType object;
        SkinColourProperty property;
        SkinColourState state;
        ColourSet colourSet;
        uint32_t argb;
    };

    const MinimalColourEntry* blackMinimalEntries(size_t& count) noexcept;
    const MinimalColourEntry* creamMinimalEntries(size_t& count) noexcept;
}
