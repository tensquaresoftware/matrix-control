#include <algorithm>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Core/Services/GettingStartedMachineDefaults.h"
#include "GUI/Dialogs/GettingStartedWizardFlow.h"
#include "GUI/Dialogs/GettingStartedWizardMetrics.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using GettingStartedWizard::NavButton;
using GettingStartedWizard::Step;

class GettingStartedWizardContractTests : public juce::UnitTest
{
public:
    GettingStartedWizardContractTests()
        : juce::UnitTest("GettingStartedWizardContract")
    {
    }

    void runTest() override
    {
        geometryMatchesSettingsWidthAndStaysLower();
        titlesAreFrozenAsciiBandStrings();
        bodyCopyIsFrozenPerStepAndFormat();
        buttonSetsMatchJourneyMatrix();
        forbiddenLabelsNeverAppear();
        navigationWalksApplicableStepsOnly();
        lastApplicableStepUsesFinish();
        navButtonsResolveTargetSteps();
        closeSemanticsAreDismissOnly();
        runSetupAgainOpensIntroWithoutChangingPreference();
    }

private:
    static constexpr Step kAllSteps[] = {
        Step::kIntro, Step::kUserInterface, Step::kSynthCommunication, Step::kMidiKeyboard, Step::kAudio,
    };

    static juce::String labels(const std::vector<NavButton>& buttons)
    {
        juce::StringArray names;
        for (const auto button : buttons)
            names.add(GettingStartedWizard::labelFor(button));

        return names.joinIntoString(" | ");
    }

    static bool isAscii(const juce::String& text)
    {
        for (auto c : text)
        {
            if (c < 0x09 || c > 0x7e)
                return false;
        }

        return true;
    }

    void geometryMatchesSettingsWidthAndStaysLower()
    {
        beginTest("Geometry - Settings width, per-step height below Settings");

        expectEquals(GettingStartedWizardMetrics::kDesignWidth, SettingsShellMetrics::kDesignWidth);

        for (const bool isPluginMode : { true, false })
        {
            const int settingsHeight = GettingStartedWizardMetrics::settingsDialogDesignHeight(isPluginMode);
            for (const auto step : kAllSteps)
            {
                if (! GettingStartedWizard::isApplicable(step, isPluginMode))
                    continue;

                const int height = GettingStartedWizardMetrics::dialogDesignHeight(step, isPluginMode);
                expect(height < settingsHeight,
                       "step " + juce::String(static_cast<int>(step)) + " height " + juce::String(height)
                           + " must be below Settings " + juce::String(settingsHeight));
                expect(height > 0);
            }
        }
    }

    void titlesAreFrozenAsciiBandStrings()
    {
        beginTest("Titles - title band strings per step (ASCII)");

        expectEquals(juce::String(GettingStartedWizard::titleFor(Step::kIntro)),
                     juce::String("GETTING STARTED"));
        expectEquals(juce::String(GettingStartedWizard::titleFor(Step::kUserInterface)),
                     juce::String("GETTING STARTED - STEP 1: USER INTERFACE"));
        expectEquals(juce::String(GettingStartedWizard::titleFor(Step::kSynthCommunication)),
                     juce::String("GETTING STARTED - STEP 2: SYNTH COMMUNICATION"));
        expectEquals(juce::String(GettingStartedWizard::titleFor(Step::kMidiKeyboard)),
                     juce::String("GETTING STARTED - STEP 3: MIDI KEYBOARD"));
        expectEquals(juce::String(GettingStartedWizard::titleFor(Step::kAudio)),
                     juce::String("GETTING STARTED - STEP 4: AUDIO"));

        for (const auto step : kAllSteps)
        {
            const juce::String title(GettingStartedWizard::titleFor(step));
            expect(isAscii(title));
            expect(! title.contains(" :"));
        }
    }

    void bodyCopyIsFrozenPerStepAndFormat()
    {
        beginTest("Body copy - ui-copy.md text per step and format");

        expectEquals(juce::String(GettingStartedWizard::bodyFor(Step::kIntro, false)),
                     juce::String("Welcome to Matrix-Control, a modern SysEx editor for the Oberheim "
                                  "Matrix-1000, 6, and 6R synthesizers.\n\n"
                                  "We'll set appearance, MIDI connection, optional keyboard input, and audio "
                                  "monitoring (standalone application only) so you can edit, play, and hear "
                                  "your synth. Continue, or choose Configure later and finish in Settings."));
        expectEquals(juce::String(GettingStartedWizard::bodyFor(Step::kUserInterface, true)),
                     juce::String("Start with UI scale and skin so the next steps stay readable on your "
                                  "screen. You can change these anytime from the logo menu or Settings."));
        expectEquals(juce::String(GettingStartedWizard::bodyFor(Step::kSynthCommunication, false)),
                     juce::String("Select the MIDI ports wired to your synth and the EPROM type installed "
                                  "in it. Wait until the device is recognized when possible - this unlocks "
                                  "reliable editing and timing."));
        expectEquals(juce::String(GettingStartedWizard::bodyFor(Step::kMidiKeyboard, false)),
                     juce::String("If you use a separate MIDI keyboard, choose it here. Matrix-6 owners "
                                  "who play the built-in keys can skip this step."));
        expectEquals(juce::String(GettingStartedWizard::bodyFor(Step::kMidiKeyboard, true)),
                     juce::String("When Matrix-Control runs as a plugin, MIDI notes come from the host. "
                                  "Route your master keyboard on a DAW track (or MIDI input) to "
                                  "Matrix-Control - not in this window. See the user manual for host "
                                  "examples."));
        expectEquals(juce::String(GettingStartedWizard::bodyFor(Step::kAudio, false)),
                     juce::String("Choose the audio interface and input so you can hear your synth in "
                                  "Matrix-Control. Pick the SYNTH FROM channel(s) that carry the synth "
                                  "output."));

        variantCopyIsFrozenAndAllBodiesAreAscii();
    }

