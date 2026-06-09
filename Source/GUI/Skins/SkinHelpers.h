#pragma once

#include "ISkin.h"

namespace TSS
{
    template <typename... T>
    void propagateSkin(ISkin& skin, T*... components)
    {
        ([&](auto* c) { if (c) c->setSkin(skin); }(components), ...);
    }
}
