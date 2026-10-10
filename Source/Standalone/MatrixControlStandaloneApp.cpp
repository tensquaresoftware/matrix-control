#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_plugin_client/detail/juce_CheckSettingMacros.h>
#include <juce_audio_plugin_client/detail/juce_IncludeModuleHeaders.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "Core/Audio/AudioDeviceProfiles.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/PluginProcessor.h"
#include "Shared/ProjectPaths.h"
#include "Standalone/MatrixControlStandaloneFilterWindow.h"
#include "Standalone/StandaloneQuitCommands.h"

namespace
{
class MatrixControlStandaloneApp final : public juce::JUCEApplication
{
public:
    MatrixControlStandaloneApp()
    {
        appProperties.setStorageParameters(
            ProjectPaths::makeProductPropertiesFileOptions(
                juce::CharPointer_UTF8(JucePlugin_Name)));
    }

    const juce::String getApplicationName() override           { return juce::CharPointer_UTF8 (JucePlugin_Name); }
    const juce::String getApplicationVersion() override        { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override                 { return true; }
    void anotherInstanceStarted (const juce::String&) override {}

    MatrixControlStandaloneFilterWindow* createWindow()
    {
        if (juce::Desktop::getInstance().getDisplays().displays.isEmpty())
        {
            jassertfalse;
            return nullptr;
        }

        return new MatrixControlStandaloneFilterWindow (
            getApplicationName(),
            juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
            createPluginHolder());
    }

    std::unique_ptr<juce::StandalonePluginHolder> createPluginHolder()
    {
        constexpr auto autoOpenMidiDevices =
       #if (JUCE_ANDROID || JUCE_IOS) && ! JUCE_DONT_AUTO_OPEN_MIDI_DEVICES_ON_MOBILE
            true;
       #else
            false;
       #endif

       #ifdef JucePlugin_PreferredChannelConfigurations
        constexpr juce::StandalonePluginHolder::PluginInOuts channels[] { JucePlugin_PreferredChannelConfigurations };
        const juce::Array<juce::StandalonePluginHolder::PluginInOuts> channelConfig (
            channels, juce::numElementsInArray (channels));
       #else
        const juce::Array<juce::StandalonePluginHolder::PluginInOuts> channelConfig;
       #endif

        auto holder = std::make_unique<juce::StandalonePluginHolder> (appProperties.getUserSettings(),
                                                                     false,
                                                                     juce::String{},
                                                                     nullptr,
                                                                     channelConfig,
                                                                     autoOpenMidiDevices);
        // Criterion C as early as the holder exists — before the editor window monitors.
        const bool appliedFirstRunDefaults =
            Core::StandaloneAudioInputRouter::applySceneAudioSafetyDefaultsIfNeeded();
        if (appliedFirstRunDefaults)
        {
            if (auto* processor = dynamic_cast<PluginProcessor*>(holder->processor.get()))
                processor->setAudioFromSourceId({});
        }

        // Scarlett (etc.) gone: do not keep OS fallback Mic/Speakers — force None endpoints.
        if (Core::StandaloneAudioInputRouter::applyMissingAudioDeviceNonePolicy())
        {
            if (auto* processor = dynamic_cast<PluginProcessor*>(holder->processor.get()))
                processor->setAudioFromSourceId({});
        }

        // Defer profile restore: CoreAudio/USB enumeration is often incomplete in this ctor.
        // Immediate apply raced MIDI open and sometimes missed Scarlett entirely.
        if (Core::shouldScheduleAvailableAudioDeviceProfileRestoreAtLaunch(appliedFirstRunDefaults))
            Core::StandaloneAudioInputRouter::scheduleAvailableAudioDeviceProfileRestoreAtLaunch();

        return holder;
    }

    void initialise (const juce::String&) override
    {
        mainWindow = juce::rawToUniquePtr (createWindow());

        if (mainWindow != nullptr)
        {
           #if JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE
            juce::Desktop::getInstance().setKioskModeComponent (mainWindow.get(), false);
           #endif

            MatrixStandalone::bindStandaloneQuitCommands (commandManager, *this, *mainWindow);

            mainWindow->setVisible (true);
            mainWindow->fitWindowToContent();
            // After real content size: one placement pass (not on every resized — multi-monitor drag).
            mainWindow->ensureLaunchTitleBarOnScreen();
            // Placement may SetWindowPos without activation; bring forward after show so the
            // window is not left under the IDE / terminal that launched the process.
            mainWindow->toFront (true);

            // juce_IncludeModuleHeaders.h #defines Component as juce::Component — use bare Component::.
            // Second async pass: native frame top may be unknown on the first ensure.
            const Component::SafePointer<MatrixControlStandaloneFilterWindow> safeWindow (mainWindow.get());
            juce::MessageManager::callAsync ([safeWindow]
            {
                if (safeWindow != nullptr)
                {
                    safeWindow->ensureLaunchTitleBarOnScreen();
                   #if JUCE_WINDOWS
                    safeWindow->toFront (true);
                   #endif
                }
            });
        }
        else
        {
            pluginHolder = createPluginHolder();
        }
    }

    void shutdown() override
    {
        pluginHolder = nullptr;
        mainWindow = nullptr;
        appProperties.saveIfNeeded();
    }

    PluginProcessor* getActivePluginProcessor() const
    {
        if (mainWindow != nullptr)
            return dynamic_cast<PluginProcessor*>(mainWindow->getAudioProcessor());

        if (pluginHolder != nullptr)
            return dynamic_cast<PluginProcessor*>(pluginHolder->processor.get());

        return nullptr;
    }

    void systemRequestedQuit() override
    {
        if (auto* processor = getActivePluginProcessor())
        {
            if (! processor->confirmSessionCloseGateIfNeeded())
                return;
        }

        if (pluginHolder != nullptr)
            pluginHolder->savePluginState();

        if (mainWindow != nullptr)
            mainWindow->pluginHolder->savePluginState();

        if (juce::ModalComponentManager::getInstance()->cancelAllModalComponents())
        {
            juce::Timer::callAfterDelay (100, []
            {
                if (auto* app = juce::JUCEApplicationBase::getInstance())
                    app->systemRequestedQuit();
            });
        }
        else
        {
            quit();
        }
    }

private:
    juce::ApplicationProperties appProperties;
    juce::ApplicationCommandManager commandManager;
    std::unique_ptr<MatrixControlStandaloneFilterWindow> mainWindow;
    std::unique_ptr<juce::StandalonePluginHolder> pluginHolder;
};
} // namespace

juce::JUCEApplicationBase* juce_CreateApplication();

juce::JUCEApplicationBase* juce_CreateApplication()
{
    return new MatrixControlStandaloneApp();
}
