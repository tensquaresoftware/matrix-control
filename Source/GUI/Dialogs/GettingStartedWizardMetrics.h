#pragma once

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Dialogs/GettingStartedWizardFlow.h"
#include "GUI/Settings/SettingsShellMetrics.h"

/** GETTING STARTED geometry (design px at 100% UI scale).
    Width matches Settings. Planning floors keep most steps below the Settings dialog; runtime
    body height follows measured copy and may grow (STEP 2 with firmware suggestion, STEP 4 Audio).
    Per-step height = chrome + gap + body text + optional reserved control band + buttons. */
namespace GettingStartedWizardMetrics
{
    // Same design width as Settings (never a second width SSOT).
    inline constexpr int kDesignWidth = SettingsShellMetrics::kDesignWidth;

    // Body help text planning floors (design px) for dialogDesignHeight / Settings ceiling checks.
    // Runtime body height follows measured copy so short variants do not pad a second blank above
    // the control band. Non-intro steps: the single visual gap is DialogMatrixHelpers::kGapBeforeButtons.
    // Intro runtime uses a taller air + rule + air band (bandHeightOverride); planning below still
    // counts only kGapBeforeButtons for the intro gap (pilot divergence, no formula rewrite yet).
    // Intro: three paragraphs with two blank separators; floor covers wrap at Settings-width inset.
    inline constexpr int kBodyIntroDesignHeight = 156;
    inline constexpr int kBodyUserInterfaceDesignHeight = 46;
    inline constexpr int kBodySynthCommunicationDesignHeight = 58;
    inline constexpr int kBodyMidiKeyboardStandaloneDesignHeight = 46;
    inline constexpr int kBodyMidiKeyboardPluginDesignHeight = 58;
    inline constexpr int kBodyAudioDesignHeight = 58;

    // Control rows per step. STEP 4 Audio = Settings AUDIO row count (kTallestPageRows SSOT).
    inline constexpr int kRowsUserInterface = 2;
    inline constexpr int kRowsSynthCommunication = 4;
    inline constexpr int kRowsMidiKeyboardStandalone = 1;
    inline constexpr int kRowsAudio = SettingsShellMetrics::kTallestPageRows;
    /** Extra body wrap budget when STEP 4 may grow past the Settings-height ceiling. */
    inline constexpr int kAudioBodyWrapSlackDesignPx = 240;

    inline constexpr int bodyDesignHeight(GettingStartedWizard::Step step, bool isPluginMode) noexcept
    {
        using GettingStartedWizard::Step;
        switch (step)
        {
            case Step::kIntro: return kBodyIntroDesignHeight;
            case Step::kUserInterface: return kBodyUserInterfaceDesignHeight;
            case Step::kSynthCommunication: return kBodySynthCommunicationDesignHeight;
            case Step::kMidiKeyboard:
                return isPluginMode ? kBodyMidiKeyboardPluginDesignHeight
                                    : kBodyMidiKeyboardStandaloneDesignHeight;
            case Step::kAudio: return kBodyAudioDesignHeight;
        }

        return 0;
    }

    inline constexpr int reservedControlRows(GettingStartedWizard::Step step, bool isPluginMode) noexcept
    {
        using GettingStartedWizard::Step;
        switch (step)
        {
            case Step::kIntro: return 0;
            case Step::kUserInterface: return kRowsUserInterface;
            case Step::kSynthCommunication: return kRowsSynthCommunication;
            case Step::kMidiKeyboard: return isPluginMode ? 0 : kRowsMidiKeyboardStandalone;
            case Step::kAudio: return kRowsAudio;
        }

        return 0;
    }

    inline constexpr int reservedControlBandDesignHeight(GettingStartedWizard::Step step,
                                                         bool isPluginMode) noexcept
    {
        const int rows = reservedControlRows(step, isPluginMode);
        if (rows <= 0)
            return 0;

        return rows * SettingsShellMetrics::kControlHeight + (rows - 1) * SettingsShellMetrics::kRowGap;
    }

    /** Full dialog height including border and title band (planning mirror of computeModalGeometry
        for non-intro steps). Intro runtime band is taller (air + rule + air); this formula still
        uses kGapBeforeButtons only for the intro gap. */
    inline constexpr int dialogDesignHeight(GettingStartedWizard::Step step, bool isPluginMode) noexcept
    {
        namespace Helpers = DialogMatrixHelpers;
        const int band = reservedControlBandDesignHeight(step, isPluginMode);
        const int bandWithGaps = band > 0 ? Helpers::kGapBeforeButtons + band + Helpers::kGapBeforeButtons
                                          : Helpers::kGapBeforeButtons;
        const int content = Helpers::kGapAfterTitle + bodyDesignHeight(step, isPluginMode) + bandWithGaps
                            + Helpers::kDefaultButtonHeight + Helpers::kButtonBottomMargin;
        return content + Helpers::kTitleBarHeight + Helpers::kBorderThickness * 2;
    }

    /** Settings dialog height (tallest page). Non-Audio wizard steps stay below this. */
    inline int settingsDialogDesignHeight(bool isPluginMode) noexcept
    {
        return SettingsShellMetrics::paddedBodyDesignHeight(isPluginMode)
               + DialogMatrixHelpers::kTitleBarHeight + DialogMatrixHelpers::kBorderThickness * 2;
    }

    /** STEP 4 embeds the full Settings AUDIO page; dialog may exceed Settings height. */
    inline constexpr bool mayExceedSettingsDialogHeight(GettingStartedWizard::Step step) noexcept
    {
        return step == GettingStartedWizard::Step::kAudio;
    }

    /** Max body design height. Non-Audio steps stay strictly below Settings dialog height. */
    inline int maxBodyDesignHeightBelowSettings(GettingStartedWizard::Step step,
                                                bool isPluginMode) noexcept
    {
        if (mayExceedSettingsDialogHeight(step))
            return bodyDesignHeight(step, isPluginMode) + kAudioBodyWrapSlackDesignPx;

        namespace Helpers = DialogMatrixHelpers;
        const int band = reservedControlBandDesignHeight(step, isPluginMode);
        const int bandWithGaps = band > 0
            ? Helpers::kGapBeforeButtons + band + Helpers::kGapBeforeButtons
            : Helpers::kGapBeforeButtons;
        const int chrome = Helpers::kGapAfterTitle + bandWithGaps + Helpers::kDefaultButtonHeight
                           + Helpers::kButtonBottomMargin + Helpers::kTitleBarHeight
                           + Helpers::kBorderThickness * 2;
        return juce::jmax(0, settingsDialogDesignHeight(isPluginMode) - 1 - chrome);
    }
}
