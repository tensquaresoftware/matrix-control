#include "Core/Actions/ActionDispatcher.h"

#include "Core/Actions/ActionPropertyRegistry.h"

namespace Core
{

    ActionDispatcher::ActionDispatcher(IActionHandler& moduleHandler,
                                       IActionHandler& patchManagerHandler,
                                       IActionHandler& mutatorHandler)
        : moduleHandler_(moduleHandler)
        , patchManagerHandler_(patchManagerHandler)
        , mutatorHandler_(mutatorHandler)
    {
    }

    void ActionDispatcher::onActionPropertyChanged(const juce::String& propertyId, const juce::var& newValue)
    {
        const auto handlerKind = ActionPropertyRegistry::handlerKindFor(propertyId);
        if (!handlerKind.has_value())
            return;

        switch (*handlerKind)
        {
            case ActionHandlerKind::Module:
                moduleHandler_.handleAction(propertyId, newValue);
                break;
            case ActionHandlerKind::PatchManager:
                patchManagerHandler_.handleAction(propertyId, newValue);
                break;
            case ActionHandlerKind::Mutator:
                mutatorHandler_.handleAction(propertyId, newValue);
                break;
        }
    }

} // namespace Core
