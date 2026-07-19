#pragma once

#include <juce_core/juce_core.h>

#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
    /** Preferred Audio From kind for standalone after Device Inquiry (FR-4 / AD-11).
        Does not change JUCE bus channel count — stereo input bus stays declared. */
    enum class PreferredAudioFromKind
    {
        kNone,
        kMono,
        kStereo
    };

    inline PreferredAudioFromKind preferredAudioFromKind(MatrixDeviceTypes::Type deviceType,
                                                         bool deviceDetected) noexcept
    {
        if (! deviceDetected)
            return PreferredAudioFromKind::kNone;

        switch (deviceType)
        {
            case MatrixDeviceTypes::Type::kMatrix1000:
                return PreferredAudioFromKind::kMono;
            case MatrixDeviceTypes::Type::kMatrix6:
            case MatrixDeviceTypes::Type::kMatrix6R:
                return PreferredAudioFromKind::kStereo;
            case MatrixDeviceTypes::Type::kUnknown:
            default:
                return PreferredAudioFromKind::kNone;
        }
    }

    /** Picks the first catalog sourceId matching the preferred kind, or empty if none. */
    inline juce::String pickPreferredAudioFromSourceId(MatrixDeviceTypes::Type deviceType,
                                                       bool deviceDetected,
                                                       const juce::StringArray& availableSourceIds) noexcept
    {
        const auto kind = preferredAudioFromKind(deviceType, deviceDetected);

        if (kind == PreferredAudioFromKind::kNone || availableSourceIds.isEmpty())
            return {};

        const juce::String prefix = (kind == PreferredAudioFromKind::kMono) ? "mono:" : "stereo:";

        for (const auto& sourceId : availableSourceIds)
        {
            if (sourceId.startsWith(prefix))
                return sourceId;
        }

        return {};
    }
}
