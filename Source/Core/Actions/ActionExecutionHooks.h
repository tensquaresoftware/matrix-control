#pragma once

#include <functional>

namespace Core
{

    struct ActionExecutionHooks
    {
        std::function<void(bool)> setSuppressMatrixModSysEx;
        std::function<void(bool)> setSuppressMasterSysEx;
        std::function<void(bool)> setSuppressPatchSysEx;
        std::function<void(bool)> setSuppressPatchSelectionMidiSync;
        std::function<void(bool)> setSuppressMutatorHistorySelectionDebounce;
        std::function<void()> onPatchLoaded;
    };

} // namespace Core
