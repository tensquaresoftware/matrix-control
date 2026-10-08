#pragma once

#include <optional>
#include <vector>

#include <juce_core/juce_core.h>

#include "Core/Services/GettingStartedMachineDefaults.h"
#include "Shared/Definitions/PluginDisplayNames.h"

/** GETTING STARTED step flow: applicable steps, navigation, titles, copy, button sets,
    resume / Configure-later helpers. Pure data and rules, no GUI types.
    (`_bmad-output/specs/spec-getting-started/journey-and-flags.md`, `ui-copy.md`). */
namespace GettingStartedWizard
{
    enum class Step
    {
        kIntro = 0,
        kUserInterface = 1,
        kSynthCommunication = 2,
        kMidiKeyboard = 3,
        kAudio = 4,
    };

    inline constexpr int kStepCount = 5;

    enum class NavButton
    {
        kConfigureLater,
        kContinue,
        kPrevious,
        kNext,
        kSkip,
        kFinish,
    };

    /** Entry step for RUN SETUP AGAIN (GS-3 adds flag reset on top of this). */
    inline constexpr Step runSetupAgainStartStep() noexcept
    {
        return Step::kIntro;
    }

    /** Audio (step 4) is Standalone only; every other step applies to both formats. */
    inline constexpr bool isApplicable(Step step, bool isPluginMode) noexcept
    {
        return step != Step::kAudio || ! isPluginMode;
    }

    inline std::vector<Step> applicableSteps(bool isPluginMode)
    {
        std::vector<Step> steps;
        for (int index = 0; index < kStepCount; ++index)
        {
            const auto step = static_cast<Step>(index);
            if (isApplicable(step, isPluginMode))
                steps.push_back(step);
        }

        return steps;
    }

    inline Step lastApplicableStep(bool isPluginMode)
    {
        return applicableSteps(isPluginMode).back();
    }

    /** Next applicable step after `step`, or nullopt when `step` is already the last one. */
    inline std::optional<Step> nextApplicableStep(Step step, bool isPluginMode)
    {
        for (int index = static_cast<int>(step) + 1; index < kStepCount; ++index)
        {
            const auto candidate = static_cast<Step>(index);
            if (isApplicable(candidate, isPluginMode))
                return candidate;
        }

        return std::nullopt;
    }

    /** Previous applicable step before `step`, or nullopt from the intro. */
    inline std::optional<Step> previousApplicableStep(Step step, bool isPluginMode)
    {
        for (int index = static_cast<int>(step) - 1; index >= 0; --index)
        {
            const auto candidate = static_cast<Step>(index);
            if (isApplicable(candidate, isPluginMode))
                return candidate;
        }

        return std::nullopt;
    }

    /** Falls back to the intro when `step` does not apply to this format. */
    inline Step coerceToApplicableStep(Step step, bool isPluginMode) noexcept
    {
        return isApplicable(step, isPluginMode) ? step : Step::kIntro;
    }

    /** Button row, left to right, as specified in journey-and-flags.md. */
    inline std::vector<NavButton> buttonsFor(Step step, bool isPluginMode)
    {
        switch (step)
        {
            case Step::kIntro:
                return { NavButton::kConfigureLater, NavButton::kContinue };
            case Step::kUserInterface:
            case Step::kSynthCommunication:
                return { NavButton::kPrevious, NavButton::kNext };
            case Step::kMidiKeyboard:
                if (isPluginMode)
                    return { NavButton::kPrevious, NavButton::kFinish };
                return { NavButton::kPrevious, NavButton::kSkip, NavButton::kNext };
            case Step::kAudio:
                return { NavButton::kPrevious, NavButton::kFinish };
        }

        return {};
    }

    inline const char* titleFor(Step step) noexcept
    {
        namespace Copy = PluginDisplayNames::Dialogs::GettingStarted;
        switch (step)
        {
            case Step::kIntro: return Copy::kTitleIntro;
            case Step::kUserInterface: return Copy::kTitleUserInterface;
            case Step::kSynthCommunication: return Copy::kTitleSynthCommunication;
            case Step::kMidiKeyboard: return Copy::kTitleMidiKeyboard;
            case Step::kAudio: return Copy::kTitleAudio;
        }

        return "";
    }

