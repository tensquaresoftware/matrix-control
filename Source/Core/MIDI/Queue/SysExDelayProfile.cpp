#include "Core/MIDI/Queue/SysExDelayProfile.h"

#include "Core/MIDI/SysEx/SysExDecoder.h"

namespace Core
{
    namespace
    {
        bool matchesOptimisedFirmware(const juce::String& version)
        {
            const auto upper = version.toUpperCase();
            return upper.contains("TAUNTEK")
                || upper.contains("GLIGLI")
                || upper.contains("NORDCORE");
        }

        MatrixDeviceFamily deviceFamilyFromMemberBytes(juce::uint8 memberLow,
                                                      juce::uint8 memberHigh) noexcept
        {
            if (memberLow == 0x02 && memberHigh == 0x00)
                return MatrixDeviceFamily::kMatrix1000;

            // Provisional M-6/6R pattern — Epic 8 / SM-1 may revise when hardware confirms.
            if (memberLow == 0x01 && memberHigh == 0x00)
                return MatrixDeviceFamily::kMatrix6Or6R;

            return MatrixDeviceFamily::kMatrix1000;
        }
    }

    SysExDelayProfile SysExDelayProfile::fromDeviceInquiry(const DeviceIdInfo& info)
    {
        const auto epromClass = matchesOptimisedFirmware(info.version)
            ? EpromClass::kOptimised
            : EpromClass::kStock;
        const auto deviceFamily = deviceFamilyFromMemberBytes(info.memberLow, info.memberHigh);
        return SysExDelayProfile { epromClass, deviceFamily };
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
