#include "PatchSelectionMidiSync.h"

#include "MidiManager.h"

namespace Core
{

    PatchSelectionMidiSync::PatchSelectionMidiSync(MidiManager* midiManager) noexcept
        : midiManager_(midiManager)
    {
    }

    void PatchSelectionMidiSync::setMidiManager(MidiManager* midiManager) noexcept
    {
        midiManager_ = midiManager;
    }

    void PatchSelectionMidiSync::resetLastSyncedBank(int bank) noexcept
    {
        lastSyncedBank_ = bank;
        hasLastSyncedBank_ = true;
    }

    void PatchSelectionMidiSync::clearSyncedBankState() noexcept
    {
        hasLastSyncedBank_ = false;
    }

    bool PatchSelectionMidiSync::syncSelection(int bank,
                                               int patch,
                                               const DeviceMemoryLimits& limits,
                                               bool forceSetBank)
    {
        if (midiManager_ == nullptr)
            return false;

        bool setBankSent = false;

        if (limits.hasBankConcept())
        {
            const bool shouldSendSetBank = forceSetBank
                                           || !hasLastSyncedBank_
                                           || bank != lastSyncedBank_;

            if (shouldSendSetBank)
            {
                midiManager_->sendSetBank(bank);
                setBankSent = true;
            }

            lastSyncedBank_ = bank;
            hasLastSyncedBank_ = true;
        }

        midiManager_->sendProgramChange(patch);
        return setBankSent;
    }

    void PatchSelectionMidiSync::sendSetBank(int bank, const DeviceMemoryLimits& limits)
    {
        if (midiManager_ == nullptr || !limits.hasBankConcept())
            return;

        midiManager_->sendSetBank(bank);
        lastSyncedBank_ = bank;
        hasLastSyncedBank_ = true;
    }

    void PatchSelectionMidiSync::sendUnlockBank(const DeviceMemoryLimits& limits)
    {
        if (midiManager_ == nullptr || !limits.hasBankConcept())
            return;

        midiManager_->sendUnlockBank();
    }

} // namespace Core
