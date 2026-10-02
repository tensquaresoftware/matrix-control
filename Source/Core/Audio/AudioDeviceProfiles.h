#pragma once

#include <juce_core/juce_core.h>

#include "Core/Audio/AudioDevicePreferredSetup.h"

namespace Core
{
    /** Max retained profiles; eviction uses least-recently-used lastUsedUtcMs. */
    inline constexpr int kMaxAudioDeviceProfiles = 20;

    inline constexpr const char* kAudioDeviceProfilesFileName = "profiles.xml";
    inline constexpr const char* kAudioDeviceProfilesRootTag = "AudioDeviceProfiles";
    inline constexpr const char* kAudioDeviceProfileTag = "Profile";

    /**
        Strict composite identity for profile lookup (v1).
        Fingerprint = available channel-name counts (capability), not active-bit counts.
    */
    struct AudioDeviceProfileKey
    {
        juce::String driverTypeName;
        juce::String inputDeviceName;
        juce::String outputDeviceName;
        int availableInputChannelCount = 0;
        int availableOutputChannelCount = 0;
    };

    /** Persisted payload for one interface profile (never includes audioFromSourceId). */
    struct AudioDeviceProfile
    {
        AudioDeviceProfileKey key;
        juce::BigInteger inputChannels;
        juce::BigInteger outputChannels;
        double sampleRate = 0.0;
        int bufferSize = 0;
        juce::int64 lastUsedUtcMs = 0;
    };

    /** Live device capabilities used to validate / truncate a stored profile. */
    struct AudioDeviceCapabilities
    {
        int availableInputChannelCount = 0;
        int availableOutputChannelCount = 0;
        juce::Array<double> sampleRates;
        juce::Array<int> bufferSizes;
    };

    /** Validated subset of a profile safe to apply to the current device. */
    struct ValidatedAudioDeviceProfile
    {
        juce::BigInteger inputChannels;
        juce::BigInteger outputChannels;
        double sampleRate = 0.0;
        int bufferSize = 0;
        bool applySampleRate = false;
        bool applyBufferSize = false;
    };

    /** Normalize counts; Key v1 identity fields stay exact OS strings. */
    inline AudioDeviceProfileKey buildProfileKey(AudioDeviceProfileKey key) noexcept
    {
        key.availableInputChannelCount = juce::jmax(0, key.availableInputChannelCount);
        key.availableOutputChannelCount = juce::jmax(0, key.availableOutputChannelCount);
        return key;
    }

    inline bool keysMatch(const AudioDeviceProfileKey& a, const AudioDeviceProfileKey& b) noexcept
    {
        return a.driverTypeName == b.driverTypeName
            && a.inputDeviceName == b.inputDeviceName
            && a.outputDeviceName == b.outputDeviceName
            && a.availableInputChannelCount == b.availableInputChannelCount
            && a.availableOutputChannelCount == b.availableOutputChannelCount;
    }

    /** Skip save when both Input and Output endpoints are None. */
    inline bool shouldCaptureAudioDeviceProfile(const AudioDeviceProfileKey& key) noexcept
    {
        return key.inputDeviceName.isNotEmpty() || key.outputDeviceName.isNotEmpty();
    }

    /**
        Restore only after a seeded identity change to a non-None selection.
        First observation (window open) must not restore — use hasSeededIdentity=false.
    */
    inline bool shouldRestoreAudioDeviceProfile(bool hasSeededIdentity,
                                                const AudioDeviceIdentity& previous,
                                                const AudioDeviceIdentity& current) noexcept
    {
        if (! hasSeededIdentity)
            return false;

        if (! didAudioDeviceIdentityChange(previous, current))
            return false;

        return current.inputDeviceName.isNotEmpty() || current.outputDeviceName.isNotEmpty();
    }

    inline juce::BigInteger truncateChannelBits(juce::BigInteger bits, int availableCount) noexcept
    {
        if (availableCount <= 0)
        {
            bits.clear();
            return bits;
        }

        const int highest = bits.getHighestBit();
        for (int bit = availableCount; bit <= highest; ++bit)
            bits.clearBit(bit);

        return bits;
    }

