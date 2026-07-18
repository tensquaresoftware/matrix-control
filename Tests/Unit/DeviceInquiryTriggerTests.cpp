#include <juce_core/juce_core.h>

#include "Core/MIDI/DeviceInquiryTrigger.h"

class DeviceInquiryTriggerTests : public juce::UnitTest
{
public:
    DeviceInquiryTriggerTests() : juce::UnitTest("DeviceInquiryTrigger") {}

    void runTest() override
    {
        testRejectsWhenPortsUnavailable();
        testRejectsWhenEitherOpenIdEmpty();
        testStartsOnNewOpenPair();
        testDebouncesSameOpenPair();
        testRestartsWhenEitherPortChanges();
    }

private:
    void testRejectsWhenPortsUnavailable()
    {
        beginTest("shouldStartDeviceInquiry — false when dump/ports unavailable");

        expect(! Core::shouldStartDeviceInquiry(false, "in-a", "out-a", {}, {}));
        expect(! Core::shouldStartDeviceInquiry(false, "in-a", "out-a", "in-a", "out-a"));
    }

    void testRejectsWhenEitherOpenIdEmpty()
    {
        beginTest("shouldStartDeviceInquiry — false when either open id is empty");

        expect(! Core::shouldStartDeviceInquiry(true, {}, "out-a", {}, {}));
        expect(! Core::shouldStartDeviceInquiry(true, "in-a", {}, {}, {}));
        expect(! Core::shouldStartDeviceInquiry(true, {}, {}, {}, {}));
    }

    void testStartsOnNewOpenPair()
    {
        beginTest("shouldStartDeviceInquiry — true for a new complete open pair");

        expect(Core::shouldStartDeviceInquiry(true, "in-a", "out-a", {}, {}));
        expect(Core::shouldStartDeviceInquiry(true, "in-a", "out-a", "in-other", "out-a"));
    }

    void testDebouncesSameOpenPair()
    {
        beginTest("shouldStartDeviceInquiry — false for the same pair already inquired");

        expect(! Core::shouldStartDeviceInquiry(true, "in-a", "out-a", "in-a", "out-a"));
    }

    void testRestartsWhenEitherPortChanges()
    {
        beginTest("shouldStartDeviceInquiry — true when From or To changes");

        expect(Core::shouldStartDeviceInquiry(true, "in-b", "out-a", "in-a", "out-a"));
        expect(Core::shouldStartDeviceInquiry(true, "in-a", "out-b", "in-a", "out-a"));
        expect(Core::shouldStartDeviceInquiry(true, "in-b", "out-b", "in-a", "out-a"));
    }
};

static DeviceInquiryTriggerTests deviceInquiryTriggerTests;
