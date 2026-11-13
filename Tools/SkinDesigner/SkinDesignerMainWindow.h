#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "Standalone/StandaloneTitleBarDoubleClick.h"

/**
 * Height and width are user-resizable within the nearest display; width stretches
 * Palette and Mappings Mapped colour together (min Palette 125px) — see windowMinWidth().
 * Position/size persist in Application Support (skin-designer-window.state).
 */
class SkinDesignerMainWindow final : public juce::DocumentWindow,
                                   private juce::Timer
{
public:
    explicit SkinDesignerMainWindow(const juce::String& name);

    void persistWindowState();

    void closeButtonPressed() override;
    void maximiseButtonPressed() override;
    void resized() override;
    void moved() override;
    void visibilityChanged() override;

private:
    void applySizeThenRecentre(int contentWidth, int contentHeight);
    void zoomToMaxHeightAndCentre();
    void shrinkToMinSizeAndRecentre();
    void timerCallback() override;
    void schedulePersist();
    bool restoreWindowState();
    void updateSizeLimits();
    void normalizeClientSize();
    void ensureTitleBarOnScreen();
    void scheduleEnsureTitleBarRetry();
    juce::BorderSize<int> getNativeFrameSize() const;
    void attachTitleBarDoubleClickRecentre();

    juce::ComponentBoundsConstrainer constrainer_;
    MatrixStandalone::TitleBarDoubleClickHook titleBarDoubleClickHook_;
    int maxContentWidth_ = 8192;
    int maxContentHeight_ = 4096;
    int titleBarEnsureRetries_ = 0;
    bool ignorePersist_ = false;
};
