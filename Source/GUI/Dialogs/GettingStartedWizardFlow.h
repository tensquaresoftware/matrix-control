#pragma once

#include <optional>
#include <vector>

#include "Shared/Definitions/PluginDisplayNames.h"

/** GETTING STARTED step flow (GS-2 shell): applicable steps, navigation, titles, copy, button sets.
    Pure data and rules, no GUI types, so unit tests can pin the product contract
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

    /** Frozen help copy for the step. Firmware-suggestion suffix and the Audio resume variant
        depend on live state and flags, so they are chosen by GS-3. */
    inline const char* bodyFor(Step step, bool isPluginMode) noexcept
    {
        namespace Copy = PluginDisplayNames::Dialogs::GettingStarted;
        switch (step)
        {
            case Step::kIntro: return Copy::kBodyIntro;
            case Step::kUserInterface: return Copy::kBodyUserInterface;
            case Step::kSynthCommunication: return Copy::kBodySynthCommunication;
            case Step::kMidiKeyboard:
                return isPluginMode ? Copy::kBodyMidiKeyboardPlugin : Copy::kBodyMidiKeyboardStandalone;
            case Step::kAudio: return Copy::kBodyAudioFirstPass;
        }

        return "";
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

    /** RUN SETUP AGAIN: opens the wizard at the intro. Flag reset is GS-3. */
    template <typename OpenWizardFn>
    void runSetupAgain(OpenWizardFn&& openWizard)
    {
        openWizard(runSetupAgainStartStep());
    }

    /** Rightmost (primary) button of the row: what Enter triggers. */
    inline NavButton primaryButtonFor(Step step, bool isPluginMode)
    {
        return buttonsFor(step, isPluginMode).back();
    }
}
