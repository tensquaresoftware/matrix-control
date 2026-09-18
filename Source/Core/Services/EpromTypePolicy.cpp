#include "Core/Services/EpromTypePolicy.h"

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace Core
{
    namespace
    {
        juce::String normalizeVersionToken(const juce::String& version)
        {
            // Device Inquiry often stores a packed ASCII token (e.g. "120");
            // suggestion table keys use dotted form ("1.20"), matching footer display.
            auto cleaned = version.removeCharacters(" \t").toUpperCase();
            if (cleaned.isEmpty())
                return {};

            if (cleaned.containsChar('.') || cleaned.length() < 3)
                return cleaned;

            return cleaned.dropLastCharacters(2) + "." + cleaned.getLastCharacters(2);
        }
    }

    int EpromTypePolicy::normalize(int rawId) noexcept
    {
        using namespace PluginIDs::Settings::EpromType;

        switch (rawId)
        {
            case kFactory:
            case kGligli:
            case kTauntek:
            case kUntergeek:
            case kUnknown:
                return rawId;
            default:
                return kDefault;
        }
    }

    bool EpromTypePolicy::isValidForDeviceFamily(int typeId, MatrixDeviceFamily family) noexcept
    {
        const int id = normalize(typeId);
        if (family != MatrixDeviceFamily::kMatrix6Or6R)
            return true;

        return id != PluginIDs::Settings::EpromType::kGligli;
    }

    int EpromTypePolicy::coerceForDeviceFamily(int typeId, MatrixDeviceFamily family) noexcept
    {
        const int id = normalize(typeId);
        if (isValidForDeviceFamily(id, family))
            return id;

        return PluginIDs::Settings::EpromType::kUnknown;
    }

    EpromClass EpromTypePolicy::toEpromClass(int typeId) noexcept
    {
        using namespace PluginIDs::Settings::EpromType;

        switch (normalize(typeId))
        {
            case kGligli:
            case kTauntek:
            case kUntergeek:
                return EpromClass::kOptimised;
            case kFactory:
            case kUnknown:
            default:
                return EpromClass::kStock;
        }
    }

    MatrixDeviceFamily EpromTypePolicy::deviceFamilyFromType(MatrixDeviceTypes::Type deviceType) noexcept
    {
        if (MatrixDeviceTypes::isMatrix6Family(deviceType))
            return MatrixDeviceFamily::kMatrix6Or6R;

        return MatrixDeviceFamily::kMatrix1000;
    }

    MatrixDeviceFamily EpromTypePolicy::deviceFamilyFromMemberBytes(juce::uint8 memberLow,
                                                                  juce::uint8 memberHigh) noexcept
    {
        if (memberLow == SysExConstants::DeviceInquiry::kExpectedMemberLow
            && memberHigh == SysExConstants::DeviceInquiry::kExpectedMemberHigh)
            return MatrixDeviceFamily::kMatrix1000;

        if (memberLow == SysExConstants::DeviceInquiry::kMatrix6MemberLow
            && memberHigh == SysExConstants::DeviceInquiry::kMatrix6MemberHigh)
            return MatrixDeviceFamily::kMatrix6Or6R;

        return MatrixDeviceFamily::kMatrix1000;
    }

    int EpromTypePolicy::suggestFromInquiryVersion(const juce::String& version,
                                                   MatrixDeviceFamily family) noexcept
    {
        using namespace PluginIDs::Settings::EpromType;

        const auto token = normalizeVersionToken(version);

        if (family == MatrixDeviceFamily::kMatrix6Or6R)
        {
            if (token == "2.15")
                return kTauntek;
            return kUnknown;
        }

        if (token == "1.16")
            return kGligli;
        if (token == "1.20")
            return kTauntek;
        if (token == "1.21")
            return kUntergeek;
        if (token == "1.11" || token == "1.09")
            return kFactory;

        return kUnknown;
    }

    int EpromTypePolicy::preferredForPrompt(int suggestedId, int storedId) noexcept
    {
        using namespace PluginIDs::Settings::EpromType;

        const int suggested = normalize(suggestedId);
        if (suggested != kUnknown)
            return suggested;

        return normalize(storedId);
    }

    const char* EpromTypePolicy::displayNameForId(int typeId) noexcept
    {
        using namespace PluginIDs::Settings::EpromType;
        namespace Names = PluginDisplayNames::Settings;

        switch (normalize(typeId))
        {
            case kFactory:   return Names::kEpromTypeFactory;
            case kGligli:    return Names::kEpromTypeGligli;
            case kTauntek:   return Names::kEpromTypeTauntek;
            case kUntergeek: return Names::kEpromTypeUntergeek;
            case kUnknown:
            default:         return Names::kEpromTypeUnknown;
        }
    }
}
