#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include <juce_core/juce_core.h>

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
        // true = risk modal + Mutator history; false = history-only. PASTE skips this hook.
        // Returns true to proceed, false to abort. May be empty (proceed).
        std::function<bool(bool includeUnsavedEditWarning)> confirmPatchContextChange;
        // Optional test seams for device dump (empty = MidiManager).
        std::function<bool()> isDeviceDumpAvailable;
        using DeviceDumpCallback = std::function<void(std::vector<juce::uint8>)>;
        std::function<void(juce::uint8 patchNumber, DeviceDumpCallback onResult)> requestDeviceDump;
        // Stops Copy/Paste blink feedback after a successful paste (clipboard content kept).
        std::function<void()> disarmClipboardFeedback;
        // Bank Utility COPY: blink COPY immediately while the MIDI dump runs (clipboard unchanged).
        std::function<void()> armBankCopyFeedbackPending;
        std::function<void()> clearBankCopyFeedbackPending;
        // Arms normal type-aware blink after clipboard content is ready.
        std::function<void()> armClipboardFeedback;
        // Refresh paste-enabled + blink mirrors (e.g. selected bank changed) without arming.
        std::function<void()> refreshClipboardMirrors;
        // Clears editorial undo stack and refreshes DirtyPatchTracker snapshot (load/paste/mutate).
        std::function<void()> onEditorialCheckpoint;
        // Opens one editorial undo transaction before bulk APVTS writes (Init/Paste module).
        std::function<void(const juce::String&)> beginEditorialTransaction;
        // STORE hit an active `* INIT *` sentinel: Editor arms name-required + pending STORE.
        // Core must not include GUI types; empty = gate still blocks SysEx (headless/tests).
        std::function<void()> requestNameRequiredBeforeStore;
    };

} // namespace Core