    void variantCopyIsFrozenAndAllBodiesAreAscii()
    {
        namespace Copy = PluginDisplayNames::Dialogs::GettingStarted;
        expectEquals(juce::String(Copy::kBodySynthCommunicationSuggestionSuffix),
                     juce::String("\n\nA suggestion is preselected from the reported firmware version "
                                  "when possible."));
        expectEquals(juce::String(Copy::kBodyAudioResume),
                     juce::String("MIDI is already set. One more step: route audio so the standalone "
                                  "application can monitor your synth."));

        for (const bool isPluginMode : { true, false })
        {
            for (const auto step : kAllSteps)
            {
                const juce::String body(GettingStartedWizard::bodyFor(step, isPluginMode));
                expect(isAscii(body));
                expect(body.isNotEmpty());
                // No duplicated step heading in the body: titles live in the title band only.
                expect(! body.contains(GettingStartedWizard::titleFor(step)));
            }
        }
    }

    void buttonSetsMatchJourneyMatrix()
    {
        beginTest("Buttons - journey-and-flags.md button sets per step and format");

        for (const bool isPluginMode : { true, false })
        {
            expectEquals(labels(GettingStartedWizard::buttonsFor(Step::kIntro, isPluginMode)),
                         juce::String("CONFIGURE LATER | CONTINUE"));
            expectEquals(labels(GettingStartedWizard::buttonsFor(Step::kUserInterface, isPluginMode)),
                         juce::String("PREVIOUS | NEXT"));
            expectEquals(labels(GettingStartedWizard::buttonsFor(Step::kSynthCommunication, isPluginMode)),
                         juce::String("PREVIOUS | NEXT"));
        }

        expectEquals(labels(GettingStartedWizard::buttonsFor(Step::kMidiKeyboard, false)),
                     juce::String("PREVIOUS | SKIP | NEXT"));
        expectEquals(labels(GettingStartedWizard::buttonsFor(Step::kMidiKeyboard, true)),
                     juce::String("PREVIOUS | FINISH"));
        expectEquals(labels(GettingStartedWizard::buttonsFor(Step::kAudio, false)),
                     juce::String("PREVIOUS | FINISH"));
    }

    void forbiddenLabelsNeverAppear()
    {
        beginTest("Buttons - no QUIT, SPECIFY LATER or STEP 2 Confirm");

        for (const bool isPluginMode : { true, false })
        {
            for (const auto step : kAllSteps)
            {
                const auto row = labels(GettingStartedWizard::buttonsFor(step, isPluginMode));
                expect(! row.containsIgnoreCase("QUIT"));
                expect(! row.containsIgnoreCase("SPECIFY LATER"));
                expect(! row.containsIgnoreCase("CONFIRM"));
            }
        }

        const auto step2 = labels(GettingStartedWizard::buttonsFor(Step::kSynthCommunication, false));
        expectEquals(step2, juce::String("PREVIOUS | NEXT"));
    }

    void navigationWalksApplicableStepsOnly()
    {
        beginTest("Navigation - plugin skips Audio, Standalone includes it");

        expectEquals(static_cast<int>(GettingStartedWizard::applicableSteps(true).size()), 4);
        expectEquals(static_cast<int>(GettingStartedWizard::applicableSteps(false).size()), 5);
        expect(! GettingStartedWizard::isApplicable(Step::kAudio, true));
        expect(GettingStartedWizard::isApplicable(Step::kAudio, false));

        // Plugin: walk forward from the intro and never land on Audio.
        std::vector<Step> pluginWalk { Step::kIntro };
        while (const auto next = GettingStartedWizard::nextApplicableStep(pluginWalk.back(), true))
            pluginWalk.push_back(*next);
        expect(pluginWalk == std::vector<Step>({ Step::kIntro, Step::kUserInterface,
                                                 Step::kSynthCommunication, Step::kMidiKeyboard }));

        std::vector<Step> standaloneWalk { Step::kIntro };
        while (const auto next = GettingStartedWizard::nextApplicableStep(standaloneWalk.back(), false))
            standaloneWalk.push_back(*next);
        expect(standaloneWalk == std::vector<Step>({ Step::kIntro, Step::kUserInterface,
                                                     Step::kSynthCommunication, Step::kMidiKeyboard,
                                                     Step::kAudio }));

        // Previous mirrors Next; intro has nothing before it.
        expect(! GettingStartedWizard::previousApplicableStep(Step::kIntro, false).has_value());
        expect(GettingStartedWizard::previousApplicableStep(Step::kUserInterface, true) == Step::kIntro);
        expect(GettingStartedWizard::previousApplicableStep(Step::kAudio, false) == Step::kMidiKeyboard);

        // A stale Audio request in plugin mode is coerced, never shown.
        expect(GettingStartedWizard::coerceToApplicableStep(Step::kAudio, true) == Step::kIntro);
        expect(GettingStartedWizard::coerceToApplicableStep(Step::kAudio, false) == Step::kAudio);
    }

