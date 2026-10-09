#pragma once

#include <optional>

#include <juce_data_structures/juce_data_structures.h>

#include "Shared/Definitions/PluginIDs.h"

namespace Core::GettingStartedMachineDefaults
{
    /** Machine-scoped Getting Started auto-open preference (Settings User Interface). */
    int readAutoOpenPreference(const juce::PropertiesFile& store);
    void writeAutoOpenPreference(juce::PropertiesFile& store, int autoOpenId);

    int loadAutoOpenPreference();
    void writeAutoOpenPreference(int autoOpenId);

    /** Configure-later arm stored in machine prefs. */
    enum class ConfigureLaterArm
    {
        kNormal = 0,
        kOneReminder = 1,
        kSilenced = 2,
    };

    inline constexpr ConfigureLaterArm normalizeConfigureLaterArm(int raw) noexcept
    {
        if (raw == static_cast<int>(ConfigureLaterArm::kOneReminder)
            || raw == static_cast<int>(ConfigureLaterArm::kSilenced))
            return static_cast<ConfigureLaterArm>(raw);

        return ConfigureLaterArm::kNormal;
    }

    /** Durable per-step completion flags (Audio only meaningful when Standalone).
        Step indices match GettingStartedWizard::Step (1 UI, 2 Synth, 3 Keyboard, 4 Audio). */
    struct StepFlags
    {
        bool userInterfaceDone = false;
        bool synthCommunicationDone = false;
        bool midiKeyboardDone = false;
        bool audioDone = false;
    };

    struct WizardPrefs
    {
        int autoOpenPreference = PluginIDs::Settings::GettingStartedAutoOpen::kDefault;
        StepFlags flags;
        ConfigureLaterArm configureLaterArm = ConfigureLaterArm::kNormal;
        /** Format that last armed Configure-later (OneReminder or Silenced): true = plugin. */
        bool lastSilencedWasPlugin = false;
        /** False until Continue (or migration); auto-open uses intro while false. */
        bool hasLeftIntro = false;
    };

    StepFlags readStepFlags(const juce::PropertiesFile& store);
    void writeStepFlags(juce::PropertiesFile& store, const StepFlags& flags);

    ConfigureLaterArm readConfigureLaterArm(const juce::PropertiesFile& store);
    void writeConfigureLaterArm(juce::PropertiesFile& store, ConfigureLaterArm arm);

    bool readLastSilencedWasPlugin(const juce::PropertiesFile& store);
    void writeLastSilencedWasPlugin(juce::PropertiesFile& store, bool wasPlugin);

    WizardPrefs readWizardPrefs(const juce::PropertiesFile& store);
    void writeWizardPrefs(juce::PropertiesFile& store, const WizardPrefs& prefs);

    /**
     * Migrate legacy Device Setup promptDone into Synth Communication complete.
     * Other step flags stay incomplete when keys were missing.
     */
    void migrateLegacyPromptDone(juce::PropertiesFile& store,
                                 bool sessionPromptDone,
                                 bool machinePromptDone);

    /** Reset flags applicable to the current format (plugin skips Audio). */
    inline StepFlags resetApplicableFlags(StepFlags current, bool isPluginMode) noexcept
    {
        current.userInterfaceDone = false;
        current.synthCommunicationDone = false;
        current.midiKeyboardDone = false;
        if (! isPluginMode)
            current.audioDone = false;
        return current;
    }

    inline bool isContentStepApplicable(int stepIndex, bool isPluginMode) noexcept
    {
        if (stepIndex < 1 || stepIndex > 4)
            return false;
        if (stepIndex == 4)
            return ! isPluginMode;
        return true;
    }

    inline bool isContentStepDone(const StepFlags& flags, int stepIndex) noexcept
    {
        switch (stepIndex)
        {
            case 1: return flags.userInterfaceDone;
            case 2: return flags.synthCommunicationDone;
            case 3: return flags.midiKeyboardDone;
            case 4: return flags.audioDone;
            default: return true;
        }
    }

    inline void markContentStepDone(StepFlags& flags, int stepIndex) noexcept
    {
        switch (stepIndex)
        {
            case 1: flags.userInterfaceDone = true; break;
            case 2: flags.synthCommunicationDone = true; break;
            case 3: flags.midiKeyboardDone = true; break;
            case 4: flags.audioDone = true; break;
            default: break;
        }
    }

    /** First incomplete content step index (1–4), or nullopt when all applicable are done. */
    inline std::optional<int> firstIncompleteApplicableStepIndex(const StepFlags& flags,
                                                                 bool isPluginMode) noexcept
    {
        for (int step = 1; step <= 4; ++step)
        {
            if (! isContentStepApplicable(step, isPluginMode))
                continue;
            if (! isContentStepDone(flags, step))
                return step;
        }

        return std::nullopt;
    }

