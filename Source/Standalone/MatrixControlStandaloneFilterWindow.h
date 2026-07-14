#pragma once

#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

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

    MatrixControlStandaloneFilterWindow (const juce::String& title,
                                         juce::Colour backgroundColour,
                                         juce::PropertySet* settingsToUse,
                                         bool takeOwnershipOfSettings,
                                         const juce::String& preferredDefaultDeviceName = {},
                                         const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr,
                                         const juce::Array<juce::StandalonePluginHolder::PluginInOuts>& constrainToConfiguration = {},
                                         bool autoOpenMidiDevices = false)
        : juce::StandaloneFilterWindow (title,
                                        backgroundColour,
                                        settingsToUse,
                                        takeOwnershipOfSettings,
                                        preferredDefaultDeviceName,
                                        preferredSetupOptions,
                                        constrainToConfiguration,
                                        autoOpenMidiDevices)
    {
        enableNativeTitleBar();
        hideJuceOptionsButton();
    }

    void fitWindowToContent()
    {
        if (auto* processor = getAudioProcessor())
        {
            if (auto* editor = processor->getActiveEditor())
                setContentComponentSize (editor->getWidth(), editor->getHeight());
        }
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
};
