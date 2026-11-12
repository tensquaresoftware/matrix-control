#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

// juce_IncludeModuleHeaders.h may #define Component as juce::Component; keep our API usable.
#ifdef Component
 #pragma push_macro ("Component")
 #undef Component
 #define MATRIX_STANDALONE_RESTORE_COMPONENT_MACRO 1
#endif

namespace MatrixStandalone
{
/** Platform hook: native title-bar double-click → callback (Standalone only).
    Peer-level per OS (Windows caption dblclk; macOS NSEvent title-bar swallow + zoom
    delegate; Linux strip poll). */
class TitleBarDoubleClickHook final
{
public:
    using Callback = std::function<void()>;

    TitleBarDoubleClickHook();
    ~TitleBarDoubleClickHook();

    TitleBarDoubleClickHook (const TitleBarDoubleClickHook&) = delete;
    TitleBarDoubleClickHook& operator= (const TitleBarDoubleClickHook&) = delete;

    void attach (juce::Component& window, Callback onDoubleClick);
    void detach();

private:
    struct PlatformState;
    std::unique_ptr<PlatformState> platform_;
};
} // namespace MatrixStandalone

#ifdef MATRIX_STANDALONE_RESTORE_COMPONENT_MACRO
 #pragma pop_macro ("Component")
 #undef MATRIX_STANDALONE_RESTORE_COMPONENT_MACRO
#endif
