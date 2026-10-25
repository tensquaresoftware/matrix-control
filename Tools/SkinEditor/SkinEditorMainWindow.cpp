#include "Tools/SkinEditor/SkinEditorMainWindow.h"

#include "Shared/ProjectPaths.h"
#include "Standalone/StandaloneWindowPlacement.h"
#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorMainComponent.h"

namespace
{
    constexpr int kMinContentHeight = 480;

    juce::File skinEditorWindowStateFile()
    {
        return ProjectPaths::getApplicationDataDirectory().getChildFile("skin-editor-window.state");
    }
}

SkinEditorMainWindow::SkinEditorMainWindow(const juce::String& name)
    : DocumentWindow(name,
                     SkinEditorChrome::surfaceApp(),
                     DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new SkinEditorMainComponent(), true);

    updateSizeLimits();
    setConstrainer(&constrainer_);
    setResizable(true, true);

    centreWithSize(SkinEditorChrome::kWindowMaxWidth, 820);
    setVisible(true);

    if (! restoreWindowState())
        centreWithSize(SkinEditorChrome::kWindowMaxWidth, 820);

    normalizeClientSize();
    ensureTitleBarOnScreen();
    attachTitleBarDoubleClickZoom();
}

void SkinEditorMainWindow::persistWindowState()
{
    stopTimer();
    const auto state = getWindowStateAsString().trim();
    if (state.isEmpty())
        return;

    const auto file = skinEditorWindowStateFile();
    file.getParentDirectory().createDirectory();
    file.replaceWithText(state);
}

void SkinEditorMainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

void SkinEditorMainWindow::maximiseButtonPressed()
{
    zoomToMaxHeightAndCentre();
}

void SkinEditorMainWindow::resized()
{
    DocumentWindow::resized();
    schedulePersist();
}

void SkinEditorMainWindow::moved()
{
    DocumentWindow::moved();
    updateSizeLimits();
    schedulePersist();
}

void SkinEditorMainWindow::visibilityChanged()
{
    DocumentWindow::visibilityChanged();

    if (isShowing())
    {
        updateSizeLimits();
        setConstrainer(&constrainer_);
        ensureTitleBarOnScreen();
        attachTitleBarDoubleClickZoom();
    }
}

void SkinEditorMainWindow::zoomToMaxHeightAndCentre()
{
    updateSizeLimits();

    const auto frame = getNativeFrameSize();
    const auto& displays = juce::Desktop::getInstance().getDisplays();
    const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());

    if (display == nullptr)
        return;

    const auto area = display->userBounds.toNearestInt();
    if (area.isEmpty())
        return;

    const int contentH = juce::jmax(kMinContentHeight,
                                    area.getHeight() - juce::jmax(0, frame.getTopAndBottom()));

    ignorePersist_ = true;
    setSize(SkinEditorChrome::kWindowMaxWidth, contentH);
    ignorePersist_ = false;

    auto nextBounds = MatrixStandalone::recentreClientBoundsOnNearestDisplayUserArea(
        getBounds(),
        frame,
        displays);

    if (frame.getTop() <= 0)
    {
        const auto full = frame.addedTo(getBounds());
        nextBounds = frame.subtractedFrom(full.withCentre(area.getCentre()));
    }

    if (nextBounds != getBounds())
    {
        ignorePersist_ = true;
        setBounds(nextBounds);
        ignorePersist_ = false;

        if (isShowing())
            toFront(true);
    }

    schedulePersist();
}

void SkinEditorMainWindow::timerCallback()
{
    persistWindowState();
}

void SkinEditorMainWindow::schedulePersist()
{
    if (ignorePersist_)
        return;
    startTimer(400);
}

bool SkinEditorMainWindow::restoreWindowState()
{
    const auto file = skinEditorWindowStateFile();
    if (! file.existsAsFile())
        return false;

    const auto state = file.loadFileAsString().trim();
    if (state.isEmpty())
        return false;

    ignorePersist_ = true;
    const bool ok = restoreWindowStateFromString(state);
    ignorePersist_ = false;
    return ok;
}

void SkinEditorMainWindow::updateSizeLimits()
{
    int maxH = 4096;

    const auto& displays = juce::Desktop::getInstance().getDisplays();
    if (! displays.displays.isEmpty())
    {
        const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());
        if (display != nullptr)
        {
            const auto area = display->userBounds.toNearestInt();
            const auto frame = getNativeFrameSize();
            maxH = juce::jmax(kMinContentHeight,
                              area.getHeight() - juce::jmax(0, frame.getTopAndBottom()));
        }
    }

    constrainer_.setMinimumWidth(SkinEditorChrome::windowMinWidth());
    constrainer_.setMaximumWidth(SkinEditorChrome::kWindowMaxWidth);
    constrainer_.setMinimumHeight(kMinContentHeight);
    constrainer_.setMaximumHeight(maxH);
    maxContentHeight_ = maxH;
}

void SkinEditorMainWindow::normalizeClientSize()
{
    updateSizeLimits();

    const int w = juce::jlimit(SkinEditorChrome::windowMinWidth(),
                               SkinEditorChrome::kWindowMaxWidth,
                               getWidth());
    const int h = juce::jlimit(kMinContentHeight, maxContentHeight_, getHeight());

    if (getWidth() == w && getHeight() == h)
        return;

    ignorePersist_ = true;
    setSize(w, h);
    ignorePersist_ = false;
}

void SkinEditorMainWindow::ensureTitleBarOnScreen()
{
    const auto frame = getNativeFrameSize();
    if (frame.getTop() <= 0)
    {
        scheduleEnsureTitleBarRetry();
        return;
    }

    titleBarEnsureRetries_ = 0;
    normalizeClientSize();

    const auto& displays = juce::Desktop::getInstance().getDisplays();
    if (displays.displays.isEmpty())
        return;

    juce::Array<juce::Rectangle<int>> userAreas;
    for (const auto& display : displays.displays)
        userAreas.add(display.userBounds.toNearestInt());

    const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());
    if (display == nullptr)
        return;

    const auto nextBounds = MatrixStandalone::ensureClientBoundsTitleBarOnScreen(
        getBounds(),
        frame,
        userAreas,
        display->userBounds.toNearestInt());

    if (nextBounds != getBounds())
    {
        ignorePersist_ = true;
        setBounds(nextBounds);
        ignorePersist_ = false;

        if (isShowing())
            toFront(true);
    }
}

void SkinEditorMainWindow::scheduleEnsureTitleBarRetry()
{
    constexpr int kMaxRetries = 12;
    if (titleBarEnsureRetries_ >= kMaxRetries)
        return;

    ++titleBarEnsureRetries_;
    const juce::Component::SafePointer<SkinEditorMainWindow> safe(this);
    juce::Timer::callAfterDelay(50, [safe]
    {
        if (safe != nullptr && safe->isShowing())
            safe->ensureTitleBarOnScreen();
    });
}

juce::BorderSize<int> SkinEditorMainWindow::getNativeFrameSize() const
{
    if (auto* peer = getPeer())
        if (const auto frameSize = peer->getFrameSizeIfPresent())
            return *frameSize;

    return {};
}

void SkinEditorMainWindow::attachTitleBarDoubleClickZoom()
{
   #if ! (JUCE_IOS || JUCE_ANDROID)
    const juce::Component::SafePointer<SkinEditorMainWindow> safe(this);
    titleBarDoubleClickHook_.attach(*this, [safe]
    {
        if (safe != nullptr)
            safe->zoomToMaxHeightAndCentre();
    });
   #endif
}
