#pragma once

#include <vector>

#include "GUI/Skins/SkinColoursCommon.h"

namespace TSS::DevSkin
{
    /** Pointers to every compiled Skin SSOT ColourElement (stable for process lifetime). */
    const std::vector<const SkinColours::ColourElement*>& compiledColourElements();

    template <typename Fn>
    void forEachCompiledColourElement(Fn&& fn)
    {
        for (const auto* element : compiledColourElements())
            fn(*element);
    }
}
