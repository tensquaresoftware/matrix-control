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
    /** Footer / UI label for shared Matrix-6 / 6R family (APVTS id stays Matrix-6). */
    constexpr const char* kMatrix6FamilyDisplayName = "Matrix-6/6R";
    /** Footer / UI label when a Matrix-family member is connected but unsupported. */
    constexpr const char* kUnknownDisplayName = "Unknown Device";

    inline bool isMatrix6Family(Type type) noexcept
    {
        return type == Type::kMatrix6 || type == Type::kMatrix6R;
    }

    /** True for Matrix members the editor may control (not family-Unknown). */
    inline bool isSupportedMatrixDevice(Type type) noexcept
    {
        return type == Type::kMatrix1000
            || type == Type::kMatrix6
            || type == Type::kMatrix6R;
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

    /** Human-readable identity for the footer (and similar UI). */
    inline juce::String toDisplayString(Type type)
    {
        switch (type)
        {
            case Type::kMatrix1000: return kMatrix1000Id;
            case Type::kMatrix6:    return kMatrix6FamilyDisplayName;
            case Type::kMatrix6R:   return kMatrix6RId;
            case Type::kUnknown:
            default:                return kUnknownDisplayName;
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
