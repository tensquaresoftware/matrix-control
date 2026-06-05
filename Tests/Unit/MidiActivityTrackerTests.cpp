#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"

class MidiActivityTrackerTests : public juce::UnitTest
{
public:
    MidiActivityTrackerTests() : juce::UnitTest("MidiActivityTracker") {}

    void runTest() override
    {
        testNotifySetsLevelToOne();
        testPathsAreIndependent();
        testOutboundPathIsIndependent();
        testHoldThenDecayTowardZero();
        testRapidRenotifyExtendsHold();
    }

private:
    void testNotifySetsLevelToOne()
    {
        beginTest("notifyActivity sets level to 1.0 immediately");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kInstrument);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 0.0f);
    }

    void testPathsAreIndependent()
    {
        beginTest("Instrument and editor paths are independent");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kEditor);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 0.0f);
    }

    void testOutboundPathIsIndependent()
    {
        beginTest("Outbound path is independent of instrument and editor paths");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 0.0f);
    }

    void testHoldThenDecayTowardZero()
    {
        beginTest("Level decays toward zero after hold window");

        constexpr juce::int64 kBaseMs = 1'000'000;
        const juce::int64 holdEnd = kBaseMs + 150;
        const juce::int64 midDecay = kBaseMs + 150 + 175;
        const juce::int64 decayEnd = kBaseMs + 150 + 350;

        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, kBaseMs), 1.0f);
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, holdEnd), 1.0f);

        const float midLevel = Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, midDecay);
        expect(midLevel > 0.0f && midLevel < 1.0f);

        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, decayEnd), 0.0f);
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, decayEnd + 100), 0.0f);
    }

    void testRapidRenotifyExtendsHold()
    {
        beginTest("Re-notify within hold keeps level at 1.0");

        constexpr juce::int64 kFirstNotifyMs = 1'000'000;
        const juce::int64 kRenotifyMs = kFirstNotifyMs + 50;
        const juce::int64 kCheckMs = kRenotifyMs + 50;

        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kFirstNotifyMs, kCheckMs), 1.0f);
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kRenotifyMs, kCheckMs), 1.0f);

        const juce::int64 kPastHoldMs = kFirstNotifyMs + 200;
        expect(Core::MidiActivityTracker::computeLevelFromTimestamp(kFirstNotifyMs, kPastHoldMs) < 1.0f);
    }
};

static MidiActivityTrackerTests midiActivityTrackerTests;
