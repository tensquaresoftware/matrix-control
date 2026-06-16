#include <juce_core/juce_core.h>

#include "Shared/Definitions/PluginVersion.h"

#ifndef MATRIX_CONTROL_PROJECT_VERSION
 #define MATRIX_CONTROL_PROJECT_VERSION "0.0.0"
#endif

#ifndef MATRIX_CONTROL_PRERELEASE_SUFFIX
 #define MATRIX_CONTROL_PRERELEASE_SUFFIX ""
#endif

class PluginVersionTests : public juce::UnitTest
{
public:
    PluginVersionTests() : juce::UnitTest("PluginVersion") {}

    void runTest() override
    {
        testVersionStringContainsProjectVersion();
        testReleaseDateStringFormat();
    }

private:
    void testVersionStringContainsProjectVersion()
    {
        beginTest("getVersionString contains CMake project version and prerelease suffix");

        const auto version = PluginVersion::getVersionString();
        const auto projectVersion = juce::String(MATRIX_CONTROL_PROJECT_VERSION).unquoted();

        expect(version.contains(projectVersion),
               "Version string should contain PROJECT_VERSION");

        const auto suffix = juce::String(MATRIX_CONTROL_PRERELEASE_SUFFIX).unquoted();
        if (suffix.isNotEmpty())
            expect(version.endsWith("-" + suffix), "Version string should include prerelease suffix");
    }

    void testReleaseDateStringFormat()
    {
        beginTest("getReleaseDateString is ISO YYYY-MM-DD");

        const auto date = PluginVersion::getReleaseDateString();
        expect(date.length() == 10, "Release date should be 10 characters");
        expect(date[4] == '-' && date[7] == '-', "Release date should use ISO separators");
    }
};

static PluginVersionTests pluginVersionTests;
