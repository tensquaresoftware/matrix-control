#include <limits>

#include <juce_core/juce_core.h>

#include "Core/Audio/HardwareLatency.h"

class HardwareLatencyTests : public juce::UnitTest
{
public:
    HardwareLatencyTests() : juce::UnitTest("HardwareLatency") {}

    void runTest() override
    {
        testClampMs();
        testQuantizeMs();
        testMsToSamples();
    }

private:
    void testClampMs()
    {
        beginTest("clampMs bounds non-finite input");

        expectEquals(Core::HardwareLatency::clampMs(-5.0f), Core::HardwareLatency::kMinMs);
        expectEquals(Core::HardwareLatency::clampMs(250.0f), Core::HardwareLatency::kMaxMs);
        expectEquals(Core::HardwareLatency::clampMs(12.3f), 12.3f);
        expectEquals(Core::HardwareLatency::clampMs(std::numeric_limits<float>::quiet_NaN()),
                     Core::HardwareLatency::kMinMs);
    }

    void testQuantizeMs()
    {
        beginTest("quantizeMs rounds to 0.1 ms step");

        expectEquals(Core::HardwareLatency::quantizeMs(0.04f), 0.0f);
        expectEquals(Core::HardwareLatency::quantizeMs(0.05f), 0.1f);
        expectWithinAbsoluteError(Core::HardwareLatency::quantizeMs(12.34f), 12.3f, 0.001f);
        expectWithinAbsoluteError(Core::HardwareLatency::quantizeMs(12.36f), 12.4f, 0.001f);
        expectEquals(Core::HardwareLatency::quantizeMs(200.04f), Core::HardwareLatency::kMaxMs);
    }

    void testMsToSamples()
    {
        beginTest("msToSamples converts using sample rate");

        expectEquals(Core::HardwareLatency::msToSamples(0.0f, 44100.0), 0);
        expectEquals(Core::HardwareLatency::msToSamples(1.0f, 44100.0), 44);
        expectEquals(Core::HardwareLatency::msToSamples(5.0f, 44100.0), 220);
        expectEquals(Core::HardwareLatency::msToSamples(10.0f, 48000.0), 480);
        expectEquals(Core::HardwareLatency::msToSamples(25.0f, 96000.0), 2400);
        expectEquals(Core::HardwareLatency::msToSamples(100.0f, 44100.0), 4410);
        expectEquals(Core::HardwareLatency::msToSamples(200.0f, 44100.0), 8820);
        expectEquals(Core::HardwareLatency::msToSamples(1.0f, 0.0), 0);
    }
};

static HardwareLatencyTests hardwareLatencyTests;
