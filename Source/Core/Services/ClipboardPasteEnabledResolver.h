#pragma once

#include <optional>

#include <juce_core/juce_core.h>

#include "Core/Services/ClipboardService.h"

namespace Core
{

    struct ClipboardPasteEnabledState
    {
        bool dco1 = false;
        bool dco2 = false;
        bool env1 = false;
        bool env2 = false;
        bool env3 = false;
        bool lfo1 = false;
        bool lfo2 = false;
        bool internalPatches = false;
        bool matrixModulation = false;
    };

    ClipboardPasteEnabledState resolvePasteEnabled(const ClipboardService& clipboard);
    std::optional<PatchModuleKind> patchModuleKindFromWidgetId(const juce::String& widgetId);

} // namespace Core
