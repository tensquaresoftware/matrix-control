#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ISkinColours.h"
#include "ISkinValues.h"
#include "ISkinFonts.h"

namespace tss
{
    class ISkin : public ISkinColours, public ISkinValues, public ISkinFonts
    {
    public:
        ~ISkin() override = default;
    };
}
