#include "Tools/SkinDesigner/SkinDesignerMainWindow.h"

#include "Shared/ProjectPaths.h"
#include "Standalone/StandaloneWindowPlacement.h"
#include "Tools/SkinDesigner/SkinDesignerChromeFonts.h"
#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"
#include "Tools/SkinDesigner/SkinDesignerColourPicker.h"
#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"
#include "Tools/SkinDesigner/SkinDesignerSkinsPanel.h"

namespace
{
    juce::File skinDesignerWindowStateFile()
    {
        return ProjectPaths::getApplicationDataDirectory().getChildFile("skin-designer-window.state");
    }

    /**
     * Min MainComponent / content-view height: fixed SKINS + COLOR stack
     * (COLOR preferredHeight owns #/R/G/B and axis-row gaps).
     * Mirrors SkinDesignerMainComponent::resized + layoutBodyPanels.
     */
    int skinDesignerMinContentHeight() noexcept
    {
        const int skinsH = SkinDesignerSkinsPanel::preferredHeight();
        const int colorH = SkinDesignerColourPicker::preferredHeightForWidth(
            SkinDesignerChrome::kColorPanelWidth);
        const int leftColumn = skinsH + colorH;
        // title (logo + OPERATION + SESSION) | stack | body | status
        return SkinDesignerChrome::kTitleBarHeight + SkinDesignerChrome::kGutter
               + leftColumn + SkinDesignerChrome::kStatusBarHeight;
    }
}

SkinDesignerMainWindow::SkinDesignerMainWindow(const juce::String& name)
    : DocumentWindow(name,
                     SkinDesignerChrome::surfaceApp(),
                     DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new SkinDesignerMainComponent(), true);

    SkinDesignerChrome::warmFonts();
    updateSizeLimits();
    setConstrainer(&constrainer_);
    setResizable(true, true);

    const int contentW = SkinDesignerChrome::windowContentWidth();
    centreWithSize(contentW, 820);
    setVisible(true);

    if (! restoreWindowState())
        centreWithSize(contentW, 820);

    normalizeClientSize();
    ensureTitleBarOnScreen();
    attachTitleBarDoubleClickZoom();
}

void SkinDesignerMainWindow::persistWindowState()
{
    stopTimer();
    const auto state = getWindowStateAsString().trim();
    if (state.isEmpty())
        return;

    const auto file = skinDesignerWindowStateFile();
    file.getParentDirectory().createDirectory();
    file.replaceWithText(state);
}

void SkinDesignerMainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

void SkinDesignerMainWindow::maximiseButtonPressed()
{
    zoomToMaxHeightAndCentre();
}

void SkinDesignerMainWindow::resized()
{
    DocumentWindow::resized();
    schedulePersist();
}

void SkinDesignerMainWindow::moved()
{
    DocumentWindow::moved();
    updateSizeLimits();
    schedulePersist();
}

void SkinDesignerMainWindow::visibilityChanged()
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

void SkinDesignerMainWindow::zoomToMaxHeightAndCentre()
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

    const int contentH = juce::jmax(skinDesignerMinContentHeight(),
                                    area.getHeight() - juce::jmax(0, frame.getTopAndBottom()));

    ignorePersist_ = true;
    setSize(juce::jlimit(SkinDesignerChrome::windowMinWidth(), maxContentWidth_, getWidth()),
            contentH);
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

void SkinDesignerMainWindow::timerCallback()
{
    persistWindowState();
}

void SkinDesignerMainWindow::schedulePersist()
{
    if (ignorePersist_)
        return;
    startTimer(400);
}

bool SkinDesignerMainWindow::restoreWindowState()
{
    const auto file = skinDesignerWindowStateFile();
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

void SkinDesignerMainWindow::updateSizeLimits()
{
    const int clientMinH = skinDesignerMinContentHeight();
    const int contentMinW = SkinDesignerChrome::windowMinWidth();
    int clientMaxH = 4096;
    int contentMaxW = 8192;

    const auto frame = getNativeFrameSize();
    const int frameTB = juce::jmax(0, frame.getTopAndBottom());
    const int frameLR = juce::jmax(0, frame.getLeftAndRight());

    const auto& displays = juce::Desktop::getInstance().getDisplays();
    if (! displays.displays.isEmpty())
    {
        const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());
        if (display != nullptr)
        {
            const auto area = display->userBounds.toNearestInt();
            // Max content size that still fits the display's user area.
            clientMaxH = juce::jmax(clientMinH, area.getHeight() - frameTB);
            contentMaxW = juce::jmax(contentMinW, area.getWidth() - frameLR);
        }
    }

    // macOS live-resize constrainRect feeds the constrainer the NSWindow frame
    // (native title bar included), not the content view — add frameTB so the
    // content view still reaches clientMinH. Width uses the same display user
    // area (no fixed 1210px ceiling); frameLR is usually 0 with a native title bar.
    constrainer_.setMinimumWidth(contentMinW);
    constrainer_.setMaximumWidth(contentMaxW + frameLR);
    constrainer_.setMinimumHeight(clientMinH + frameTB);
    constrainer_.setMaximumHeight(clientMaxH + frameTB);
    maxContentWidth_ = contentMaxW;
    maxContentHeight_ = clientMaxH;
}

void SkinDesignerMainWindow::normalizeClientSize()
{
    updateSizeLimits();

    const int w = juce::jlimit(SkinDesignerChrome::windowMinWidth(), maxContentWidth_, getWidth());
    const int h = juce::jlimit(skinDesignerMinContentHeight(), maxContentHeight_, getHeight());

    if (getWidth() == w && getHeight() == h)
        return;

    ignorePersist_ = true;
    setSize(w, h);
    ignorePersist_ = false;
}

void SkinDesignerMainWindow::ensureTitleBarOnScreen()
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

void SkinDesignerMainWindow::scheduleEnsureTitleBarRetry()
{
    constexpr int kMaxRetries = 12;
    if (titleBarEnsureRetries_ >= kMaxRetries)
        return;

    ++titleBarEnsureRetries_;
    const juce::Component::SafePointer<SkinDesignerMainWindow> safe(this);
    juce::Timer::callAfterDelay(50, [safe]
    {
        if (safe != nullptr && safe->isShowing())
            safe->ensureTitleBarOnScreen();
    });
}

juce::BorderSize<int> SkinDesignerMainWindow::getNativeFrameSize() const
{
    if (auto* peer = getPeer())
        if (const auto frameSize = peer->getFrameSizeIfPresent())
            return *frameSize;

    return {};
}

void SkinDesignerMainWindow::attachTitleBarDoubleClickZoom()
{
   #if ! (JUCE_IOS || JUCE_ANDROID)
    const juce::Component::SafePointer<SkinDesignerMainWindow> safe(this);
    titleBarDoubleClickHook_.attach(*this, [safe]
    {
        if (safe != nullptr)
            safe->zoomToMaxHeightAndCentre();
    });
   #endif
}
