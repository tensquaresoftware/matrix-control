#include <juce_core/juce_core.h>

#include "GUI/Helpers/FooterSeverityBadge.h"
#include "GUI/Helpers/StickyInfoMessagePolicy.h"
#include "Shared/Definitions/PluginIDs.h"

class StickyInfoMessagePolicyTests : public juce::UnitTest
{
public:
    StickyInfoMessagePolicyTests()
        : juce::UnitTest("StickyInfoMessagePolicy")
    {
    }

    void runTest() override
    {
        preferenceNormalizesToKeep();
        autoClearGatesInfoOnly();
        remainingDelayNeverNegative();
        pauseResumeAndFireGates();
        timerFireWhileHelpDefersClear();
        badgeHitAreaAndStableSquare();
        autoClearDurationIsFiveSeconds();
    }

private:
    void preferenceNormalizesToKeep()
    {
        beginTest("Preference - KEEP / AUTO CLEAR normalize; invalid -> KEEP");

        using namespace PluginIDs::Settings::InfoMessage;

        expectEquals(TSS::normalizeInfoMessagePreference(kKeep), kKeep);
        expectEquals(TSS::normalizeInfoMessagePreference(kAutoClear), kAutoClear);
        expectEquals(TSS::normalizeInfoMessagePreference(0), kDefault);
        expectEquals(TSS::normalizeInfoMessagePreference(-1), kDefault);
        expectEquals(TSS::normalizeInfoMessagePreference(99), kDefault);
        expectEquals(kDefault, kKeep);
        expect(! TSS::isInfoMessageAutoClearEnabled(kKeep));
        expect(TSS::isInfoMessageAutoClearEnabled(kAutoClear));
        expect(! TSS::isInfoMessageAutoClearEnabled(0));
    }

    void autoClearGatesInfoOnly()
    {
        beginTest("AUTO CLEAR - INFO only; WARNING / ERROR never");

        using namespace PluginIDs::Settings::InfoMessage;
        using TSS::StickyMessageSeverity;

        expect(TSS::shouldAutoClearStickySeverity(StickyMessageSeverity::Info, kAutoClear));
        expect(! TSS::shouldAutoClearStickySeverity(StickyMessageSeverity::Warning, kAutoClear));
        expect(! TSS::shouldAutoClearStickySeverity(StickyMessageSeverity::Error, kAutoClear));
        expect(! TSS::shouldAutoClearStickySeverity(StickyMessageSeverity::Info, kKeep));
        expect(! TSS::shouldAutoClearStickySeverity(StickyMessageSeverity::None, kAutoClear));
        expect(! TSS::shouldAutoClearStickySeverity(StickyMessageSeverity::Info, 99));
    }

    void remainingDelayNeverNegative()
    {
        beginTest("Remaining delay - subtract elapsed; clamp at zero");

        expectEquals(TSS::remainingAutoClearDelayMs(5000, 1200), 3800);
        expectEquals(TSS::remainingAutoClearDelayMs(5000, 5000), 0);
        expectEquals(TSS::remainingAutoClearDelayMs(5000, 9000), 0);
        expectEquals(TSS::remainingAutoClearDelayMs(5000, -10), 5000);
    }

    void pauseResumeAndFireGates()
    {
        beginTest("Pause / resume / fire - HELP cover gates");

        expect(TSS::shouldPauseAutoClearForHelp(true, true));
        expect(! TSS::shouldPauseAutoClearForHelp(true, false));
        expect(! TSS::shouldPauseAutoClearForHelp(false, true));

        expect(TSS::shouldResumeAutoClearAfterHelp(true, true, false, 2500));
        expect(! TSS::shouldResumeAutoClearAfterHelp(true, true, true, 2500));
        expect(! TSS::shouldResumeAutoClearAfterHelp(true, false, false, 2500));
        expect(! TSS::shouldResumeAutoClearAfterHelp(true, true, false, 0));

        expect(TSS::shouldFireAutoClear(0, false));
        expect(! TSS::shouldFireAutoClear(0, true));
        expect(! TSS::shouldFireAutoClear(100, false));
    }

    void timerFireWhileHelpDefersClear()
    {
        beginTest("Timer fire + HELP - pause remaining 0; do not clear yet");

        const auto deferred = TSS::autoClearTimerFireWhileHelpCovers(true, true);
        expect(deferred.deferred);
        expect(deferred.paused);
        expectEquals(deferred.remainingMs, 0);

        expect(! TSS::autoClearTimerFireWhileHelpCovers(true, false).deferred);
        expect(! TSS::autoClearTimerFireWhileHelpCovers(false, true).deferred);
        expect(! TSS::autoClearTimerFireWhileHelpCovers(false, false).deferred);
    }

    void badgeHitAreaAndStableSquare()
    {
        beginTest("Badge hit area - sticky only when HELP not covering; square width stable");

        expect(TSS::shouldShowStickySeverityBadgeHitArea(true, false));
        expect(! TSS::shouldShowStickySeverityBadgeHitArea(true, true));
        expect(! TSS::shouldShowStickySeverityBadgeHitArea(false, false));

        const juce::Rectangle<int> band { 10, 20, 200, 18 };
        const auto square = TSS::severityBadgeSquareBounds(band, 14);
        expectEquals(square.getWidth(), 14);
        expectEquals(square.getHeight(), 14);
        expectEquals(square.getX(), 10);
        // Hover does not participate in bounds — layout stays fixed for message text.
        const auto squareAgain = TSS::severityBadgeSquareBounds(band, 14);
        expectEquals(squareAgain.getX(), square.getX());
        expectEquals(squareAgain.getY(), square.getY());
        expectEquals(squareAgain.getWidth(), square.getWidth());
        expectEquals(squareAgain.getHeight(), square.getHeight());
    }

    void autoClearDurationIsFiveSeconds()
    {
        beginTest("AUTO CLEAR duration - fixed 5000 ms");

        expectEquals(PluginIDs::Settings::InfoMessage::kAutoClearDurationMs, 5000);
    }
};

static StickyInfoMessagePolicyTests stickyInfoMessagePolicyTests;
