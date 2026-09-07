#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    bool PatchManagerActionHandler::tryHandleModuleHeaderClicks(const juce::String& propertyId,
                                                                const DeviceMemoryLimits& limits)
    {
        namespace InternalWidgets = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
        namespace ComputerWidgets = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets;

        if (propertyId == InternalWidgets::kHeaderClick)
        {
            setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kInternal);

            if (! arePatchCoordinatesEstablished())
                return true;

            if (! confirmPatchContextChange())
                return true;

            abandonPendingComputerSelectSettle();
            abandonPendingInternalNavSettle();
            patchNavDebouncer_.cancel();
            computerSelectDebouncer_.cancel();

            // Recall the current memory slot on the synth before dumping into the editor.
            // Computer header reload uses sendFullPatchForAudition; Internal must Program Change
            // (and Set Bank when needed) or the last .syx audition stays in the edit buffer.
            const auto coords = captureInternalCoordinates(limits);
            if (patchSelectionMidiSync_ != nullptr)
                patchSelectionMidiSync_->syncSelection(coords.bank, coords.patch, limits, true);

            beginPendingDeviceLoad(coords);
            loadCurrentPatchFromDevice(limits);
            return true;
        }

        if (propertyId == ComputerWidgets::kHeaderClick)
        {
            setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kComputer);

            if (readComputerPatchesSelectedId() < 1)
                return true;

            loadSelectedPatchFileImmediately(limits);
            return true;
        }

        return false;
    }

} // namespace Core
