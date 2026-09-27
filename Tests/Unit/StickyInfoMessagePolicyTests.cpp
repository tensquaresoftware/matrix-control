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

        using Decision = TSS::AutoClearAfterHelpUncoverDecision;
        expect(TSS::decideAutoClearAfterHelpUncover(true, true, false, 2500)
               == Decision::ResumeRemaining);
        expect(TSS::decideAutoClearAfterHelpUncover(true, true, false, 0)
               == Decision::FireClear);
        expect(TSS::decideAutoClearAfterHelpUncover(true, true, true, 0)
               == Decision::None);
        expect(TSS::decideAutoClearAfterHelpUncover(false, true, false, 0)
               == Decision::None);
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
        beginTest("Badge hit area - square + label width stable; HELP hides hit area");

        expect(TSS::shouldShowStickySeverityBadgeHitArea(true, false));
        expect(! TSS::shouldShowStickySeverityBadgeHitArea(true, true));
        expect(! TSS::shouldShowStickySeverityBadgeHitArea(false, false));

        const juce::Rectangle<int> band { 10, 20, 200, 18 };
        // Combined badge = left uniform inset + square + label padding; hover is not an input.
        const TSS::StickySeverityBadgeMetrics metrics { 12, 2, 40, 4, 16 };
        expectEquals(TSS::stickySeverityIconStripWidth(metrics), 2 + 12);
        expectEquals(TSS::stickySeverityBadgeWidth(metrics), 2 + 12 + 40 + 8);
        const auto badge = TSS::stickySeverityBadgeBounds(band, metrics);
        expectEquals(badge.getWidth(), 2 + 12 + 40 + 8);
        expectEquals(badge.getX(), 10);
        const auto square = TSS::stickySeverityIconSquareBounds(badge, metrics);
        expectEquals(square.getX(), badge.getX() + 2);
        expectEquals(square.getY(), badge.getY() + 2);
        expectEquals(square.getWidth(), 12);
        expectEquals(square.getHeight(), 12);
        const auto badgeAgain = TSS::stickySeverityBadgeBounds(band, metrics);
        expectEquals(badgeAgain.getWidth(), badge.getWidth());
        expectEquals(badgeAgain.getHeight(), badge.getHeight());

        const juce::Rectangle<int> narrowBand { 10, 20, 10, 18 };
        const auto narrowBadge = TSS::stickySeverityBadgeBounds(narrowBand, metrics);
        expectEquals(narrowBadge.getWidth(), 10);
        const auto narrowSquare = TSS::stickySeverityIconSquareBounds(narrowBadge, metrics);
        expect(narrowSquare.getWidth() <= juce::jmax(0, narrowBadge.getWidth() - 4));
        expectEquals(narrowSquare.getWidth(), narrowSquare.getHeight());
    }

    void autoClearDurationIsFiveSeconds()
    {
        beginTest("AUTO CLEAR duration - fixed 5000 ms");

        expectEquals(PluginIDs::Settings::InfoMessage::kAutoClearDurationMs, 5000);
    }
};

static StickyInfoMessagePolicyTests stickyInfoMessagePolicyTests;
