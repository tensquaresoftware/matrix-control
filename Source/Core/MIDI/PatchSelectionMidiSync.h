#pragma once

#include "Core/Services/DeviceMemoryLimits.h"

class MidiManager;

namespace Core
{

    class PatchSelectionMidiSync
    {
    public:
        explicit PatchSelectionMidiSync(MidiManager* midiManager = nullptr) noexcept;

        void setMidiManager(MidiManager* midiManager) noexcept;

        bool syncSelection(int bank, int patch, const DeviceMemoryLimits& limits, bool forceSetBank = false);
        void sendSetBank(int bank, const DeviceMemoryLimits& limits);
        void sendUnlockBank(const DeviceMemoryLimits& limits);

        void resetLastSyncedBank(int bank) noexcept;
        void clearSyncedBankState() noexcept;

    private:
        MidiManager* midiManager_ = nullptr;
        int lastSyncedBank_ = 0;
        bool hasLastSyncedBank_ = false;
    };

} // namespace Core
