#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    enum class TypographyStyleId
    {
        kDefault,
        kSmall,
        kModuleHeader,
        kSectionHeader,
        kPatchName
    };

    struct TypographyStyle
    {
        float fontHeight = 14.0f;
        bool useBoldFont = false;
    };

    TypographyStyle getTypographyStyle(TypographyStyleId styleId);
}
