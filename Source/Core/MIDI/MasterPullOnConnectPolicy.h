#pragma once

#include "Core/MIDI/MasterEditGate.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
    /** Snapshot taken on Device Inquiry success before (or independent of) status update:
        detection/type seen before success, plus consumed port-pair force flag. */
    struct MasterPullInquirySnapshot
    {
        bool wasDetectedBeforeSuccess = false;
        MatrixDeviceTypes::Type previousType = MatrixDeviceTypes::Type::kUnknown;
        bool forceBecausePortPairChanged = false;
    };

    /** Consumes `forceMasterPullOnNextInquirySuccess` into the snapshot (clears the flag). */
    inline MasterPullInquirySnapshot consumeMasterPullInquirySnapshot(
        bool wasDetectedBeforeSuccess,
        MatrixDeviceTypes::Type previousType,
        bool& forceMasterPullOnNextInquirySuccess) noexcept
    {
        const MasterPullInquirySnapshot snapshot {
            wasDetectedBeforeSuccess,
            previousType,
            forceMasterPullOnNextInquirySuccess
        };
        forceMasterPullOnNextInquirySuccess = false;
        return snapshot;
    }

    /** True when Device Inquiry success should silently request Master Parameter Data.
        Pulls on newly detected Matrix-1000 (including reconnect after loss), on port-pair
        reconnect while still flagged detected, and on type promotion to Matrix-1000 —
        not on presence-heartbeat success while already detected as Matrix-1000. */
    inline bool shouldPullMasterAfterDeviceInquirySuccess(
        bool wasDetectedBeforeSuccess,
        MatrixDeviceTypes::Type previousType,
        MatrixDeviceTypes::Type newType,
        bool forceBecausePortPairChanged = false) noexcept
    {
        if (! isMasterEditAllowed(true, newType))
            return false;

        if (forceBecausePortPairChanged)
            return true;

        if (! wasDetectedBeforeSuccess)
            return true;

        return previousType != MatrixDeviceTypes::Type::kMatrix1000
            && newType == MatrixDeviceTypes::Type::kMatrix1000;
    }

    inline bool shouldPullMasterAfterDeviceInquirySuccess(
        const MasterPullInquirySnapshot& snapshot,
        MatrixDeviceTypes::Type newType) noexcept
    {
        return shouldPullMasterAfterDeviceInquirySuccess(snapshot.wasDetectedBeforeSuccess,
                                                         snapshot.previousType,
                                                         newType,
                                                         snapshot.forceBecausePortPairChanged);
    }
}
