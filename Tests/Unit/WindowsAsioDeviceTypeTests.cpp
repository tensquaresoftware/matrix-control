#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#ifndef MATRIX_CONTROL_ENABLE_ASIO
 #define MATRIX_CONTROL_ENABLE_ASIO 0
#endif

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
    static bool managerListsAsioType()
    {
        juce::AudioDeviceManager manager;

        for (auto* type : manager.getAvailableDeviceTypes())
        {
            if (type != nullptr && type->getTypeName().containsIgnoreCase("ASIO"))
                return true;
        }

        return false;
    }

    void asioCompileFlagMatchesPlatform()
    {
#if JUCE_WINDOWS
 #if MATRIX_CONTROL_ENABLE_ASIO
        beginTest("Windows - JUCE_ASIO compile flag is on (GH-42)");
        expectEquals(static_cast<int>(JUCE_ASIO), 1);
 #else
        beginTest("Windows - JUCE_ASIO compile flag stays off until Steinberg licence");
        expectEquals(static_cast<int>(JUCE_ASIO), 0);
 #endif
#else
        beginTest("Non-Windows - JUCE_ASIO compile flag stays off");
        expectEquals(static_cast<int>(JUCE_ASIO), 0);
#endif
    }

    void windowsRegistersAsioDeviceType()
    {
#if JUCE_WINDOWS
 #if MATRIX_CONTROL_ENABLE_ASIO
        beginTest("Windows - AudioDeviceManager registers ASIO device type (GH-42)");
        expect(managerListsAsioType());
 #else
        beginTest("Windows - AudioDeviceManager does not register ASIO while gated off");
        expect(! managerListsAsioType());
 #endif
#else
        beginTest("Non-Windows - AudioDeviceManager does not register ASIO");
        expect(! managerListsAsioType());
#endif
    }
};

static WindowsAsioDeviceTypeTests windowsAsioDeviceTypeTests;
