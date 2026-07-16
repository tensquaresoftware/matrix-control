#pragma once

#include <functional>

#include "Core/Services/PatchMutator/PatchLoadContext.h"

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
        // Records where the editor patch just came from (device vs computer file) so the
        // Mutator can freeze Export folder names. Set at real load sites; may be empty.
        std::function<void(const PatchLoadContext&)> setPatchLoadContext;
        // History gate: consulted before any patch-context change. Returns true to proceed
        // (history empty, or user chose Export/Discard), false to abort. May be empty.
        std::function<bool()> confirmPatchContextChange;
    };

} // namespace Core
