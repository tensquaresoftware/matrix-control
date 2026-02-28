#pragma once

#include "ISkin.h"

namespace tss
{
    template <typename... T>
    void propagateSkin(ISkin& skin, T*... components)
    {
        ((components && (components->setSkin(skin), true)), ...);
    }
}
