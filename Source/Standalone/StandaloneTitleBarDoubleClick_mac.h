#pragma once

#include "Standalone/StandaloneTitleBarDoubleClick.h"

namespace MatrixStandalone::MacTitleBarDetail
{
const void* attach (juce::Component& window, TitleBarDoubleClickHook::Callback callback);
void detach (const void* nsWindowKey);
}
