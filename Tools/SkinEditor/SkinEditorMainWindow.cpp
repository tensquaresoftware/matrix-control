#include "Tools/SkinEditor/SkinEditorMainWindow.h"

#include "Shared/ProjectPaths.h"
#include "Standalone/StandaloneWindowPlacement.h"
#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorColourPicker.h"
#include "Tools/SkinEditor/SkinEditorMainComponent.h"
#include "Tools/SkinEditor/SkinEditorSkinsPanel.h"

namespace
{
    juce::File skinEditorWindowStateFile()
    {
        return ProjectPaths::getApplicationDataDirectory().getChildFile("skin-editor-window.state");
    }

    /**
     * Min MainComponent / content-view height: fixed SKINS + COLOR stack with
     * equal kPanelContentPad above and below the #/R/G/B row.
     * Mirrors SkinEditorMainComponent::resized + layoutBodyPanels.
     */
    int skinEditorMinContentHeight() noexcept
    {
        const int skinsH = juce::jmax(SkinEditorSkinsPanel::preferredHeight(),
                                      SkinEditorSkinsPanel::preferredHeightForMinWindow());
        const int colorH = SkinEditorColourPicker::preferredHeightForWidth(
            SkinEditorChrome::kColorPanelWidth);
        const int leftColumn = skinsH + colorH;
        // title | stack | OPERATION | stack | body | status (no gutter above status)
        return SkinEditorChrome::kTitleBarHeight + SkinEditorChrome::kGutter
               + SkinEditorChrome::kMappingBandHeight + SkinEditorChrome::kGutter
               + leftColumn + SkinEditorChrome::kStatusBarHeight;
    }
}

SkinEditorMainWindow::SkinEditorMainWindow(const juce::String& name)
    : DocumentWindow(name,
                     SkinEditorChrome::surfaceApp(),
                     DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new SkinEditorMainComponent(), true);

    SkinEditorChrome::warmFonts();
    updateSizeLimits();
    setConstrainer(&constrainer_);
    setResizable(true, true);

    const int contentW = SkinEditorChrome::windowContentWidth();
    centreWithSize(contentW, 820);
    setVisible(true);

    if (! restoreWindowState())
        centreWithSize(contentW, 820);

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

    const int contentH = juce::jmax(skinEditorMinContentHeight(),
                                    area.getHeight() - juce::jmax(0, frame.getTopAndBottom()));

    ignorePersist_ = true;
    setSize(SkinEditorChrome::windowContentWidth(), contentH);
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
    const int clientMinH = skinEditorMinContentHeight();
    int clientMaxH = 4096;

    const auto frame = getNativeFrameSize();
    const int frameTB = juce::jmax(0, frame.getTopAndBottom());

    const auto& displays = juce::Desktop::getInstance().getDisplays();
    if (! displays.displays.isEmpty())
    {
        const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());
        if (display != nullptr)
        {
            const auto area = display->userBounds.toNearestInt();
            // Max content height that still fits the display's user area.
            clientMaxH = juce::jmax(clientMinH,
                                    area.getHeight() - frameTB);
        }
    }

    const int contentW = SkinEditorChrome::windowContentWidth();
    // macOS live-resize constrainRect feeds the constrainer the NSWindow frame
    // (native title bar included), not the content view — add frameTB so the
    // content view still reaches clientMinH (≈658 outer when title bar ≈31).
    constrainer_.setMinimumWidth(contentW);
    constrainer_.setMaximumWidth(contentW);
    constrainer_.setMinimumHeight(clientMinH + frameTB);
    constrainer_.setMaximumHeight(clientMaxH + frameTB);
    maxContentHeight_ = clientMaxH;
}

void SkinEditorMainWindow::normalizeClientSize()
{
    updateSizeLimits();

    const int w = SkinEditorChrome::windowContentWidth();
    const int h = juce::jlimit(skinEditorMinContentHeight(), maxContentHeight_, getHeight());

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
