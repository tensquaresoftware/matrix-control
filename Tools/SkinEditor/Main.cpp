#include <memory>

#include <juce_gui_extra/juce_gui_extra.h>

#include "Tools/SkinEditor/SkinEditorMainComponent.h"
#include "Tools/SkinEditor/SkinEditorMainWindow.h"

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
            mainWindow_ = std::make_unique<SkinEditorMainWindow>(getApplicationName());
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
        std::unique_ptr<SkinEditorMainWindow> mainWindow_;
    };
}

START_JUCE_APPLICATION(SkinEditorApplication)
