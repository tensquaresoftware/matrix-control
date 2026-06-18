#pragma once

#include <optional>

#include <juce_core/juce_core.h>

#include "Core/Actions/ActionHandlerKind.h"

namespace Core
{

    class ActionPropertyRegistry
    {
    public:
        static bool isActionProperty(const juce::String& propertyId);
        static std::optional<ActionHandlerKind> handlerKindFor(const juce::String& propertyId);
    };

} // namespace Core
