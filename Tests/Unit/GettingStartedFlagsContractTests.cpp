#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Core/Audio/SceneAudioSafety.h"
#include "Core/Services/DeviceSetupDeviceRow.h"
#include "Core/Services/GettingStartedMachineDefaults.h"
#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Dialogs/GettingStartedWizardFlow.h"
#include "GUI/Dialogs/GettingStartedWizardMetrics.h"
#include "Shared/Definitions/PluginIDs.h"

using GettingStartedWizard::NavButton;
using GettingStartedWizard::Step;
using namespace Core::GettingStartedMachineDefaults;

class GettingStartedFlagsContractTests : public juce::UnitTest
{
public:
    GettingStartedFlagsContractTests()
        : juce::UnitTest("GettingStartedFlagsContract")
    {
    }

    void runTest() override
    {
        firstIncompleteAndApplicability();
        markContentStepDoneSetsMatchingFlags();
        autoOpenResumeAndNeverAtLaunch();
        configureLaterPolicyAndRearm();
        showWhenIncompleteClearsConfigureLaterSilence();
        runSetupAgainResetsApplicableFlags();
        runSetupAgainClearsTempStore();
        migrationFromLegacyPromptDone();
        marksStepDoneRules();
        bodyVariantsFirmwareAndAudioResume();
        audioSafetyPropertyIsSeparate();
        absorbDoesNotArmDeviceSetupPendingAfterInquiry();
        synthDoneMarksLegacyDeviceSetupSessionComplete();
        geometryBodyCapStaysBelowSettings();
    }

private:
    static juce::File makeTempPrefsFile()
    {
        const auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("matrix-gs-3-tests-" + juce::Uuid().toString());
        dir.createDirectory();
        return dir.getChildFile("prefs.settings");
    }

    static std::unique_ptr<juce::PropertiesFile> openTempStore(const juce::File& file)
    {
        juce::PropertiesFile::Options options;
        options.applicationName = "Matrix-Control-GettingStarted-Test";
        options.filenameSuffix = ".settings";
        options.ignoreCaseOfKeyNames = true;
        return std::make_unique<juce::PropertiesFile>(file, options);
    }

    void firstIncompleteAndApplicability()
    {
        beginTest("Flags - first incomplete applicable step; plugin skips Audio");

        StepFlags flags;
        expect(firstIncompleteApplicableStepIndex(flags, false) == 1);
        expect(firstIncompleteApplicableStepIndex(flags, true) == 1);

        flags.userInterfaceDone = true;
        expect(GettingStartedWizard::firstIncompleteApplicableStep(flags, false) == Step::kSynthCommunication);

        flags.synthCommunicationDone = true;
        flags.midiKeyboardDone = true;
        expect(GettingStartedWizard::firstIncompleteApplicableStep(flags, false) == Step::kAudio);
        expect(! GettingStartedWizard::firstIncompleteApplicableStep(flags, true).has_value());

        flags.audioDone = true;
        expect(! hasIncompleteApplicableStep(flags, false));
    }

    void autoOpenResumeAndNeverAtLaunch()
    {
        beginTest("Auto-open - resume at first incomplete; NEVER SHOW AT LAUNCH suppresses");

        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        WizardPrefs prefs;
        // True first contact: incomplete work but never Continued → intro.
        expect(decideAutoOpen(prefs, false).shouldOpen);
        expectEquals(decideAutoOpen(prefs, false).startStepIndex, 0);

        prefs.hasLeftIntro = true;
        prefs.flags.userInterfaceDone = true;
        auto decision = decideAutoOpen(prefs, false);
        expect(decision.shouldOpen);
        expectEquals(decision.startStepIndex, 2);

        // Resume at Audio (step 4) when prior steps done and hasLeftIntro.
        prefs.flags.synthCommunicationDone = true;
        prefs.flags.midiKeyboardDone = true;
        prefs.flags.audioDone = false;
        decision = decideAutoOpen(prefs, false);
        expect(decision.shouldOpen);
        expectEquals(decision.startStepIndex, 4);

        prefs.autoOpenPreference = kNeverAtLaunch;
        decision = decideAutoOpen(prefs, false);
        expect(! decision.shouldOpen);

        prefs.autoOpenPreference = kShowWhenIncomplete;
        prefs.flags.audioDone = true;
        decision = decideAutoOpen(prefs, false);
        expect(! decision.shouldOpen);
    }

