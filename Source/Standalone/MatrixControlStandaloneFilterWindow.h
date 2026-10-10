#pragma once

#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

#include "Standalone/StandaloneQuitCommands.h"
#include "Standalone/StandaloneWindowPlacement.h"

// macOS multi-monitor drag uses OS-native window movement when the native title bar is enabled.
// JUCE's custom title bar + ComponentDragger can clamp the window to the primary display on
// mixed-scale Retina + external monitor setups.
class MatrixControlStandaloneFilterWindow final : public juce::StandaloneFilterWindow
{
public:
    MatrixControlStandaloneFilterWindow (const juce::String& title,
                                         juce::Colour backgroundColour,
                                         std::unique_ptr<juce::StandalonePluginHolder> pluginHolderIn)
        : juce::StandaloneFilterWindow (title, backgroundColour, std::move (pluginHolderIn))
    {
        enableNativeTitleBar();
        hideJuceOptionsButton();
    }

    void fitWindowToContent()
    {
        if (auto* processor = getAudioProcessor())
            if (auto* editor = processor->getActiveEditor())
                setContentComponentSize (editor->getWidth(), editor->getHeight());
    }

    /** Launch-only placement: clamp/recentre so the title-bar strip meets a display user area.
        Call after show/fit and once more async — not from every resized (avoids fighting multi-monitor drag). */
    void ensureLaunchTitleBarOnScreen()
    {
        ensureTitleBarIntersectsDisplayUserArea();
    }

    void resized() override
    {
        juce::StandaloneFilterWindow::resized();
        fitWindowToContent();
    }

    void visibilityChanged() override
    {
        juce::StandaloneFilterWindow::visibilityChanged();

        if (isShowing())
            fitWindowToContent();
    }

    void closeButtonPressed() override
    {
        // Route through the app quit path so risk gating and modal-cancel retry stay unified.
        if (auto* app = juce::JUCEApplicationBase::getInstance())
            app->systemRequestedQuit();
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
       #if JUCE_WINDOWS
        if (MatrixStandalone::isWindowsAltF4QuitKey (key))
        {
            closeButtonPressed();
            return true;
        }
       #endif

        return juce::StandaloneFilterWindow::keyPressed (key);
    }

private:
    void hideJuceOptionsButton()
    {
        for (int i = getNumChildComponents(); --i >= 0;)
        {
            if (auto* button = dynamic_cast<juce::TextButton*> (getChildComponent (i)))
            {
                if (button->getButtonText() == "Options")
                    button->setVisible (false);
            }
        }
    }

    void enableNativeTitleBar()
    {
       #if ! (JUCE_IOS || JUCE_ANDROID)
        setUsingNativeTitleBar (true);
        setConstrainer (nullptr);
        fitWindowToContent();
       #endif
    }

    juce::BorderSize<int> getNativeFrameSize() const
    {
        if (auto* peer = getPeer())
            if (const auto frameSize = peer->getFrameSizeIfPresent())
                return *frameSize;

        return {};
    }

    void ensureTitleBarIntersectsDisplayUserArea()
    {
        const auto& displays = juce::Desktop::getInstance().getDisplays();

        if (displays.displays.isEmpty())
            return;

        juce::Array<juce::Rectangle<int>> userAreas;

        for (const auto& display : displays.displays)
            userAreas.add (display.userBounds.toNearestInt());

        const auto* display = MatrixStandalone::findNearestDisplayForBounds (displays, getBounds());

        if (display == nullptr)
            return;

        const auto nextBounds = MatrixStandalone::ensureClientBoundsTitleBarOnScreen (
            getBounds(),
            getNativeFrameSize(),
            userAreas,
            display->userBounds.toNearestInt());

        if (nextBounds != getBounds())
        {
            setBounds (nextBounds);

            // Moving after show can leave the HWND non-active on Windows; restore foreground.
            if (isShowing())
                toFront (true);
        }
    }
};
