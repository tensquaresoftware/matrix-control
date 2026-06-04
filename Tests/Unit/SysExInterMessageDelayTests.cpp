#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/MIDI/Queue/SysExInterMessageDelay.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"

class SysExInterMessageDelayTests : public juce::UnitTest
{
public:
    SysExInterMessageDelayTests() : juce::UnitTest("SysExInterMessageDelay") {}

    void runTest() override
    {
        testFirstSendAllowedImmediately();
        testGateTimingStockProfile();
        testProfileSwapDoesNotResetLastSendTimestamp();
    }

private:
    void testFirstSendAllowedImmediately()
    {
        beginTest("no prior send — millisUntilNextAllowed returns 0");

        Core::SysExInterMessageDelay gate;
        expectEquals(gate.millisUntilNextAllowed(0), 0);
        expectEquals(gate.getRequiredDelayMs(), Core::SysExDelayProfile::kStockDelayMsMatrix1000);
    }

    void testGateTimingStockProfile()
    {
        beginTest("recordSysExSent then millisUntilNextAllowed — 10 ms stock profile");

        Core::SysExInterMessageDelay gate { Core::SysExDelayProfile::stockDefault() };
        gate.recordSysExSent(0);
        expectEquals(gate.millisUntilNextAllowed(5), 5);
        expectEquals(gate.millisUntilNextAllowed(10), 0);
        expectEquals(gate.millisUntilNextAllowed(100), 0);
    }

    void testProfileSwapDoesNotResetLastSendTimestamp()
    {
        beginTest("setProfile mid-session — last-send unchanged, subsequent wait uses new delay");

        Core::SysExInterMessageDelay gate { Core::SysExDelayProfile::stockDefault() };
        gate.recordSysExSent(100);
        expectEquals(gate.millisUntilNextAllowed(105), 5);

        const auto optimisedM1000 = Core::SysExDelayProfile::fromDeviceInquiry(
            []() {
                DeviceIdInfo info {};
                info.memberLow = 0x02;
                info.memberHigh = 0x00;
                info.version = "TAUNTEK";
                info.isValid = true;
                return info;
            }());
        gate.setProfile(optimisedM1000);

        expectEquals(gate.millisUntilNextAllowed(105), 0);
        expectEquals(gate.millisUntilNextAllowed(102),
                     Core::SysExDelayProfile::kOptimisedDelayMsMatrix1000 - 2);
    }
};

static SysExInterMessageDelayTests sysExInterMessageDelayTests;
