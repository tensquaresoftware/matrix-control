#include "PluginVersion.h"

#ifndef MATRIX_CONTROL_PROJECT_VERSION
 #define MATRIX_CONTROL_PROJECT_VERSION "0.0.0"
#endif

#ifndef MATRIX_CONTROL_PRERELEASE_SUFFIX
 #define MATRIX_CONTROL_PRERELEASE_SUFFIX ""
#endif

#ifndef MATRIX_CONTROL_BUILD_DATE
 #define MATRIX_CONTROL_BUILD_DATE "1970-01-01"
#endif

namespace PluginVersion
{
namespace
{
    juce::String stringFromMacro(juce::String macroValue)
    {
        return macroValue.unquoted();
    }
}

juce::String getVersionString()
{
    auto version = stringFromMacro(MATRIX_CONTROL_PROJECT_VERSION);

    const auto suffix = stringFromMacro(MATRIX_CONTROL_PRERELEASE_SUFFIX);
    if (suffix.isNotEmpty())
        version << "-" << suffix;

    return version;
}

juce::String getReleaseDateString()
{
    return stringFromMacro(MATRIX_CONTROL_BUILD_DATE);
}
}
