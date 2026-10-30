#include <juce_core/juce_core.h>

#include "Core/MIDI/PresenceCheckPolicy.h"
#include "Core/MIDI/Queue/MidiRequestTiming.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

class PresenceCheckPolicyTests : public juce::UnitTest
{
public:
    PresenceCheckPolicyTests()
        : juce::UnitTest("PresenceCheckPolicy")
    {
    }

    void runTest() override
    {
        testAsciiDisplayCopyContract();
        testNormalizeClampsInvalidToDefault();
        testDetectedIntervalMapping();
        testOnceDisablesDetectedHeartbeat();
        testOnceUnresponsiveUsesRecoveryCadence();
        testPeriodicIntervalsIgnoreUnresponsiveFlag();
        testMonitoringIntervalSchedule();
    }

private:
    void testAsciiDisplayCopyContract()
    {
        beginTest("Settings PRESENCE CHECK - ASCII label, items, default EVERY 15 S");

        expectEquals(juce::String(PluginDisplayNames::Settings::kPresenceCheckLabel),
                     juce::String("PRESENCE CHECK"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kPresenceCheckOnceOnConnect),
                     juce::String("ONCE ON CONNECT"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kPresenceCheckEvery5s),
                     juce::String("EVERY 5 SECONDS"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kPresenceCheckEvery15s),
                     juce::String("EVERY 15 SECONDS"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kPresenceCheckEvery30s),
                     juce::String("EVERY 30 SECONDS"));
        expectEquals(PluginIDs::Settings::PresenceCheck::kDefault,
                     PluginIDs::Settings::PresenceCheck::kEvery15s);

        const auto help = juce::String(PluginDisplayNames::Settings::ContextualHelp::kPresenceCheck);
        expect(help.contains("Matrix-6/6R"));
        expect(help.contains("ONCE ON CONNECT"));
        expect(! help.containsChar(static_cast<juce::juce_wchar>(0x2014)));
    }

    void testNormalizeClampsInvalidToDefault()
    {
        beginTest("normalize - invalid ids clamp to EVERY 15 S");

        expectEquals(Core::PresenceCheckPolicy::normalize(0),
                     PluginIDs::Settings::PresenceCheck::kDefault);
        expectEquals(Core::PresenceCheckPolicy::normalize(99),
                     PluginIDs::Settings::PresenceCheck::kDefault);
        expectEquals(Core::PresenceCheckPolicy::normalize(
                         PluginIDs::Settings::PresenceCheck::kOnceOnConnect),
                     PluginIDs::Settings::PresenceCheck::kOnceOnConnect);
    }

    void testDetectedIntervalMapping()
    {
        beginTest("presenceCheckDetectedIntervalMs - EVERY N S maps to N000 ms");

        using namespace PluginIDs::Settings::PresenceCheck;

        expectEquals(Core::PresenceCheckPolicy::presenceCheckDetectedIntervalMs(kEvery5s),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatDetectedMs);
        expectEquals(Core::PresenceCheckPolicy::presenceCheckDetectedIntervalMs(kEvery15s),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatEvery15sMs);
        expectEquals(Core::PresenceCheckPolicy::presenceCheckDetectedIntervalMs(kEvery30s),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatEvery30sMs);
        expectEquals(Core::PresenceCheckPolicy::presenceCheckDetectedIntervalMs(0),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatEvery15sMs);
    }

    void testOnceDisablesDetectedHeartbeat()
    {
        beginTest("ONCE ON CONNECT - detected interval is 0 when responsive");

        using namespace PluginIDs::Settings::PresenceCheck;

        expectEquals(Core::PresenceCheckPolicy::presenceCheckDetectedIntervalMs(kOnceOnConnect), 0);
        expectEquals(Core::PresenceCheckPolicy::detectedPresenceTimerIntervalMs(kOnceOnConnect, false),
                     0);
    }

    void testOnceUnresponsiveUsesRecoveryCadence()
    {
        beginTest("ONCE + unresponsive - recovery uses EVERY 15 S cadence");

        using namespace PluginIDs::Settings::PresenceCheck;

        expectEquals(Core::PresenceCheckPolicy::detectedPresenceTimerIntervalMs(kOnceOnConnect, true),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatEvery15sMs);
    }

    void testPeriodicIntervalsIgnoreUnresponsiveFlag()
    {
        beginTest("EVERY N S - unresponsive does not change mapped interval");

        using namespace PluginIDs::Settings::PresenceCheck;

        expectEquals(Core::PresenceCheckPolicy::detectedPresenceTimerIntervalMs(kEvery5s, true),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatDetectedMs);
        expectEquals(Core::PresenceCheckPolicy::detectedPresenceTimerIntervalMs(kEvery30s, false),
                     Core::MidiRequestTiming::kDevicePresenceHeartbeatEvery30sMs);
    }

    void testMonitoringIntervalSchedule()
    {
        beginTest("monitoringIntervalMs - ports / undetected / EVERY 15 S / ONCE paths");

        using namespace PluginIDs::Settings::PresenceCheck;
        using Core::PresenceCheckPolicy::monitoringIntervalMs;
        using namespace Core::MidiRequestTiming;

        expectEquals(monitoringIntervalMs(false, true, kEvery15s, false), 0);
        expectEquals(monitoringIntervalMs(true, false, kOnceOnConnect, false),
                     kDevicePresenceRetryUndetectedMs);
        expectEquals(monitoringIntervalMs(true, true, kEvery15s, false),
                     kDevicePresenceHeartbeatEvery15sMs);
        expectEquals(monitoringIntervalMs(true, true, kOnceOnConnect, false), 0);
        expectEquals(monitoringIntervalMs(true, true, kOnceOnConnect, true),
                     kDevicePresenceHeartbeatEvery15sMs);
    }
};

static PresenceCheckPolicyTests presenceCheckPolicyTests;
