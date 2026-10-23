#include <juce_gui_extra/juce_gui_extra.h>

#include "Tools/SkinEditor/SkinEditorMainComponent.h"

namespace
{
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
            mainWindow_.reset();
        }

        void systemRequestedQuit() override
        {
            quit();
        }

    private:
        class MainWindow final : public juce::DocumentWindow
        {
        public:
            explicit MainWindow(const juce::String& name)
                : DocumentWindow(name,
                                 juce::Desktop::getInstance().getDefaultLookAndFeel()
                                     .findColour(juce::ResizableWindow::backgroundColourId),
                                 DocumentWindow::allButtons)
            {
                setUsingNativeTitleBar(true);
                setContentOwned(new SkinEditorMainComponent(), true);
                setResizable(true, true);
                centreWithSize(1100, 720);
                setVisible(true);
            }

            void closeButtonPressed() override
            {
                juce::JUCEApplication::getInstance()->systemRequestedQuit();
            }
        };

        std::unique_ptr<MainWindow> mainWindow_;
    };
}

START_JUCE_APPLICATION(SkinEditorApplication)
