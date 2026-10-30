#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

class WindowsAsioDeviceTypeTests : public juce::UnitTest
{
public:
    WindowsAsioDeviceTypeTests()
        : juce::UnitTest("WindowsAsioDeviceType")
    {
    }

    void runTest() override
    {
        asioCompileFlagMatchesPlatform();
        windowsRegistersAsioDeviceType();
    }

private:
    void asioCompileFlagMatchesPlatform()
    {
#if JUCE_WINDOWS
        beginTest("Windows - JUCE_ASIO compile flag is on (GH-42)");
        expectEquals(static_cast<int>(JUCE_ASIO), 1);
#else
        beginTest("Non-Windows - JUCE_ASIO compile flag stays off");
        expectEquals(static_cast<int>(JUCE_ASIO), 0);
#endif
    }

    void windowsRegistersAsioDeviceType()
    {
#if JUCE_WINDOWS
        beginTest("Windows - AudioDeviceManager registers ASIO device type (GH-42)");

        juce::AudioDeviceManager manager;
        bool foundAsio = false;

        for (auto* type : manager.getAvailableDeviceTypes())
        {
            if (type != nullptr && type->getTypeName().containsIgnoreCase("ASIO"))
                foundAsio = true;
        }

        expect(foundAsio);
#else
        beginTest("Non-Windows - AudioDeviceManager does not register ASIO");

        juce::AudioDeviceManager manager;
        bool foundAsio = false;

        for (auto* type : manager.getAvailableDeviceTypes())
        {
            if (type != nullptr && type->getTypeName().containsIgnoreCase("ASIO"))
                foundAsio = true;
        }

        expect(! foundAsio);
#endif
    }
};

static WindowsAsioDeviceTypeTests windowsAsioDeviceTypeTests;
