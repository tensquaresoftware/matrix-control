#include "DeviceMemoryLimits.h"

#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/Matrix6Or6RLimits.h"

namespace Core
{
    DeviceMemoryLimits::DeviceMemoryLimits(bool hasBankConcept,
                                           int minBank,
                                           int maxBank,
                                           int minPatch,
                                           int maxPatch,
                                           bool hasRomBanks,
                                           int internalPatchSlotCount) noexcept
        : hasBankConcept_(hasBankConcept)
        , minBankNumber_(minBank)
        , maxBankNumber_(maxBank)
        , minPatchNumber_(minPatch)
        , maxPatchNumber_(maxPatch)
        , hasRomBanks_(hasRomBanks)
        , internalPatchSlotCount_(internalPatchSlotCount)
    {
    }

    DeviceMemoryLimits DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type deviceType) noexcept
    {
        switch (deviceType)
        {
            case MatrixDeviceTypes::Type::kMatrix6:
            case MatrixDeviceTypes::Type::kMatrix6R:
                return DeviceMemoryLimits(
                    false,
                    0,
                    0,
                    Matrix6Or6RLimits::kMinPatchNumber,
                    Matrix6Or6RLimits::kMaxPatchNumber,
                    false,
                    Matrix6Or6RLimits::kInternalPatchSlotCount);

            case MatrixDeviceTypes::Type::kMatrix1000:
                return DeviceMemoryLimits(
                    true,
                    Matrix1000Limits::kMinBankNumber,
                    Matrix1000Limits::kMaxBankNumber,
                    Matrix1000Limits::kMinPatchNumber,
                    Matrix1000Limits::kMaxPatchNumber,
                    true,
                    (Matrix1000Limits::kMaxBankNumber - Matrix1000Limits::kMinBankNumber + 1)
                        * (Matrix1000Limits::kMaxPatchNumber - Matrix1000Limits::kMinPatchNumber + 1));

            case MatrixDeviceTypes::Type::kUnknown:
            default:
                return resolve(MatrixDeviceTypes::Type::kMatrix1000);
        }
    }

    bool DeviceMemoryLimits::isRomBank(int bankNumber) const noexcept
    {
        if (!hasRomBanks_)
            return false;

        return bankNumber >= Matrix1000Limits::kMinBankNumber + 2
            && bankNumber <= Matrix1000Limits::kMaxBankNumber;
    }

    bool DeviceMemoryLimits::isPasteStoreAllowed(int bankNumber) const noexcept
    {
        if (!hasBankConcept_)
            return true;

        return !isRomBank(bankNumber);
    }

    int DeviceMemoryLimits::wrapPatchWithinDevice(PatchCoordinates current, int direction) const noexcept
    {
        const int span = maxPatchNumber_ - minPatchNumber_ + 1;
        int offset = (current.patch - minPatchNumber_) + direction;
        offset %= span;
        if (offset < 0)
            offset += span;

        return minPatchNumber_ + offset;
    }

    PatchCoordinates DeviceMemoryLimits::advancePatch(PatchCoordinates current,
                                                      int direction) const noexcept
    {
        const int step = direction < 0 ? -1 : 1;
        auto result = current;

        if (!hasBankConcept_)
        {
            result.patch = wrapPatchWithinDevice(current, step);
            result.bank = 0;
            return result;
        }

        const int nextPatch = current.patch + step;

        if (nextPatch >= minPatchNumber_ && nextPatch <= maxPatchNumber_)
        {
            result.patch = nextPatch;
            return result;
        }

        result.patch = wrapPatchWithinDevice(current, step);
        result.bank = current.bank;
        return result;
    }
}
