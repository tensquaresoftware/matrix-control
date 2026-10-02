#pragma once

#include <functional>
#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "Core/Audio/AudioDevicePreferredSetup.h"

namespace TSS
{
    class ISkin;
}

class SettingsCloseButton;

/** Matrix-skinned standalone Audio Settings (no MIDI / mute / feedback banner UI). */
class AudioMidiSettingsWindow : public juce::Component,
                                private juce::Timer,
                                private juce::ChangeListener
{
public:
    struct Config
    {
        TSS::ISkin* skin = nullptr;
        juce::AudioDeviceManager* deviceManager = nullptr;
        int maxInputChannels = 0;
        int maxOutputChannels = 0;
        std::function<float()> peakLevelProvider;
        std::function<void()> onCloseRequested;
    };

    explicit AudioMidiSettingsWindow(Config config);
    ~AudioMidiSettingsWindow() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    void timerCallback() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void syncPreferredSetupFromDeviceManager();
    void playTestSound();

    std::function<void()> onCloseRequested_;
    std::function<float()> peakLevelProvider_;
    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;

    juce::AudioDeviceManager& deviceManager_;
    Core::AudioDevicePreferredSetup preferred_;
    Core::AudioDeviceIdentity lastDeviceIdentity_;
    bool restoringSetup_ = false;
    bool hasSeededDeviceIdentity_ = false;

    std::unique_ptr<SettingsCloseButton> closeButton_;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> deviceSelector_;
    std::unique_ptr<TSS::Button> testButton_;
    std::unique_ptr<TSS::PeakIndicator> peakIndicator_;

    inline constexpr static int kDesignWidth_ = 520;
    inline constexpr static int kDesignContentHeight_ = 420;
    inline constexpr static int kPeakWidth_ = 12;
    inline constexpr static int kPeakHeight_ = 48;
    inline constexpr static int kFooterControlsHeight_ = 56;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMidiSettingsWindow)
};
