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
            ProjectPaths::getApplicationDataDirectory().createDirectory();
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
}
