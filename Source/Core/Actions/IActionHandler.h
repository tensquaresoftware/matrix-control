#pragma once

#include <juce_core/juce_core.h>

namespace Core
{

    class IActionHandler
    {
    public:
        virtual ~IActionHandler() = default;

        virtual void handleAction(const juce::String& propertyId, const juce::var& newValue) = 0;
    };

} // namespace Core
