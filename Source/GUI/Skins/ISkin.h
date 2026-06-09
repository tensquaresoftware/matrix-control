#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ISkinColours.h"
#include "ISkinFonts.h"

namespace TSS
{
    class ISkin : public ISkinColours, public ISkinFonts
    {
    public:
        ~ISkin() override = default;
    };
}
