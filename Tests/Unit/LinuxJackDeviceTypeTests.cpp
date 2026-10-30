#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

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
    }

private:
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