    void markContentStepDoneSetsMatchingFlags()
    {
        beginTest("Flags - markContentStepDone sets matching step only");

        StepFlags flags;
        markContentStepDone(flags, 1);
        expect(flags.userInterfaceDone);
        expect(! flags.synthCommunicationDone);

        markContentStepDone(flags, 2);
        expect(flags.synthCommunicationDone);
        markContentStepDone(flags, 3);
        expect(flags.midiKeyboardDone);
        markContentStepDone(flags, 4);
        expect(flags.audioDone);

        StepFlags ignored;
        markContentStepDone(ignored, 0);
        markContentStepDone(ignored, 5);
        expect(! ignored.userInterfaceDone);
        expect(! ignored.audioDone);
    }

    void configureLaterPolicyAndRearm()
    {
        beginTest("Configure later - one reminder then silence; newly applicable rearms");

        expect(advanceConfigureLaterArm(ConfigureLaterArm::kNormal) == ConfigureLaterArm::kOneReminder);
        expect(advanceConfigureLaterArm(ConfigureLaterArm::kOneReminder) == ConfigureLaterArm::kSilenced);
        expect(advanceConfigureLaterArm(ConfigureLaterArm::kSilenced) == ConfigureLaterArm::kSilenced);

        WizardPrefs prefs;
        prefs.hasLeftIntro = true;
        prefs.configureLaterArm = ConfigureLaterArm::kOneReminder;
        prefs.lastSilencedWasPlugin = true; // armed in plugin
        prefs.flags.userInterfaceDone = false;
        auto decision = decideAutoOpen(prefs, true);
        expect(decision.shouldOpen);
        expect(decision.consumeOneReminder);
        expect(configureLaterArmAfterAutoOpen(prefs.configureLaterArm, decision.consumeOneReminder)
               == ConfigureLaterArm::kSilenced);

        // Cross-format: OneReminder armed in plugin must not auto-open Standalone.
        decision = decideAutoOpen(prefs, false);
        expect(! decision.shouldOpen);
        expect(! decision.consumeOneReminder);
        expect(configureLaterArmAfterAutoOpen(prefs.configureLaterArm, decision.consumeOneReminder)
               == ConfigureLaterArm::kOneReminder);

        prefs.configureLaterArm = ConfigureLaterArm::kSilenced;
        decision = decideAutoOpen(prefs, true);
        expect(! decision.shouldOpen);

        StepFlags flags;
        flags.userInterfaceDone = true;
        flags.synthCommunicationDone = true;
        flags.midiKeyboardDone = true;
        // Plugin→Standalone with Audio incomplete rearms.
        expect(rearmIfNewlyApplicable(ConfigureLaterArm::kSilenced, true, false, flags)
               == ConfigureLaterArm::kOneReminder);
        expect(rearmIfNewlyApplicable(ConfigureLaterArm::kSilenced, false, false, flags)
               == ConfigureLaterArm::kSilenced);
    }

    void showWhenIncompleteClearsConfigureLaterSilence()
    {
        beginTest("Combo - SHOW WHEN INCOMPLETE clears Configure-later silence");

        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        expect(clearsConfigureLaterSilenceOnAutoOpenPreference(kShowWhenIncomplete));
        expect(! clearsConfigureLaterSilenceOnAutoOpenPreference(kNeverAtLaunch));
        expect(clearsConfigureLaterSilenceOnAutoOpenPreference(0)); // normalize → default SHOW
    }

    void runSetupAgainResetsApplicableFlags()
    {
        beginTest("Run Setup Again - resets applicable flags; plugin keeps Audio untouched");

        StepFlags flags {
            .userInterfaceDone = true,
            .synthCommunicationDone = true,
            .midiKeyboardDone = true,
            .audioDone = true,
        };

        const auto pluginReset = resetApplicableFlags(flags, true);
        expect(! pluginReset.userInterfaceDone);
        expect(! pluginReset.synthCommunicationDone);
        expect(! pluginReset.midiKeyboardDone);
        expect(pluginReset.audioDone); // N/A for plugin — left alone

        const auto standaloneReset = resetApplicableFlags(flags, false);
        expect(! standaloneReset.audioDone);

        std::vector<Step> openedAt;
        bool resetCalled = false;
        GettingStartedWizard::runSetupAgain(
            [&resetCalled] { resetCalled = true; },
            [&openedAt](Step step) { openedAt.push_back(step); });
        expect(resetCalled);
        expect(openedAt == std::vector<Step>({ Step::kIntro }));
    }

