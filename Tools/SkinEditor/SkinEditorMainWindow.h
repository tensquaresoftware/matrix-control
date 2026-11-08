#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "Standalone/StandaloneTitleBarDoubleClick.h"

/**
 * Height and width are user-resizable within the nearest display; width stretches
 * Palette and Mappings Mapped colour together (min Palette 160px) — see windowMinWidth().
 * Position/size persist in Application Support (skin-editor-window.state).
 */
class SkinEditorMainWindow final : public juce::DocumentWindow,
                                   private juce::Timer
{
public:
    explicit SkinEditorMainWindow(const juce::String& name);

    void persistWindowState();

    void closeButtonPressed() override;
    void maximiseButtonPressed() override;
    void resized() override;
    void moved() override;
    void visibilityChanged() override;

private:
    void zoomToMaxHeightAndCentre();
    void timerCallback() override;
    void schedulePersist();
    bool restoreWindowState();
    void updateSizeLimits();
    void normalizeClientSize();
    void ensureTitleBarOnScreen();
    void scheduleEnsureTitleBarRetry();
    juce::BorderSize<int> getNativeFrameSize() const;
    void attachTitleBarDoubleClickZoom();

    juce::ComponentBoundsConstrainer constrainer_;
    MatrixStandalone::TitleBarDoubleClickHook titleBarDoubleClickHook_;
    int maxContentWidth_ = 8192;
    int maxContentHeight_ = 4096;
    int titleBarEnsureRetries_ = 0;
    bool ignorePersist_ = false;
};
