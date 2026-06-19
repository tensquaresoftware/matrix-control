#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/Util/ComboboxPatchSendDebouncer.h"

class ComboboxPatchSendDebouncerTests : public juce::UnitTest
{
public:
    ComboboxPatchSendDebouncerTests() : juce::UnitTest("ComboboxPatchSendDebouncer") {}

    void runTest() override
    {
        debouncer_rapidSchedule_firesOnce();
        debouncer_finalSelectionWins();
    }

private:
    static constexpr int kTestDebounceMs = 20;
    static constexpr int kWaitMarginMs = 30;

    static void waitForDebounceWindow(int totalMs)
    {
        const auto deadline = juce::Time::getMillisecondCounter()
                              + static_cast<juce::uint32>(totalMs);

        while (juce::Time::getMillisecondCounter() < deadline)
        {
            juce::Timer::callPendingTimersSynchronously();
            juce::Thread::sleep(1);
        }
    }

    void debouncer_rapidSchedule_firesOnce()
    {
        beginTest("debouncer_rapidSchedule_firesOnce");

        Core::ComboboxPatchSendDebouncer debouncer(kTestDebounceMs);
        int callbackCount = 0;

        for (int i = 0; i < 5; ++i)
            debouncer.schedule([&callbackCount] { ++callbackCount; });

        waitForDebounceWindow(kTestDebounceMs + kWaitMarginMs);
        expectEquals(callbackCount, 1);
    }

    void debouncer_finalSelectionWins()
    {
        beginTest("debouncer_finalSelectionWins");

        Core::ComboboxPatchSendDebouncer debouncer(kTestDebounceMs);
        char result = '\0';

        debouncer.schedule([&result] { result = 'A'; });
        debouncer.schedule([&result] { result = 'B'; });

        waitForDebounceWindow(kTestDebounceMs + kWaitMarginMs);
        expectEquals(static_cast<int>(result), static_cast<int>('B'));
    }
};

static ComboboxPatchSendDebouncerTests comboboxPatchSendDebouncerTests;