    void runSetupAgainClearsTempStore()
    {
        beginTest("Run Setup Again - temp store clears flags, arm, and hasLeftIntro");

        const auto prefsFile = makeTempPrefsFile();
        {
            auto store = openTempStore(prefsFile);
            expect(store != nullptr);
            if (store == nullptr)
                return;

            WizardPrefs seeded {
                .flags = {
                    .userInterfaceDone = true,
                    .synthCommunicationDone = true,
                    .midiKeyboardDone = true,
                    .audioDone = true,
                },
                .configureLaterArm = ConfigureLaterArm::kSilenced,
                .lastSilencedWasPlugin = true,
                .hasLeftIntro = true,
            };
            writeWizardPrefs(*store, seeded);

            resetForRunSetupAgain(*store, false);
            const auto after = readWizardPrefs(*store);
            expect(! after.flags.userInterfaceDone);
            expect(! after.flags.synthCommunicationDone);
            expect(! after.flags.midiKeyboardDone);
            expect(! after.flags.audioDone);
            expect(after.configureLaterArm == ConfigureLaterArm::kNormal);
            expect(! after.lastSilencedWasPlugin);
            expect(! after.hasLeftIntro);
        }

        prefsFile.getParentDirectory().deleteRecursively();
    }

    void migrationFromLegacyPromptDone()
    {
        beginTest("Migration - legacy promptDone marks Synth Communication and hasLeftIntro");

        const auto prefsFile = makeTempPrefsFile();
        {
            auto store = openTempStore(prefsFile);
            expect(store != nullptr);
            if (store == nullptr)
                return;

            migrateLegacyPromptDone(*store, true, false);
            auto prefs = readWizardPrefs(*store);
            expect(prefs.flags.synthCommunicationDone);
            expect(prefs.hasLeftIntro);
            expect(! prefs.flags.userInterfaceDone);
            expect(! prefs.flags.midiKeyboardDone);
            expect(! prefs.flags.audioDone);

            // Machine-only promptDone also migrates.
            store->clear();
            store->saveIfNeeded();
            migrateLegacyPromptDone(*store, false, true);
            prefs = readWizardPrefs(*store);
            expect(prefs.flags.synthCommunicationDone);
            expect(prefs.hasLeftIntro);

            // Both false: leave incomplete / never left intro.
            store->clear();
            store->saveIfNeeded();
            migrateLegacyPromptDone(*store, false, false);
            prefs = readWizardPrefs(*store);
            expect(! prefs.flags.synthCommunicationDone);
            expect(! prefs.hasLeftIntro);

            // Second migrate must not clobber an explicit incomplete synth flag.
            store->clear();
            store->saveIfNeeded();
            migrateLegacyPromptDone(*store, true, false);
            store->setValue(PluginIDs::MachineDefaults::kGettingStartedSynthCommunicationDone, false);
            store->saveIfNeeded();
            migrateLegacyPromptDone(*store, true, true);
            expect(! readStepFlags(*store).synthCommunicationDone);
        }

        prefsFile.getParentDirectory().deleteRecursively();
    }

    void marksStepDoneRules()
    {
        beginTest("Nav - Next/Skip/Finish mark content steps; Configure later does not");

        expect(GettingStartedWizard::marksStepDone(Step::kUserInterface, NavButton::kNext));
        expect(GettingStartedWizard::marksStepDone(Step::kSynthCommunication, NavButton::kNext));
        expect(GettingStartedWizard::marksStepDone(Step::kMidiKeyboard, NavButton::kSkip));
        expect(GettingStartedWizard::marksStepDone(Step::kMidiKeyboard, NavButton::kFinish));
        expect(GettingStartedWizard::marksStepDone(Step::kAudio, NavButton::kFinish));
        expect(! GettingStartedWizard::marksStepDone(Step::kIntro, NavButton::kContinue));
        expect(! GettingStartedWizard::marksStepDone(Step::kIntro, NavButton::kConfigureLater));
        expect(! GettingStartedWizard::marksStepDone(Step::kUserInterface, NavButton::kPrevious));
    }

