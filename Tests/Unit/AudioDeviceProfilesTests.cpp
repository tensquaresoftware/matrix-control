#include <juce_core/juce_core.h>

#include "Core/Audio/AudioDeviceProfiles.h"

class AudioDeviceProfilesTests : public juce::UnitTest
{
public:
    AudioDeviceProfilesTests() : juce::UnitTest("AudioDeviceProfiles") {}

    void runTest() override
    {
        keysMatch_requiresExactCompositeIdentity();
        keysMatch_rejectsFingerprintOrNameMismatch();
        shouldCapture_skipsBothEndpointsNone();
        shouldRestore_requiresSeededIdentityChangeToNonNone();
        validate_truncatesChannelBitsBeyondAvailable();
        validate_skipsUnsupportedRateAndBuffer();
        validate_appliesSupportedRateAndBuffer();
        upsert_evictsLeastRecentlyUsedAtCap();
        collapse_keepsNewestDuplicateByKey();
        roundTrip_xmlLoadSavePreservesPayload();
    }

private:
    static Core::AudioDeviceProfileKey makeKey(const juce::String& inputName,
                                               const juce::String& outputName,
                                               int inCount,
                                               int outCount)
    {
        return Core::buildProfileKey({
            .driverTypeName = "CoreAudio",
            .inputDeviceName = inputName,
            .outputDeviceName = outputName,
            .availableInputChannelCount = inCount,
            .availableOutputChannelCount = outCount,
        });
    }

    static Core::AudioDeviceProfile makeProfile(const Core::AudioDeviceProfileKey& key,
                                                juce::int64 lastUsed,
                                                double sampleRate = 48000.0,
                                                int bufferSize = 256)
    {
        Core::AudioDeviceProfile profile;
        profile.key = key;
        profile.inputChannels.setBit(0);
        profile.inputChannels.setBit(1);
        profile.outputChannels.setBit(0);
        profile.sampleRate = sampleRate;
        profile.bufferSize = bufferSize;
        profile.lastUsedUtcMs = lastUsed;
        return profile;
    }

    void keysMatch_requiresExactCompositeIdentity()
    {
        beginTest("keysMatch_requiresExactCompositeIdentity");

        const auto a = makeKey("Scarlett In", "Scarlett Out", 8, 8);
        const auto b = makeKey("Scarlett In", "Scarlett Out", 8, 8);
        expect(Core::keysMatch(a, b));
    }

    void keysMatch_rejectsFingerprintOrNameMismatch()
    {
        beginTest("keysMatch_rejectsFingerprintOrNameMismatch");

        const auto base = makeKey("Scarlett 2i2", "Scarlett 2i2", 2, 2);
        expect(! Core::keysMatch(base, makeKey("Scarlett 6i6", "Scarlett 6i6", 2, 2)));
        expect(! Core::keysMatch(base, makeKey("Scarlett 2i2", "Scarlett 2i2", 4, 2)));
        expect(! Core::keysMatch(base, makeKey("Scarlett 2i2", "Scarlett 2i2", 2, 4)));

        auto otherDriver = base;
        otherDriver.driverTypeName = "WASAPI";
        expect(! Core::keysMatch(base, otherDriver));
    }

    void shouldCapture_skipsBothEndpointsNone()
    {
        beginTest("shouldCapture_skipsBothEndpointsNone");

        expect(! Core::shouldCaptureAudioDeviceProfile(makeKey({}, {}, 0, 0)));
        expect(Core::shouldCaptureAudioDeviceProfile(makeKey("In", {}, 2, 0)));
        expect(Core::shouldCaptureAudioDeviceProfile(makeKey({}, "Out", 0, 2)));
    }

    void shouldRestore_requiresSeededIdentityChangeToNonNone()
    {
        beginTest("shouldRestore_requiresSeededIdentityChangeToNonNone");

        const Core::AudioDeviceIdentity none {};
        const Core::AudioDeviceIdentity scarlett { .outputDeviceName = "Out", .inputDeviceName = "In" };

        expect(! Core::shouldRestoreAudioDeviceProfile(false, none, scarlett));
        expect(! Core::shouldRestoreAudioDeviceProfile(true, scarlett, scarlett));
        expect(! Core::shouldRestoreAudioDeviceProfile(true, scarlett, none));
        expect(Core::shouldRestoreAudioDeviceProfile(true, none, scarlett));
    }

    void validate_truncatesChannelBitsBeyondAvailable()
    {
        beginTest("validate_truncatesChannelBitsBeyondAvailable");

        Core::AudioDeviceProfile profile = makeProfile(makeKey("In", "Out", 8, 8), 1);
        profile.inputChannels.setBit(0);
        profile.inputChannels.setBit(7);
        profile.outputChannels.setBit(0);
        profile.outputChannels.setBit(3);

        Core::AudioDeviceCapabilities caps;
        caps.availableInputChannelCount = 2;
        caps.availableOutputChannelCount = 2;
        caps.sampleRates.add(48000.0);
        caps.bufferSizes.add(256);

        const auto validated = Core::validateProfileAgainstCapabilities(profile, caps);
        expect(validated.inputChannels[0]);
        expect(! validated.inputChannels[7]);
        expect(validated.outputChannels[0]);
        expect(! validated.outputChannels[3]);
    }

