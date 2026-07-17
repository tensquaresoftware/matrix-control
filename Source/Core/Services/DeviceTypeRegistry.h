#pragma once

#include <juce_core/juce_core.h>

#include "Shared/Definitions/MatrixDeviceTypes.h"

struct DeviceIdInfo;

namespace Core
{
    /** Maps Universal Device Inquiry member bytes to MatrixDeviceTypes.
        Matrix-6/6R member bytes remain provisional until hardware UAT (PRD §9 #6);
        inquiry never invents a distinct Matrix-6R pattern. */
    class DeviceTypeRegistry
    {
    public:
        static MatrixDeviceTypes::Type fromMemberBytes(juce::uint8 memberLow,
                                                     juce::uint8 memberHigh) noexcept;

        static MatrixDeviceTypes::Type fromDeviceInquiry(const DeviceIdInfo& info) noexcept;

        static MatrixDeviceTypes::Type fromApvtsProperty(const juce::var& propertyValue) noexcept;
    };
}
