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
         * Height is user-resizable; width stays at kWindowMaxWidth.
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
                setResizable(true, true);
                setConstrainer(nullptr);
                centreWithSize(SkinEditorChrome::kWindowMaxWidth, 820);
                setVisible(true);

                if (! restoreWindowState())
                    centreWithSize(SkinEditorChrome::kWindowMaxWidth, 820);

                clampWidthAndMinHeight();
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
                clampWidthAndMinHeight();
                schedulePersist();
            }

            void moved() override
            {
                DocumentWindow::moved();
                schedulePersist();
            }

            void visibilityChanged() override
            {
                DocumentWindow::visibilityChanged();

                if (isShowing())
                {
                    setConstrainer(nullptr);
                    attachTitleBarDoubleClickZoom();
                }
            }

            void zoomToMaxHeightAndCentre()
            {
                setConstrainer(nullptr);

                const auto frame = getNativeFrameSize();
                const auto& displays = juce::Desktop::getInstance().getDisplays();
                const auto* display = MatrixStandalone::findNearestDisplayForBounds(displays, getBounds());

                if (display == nullptr)
                    return;

                const auto area = display->userBounds.toNearestInt();
                if (area.isEmpty())
                    return;

                const int contentH = juce::jmax(1, area.getHeight() - juce::jmax(0, frame.getTopAndBottom()));

                ignoreClamp_ = true;
                setSize(SkinEditorChrome::kWindowMaxWidth, contentH);
                ignoreClamp_ = false;

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
                    ignoreClamp_ = true;
                    setBounds(nextBounds);
                    ignoreClamp_ = false;

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
                if (ignoreClamp_)
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

                ignoreClamp_ = true;
                const bool ok = restoreWindowStateFromString(state);
                ignoreClamp_ = false;
                return ok;
            }

            void ensureTitleBarOnScreen()
            {
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
                    getNativeFrameSize(),
                    userAreas,
                    display->userBounds.toNearestInt());

                if (nextBounds != getBounds())
                {
                    ignoreClamp_ = true;
                    setBounds(nextBounds);
                    ignoreClamp_ = false;
                }
            }

            juce::BorderSize<int> getNativeFrameSize() const
            {
                if (auto* peer = getPeer())
                    if (const auto frameSize = peer->getFrameSizeIfPresent())
                        return *frameSize;

                return {};
            }

            void clampWidthAndMinHeight()
            {
                if (ignoreClamp_)
                    return;

                const int w = SkinEditorChrome::kWindowMaxWidth;
                const int h = juce::jmax(480, getHeight());

                if (getWidth() == w && getHeight() == h)
                    return;

                ignoreClamp_ = true;
                setSize(w, h);
                ignoreClamp_ = false;
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

            MatrixStandalone::TitleBarDoubleClickHook titleBarDoubleClickHook_;
            bool ignoreClamp_ = false;
        };

        std::unique_ptr<MainWindow> mainWindow_;
    };
}

START_JUCE_APPLICATION(SkinEditorApplication)
