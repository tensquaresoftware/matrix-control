#include "Core/Services/DeviceConnectionMachineDefaults.h"

#include "Shared/ProjectPaths.h"

namespace Core::DeviceConnectionMachineDefaults
{
    namespace
    {
        juce::PropertiesFile::Options makeStoreOptions()
        {
            auto options = ProjectPaths::makeProductPropertiesFileOptions(
                "Matrix-Control-DeviceConnection");
            options.ignoreCaseOfKeyNames = true;
            return options;
        }

        std::unique_ptr<juce::PropertiesFile> openStore()
        {
            ProjectPaths::getApplicationDataDirectory().createDirectory();
            return std::make_unique<juce::PropertiesFile>(makeStoreOptions());
        }

        Values readFromFile(juce::PropertiesFile& file)
        {
            Values values;
            values.promptDone = file.getBoolValue(
                PluginIDs::MachineDefaults::kDeviceSetupPromptDone, false);
            values.midiInputPortId = file.getValue(
                PluginIDs::MachineDefaults::kMidiInputPortId, {});
            values.midiOutputPortId = file.getValue(
                PluginIDs::MachineDefaults::kMidiOutputPortId, {});
            values.epromTypeId = EpromTypePolicy::normalize(file.getIntValue(
                PluginIDs::MachineDefaults::kEpromType,
                PluginIDs::Settings::EpromType::kDefault));
            return values;
        }

        void writeToFile(juce::PropertiesFile& file, const Values& values)
        {
            file.setValue(PluginIDs::MachineDefaults::kDeviceSetupPromptDone, values.promptDone);
            file.setValue(PluginIDs::MachineDefaults::kMidiInputPortId, values.midiInputPortId);
            file.setValue(PluginIDs::MachineDefaults::kMidiOutputPortId, values.midiOutputPortId);
            file.setValue(PluginIDs::MachineDefaults::kEpromType,
                          EpromTypePolicy::normalize(values.epromTypeId));
            file.saveIfNeeded();
        }
    }

    Values load()
    {
        auto store = openStore();
        if (store == nullptr)
            return {};

        return readFromFile(*store);
    }

    void save(const Values& values)
    {
        auto store = openStore();
        if (store == nullptr)
            return;

        writeToFile(*store, values);
    }

    void writeAfterConfirm(int selectedEpromTypeId,
                           const juce::String& midiInputPortId,
                           const juce::String& midiOutputPortId)
    {
        save(applyConfirm(load(), selectedEpromTypeId, midiInputPortId, midiOutputPortId));
    }

    void writeAfterSpecifyLater(const juce::String& midiInputPortId,
                                const juce::String& midiOutputPortId)
    {
        save(applySpecifyLater(load(), midiInputPortId, midiOutputPortId));
    }

    void writeMidiInputPort(const juce::String& midiInputPortId)
    {
        save(applyMidiInputPort(load(), midiInputPortId));
    }

    void writeMidiOutputPort(const juce::String& midiOutputPortId)
    {
        save(applyMidiOutputPort(load(), midiOutputPortId));
    }

    void writeEpromType(int epromTypeId)
    {
        save(applyEpromType(load(), epromTypeId));
    }

    void reseedApvtsAfterHostRestoreIfNeeded(juce::ValueTree& state)
    {
        const auto machineDefaults = load();
        const bool sessionPromptDone = static_cast<bool>(
            state.getProperty(PluginIDs::Settings::kEpromTypePromptDone, false));
        if (shouldReseedAfterHostRestore(machineDefaults.promptDone, sessionPromptDone))
            seedApvtsState(state, machineDefaults);
    }
}
