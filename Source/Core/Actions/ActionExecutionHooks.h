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
    };

} // namespace Core
