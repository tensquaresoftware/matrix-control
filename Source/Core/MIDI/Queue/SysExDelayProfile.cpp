#include "Core/MIDI/Queue/SysExDelayProfile.h"

#include "Core/Services/EpromTypePolicy.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"

namespace Core
{
    SysExDelayProfile SysExDelayProfile::fromSettings(int epromTypeId,
                                                      MatrixDeviceFamily deviceFamily) noexcept
    {
        const int coercedId = EpromTypePolicy::coerceForDeviceFamily(epromTypeId, deviceFamily);
        return SysExDelayProfile { EpromTypePolicy::toEpromClass(coercedId), deviceFamily };
    }

    SysExDelayProfile SysExDelayProfile::fromDeviceInquiry(const DeviceIdInfo& info, int epromTypeId)
    {
        const auto deviceFamily =
            EpromTypePolicy::deviceFamilyFromMemberBytes(info.memberLow, info.memberHigh);
        return fromSettings(epromTypeId, deviceFamily);
    }

    SysExDelayProfile SysExDelayProfile::stockDefault() noexcept
    {
        return SysExDelayProfile { EpromClass::kStock, MatrixDeviceFamily::kMatrix1000 };
    }

    SysExDelayProfile::SysExDelayProfile(EpromClass epromClass,
                                         MatrixDeviceFamily deviceFamily) noexcept
        : epromClass_ { epromClass }
        , deviceFamily_ { deviceFamily }
    {
    }

    int SysExDelayProfile::getDelayMs() const noexcept
    {
        if (epromClass_ == EpromClass::kOptimised)
        {
            return deviceFamily_ == MatrixDeviceFamily::kMatrix6Or6R
                ? kOptimisedDelayMsMatrix6
                : kOptimisedDelayMsMatrix1000;
        }

        return deviceFamily_ == MatrixDeviceFamily::kMatrix6Or6R
            ? kStockDelayMsMatrix6
            : kStockDelayMsMatrix1000;
    }

    EpromClass SysExDelayProfile::getEpromClass() const noexcept
    {
        return epromClass_;
    }

    MatrixDeviceFamily SysExDelayProfile::getDeviceFamily() const noexcept
    {
        return deviceFamily_;
    }
}
