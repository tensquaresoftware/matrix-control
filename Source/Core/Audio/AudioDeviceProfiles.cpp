#include "Core/Audio/AudioDeviceProfiles.h"

#include <memory>

#include "Shared/ProjectPaths.h"

namespace Core
{
    namespace
    {
        constexpr const char* kAttrDriverTypeName = "driverTypeName";
        constexpr const char* kAttrInputDeviceName = "inputDeviceName";
        constexpr const char* kAttrOutputDeviceName = "outputDeviceName";
        constexpr const char* kAttrAvailableInputChannelCount = "availableInputChannelCount";
        constexpr const char* kAttrAvailableOutputChannelCount = "availableOutputChannelCount";
        constexpr const char* kAttrInputChannels = "inputChannels";
        constexpr const char* kAttrOutputChannels = "outputChannels";
        constexpr const char* kAttrSampleRate = "sampleRate";
        constexpr const char* kAttrBufferSize = "bufferSize";
        constexpr const char* kAttrLastUsedUtcMs = "lastUsedUtcMs";

        juce::String channelBitsToHex(const juce::BigInteger& bits)
        {
            return bits.toString(16);
        }

        juce::BigInteger channelBitsFromHex(const juce::String& hex)
        {
            juce::BigInteger bits;
            if (hex.isNotEmpty())
                bits.parseString(hex, 16);
            return bits;
        }

        AudioDeviceProfile profileFromXml(const juce::XmlElement& element)
        {
            AudioDeviceProfile profile;
            profile.key = buildProfileKey({
                .driverTypeName = element.getStringAttribute(kAttrDriverTypeName),
                .inputDeviceName = element.getStringAttribute(kAttrInputDeviceName),
                .outputDeviceName = element.getStringAttribute(kAttrOutputDeviceName),
                .availableInputChannelCount = element.getIntAttribute(kAttrAvailableInputChannelCount),
                .availableOutputChannelCount = element.getIntAttribute(kAttrAvailableOutputChannelCount),
            });
            profile.inputChannels = channelBitsFromHex(element.getStringAttribute(kAttrInputChannels));
            profile.outputChannels = channelBitsFromHex(element.getStringAttribute(kAttrOutputChannels));
            profile.sampleRate = element.getDoubleAttribute(kAttrSampleRate);
            profile.bufferSize = element.getIntAttribute(kAttrBufferSize);
            profile.lastUsedUtcMs = static_cast<juce::int64>(
                element.getStringAttribute(kAttrLastUsedUtcMs, "0").getLargeIntValue());
            return profile;
        }

        std::unique_ptr<juce::XmlElement> profileToXml(const AudioDeviceProfile& profile)
        {
            auto element = std::make_unique<juce::XmlElement>(kAudioDeviceProfileTag);
            element->setAttribute(kAttrDriverTypeName, profile.key.driverTypeName);
            element->setAttribute(kAttrInputDeviceName, profile.key.inputDeviceName);
            element->setAttribute(kAttrOutputDeviceName, profile.key.outputDeviceName);
            element->setAttribute(kAttrAvailableInputChannelCount,
                                  profile.key.availableInputChannelCount);
            element->setAttribute(kAttrAvailableOutputChannelCount,
                                  profile.key.availableOutputChannelCount);
            element->setAttribute(kAttrInputChannels, channelBitsToHex(profile.inputChannels));
            element->setAttribute(kAttrOutputChannels, channelBitsToHex(profile.outputChannels));
            element->setAttribute(kAttrSampleRate, profile.sampleRate);
            element->setAttribute(kAttrBufferSize, profile.bufferSize);
            element->setAttribute(kAttrLastUsedUtcMs, juce::String(profile.lastUsedUtcMs));
            return element;
        }
    }

    juce::File getAudioDeviceProfilesStoreFile()
    {
        const auto directory = ProjectPaths::getAudioDeviceProfilesDirectory();
        if (! directory.isDirectory())
            return {};

        return directory.getChildFile(kAudioDeviceProfilesFileName);
    }

    juce::Array<AudioDeviceProfile> loadAudioDeviceProfilesFromFile(const juce::File& storeFile)
    {
        juce::Array<AudioDeviceProfile> profiles;
        if (! storeFile.existsAsFile())
            return profiles;

        const auto xml = juce::XmlDocument::parse(storeFile);
        if (xml == nullptr || ! xml->hasTagName(kAudioDeviceProfilesRootTag))
            return profiles;

        for (auto* child : xml->getChildWithTagNameIterator(kAudioDeviceProfileTag))
        {
            if (child != nullptr)
                profiles.add(profileFromXml(*child));
        }

        return collapseDuplicateProfilesByKey(profiles);
    }

    bool saveAudioDeviceProfilesToFile(const juce::File& storeFile,
                                       const juce::Array<AudioDeviceProfile>& profiles)
    {
        if (storeFile == juce::File())
            return false;

        storeFile.getParentDirectory().createDirectory();

        juce::XmlElement root(kAudioDeviceProfilesRootTag);
        for (const auto& profile : profiles)
            root.addChildElement(profileToXml(profile).release());

        return root.writeTo(storeFile);
    }

    juce::Array<AudioDeviceProfile> loadAudioDeviceProfiles()
    {
        return loadAudioDeviceProfilesFromFile(getAudioDeviceProfilesStoreFile());
    }

    bool saveAudioDeviceProfiles(const juce::Array<AudioDeviceProfile>& profiles)
    {
        return saveAudioDeviceProfilesToFile(getAudioDeviceProfilesStoreFile(), profiles);
    }
}
