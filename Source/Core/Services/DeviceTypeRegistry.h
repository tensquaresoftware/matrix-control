#pragma once

#include <juce_core/juce_core.h>

#include "Shared/Definitions/MatrixDeviceTypes.h"

struct DeviceIdInfo;

namespace Core
{
    class DeviceTypeRegistry
    {
    public:
        static MatrixDeviceTypes::Type fromMemberBytes(juce::uint8 memberLow,
                                                     juce::uint8 memberHigh) noexcept;

        static MatrixDeviceTypes::Type fromDeviceInquiry(const DeviceIdInfo& info) noexcept;

        static MatrixDeviceTypes::Type fromApvtsProperty(const juce::var& propertyValue) noexcept;
    };
}
