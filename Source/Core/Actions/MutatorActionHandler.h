#pragma once

#include <juce_core/juce_core.h>

#include "Core/Actions/IActionHandler.h"

namespace Core
{

    class MutatorActionHandler final : public IActionHandler
    {
    public:
        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;
    };

} // namespace Core