    void bodyVariantsFirmwareAndAudioResume()
    {
        beginTest("Body - firmware suffix and Audio resume variants");

        const auto synth = GettingStartedWizard::bodyFor(
            Step::kSynthCommunication, false, { .includeFirmwareSuggestionSuffix = true });
        expect(synth.contains("suggestion is preselected"));

        const auto audio = GettingStartedWizard::bodyFor(
            Step::kAudio, false, { .useAudioResumeCopy = true });
        expect(audio.contains("MIDI is already set"));

        StepFlags flags {
            .userInterfaceDone = true,
            .synthCommunicationDone = true,
            .midiKeyboardDone = true,
            .audioDone = false,
        };
        expect(GettingStartedWizard::shouldUseAudioResumeCopy(flags));
    }

    void audioSafetyPropertyIsSeparate()
    {
        beginTest("Audio-safety - wizard keys distinct from sceneAudioSafetyDefaultsApplied");

        expect(juce::String(Core::kSceneAudioSafetyDefaultsAppliedProperty)
               != juce::String(PluginIDs::MachineDefaults::kGettingStartedAudioDone));
        expect(juce::String(PluginIDs::MachineDefaults::kGettingStartedAudioDone)
               != juce::String(PluginIDs::MachineDefaults::kDeviceSetupPromptDone));
    }

    void absorbDoesNotArmDeviceSetupPendingAfterInquiry()
    {
        beginTest("Absorb - inquiry must not re-arm pending; editor attach must not open Device Setup");

        // Product path used by MidiManagerDeviceInquiry after GS-3 absorb.
        expect(! Core::shouldArmDeviceSetupPromptPendingAfterInquiry(false));
        expect(! Core::shouldArmDeviceSetupPromptPendingAfterInquiry(true));
        // Product launch gate at PluginEditor::attachEditorRuntimeListeners.
        expect(! Core::shouldAutoOpenDeviceSetupAtEditorAttach());
    }

    void synthDoneMarksLegacyDeviceSetupSessionComplete()
    {
        beginTest("Absorb - STEP 2 done marks legacy session promptDone and clears pending");

        juce::ValueTree state("SETTINGS");
        state.setProperty(PluginIDs::Settings::kEpromTypePromptDone, false, nullptr);
        state.setProperty(PluginIDs::Settings::kEpromTypePromptPending, true, nullptr);

        Core::markSessionDeviceSetupCompleteAfterWizardSynthDone(state);
        expect(static_cast<bool>(state.getProperty(PluginIDs::Settings::kEpromTypePromptDone)));
        expect(! static_cast<bool>(state.getProperty(PluginIDs::Settings::kEpromTypePromptPending)));
    }

    void geometryBodyCapStaysBelowSettings()
    {
        beginTest("Geometry - body cap keeps non-Audio dialogs below Settings; Audio may exceed");

        for (const bool isPluginMode : { true, false })
        {
            const int settingsHeight =
                GettingStartedWizardMetrics::settingsDialogDesignHeight(isPluginMode);
            for (const auto step : {
                     Step::kIntro, Step::kUserInterface, Step::kSynthCommunication,
                     Step::kMidiKeyboard, Step::kAudio })
            {
                if (! GettingStartedWizard::isApplicable(step, isPluginMode))
                    continue;

                const int maxBody =
                    GettingStartedWizardMetrics::maxBodyDesignHeightBelowSettings(step, isPluginMode);
                const int minBody = GettingStartedWizardMetrics::bodyDesignHeight(step, isPluginMode);
                expect(maxBody >= minBody);

                // Simulate a large measured body: capped height must stay below Settings.
                const int capped = juce::jmin(juce::jmax(minBody, minBody + 80), maxBody);
                // Rebuild dialog height using Metrics formula with capped body.
                namespace Helpers = DialogMatrixHelpers;
                const int band =
                    GettingStartedWizardMetrics::reservedControlBandDesignHeight(step, isPluginMode);
                const int bandWithGaps = band > 0
                    ? Helpers::kGapBeforeButtons + band + Helpers::kGapBeforeButtons
                    : Helpers::kGapBeforeButtons;
                const int content = Helpers::kGapAfterTitle + capped + bandWithGaps
                                    + Helpers::kDefaultButtonHeight + Helpers::kButtonBottomMargin;
                const int dialogH = content + Helpers::kTitleBarHeight
                                    + Helpers::kBorderThickness * 2;
                if (GettingStartedWizardMetrics::mayExceedSettingsDialogHeight(step))
                    expect(dialogH > 0);
                else
                    expect(dialogH < settingsHeight);
            }
        }
    }
};

static GettingStartedFlagsContractTests gettingStartedFlagsContractTests;