    void lastApplicableStepUsesFinish()
    {
        beginTest("Finish - last applicable step per format, Enter maps to the primary button");

        expect(GettingStartedWizard::lastApplicableStep(true) == Step::kMidiKeyboard);
        expect(GettingStartedWizard::lastApplicableStep(false) == Step::kAudio);

        for (const bool isPluginMode : { true, false })
        {
            const auto last = GettingStartedWizard::lastApplicableStep(isPluginMode);
            expect(GettingStartedWizard::primaryButtonFor(last, isPluginMode) == NavButton::kFinish);
            expect(! GettingStartedWizard::nextApplicableStep(last, isPluginMode).has_value());

            // FINISH appears only on the last applicable step.
            for (const auto step : GettingStartedWizard::applicableSteps(isPluginMode))
            {
                const auto row = GettingStartedWizard::buttonsFor(step, isPluginMode);
                const bool hasFinish = std::find(row.begin(), row.end(), NavButton::kFinish) != row.end();
                expect(hasFinish == (step == last), "FINISH only on the last applicable step");
            }
        }

        expect(GettingStartedWizard::primaryButtonFor(Step::kIntro, false) == NavButton::kContinue);
    }

    void closeSemanticsAreDismissOnly()
    {
        beginTest("Close - Configure later and Finish only dismiss; no completion claim");

        expect(GettingStartedWizard::closesWizard(NavButton::kConfigureLater));
        expect(GettingStartedWizard::closesWizard(NavButton::kFinish));
        expect(! GettingStartedWizard::closesWizard(NavButton::kContinue));
        expect(! GettingStartedWizard::closesWizard(NavButton::kPrevious));
        expect(! GettingStartedWizard::closesWizard(NavButton::kNext));
        expect(! GettingStartedWizard::closesWizard(NavButton::kSkip));
    }

    void navButtonsResolveTargetSteps()
    {
        beginTest("Navigation - nav button targets (Continue, Skip, Previous) and close buttons");

        expect(GettingStartedWizard::targetStepForNavButton(Step::kIntro, NavButton::kContinue, false)
               == Step::kUserInterface);
        expect(GettingStartedWizard::targetStepForNavButton(Step::kMidiKeyboard, NavButton::kSkip, false)
               == Step::kAudio);
        expect(GettingStartedWizard::targetStepForNavButton(Step::kUserInterface, NavButton::kPrevious, false)
               == Step::kIntro);
        expect(GettingStartedWizard::targetStepForNavButton(Step::kUserInterface, NavButton::kNext, true)
               == Step::kSynthCommunication);

        for (const bool isPluginMode : { true, false })
        {
            for (const auto step : GettingStartedWizard::applicableSteps(isPluginMode))
            {
                expect(! GettingStartedWizard::targetStepForNavButton(step, NavButton::kConfigureLater, isPluginMode)
                              .has_value());
                expect(! GettingStartedWizard::targetStepForNavButton(step, NavButton::kFinish, isPluginMode)
                              .has_value());
            }
        }
    }

    static juce::File makeTempPrefsFile()
    {
        const auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("matrix-gs-2-tests-" + juce::Uuid().toString());
        dir.createDirectory();
        return dir.getChildFile("prefs.settings");
    }

    void runSetupAgainOpensIntroWithoutChangingPreference()
    {
        beginTest("Run Setup Again - collaborator opens step 0 and leaves auto-open preference unchanged");

        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        const auto prefsFile = makeTempPrefsFile();
        juce::PropertiesFile::Options options;
        options.applicationName = "Matrix-Control-GettingStarted-Test";
        options.filenameSuffix = ".settings";
        options.ignoreCaseOfKeyNames = true;
        juce::PropertiesFile store(prefsFile, options);

        Core::GettingStartedMachineDefaults::writeAutoOpenPreference(store, kNeverAtLaunch);

        std::vector<Step> openedAt;
        // Same collaborator the Settings RUN SETUP AGAIN onClick calls in PluginEditor.
        GettingStartedWizard::runSetupAgain([&openedAt](Step step) { openedAt.push_back(step); });

        expect(openedAt == std::vector<Step>({ Step::kIntro }));
        expect(GettingStartedWizard::runSetupAgainStartStep() == Step::kIntro);
        expectEquals(Core::GettingStartedMachineDefaults::readAutoOpenPreference(store), kNeverAtLaunch);

        prefsFile.getParentDirectory().deleteRecursively();
    }
};

static GettingStartedWizardContractTests gettingStartedWizardContractTests;
