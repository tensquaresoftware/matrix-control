#include <juce_gui_extra/juce_gui_extra.h>

#include "Shared/ProjectPaths.h"
#include "Standalone/StandaloneTitleBarDoubleClick.h"
#include "Standalone/StandaloneWindowPlacement.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMainComponent.h"

namespace
{
    juce::File skinEditorWindowStateFile()
    {
        return ProjectPaths::getApplicationDataDirectory().getChildFile("skin-editor-window.state");
    }

    class SkinEditorApplication final : public juce::JUCEApplication
    {
    public:
        const juce::String getApplicationName() override
        {
            return "Matrix-Control Skin Editor";
        }

        const juce::String getApplicationVersion() override
        {
            return JUCE_APPLICATION_VERSION_STRING;
        }

        bool moreThanOneInstanceAllowed() override
        {
            return true;
        }

        void initialise(const juce::String&) override
        {
            mainWindow_ = std::make_unique<MainWindow>(getApplicationName());
        }

        void shutdown() override
        {
            if (mainWindow_ != nullptr)
                mainWindow_->persistWindowState();
            mainWindow_.reset();
        }

        void systemRequestedQuit() override
        {
            if (mainWindow_ != nullptr)
            {
                if (auto* editor = dynamic_cast<SkinEditorMainComponent*>(mainWindow_->getContentComponent()))
                {
                    if (! editor->tryQuitWithDirtyGate())
                        return;
                }
                mainWindow_->persistWindowState();
            }
            quit();
        }

    private:
        /**
         * Height is user-resizable within the nearest display; width stays at kWindowMaxWidth.
         * Position/size persist in Application Support (skin-editor-window.state).
         */
        class MainWindow final : public juce::DocumentWindow,
                                 private juce::Timer
        {
        public:
            explicit MainWindow(const juce::String& name)
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

            void persistWindowState()
            {
                stopTimer();
                const auto state = getWindowStateAsString().trim();
                if (state.isEmpty())
                    return;

                const auto file = skinEditorWindowStateFile();
                file.getParentDirectory().createDirectory();
                file.replaceWithText(state);
            }

            void closeButtonPressed() override
            {
                juce::JUCEApplication::getInstance()->systemRequestedQuit();
            }

            void maximiseButtonPressed() override
            {
                zoomToMaxHeightAndCentre();
            }

            void resized() override
            {
                DocumentWindow::resized();
                schedulePersist();
            }

            void moved() override
            {
                DocumentWindow::moved();
                updateSizeLimits();
                schedulePersist();
            }

            void visibilityChanged() override
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

            void zoomToMaxHeightAndCentre()
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

                const int contentH = juce::jmax(480,
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

        private:
            void timerCallback() override
            {
                persistWindowState();
            }

            void schedulePersist()
            {
                if (ignorePersist_)
                    return;
                startTimer(400);
            }

            bool restoreWindowState()
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

            void updateSizeLimits()
            {
                constexpr int kMinH = 480;
                int maxH = 4096;

                const auto& displays = juce::Desktop::getInstance().getDisplays();
                if (! displays.displays.isEmpty())
                {
                    const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());
                    if (display != nullptr)
                    {
                        const auto area = display->userBounds.toNearestInt();
                        const auto frame = getNativeFrameSize();
                        maxH = juce::jmax(kMinH,
                                          area.getHeight() - juce::jmax(0, frame.getTopAndBottom()));
                    }
                }

                constrainer_.setMinimumWidth(SkinEditorChrome::kWindowMaxWidth);
                constrainer_.setMaximumWidth(SkinEditorChrome::kWindowMaxWidth);
                constrainer_.setMinimumHeight(kMinH);
                constrainer_.setMaximumHeight(maxH);
                maxContentHeight_ = maxH;
            }

            void normalizeClientSize()
            {
                updateSizeLimits();

                const int w = SkinEditorChrome::kWindowMaxWidth;
                const int h = juce::jlimit(480, maxContentHeight_, getHeight());

                if (getWidth() == w && getHeight() == h)
                    return;

                ignorePersist_ = true;
                setSize(w, h);
                ignorePersist_ = false;
            }

            void ensureTitleBarOnScreen()
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

            void scheduleEnsureTitleBarRetry()
            {
                constexpr int kMaxRetries = 12;
                if (titleBarEnsureRetries_ >= kMaxRetries)
                    return;

                ++titleBarEnsureRetries_;
                const juce::Component::SafePointer<MainWindow> safe(this);
                juce::Timer::callAfterDelay(50, [safe]
                {
                    if (safe != nullptr && safe->isShowing())
                        safe->ensureTitleBarOnScreen();
                });
            }

            juce::BorderSize<int> getNativeFrameSize() const
            {
                if (auto* peer = getPeer())
                    if (const auto frameSize = peer->getFrameSizeIfPresent())
                        return *frameSize;

                return {};
            }

            void attachTitleBarDoubleClickZoom()
            {
               #if ! (JUCE_IOS || JUCE_ANDROID)
                const juce::Component::SafePointer<MainWindow> safe(this);
                titleBarDoubleClickHook_.attach(*this, [safe]
                {
                    if (safe != nullptr)
                        safe->zoomToMaxHeightAndCentre();
                });
               #endif
            }

            juce::ComponentBoundsConstrainer constrainer_;
            MatrixStandalone::TitleBarDoubleClickHook titleBarDoubleClickHook_;
            int maxContentHeight_ = 4096;
            int titleBarEnsureRetries_ = 0;
            bool ignorePersist_ = false;
        };

        std::unique_ptr<MainWindow> mainWindow_;
    };
}

START_JUCE_APPLICATION(SkinEditorApplication)
