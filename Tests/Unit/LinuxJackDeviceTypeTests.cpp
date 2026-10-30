#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#include "GUI/Settings/AudioDeviceSetupSync.h"

class LinuxJackDeviceTypeTests : public juce::UnitTest
{
public:
    LinuxJackDeviceTypeTests()
        : juce::UnitTest("LinuxJackDeviceType")
    {
    }

    void runTest() override
    {
        jackCompileFlagMatchesPlatform();
        linuxRegistersJackDeviceType();
        jackApplyHelpersMatchHostModel();
    }

private:
    void jackApplyHelpersMatchHostModel()
    {
        beginTest("JACK apply - server clock skips rate enforce; endpoints stay independent");

        expect(AudioDeviceSetupSync::isJackDeviceType("JACK"));
        expect(AudioDeviceSetupSync::isJackDeviceType("jack"));
        expect(! AudioDeviceSetupSync::isJackDeviceType("ALSA"));
        expect(! AudioDeviceSetupSync::isJackDeviceType("ASIO"));
        expect(! AudioDeviceSetupSync::isAsioDeviceType("JACK"));

        expect(AudioDeviceSetupSync::shouldEnforceRequestedSampleRate("ALSA"));
        expect(AudioDeviceSetupSync::shouldEnforceRequestedSampleRate("CoreAudio"));
        expect(AudioDeviceSetupSync::shouldEnforceRequestedSampleRate("ASIO"));
        expect(! AudioDeviceSetupSync::shouldEnforceRequestedSampleRate("JACK"));
        expect(! AudioDeviceSetupSync::shouldEnforceRequestedSampleRate("jack"));

        expectEquals(AudioDeviceSetupSync::requestedSampleRateForBufferFallbackSuccess("ALSA", 44100.0),
                     44100.0);
        expectEquals(AudioDeviceSetupSync::requestedSampleRateForBufferFallbackSuccess("JACK", 44100.0),
                     0.0);
        expectEquals(AudioDeviceSetupSync::requestedSampleRateForBufferFallbackSuccess("jack", 48000.0),
                     0.0);

        juce::String input = "In A";
        juce::String output = "Out B";
        AudioDeviceSetupSync::resolveEndpointNamesForApply("JACK", input, output, true);
        expectEquals(input, juce::String("In A"));
        expectEquals(output, juce::String("Out B"));
    }

    void jackCompileFlagMatchesPlatform()
    {
#if JUCE_LINUX
        beginTest("Linux - JUCE_JACK compile flag is on");
        expectEquals(static_cast<int>(JUCE_JACK), 1);
#else
        beginTest("Non-Linux - JUCE_JACK compile flag stays off");
        expectEquals(static_cast<int>(JUCE_JACK), 0);
#endif
    }

    void linuxRegistersJackDeviceType()
    {
#if JUCE_LINUX
        beginTest("Linux - AudioDeviceManager registers JACK device type");

        juce::AudioDeviceManager manager;
        bool foundJack = false;

        for (auto* type : manager.getAvailableDeviceTypes())
        {
            if (type != nullptr && type->getTypeName().containsIgnoreCase("JACK"))
                foundJack = true;
        }

        expect(foundJack);
#else
        beginTest("Non-Linux - AudioDeviceManager does not register JACK");

        juce::AudioDeviceManager manager;
        bool foundJack = false;

        for (auto* type : manager.getAvailableDeviceTypes())
        {
            if (type != nullptr && type->getTypeName().containsIgnoreCase("JACK"))
                foundJack = true;
        }

        expect(! foundJack);
#endif
    }
};

static LinuxJackDeviceTypeTests linuxJackDeviceTypeTests;