    void validate_skipsUnsupportedRateAndBuffer()
    {
        beginTest("validate_skipsUnsupportedRateAndBuffer");

        const auto profile = makeProfile(makeKey("In", "Out", 2, 2), 1, 96000.0, 64);
        Core::AudioDeviceCapabilities caps;
        caps.availableInputChannelCount = 2;
        caps.availableOutputChannelCount = 2;
        caps.sampleRates.add(48000.0);
        caps.bufferSizes.add(256);

        const auto validated = Core::validateProfileAgainstCapabilities(profile, caps);
        expect(! validated.applySampleRate);
        expect(! validated.applyBufferSize);
    }

    void validate_appliesSupportedRateAndBuffer()
    {
        beginTest("validate_appliesSupportedRateAndBuffer");

        const auto profile = makeProfile(makeKey("In", "Out", 2, 2), 1, 48000.0, 256);
        Core::AudioDeviceCapabilities caps;
        caps.availableInputChannelCount = 2;
        caps.availableOutputChannelCount = 2;
        caps.sampleRates.add(48000.0);
        caps.bufferSizes.add(256);

        const auto validated = Core::validateProfileAgainstCapabilities(profile, caps);
        expect(validated.applySampleRate);
        expect(validated.applyBufferSize);
        expectEquals(validated.sampleRate, 48000.0);
        expectEquals(validated.bufferSize, 256);
    }

    void upsert_evictsLeastRecentlyUsedAtCap()
    {
        beginTest("upsert_evictsLeastRecentlyUsedAtCap");

        juce::Array<Core::AudioDeviceProfile> profiles;
        for (int i = 0; i < Core::kMaxAudioDeviceProfiles; ++i)
        {
            profiles.add(makeProfile(makeKey("In" + juce::String(i), "Out" + juce::String(i), 2, 2),
                                     static_cast<juce::int64>(1000 + i)));
        }

        const auto newest = makeProfile(makeKey("InNew", "OutNew", 2, 2), 5000);
        profiles = Core::upsertProfileLru(profiles, newest);

        expectEquals(profiles.size(), Core::kMaxAudioDeviceProfiles);
        expect(Core::findProfileIndex(profiles, newest.key) >= 0);
        expectEquals(Core::findProfileIndex(profiles, makeKey("In0", "Out0", 2, 2)), -1);
    }

    void collapse_keepsNewestDuplicateByKey()
    {
        beginTest("collapse_keepsNewestDuplicateByKey");

        const auto key = makeKey("In", "Out", 2, 2);
        auto older = makeProfile(key, 1000, 44100.0, 128);
        auto newer = makeProfile(key, 2000, 48000.0, 256);
        newer.inputChannels.clear();
        newer.inputChannels.setBit(1);

        juce::Array<Core::AudioDeviceProfile> duplicates;
        duplicates.add(older);
        duplicates.add(newer);
        duplicates.add(makeProfile(makeKey("Other", "Out", 2, 2), 1500));

        const auto collapsed = Core::collapseDuplicateProfilesByKey(duplicates);
        expectEquals(collapsed.size(), 2);

        const int index = Core::findProfileIndex(collapsed, key);
        expect(index >= 0);
        expectEquals(collapsed.getReference(index).lastUsedUtcMs, static_cast<juce::int64>(2000));
        expectEquals(collapsed.getReference(index).sampleRate, 48000.0);
        expectEquals(collapsed.getReference(index).bufferSize, 256);
        expect(collapsed.getReference(index).inputChannels[1]);
    }

    void roundTrip_xmlLoadSavePreservesPayload()
    {
        beginTest("roundTrip_xmlLoadSavePreservesPayload");

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("MatrixControlAudioDeviceProfilesTest");
        tempDir.deleteRecursively();
        expect(tempDir.createDirectory());

        const auto storeFile = tempDir.getChildFile("profiles.xml");
        const auto key = makeKey("Scarlett In", "Scarlett Out", 8, 4);
        auto original = makeProfile(key, 123456789);
        original.inputChannels.clear();
        original.inputChannels.setBit(2);
        original.outputChannels.clear();
        original.outputChannels.setBit(1);

        juce::Array<Core::AudioDeviceProfile> toSave;
        toSave.add(original);
        expect(Core::saveAudioDeviceProfilesToFile(storeFile, toSave));

        const auto loaded = Core::loadAudioDeviceProfilesFromFile(storeFile);
        expectEquals(loaded.size(), 1);
        expect(Core::keysMatch(loaded.getReference(0).key, key));
        expect(loaded.getReference(0).inputChannels[2]);
        expect(loaded.getReference(0).outputChannels[1]);
        expectEquals(loaded.getReference(0).sampleRate, 48000.0);
        expectEquals(loaded.getReference(0).bufferSize, 256);
        expectEquals(loaded.getReference(0).lastUsedUtcMs, static_cast<juce::int64>(123456789));

        tempDir.deleteRecursively();
    }
};

static AudioDeviceProfilesTests audioDeviceProfilesTests;
