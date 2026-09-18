#pragma once

#include <array>
#include <cstddef>

#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{
    /** Settings EPROM TYPE → capability class, device-family item sets, Inquiry suggestion. */
    class EpromTypePolicy
    {
    public:
        static constexpr std::array<int, 5> kMatrix1000Items {
            PluginIDs::Settings::EpromType::kFactory,
            PluginIDs::Settings::EpromType::kGligli,
            PluginIDs::Settings::EpromType::kTauntek,
            PluginIDs::Settings::EpromType::kUntergeek,
            PluginIDs::Settings::EpromType::kUnknown
        };

        static constexpr std::array<int, 4> kMatrix6FamilyItems {
            PluginIDs::Settings::EpromType::kFactory,
            PluginIDs::Settings::EpromType::kTauntek,
            PluginIDs::Settings::EpromType::kUntergeek,
            PluginIDs::Settings::EpromType::kUnknown
        };

        static int normalize(int rawId) noexcept;
        static bool isValidForDeviceFamily(int typeId, MatrixDeviceFamily family) noexcept;
        static int coerceForDeviceFamily(int typeId, MatrixDeviceFamily family) noexcept;

        static EpromClass toEpromClass(int typeId) noexcept;
        static MatrixDeviceFamily deviceFamilyFromType(MatrixDeviceTypes::Type deviceType) noexcept;
        static MatrixDeviceFamily deviceFamilyFromMemberBytes(juce::uint8 memberLow,
                                                             juce::uint8 memberHigh) noexcept;

        /** Modal combo default only — never authoritative for live delay. */
        static int suggestFromInquiryVersion(const juce::String& version,
                                             MatrixDeviceFamily family) noexcept;

        /** Prefer a mapped Inquiry suggestion; otherwise keep the stored Settings value. */
        static int preferredForPrompt(int suggestedId, int storedId) noexcept;

        static const char* displayNameForId(int typeId) noexcept;

        template <typename Fn>
        static void forEachValidItem(MatrixDeviceFamily family, Fn&& fn)
        {
            if (family == MatrixDeviceFamily::kMatrix6Or6R)
            {
                for (const int id : kMatrix6FamilyItems)
                    fn(id);
                return;
            }

            for (const int id : kMatrix1000Items)
                fn(id);
        }
    };
}
