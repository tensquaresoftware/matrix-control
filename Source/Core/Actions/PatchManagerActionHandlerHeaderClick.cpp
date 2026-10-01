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

            // Recall the current memory slot on the synth before dumping into the editor.
            // Computer header reload uses sendFullPatchForAudition; Internal must Program Change
            // (and Set Bank when needed) or the last .syx audition stays in the edit buffer.
            reloadCurrentInternalSlotFromDevice(limits);
            // abandonPendingInternalNavSettle may restore a prior focus from its baseline.
            setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kInternal);
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

    void PatchManagerActionHandler::reloadCurrentInternalSlotFromDevice(const DeviceMemoryLimits& limits)
    {
        abandonPendingComputerSelectSettle();
        abandonPendingInternalNavSettle();
        patchNavDebouncer_.cancel();
        computerSelectDebouncer_.cancel();

        const auto coords = captureInternalCoordinates(limits);
        if (patchSelectionMidiSync_ != nullptr)
            patchSelectionMidiSync_->syncSelection(coords.bank, coords.patch, limits, true);

        beginPendingDeviceLoad(coords);
        loadCurrentPatchFromDevice(limits);
    }

    bool PatchManagerActionHandler::tryHandleSameBankReclick(int clampedBank,
                                                             const DeviceMemoryLimits& limits)
    {
        if (! arePatchCoordinatesEstablished() || clampedBank != getCurrentBank(limits))
            return false;

        // Same-bank reclick: full no-op while Internal (or none) owns focus.
        // While Computer owns focus, reclaim like the Internal header (minus unsaved gate).
        const int focus = static_cast<int>(apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::StateProperties::kNavigationFocus,
            PluginIDs::PatchManagerSection::NavigationFocus::kDefault));

        if (focus != PluginIDs::PatchManagerSection::NavigationFocus::kComputer)
            return true;

        reloadCurrentInternalSlotFromDevice(limits);
        // After reload: abandonPendingInternalNavSettle may restore a prior focus from baseline.
        setNavigationFocus(PluginIDs::PatchManagerSection::NavigationFocus::kInternal);
        return true;
    }

} // namespace Core