    inline bool sampleRateSupported(double sampleRate, const juce::Array<double>& rates) noexcept
    {
        if (sampleRate <= 0.0)
            return false;

        for (const auto rate : rates)
        {
            if (juce::approximatelyEqual(rate, sampleRate))
                return true;
        }

        return false;
    }

    inline bool bufferSizeSupported(int bufferSize, const juce::Array<int>& sizes) noexcept
    {
        return bufferSize > 0 && sizes.contains(bufferSize);
    }

    /** Truncate channel bits; skip unsupported rate/buffer (leave apply flags false). */
    inline ValidatedAudioDeviceProfile validateProfileAgainstCapabilities(
        const AudioDeviceProfile& profile,
        const AudioDeviceCapabilities& capabilities) noexcept
    {
        ValidatedAudioDeviceProfile validated;
        validated.inputChannels = truncateChannelBits(profile.inputChannels,
                                                      capabilities.availableInputChannelCount);
        validated.outputChannels = truncateChannelBits(profile.outputChannels,
                                                       capabilities.availableOutputChannelCount);

        if (sampleRateSupported(profile.sampleRate, capabilities.sampleRates))
        {
            validated.sampleRate = profile.sampleRate;
            validated.applySampleRate = true;
        }

        if (bufferSizeSupported(profile.bufferSize, capabilities.bufferSizes))
        {
            validated.bufferSize = profile.bufferSize;
            validated.applyBufferSize = true;
        }

        return validated;
    }

    inline int findProfileIndex(const juce::Array<AudioDeviceProfile>& profiles,
                                const AudioDeviceProfileKey& key) noexcept
    {
        for (int i = 0; i < profiles.size(); ++i)
        {
            if (keysMatch(profiles.getReference(i).key, key))
                return i;
        }

        return -1;
    }

    /**
        Upsert by exact key, touch lastUsed, then evict LRU until size <= maxProfiles.
        Never exceeds the cap after return.
    */
    inline juce::Array<AudioDeviceProfile> upsertProfileLru(
        juce::Array<AudioDeviceProfile> profiles,
        AudioDeviceProfile incoming,
        int maxProfiles = kMaxAudioDeviceProfiles)
    {
        const int existing = findProfileIndex(profiles, incoming.key);
        if (existing >= 0)
            profiles.set(existing, incoming);
        else
            profiles.add(incoming);

        const int cap = juce::jmax(1, maxProfiles);
        while (profiles.size() > cap)
        {
            int oldestIndex = 0;
            auto oldestStamp = profiles.getReference(0).lastUsedUtcMs;

            for (int i = 1; i < profiles.size(); ++i)
            {
                const auto stamp = profiles.getReference(i).lastUsedUtcMs;
                if (stamp < oldestStamp)
                {
                    oldestStamp = stamp;
                    oldestIndex = i;
                }
            }

            profiles.remove(oldestIndex);
        }

        return profiles;
    }

    /** Keep one entry per exact key; prefer the highest lastUsedUtcMs. */
    inline juce::Array<AudioDeviceProfile> collapseDuplicateProfilesByKey(
        const juce::Array<AudioDeviceProfile>& profiles)
    {
        juce::Array<AudioDeviceProfile> collapsed;

        for (const auto& profile : profiles)
        {
            const int existing = findProfileIndex(collapsed, profile.key);
            if (existing < 0)
            {
                collapsed.add(profile);
                continue;
            }

            if (profile.lastUsedUtcMs >= collapsed.getReference(existing).lastUsedUtcMs)
                collapsed.set(existing, profile);
        }

        return collapsed;
    }

    juce::File getAudioDeviceProfilesStoreFile();
    juce::Array<AudioDeviceProfile> loadAudioDeviceProfilesFromFile(const juce::File& storeFile);
    bool saveAudioDeviceProfilesToFile(const juce::File& storeFile,
                                       const juce::Array<AudioDeviceProfile>& profiles);
    juce::Array<AudioDeviceProfile> loadAudioDeviceProfiles();
    bool saveAudioDeviceProfiles(const juce::Array<AudioDeviceProfile>& profiles);
}
