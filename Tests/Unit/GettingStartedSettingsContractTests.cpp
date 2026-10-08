#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Core/Services/GettingStartedMachineDefaults.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

class GettingStartedSettingsContractTests : public juce::UnitTest
{
public:
    GettingStartedSettingsContractTests()
        : juce::UnitTest("GettingStartedSettingsContract")
    {
    }

    void runTest() override
    {
        userInterfaceSettingsCopyContract();
        scaleAndSkinSettingsShareLogoApvtsKeys();
        logoMenuScaleAndSkinSectionCopy();
        autoOpenPreferencePersistsAndDefaults();
        runSetupAgainDoesNotMutateGettingStartedPreference();
    }

private:
    static juce::File makeTempPrefsFile()
    {
        const auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("matrix-gs-1-tests-" + juce::Uuid().toString());
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

    void userInterfaceSettingsCopyContract()
    {
        beginTest("Copy - User Interface Settings labels and Getting Started strings");

        expectEquals(juce::String(PluginDisplayNames::Settings::kUiScaleRowLabel),
                     juce::String("UI SCALE"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kSkinRowLabel),
                     juce::String("SKIN"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kInfoMessageLabel),
                     juce::String("INFO MESSAGE"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kContextualHelpLabel),
                     juce::String("CONTEXTUAL HELP"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kGettingStartedLabel),
                     juce::String("GETTING STARTED"));

        expectEquals(juce::String(PluginDisplayNames::Settings::kShowWhenIncomplete),
                     juce::String("SHOW WHEN INCOMPLETE"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kNeverAtLaunch),
                     juce::String("NEVER AT LAUNCH"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kRunSetupAgainButton),
                     juce::String("RUN SETUP AGAIN"));

        // Settings rows: no colon (peers of INFO MESSAGE).
        expect(! juce::String(PluginDisplayNames::Settings::kUiScaleRowLabel).containsChar(':'));
        expect(! juce::String(PluginDisplayNames::Settings::kSkinRowLabel).containsChar(':'));
    }

    void scaleAndSkinSettingsShareLogoApvtsKeys()
    {
        beginTest("Scale/Skin - Settings reuse logo APVTS keys and choice ids");

        expectEquals(juce::String(PluginIDs::Settings::kGuiScale), juce::String("settingsGuiScale"));
        expectEquals(juce::String(PluginIDs::Settings::kSkinVariant),
                     juce::String("settingsSkinVariant"));

        using namespace PluginIDs::Settings::ScaleLevels;
        expect(kMin <= kDefault && kDefault <= kMax);
        expectEquals(k100, kDefault);
        expectEquals(getUiScale(k50), 0.5f);
        expectEquals(getUiScale(k200), 2.0f);
        expectEquals(getUiScale(0), getUiScale(kDefault));

        using namespace PluginIDs::Settings::SkinVariants;
        expectEquals(kBlack, 1);
        expectEquals(kCream, 2);
    }

    void logoMenuScaleAndSkinSectionCopy()
    {
        beginTest("Logo - Scale/Skin section headers and choice lists");

        expectEquals(juce::String(PluginDisplayNames::HeaderPanel::kLogoUiScaleSection),
                     juce::String("UI SCALE"));
        expectEquals(juce::String(PluginDisplayNames::HeaderPanel::kLogoSkinSection),
                     juce::String("SKIN"));
        expectEquals(juce::String(PluginDisplayNames::ChoiceLists::ScaleLevels::k100),
                     juce::String("100%"));
        expectEquals(juce::String(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack),
                     juce::String("BLACK"));
    }

    void autoOpenPreferencePersistsAndDefaults()
    {
        beginTest("Combo persist - machine store round-trip and missing-key default");

        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        expectEquals(normalize(0), kDefault);
        expectEquals(normalize(99), kDefault);
        expectEquals(normalize(kNeverAtLaunch), kNeverAtLaunch);

        const auto prefsFile = makeTempPrefsFile();
        {
            auto store = openTempStore(prefsFile);
            expect(store != nullptr);
            if (store == nullptr)
                return;

            expectEquals(Core::GettingStartedMachineDefaults::readAutoOpenPreference(*store),
                         kDefault);

            Core::GettingStartedMachineDefaults::writeAutoOpenPreference(*store, kNeverAtLaunch);
            expectEquals(Core::GettingStartedMachineDefaults::readAutoOpenPreference(*store),
                         kNeverAtLaunch);
        }

        {
            auto reopened = openTempStore(prefsFile);
            expect(reopened != nullptr);
            if (reopened == nullptr)
                return;

            expectEquals(Core::GettingStartedMachineDefaults::readAutoOpenPreference(*reopened),
                         kNeverAtLaunch);

            Core::GettingStartedMachineDefaults::writeAutoOpenPreference(*reopened, 12345);
            expectEquals(Core::GettingStartedMachineDefaults::readAutoOpenPreference(*reopened),
                         kDefault);
        }

        prefsFile.getParentDirectory().deleteRecursively();
    }

    void runSetupAgainDoesNotMutateGettingStartedPreference()
    {
        beginTest("Run Setup Again - GS-1 no-op does not change auto-open preference");

        using namespace PluginIDs::Settings::GettingStartedAutoOpen;

        const auto prefsFile = makeTempPrefsFile();
        auto store = openTempStore(prefsFile);
        expect(store != nullptr);
        if (store == nullptr)
            return;

        Core::GettingStartedMachineDefaults::writeAutoOpenPreference(*store, kNeverAtLaunch);
        const auto before = Core::GettingStartedMachineDefaults::readAutoOpenPreference(*store);
        expectEquals(before, kNeverAtLaunch);

        Core::GettingStartedMachineDefaults::runSetupAgainNoOp();

        expectEquals(Core::GettingStartedMachineDefaults::readAutoOpenPreference(*store), before);

        prefsFile.getParentDirectory().deleteRecursively();
    }
};

static GettingStartedSettingsContractTests gettingStartedSettingsContractTests;
