#include "TestVisualDebug.h"

TestVisualDebugSettings& TestVisualDebugSettings::get() noexcept
{
    static TestVisualDebugSettings settings;
    return settings;
}
