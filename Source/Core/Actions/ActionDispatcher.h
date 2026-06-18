#pragma once

#include <juce_core/juce_core.h>

#include "Core/Actions/IActionHandler.h"

namespace Core
{

    class ActionDispatcher
    {
    public:
        ActionDispatcher(IActionHandler& moduleHandler,
                         IActionHandler& patchManagerHandler,
                         IActionHandler& mutatorHandler);

        void onActionPropertyChanged(const juce::String& propertyId, const juce::var& newValue);

    private:
        IActionHandler& moduleHandler_;
        IActionHandler& patchManagerHandler_;
        IActionHandler& mutatorHandler_;
    };

} // namespace Core
