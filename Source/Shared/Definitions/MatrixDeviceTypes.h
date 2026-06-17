#pragma once

#include <juce_core/juce_core.h>

namespace MatrixDeviceTypes
{
    enum class Type
    {
        kUnknown,
        kMatrix1000,
        kMatrix6,
        kMatrix6R
    };

    constexpr const char* kApvtsPropertyName = "deviceType";

    constexpr const char* kMatrix1000Id = "Matrix-1000";
    constexpr const char* kMatrix6Id = "Matrix-6";
    constexpr const char* kMatrix6RId = "Matrix-6R";
    constexpr const char* kUnknownId = "Unknown";

    inline bool isMatrix6Family(Type type) noexcept
    {
        return type == Type::kMatrix6 || type == Type::kMatrix6R;
    }

    inline juce::String toApvtsString(Type type)
    {
        switch (type)
        {
            case Type::kMatrix1000: return kMatrix1000Id;
            case Type::kMatrix6:    return kMatrix6Id;
            case Type::kMatrix6R:   return kMatrix6RId;
            case Type::kUnknown:
            default:                return kUnknownId;
        }
    }

    inline Type fromApvtsString(const juce::String& value)
    {
        if (value == kMatrix1000Id)
            return Type::kMatrix1000;
        if (value == kMatrix6Id)
            return Type::kMatrix6;
        if (value == kMatrix6RId)
            return Type::kMatrix6R;
        return Type::kUnknown;
    }
}
