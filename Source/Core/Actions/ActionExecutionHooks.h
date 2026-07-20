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
        // Patch-context gate: consulted before navigation, load, INIT, or PASTE.
        // `includeUnsavedEditWarning` is true for navigation / file load / NumberBox;
        // false for in-place INIT/PASTE (Mutator history only — no FR-51 modal).
        // Returns true to proceed, false to abort. May be empty (proceed).
        std::function<bool(bool includeUnsavedEditWarning)> confirmPatchContextChange;
    };

} // namespace Core
