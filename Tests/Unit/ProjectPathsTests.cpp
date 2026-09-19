#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Shared/ProjectPaths.h"

class ProjectPathsTests : public juce::UnitTest
{
public:
    ProjectPathsTests() : juce::UnitTest("ProjectPaths") {}

    void runTest() override
    {
        propertiesFileFolderName_isCompanyThenProduct();
        applicationDataDirectory_matchesPropertiesFileProductFolder();
        productOptions_resolveUnderUnifiedProductFolder();
        initPath_isChildOfUnifiedProductFolder();
        linuxProductFolder_isNotUnderDotConfig();
    }

private:
    void propertiesFileFolderName_isCompanyThenProduct()
    {
        beginTest("getPropertiesFileFolderName is Ten Square Software/Matrix-Control");

        expectEquals(ProjectPaths::getPropertiesFileFolderName(),
                     juce::String("Ten Square Software/Matrix-Control"));
    }

    void applicationDataDirectory_matchesPropertiesFileProductFolder()
    {
        beginTest("getApplicationDataDirectory matches PropertiesFile product folder");

        const auto productRoot = ProjectPaths::getApplicationDataDirectory();
        const auto settingsFile = ProjectPaths::makeProductPropertiesFileOptions("Matrix-Control")
                                      .getDefaultFile();
        const auto propertiesFolder = settingsFile.getParentDirectory();

        expect(productRoot.getFullPathName().isNotEmpty());
        expect(settingsFile != juce::File());
        expectEquals(productRoot.getFullPathName(), propertiesFolder.getFullPathName());
        expectEquals(productRoot.getFileName(), juce::String("Matrix-Control"));
        expectEquals(productRoot.getParentDirectory().getFileName(),
                     juce::String("Ten Square Software"));
       #if JUCE_MAC
        expect(productRoot.getFullPathName().contains("Application Support"));
       #endif
    }

    void productOptions_resolveUnderUnifiedProductFolder()
    {
        beginTest("Standalone and Device Connection options share getApplicationDataDirectory");

        const auto productRoot = ProjectPaths::getApplicationDataDirectory();
        const auto standaloneFile = ProjectPaths::makeProductPropertiesFileOptions("Matrix-Control")
                                        .getDefaultFile();
        const auto deviceFile = ProjectPaths::makeProductPropertiesFileOptions(
                                    "Matrix-Control-DeviceConnection")
                                    .getDefaultFile();

        expectEquals(standaloneFile.getParentDirectory().getFullPathName(),
                     productRoot.getFullPathName());
        expectEquals(deviceFile.getParentDirectory().getFullPathName(),
                     productRoot.getFullPathName());
        expectEquals(standaloneFile.getFileName(), juce::String("Matrix-Control.settings"));
        expectEquals(deviceFile.getFileName(),
                     juce::String("Matrix-Control-DeviceConnection.settings"));

        const auto options = ProjectPaths::makeProductPropertiesFileOptions("Matrix-Control");
        expect(! options.commonToAllUsers);
        expectEquals(options.osxLibrarySubFolder, juce::String("Application Support"));
        expectEquals(options.folderName, ProjectPaths::getPropertiesFileFolderName());
        expect(options.folderName != juce::String("~/.config"));
    }

    void initPath_isChildOfUnifiedProductFolder()
    {
        beginTest("Init path is under the unified product folder without creating it");

        const auto appData = ProjectPaths::getApplicationDataDirectory();
        const auto initPath = appData.getChildFile("Init");

        expectEquals(initPath.getFileName(), juce::String("Init"));
        expectEquals(initPath.getParentDirectory().getFullPathName(), appData.getFullPathName());
    }

    void linuxProductFolder_isNotUnderDotConfig()
    {
        beginTest("Linux product folder is not under .config");

       #if JUCE_LINUX || JUCE_BSD
        const auto productRoot = ProjectPaths::getApplicationDataDirectory();
        expect(! productRoot.getFullPathName().contains("/.config/"));
        expectEquals(productRoot.getFileName(), juce::String("Matrix-Control"));
       #else
        expect(true);
       #endif
    }
};

static ProjectPathsTests projectPathsTests;
