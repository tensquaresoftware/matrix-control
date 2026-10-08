#include "Core/Services/GettingStartedMachineDefaults.h"

#include "Shared/ProjectPaths.h"

namespace Core::GettingStartedMachineDefaults
{
    namespace
    {
        juce::PropertiesFile::Options makeStoreOptions()
        {
            auto options = ProjectPaths::makeProductPropertiesFileOptions(
                "Matrix-Control-GettingStarted");
            options.ignoreCaseOfKeyNames = true;
            return options;
        }

        std::unique_ptr<juce::PropertiesFile> openStore()
        {
            const auto dir = ProjectPaths::getApplicationDataDirectory();
            if (! dir.createDirectory() && ! dir.isDirectory())
                return nullptr;

            return std::make_unique<juce::PropertiesFile>(makeStoreOptions());
        }
    }

    int readAutoOpenPreference(const juce::PropertiesFile& store)
    {
        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        return normalize(store.getIntValue(PluginIDs::MachineDefaults::kGettingStartedAutoOpen,
                                           kDefault));
    }

    void writeAutoOpenPreference(juce::PropertiesFile& store, int autoOpenId)
    {
        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        store.setValue(PluginIDs::MachineDefaults::kGettingStartedAutoOpen, normalize(autoOpenId));
        store.saveIfNeeded();
    }

    int loadAutoOpenPreference()
    {
        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        auto store = openStore();
        if (store == nullptr)
            return kDefault;

        return readAutoOpenPreference(*store);
    }

    void writeAutoOpenPreference(int autoOpenId)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        writeAutoOpenPreference(*store, autoOpenId);
    }

    StepFlags readStepFlags(const juce::PropertiesFile& store)
    {
        return {
            .userInterfaceDone = store.getBoolValue(
                PluginIDs::MachineDefaults::kGettingStartedUserInterfaceDone, false),
            .synthCommunicationDone = store.getBoolValue(
                PluginIDs::MachineDefaults::kGettingStartedSynthCommunicationDone, false),
            .midiKeyboardDone = store.getBoolValue(
                PluginIDs::MachineDefaults::kGettingStartedMidiKeyboardDone, false),
            .audioDone = store.getBoolValue(
                PluginIDs::MachineDefaults::kGettingStartedAudioDone, false),
        };
    }

    void writeStepFlags(juce::PropertiesFile& store, const StepFlags& flags)
    {
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedUserInterfaceDone,
                       flags.userInterfaceDone);
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedSynthCommunicationDone,
                       flags.synthCommunicationDone);
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedMidiKeyboardDone,
                       flags.midiKeyboardDone);
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedAudioDone, flags.audioDone);
        store.saveIfNeeded();
    }

    ConfigureLaterArm readConfigureLaterArm(const juce::PropertiesFile& store)
    {
        return normalizeConfigureLaterArm(store.getIntValue(
            PluginIDs::MachineDefaults::kGettingStartedConfigureLaterArm,
            static_cast<int>(ConfigureLaterArm::kNormal)));
    }

    void writeConfigureLaterArm(juce::PropertiesFile& store, ConfigureLaterArm arm)
    {
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedConfigureLaterArm,
                       static_cast<int>(arm));
        store.saveIfNeeded();
    }

    bool readLastSilencedWasPlugin(const juce::PropertiesFile& store)
    {
        return store.getBoolValue(
            PluginIDs::MachineDefaults::kGettingStartedLastSilencedWasPlugin, false);
    }

    void writeLastSilencedWasPlugin(juce::PropertiesFile& store, bool wasPlugin)
    {
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedLastSilencedWasPlugin, wasPlugin);
        store.saveIfNeeded();
    }

    WizardPrefs readWizardPrefs(const juce::PropertiesFile& store)
    {
        return {
            .autoOpenPreference = readAutoOpenPreference(store),
            .flags = readStepFlags(store),
            .configureLaterArm = readConfigureLaterArm(store),
            .lastSilencedWasPlugin = readLastSilencedWasPlugin(store),
            .hasLeftIntro = store.getBoolValue(
                PluginIDs::MachineDefaults::kGettingStartedHasLeftIntro, false),
        };
    }

    void writeWizardPrefs(juce::PropertiesFile& store, const WizardPrefs& prefs)
    {
        writeAutoOpenPreference(store, prefs.autoOpenPreference);
        writeStepFlags(store, prefs.flags);
        writeConfigureLaterArm(store, prefs.configureLaterArm);
        writeLastSilencedWasPlugin(store, prefs.lastSilencedWasPlugin);
        store.setValue(PluginIDs::MachineDefaults::kGettingStartedHasLeftIntro, prefs.hasLeftIntro);
        store.saveIfNeeded();
    }

    void migrateLegacyPromptDone(juce::PropertiesFile& store,
                                 bool sessionPromptDone,
                                 bool machinePromptDone)
    {
        using namespace PluginIDs::MachineDefaults;

        const bool synthKeyPresent = store.containsKey(kGettingStartedSynthCommunicationDone);
        if (synthKeyPresent)
            return;

        if (sessionPromptDone || machinePromptDone)
        {
            store.setValue(kGettingStartedSynthCommunicationDone, true);
            // Legacy Device Setup completion implies the user already left any intro.
            store.setValue(kGettingStartedHasLeftIntro, true);
        }

        store.saveIfNeeded();
    }

    WizardPrefs loadWizardPrefs()
    {
        auto store = openStore();
        if (store == nullptr)
            return {};

        return readWizardPrefs(*store);
    }

    void saveWizardPrefs(const WizardPrefs& prefs)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        writeWizardPrefs(*store, prefs);
    }

    std::optional<WizardPrefs> loadAndMigrate(bool sessionPromptDone, bool machinePromptDone)
    {
        auto store = openStore();
        if (store == nullptr)
            return std::nullopt;

        migrateLegacyPromptDone(*store, sessionPromptDone, machinePromptDone);
        return readWizardPrefs(*store);
    }

    void persistStepFlags(const StepFlags& flags)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        writeStepFlags(*store, flags);
    }

    void persistConfigureLaterArm(ConfigureLaterArm arm, bool lastSilencedWasPlugin)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        writeConfigureLaterArm(*store, arm);
        writeLastSilencedWasPlugin(*store, lastSilencedWasPlugin);
    }

    void persistHasLeftIntro(bool hasLeftIntro)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        store->setValue(PluginIDs::MachineDefaults::kGettingStartedHasLeftIntro, hasLeftIntro);
        store->saveIfNeeded();
    }

    void resetForRunSetupAgain(juce::PropertiesFile& store, bool isPluginMode)
    {
        writeWizardPrefs(store, applyRunSetupAgainReset(readWizardPrefs(store), isPluginMode));
    }

    void resetForRunSetupAgain(bool isPluginMode)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        resetForRunSetupAgain(*store, isPluginMode);
    }
}