    inline bool hasIncompleteApplicableStep(const StepFlags& flags, bool isPluginMode) noexcept
    {
        return firstIncompleteApplicableStepIndex(flags, isPluginMode).has_value();
    }

    struct AutoOpenDecision
    {
        bool shouldOpen = false;
        int startStepIndex = 0;
        bool consumeOneReminder = false;
    };

    inline AutoOpenDecision decideAutoOpen(const WizardPrefs& prefs, bool isPluginMode) noexcept
    {
        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        AutoOpenDecision decision;
        if (normalize(prefs.autoOpenPreference) != kShowWhenIncomplete)
            return decision;

        const auto incomplete = firstIncompleteApplicableStepIndex(prefs.flags, isPluginMode);
        if (! incomplete.has_value())
            return decision;

        if (prefs.configureLaterArm == ConfigureLaterArm::kSilenced)
            return decision;

        // CAP-5: OneReminder is format-scoped — do not auto-open on the other format.
        if (prefs.configureLaterArm == ConfigureLaterArm::kOneReminder
            && prefs.lastSilencedWasPlugin != isPluginMode)
            return decision;

        decision.shouldOpen = true;
        // True first contact / never Continued → intro (0); otherwise resume at first incomplete.
        decision.startStepIndex = prefs.hasLeftIntro ? *incomplete : 0;
        decision.consumeOneReminder = prefs.configureLaterArm == ConfigureLaterArm::kOneReminder;
        return decision;
    }

    /** Prefs arm after an auto-open decision (OneReminder → Silenced when consumed). */
    inline ConfigureLaterArm configureLaterArmAfterAutoOpen(ConfigureLaterArm current,
                                                            bool consumeOneReminder) noexcept
    {
        return consumeOneReminder ? ConfigureLaterArm::kSilenced : current;
    }

    inline ConfigureLaterArm advanceConfigureLaterArm(ConfigureLaterArm current) noexcept
    {
        if (current == ConfigureLaterArm::kNormal)
            return ConfigureLaterArm::kOneReminder;
        return ConfigureLaterArm::kSilenced;
    }

    /** SHOW WHEN INCOMPLETE clears Configure-later silence; NEVER AT LAUNCH leaves arm unchanged. */
    inline bool clearsConfigureLaterSilenceOnAutoOpenPreference(int autoOpenId) noexcept
    {
        using namespace PluginIDs::Settings::GettingStartedAutoOpen;
        return normalize(autoOpenId) == kShowWhenIncomplete;
    }

    /** Pure RUN SETUP AGAIN reset applied to in-memory prefs (also used by the machine store path). */
    inline WizardPrefs applyRunSetupAgainReset(WizardPrefs prefs, bool isPluginMode) noexcept
    {
        prefs.flags = resetApplicableFlags(prefs.flags, isPluginMode);
        prefs.configureLaterArm = ConfigureLaterArm::kNormal;
        prefs.lastSilencedWasPlugin = false;
        prefs.hasLeftIntro = false;
        return prefs;
    }

    /**
     * When a newly applicable incomplete step appears (e.g. plugin→Standalone Audio),
     * rearm one targeted open even if silenced.
     */
    inline ConfigureLaterArm rearmIfNewlyApplicable(ConfigureLaterArm current,
                                                    bool wasPluginWhenSilenced,
                                                    bool isPluginModeNow,
                                                    const StepFlags& flags) noexcept
    {
        if (current != ConfigureLaterArm::kSilenced)
            return current;

        // Plugin path completed MIDI/UI; first Standalone launch with Audio incomplete.
        if (wasPluginWhenSilenced && ! isPluginModeNow
            && isContentStepApplicable(4, false) && ! flags.audioDone)
            return ConfigureLaterArm::kOneReminder;

        return current;
    }

    WizardPrefs loadWizardPrefs();
    void saveWizardPrefs(const WizardPrefs& prefs);

    /** Migrate + load for editor use. nullopt when the machine store could not be opened. */
    std::optional<WizardPrefs> loadAndMigrate(bool sessionPromptDone, bool machinePromptDone);

    void persistStepFlags(const StepFlags& flags);
    void persistConfigureLaterArm(ConfigureLaterArm arm, bool lastSilencedWasPlugin);
    void persistHasLeftIntro(bool hasLeftIntro);
    void resetForRunSetupAgain(juce::PropertiesFile& store, bool isPluginMode);
    void resetForRunSetupAgain(bool isPluginMode);
}
