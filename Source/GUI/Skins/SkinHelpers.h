#pragma once

#include "ISkin.h"

namespace tss
{
    template <typename... T>
    void propagateSkin(ISkin& skin, T*... components)
    {
        (void) skin;
        ([&](auto* c) { if (c) c->setSkin(skin); }(components), ...);
    }
}
