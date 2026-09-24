#pragma once

#include <juce_core/juce_core.h>

namespace Core
{
    /** Live CoreMIDI id tokens used to match JUCE MidiDeviceInfo::identifier strings. */
    struct LiveMidiIdSets
    {
        juce::StringArray endpointIds;
        juce::StringArray deviceIdsWithLiveEndpoint;
    };

    [[nodiscard]] inline juce::StringArray nonEmptyIdentifierTokens(const juce::String& identifier)
    {
        juce::StringArray parts;
        for (const auto& part : juce::StringArray::fromTokens(identifier, " ,", ""))
        {
            const auto trimmed = part.trim();
            if (trimmed.isNotEmpty())
                parts.add(trimmed);
        }
        return parts;
    }

    /** True when identifier equals or shares a token with a live endpoint or device UniqueID. */
    [[nodiscard]] inline bool identifierMatchesLiveIds(const juce::String& identifier,
                                                       const LiveMidiIdSets& ids) noexcept
    {
        if (identifier.isEmpty() || ids.endpointIds.isEmpty())
            return false;

        if (ids.endpointIds.contains(identifier))
            return true;

        const auto parts = nonEmptyIdentifierTokens(identifier);
        if (parts.isEmpty())
            return false;

        for (const auto& part : parts)
        {
            if (ids.endpointIds.contains(part)
                || ids.deviceIdsWithLiveEndpoint.contains(part))
            {
                return true;
            }
        }

        return false;
    }
}