    struct BodyOptions
    {
        bool includeFirmwareSuggestionSuffix = false;
        bool useAudioResumeCopy = false;
    };

    /** Frozen help copy for the step. Firmware suffix and Audio resume are opt-in. */
    inline juce::String bodyFor(Step step, bool isPluginMode, BodyOptions options = {}) 
    {
        namespace Copy = PluginDisplayNames::Dialogs::GettingStarted;
        switch (step)
        {
            case Step::kIntro: return Copy::kBodyIntro;
            case Step::kUserInterface: return Copy::kBodyUserInterface;
            case Step::kSynthCommunication:
            {
                juce::String text(Copy::kBodySynthCommunication);
                if (options.includeFirmwareSuggestionSuffix)
                    text += Copy::kBodySynthCommunicationSuggestionSuffix;
                return text;
            }
            case Step::kMidiKeyboard:
                return isPluginMode ? Copy::kBodyMidiKeyboardPlugin : Copy::kBodyMidiKeyboardStandalone;
            case Step::kAudio:
                return options.useAudioResumeCopy ? Copy::kBodyAudioResume : Copy::kBodyAudioFirstPass;
        }

        return {};
    }

    /** True when Next / Skip / Finish should mark the current content step done. */
    inline constexpr bool marksStepDone(Step step, NavButton button) noexcept
    {
        if (step == Step::kIntro || button == NavButton::kPrevious
            || button == NavButton::kConfigureLater || button == NavButton::kContinue)
            return false;

        if (button == NavButton::kNext || button == NavButton::kSkip || button == NavButton::kFinish)
            return step != Step::kIntro;

        return false;
    }

    inline std::optional<Step> firstIncompleteApplicableStep(
        const Core::GettingStartedMachineDefaults::StepFlags& flags,
        bool isPluginMode) noexcept
    {
        const auto index = Core::GettingStartedMachineDefaults::firstIncompleteApplicableStepIndex(
            flags, isPluginMode);
        if (! index.has_value())
            return std::nullopt;

        return static_cast<Step>(*index);
    }

    /** Resume copy for Audio when UI+Synth+Keyboard are done and Audio is incomplete. */
    inline bool shouldUseAudioResumeCopy(
        const Core::GettingStartedMachineDefaults::StepFlags& flags) noexcept
    {
        return flags.userInterfaceDone && flags.synthCommunicationDone && flags.midiKeyboardDone
            && ! flags.audioDone;
    }

    inline const char* labelFor(NavButton button) noexcept
    {
        namespace Copy = PluginDisplayNames::Dialogs::GettingStarted;
        switch (button)
        {
            case NavButton::kConfigureLater: return Copy::kConfigureLater;
            case NavButton::kContinue: return Copy::kContinue;
            case NavButton::kPrevious: return Copy::kPrevious;
            case NavButton::kNext: return Copy::kNext;
            case NavButton::kSkip: return Copy::kSkip;
            case NavButton::kFinish: return Copy::kFinish;
        }

        return "";
    }

    /** Buttons that leave the wizard instead of moving between steps. */
    inline constexpr bool closesWizard(NavButton button) noexcept
    {
        return button == NavButton::kConfigureLater || button == NavButton::kFinish;
    }

    /** Step a navigation button moves to; nullopt for buttons that close the wizard
        (CONFIGURE LATER / FINISH) or when there is no step in that direction. */
    inline std::optional<Step> targetStepForNavButton(Step step, NavButton button, bool isPluginMode)
    {
        if (closesWizard(button))
            return std::nullopt;

        return button == NavButton::kPrevious ? previousApplicableStep(step, isPluginMode)
                                              : nextApplicableStep(step, isPluginMode);
    }

    /** RUN SETUP AGAIN: reset applicable flags (via caller), then open at the intro. */
    template <typename ResetFlagsFn, typename OpenWizardFn>
    void runSetupAgain(ResetFlagsFn&& resetFlags, OpenWizardFn&& openWizard)
    {
        resetFlags();
        openWizard(runSetupAgainStartStep());
    }

    /** Rightmost (primary) button of the row: what Enter triggers. */
    inline NavButton primaryButtonFor(Step step, bool isPluginMode)
    {
        return buttonsFor(step, isPluginMode).back();
    }
}
