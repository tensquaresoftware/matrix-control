#include <memory>

#include <juce_gui_extra/juce_gui_extra.h>

#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"
#include "Tools/SkinDesigner/SkinDesignerMainWindow.h"

namespace
{
    class SkinDesignerApplication final : public juce::JUCEApplication
    {
    public:
        const juce::String getApplicationName() override
        {
            return "Matrix-Control Skin Designer";
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
            mainWindow_ = std::make_unique<SkinDesignerMainWindow>(getApplicationName());
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
                if (auto* designer = dynamic_cast<SkinDesignerMainComponent*>(mainWindow_->getContentComponent()))
                {
                    if (! designer->tryQuitWithDirtyGate())
                        return;
                }
                mainWindow_->persistWindowState();
            }
            quit();
        }

    private:
        std::unique_ptr<SkinDesignerMainWindow> mainWindow_;
    };
}

START_JUCE_APPLICATION(SkinDesignerApplication)
