#include "DeviceTypeRegistry.h"

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"

namespace Core
{
    MatrixDeviceTypes::Type DeviceTypeRegistry::fromMemberBytes(juce::uint8 memberLow,
                                                                juce::uint8 memberHigh) noexcept
    {
        if (memberLow == SysExConstants::DeviceInquiry::kExpectedMemberLow
            && memberHigh == SysExConstants::DeviceInquiry::kExpectedMemberHigh)
            return MatrixDeviceTypes::Type::kMatrix1000;

        if (memberLow == SysExConstants::DeviceInquiry::kMatrix6MemberLow
            && memberHigh == SysExConstants::DeviceInquiry::kMatrix6MemberHigh)
            return MatrixDeviceTypes::Type::kMatrix6;

        return MatrixDeviceTypes::Type::kUnknown;
    }

    MatrixDeviceTypes::Type DeviceTypeRegistry::fromDeviceInquiry(const DeviceIdInfo& info) noexcept
    {
        if (!info.isValid)
            return MatrixDeviceTypes::Type::kUnknown;

        if (info.manufacturerId != SysExConstants::DeviceInquiry::kExpectedManufacturer)
            return MatrixDeviceTypes::Type::kUnknown;

        if (info.familyLow != SysExConstants::DeviceInquiry::kExpectedFamily)
            return MatrixDeviceTypes::Type::kUnknown;

        return fromMemberBytes(info.memberLow, info.memberHigh);
    }

    MatrixDeviceTypes::Type DeviceTypeRegistry::fromApvtsProperty(const juce::var& propertyValue) noexcept
    {
        return MatrixDeviceTypes::fromApvtsString(propertyValue.toString());
    }
}
