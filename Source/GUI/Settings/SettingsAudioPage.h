#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Settings/AudioDeviceSetupSync.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/RadioButtonGroup.h"

namespace TSS
{
    class ISkin;
}

/** Standalone Settings AUDIO page: devices, channel pairs, SYNTH FROM, test tone. */
class SettingsAudioPage : public juce::Component,
                          private juce::Timer,
                          private juce::ChangeListener
{
public:
    struct Config
    {
        TSS::ISkin* skin = nullptr;
        juce::AudioDeviceManager* deviceManager = nullptr;
        std::function<float()> peakLevelProvider;
        std::function<void()> onSynthFromChanged;
    };

    explicit SettingsAudioPage(Config config);
    ~SettingsAudioPage() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setVisible(bool shouldBeVisible) override;
    /** Start/stop peak timer + device ChangeListener while Settings is shown. */
    void setMonitoringActive(bool shouldBeActive);

    void populateSynthFromCombo(const juce::StringArray& channelNames,
                                const juce::StringArray& channelIds);
    juce::String getSelectedSynthFromSourceId() const;
    void selectSynthFromSourceId(const juce::String& sourceId);

    TSS::ComboBox& getSynthFromCombo() { return *synthFromCombo_; }

    void registerContextualHelp(TSS::ContextualHelpBinder& binder);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    static constexpr int kPortSentinelItemId = 1;
    static constexpr int kFirstDeviceItemId = 2;

    void timerCallback() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void buildWidgets();
    void wireCallbacks();
    void refreshAllFromDeviceManager();
    void refreshDriverTypeCombo();
    void refreshDeviceCombos();
    void refreshSampleRateAndBufferCombos();
    void refreshChannelGroups();
    void applySetupFromUi();
    void applyChannelPair(bool isInput, int pairIndex);
    void playTestSound();

    std::unique_ptr<TSS::Label> makeLabel(TSS::ISkin& skin, const juce::String& text);
    std::unique_ptr<TSS::ComboBox> makeCombo(TSS::ISkin& skin);

    Config config_;
    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    juce::AudioDeviceManager& deviceManager_;
    AudioDeviceSetupSync::SyncState syncState_;
    bool updatingUi_ = false;
    bool monitoringActive_ = false;
    enum class EndpointChange { kNone, kInput, kOutput };
    EndpointChange pendingEndpointChange_ = EndpointChange::kNone;

    std::unique_ptr<TSS::Label> driverTypeLabel_;
    std::unique_ptr<TSS::ComboBox> driverTypeCombo_;
    std::unique_ptr<TSS::Label> inputDeviceLabel_;
    std::unique_ptr<TSS::ComboBox> inputDeviceCombo_;
    std::unique_ptr<TSS::Label> outputDeviceLabel_;
    std::unique_ptr<TSS::ComboBox> outputDeviceCombo_;
    std::unique_ptr<TSS::Label> sampleRateLabel_;
    std::unique_ptr<TSS::ComboBox> sampleRateCombo_;
    std::unique_ptr<TSS::Label> bufferSizeLabel_;
    std::unique_ptr<TSS::ComboBox> bufferSizeCombo_;
    std::unique_ptr<TSS::Label> inputChannelsLabel_;
    std::unique_ptr<TSS::RadioButtonGroup> inputChannelsGroup_;
    std::unique_ptr<TSS::Label> synthFromLabel_;
    std::unique_ptr<TSS::ComboBox> synthFromCombo_;
    std::unique_ptr<TSS::PeakIndicator> peakIndicator_;
    std::unique_ptr<TSS::Label> outputChannelsLabel_;
    std::unique_ptr<TSS::RadioButtonGroup> outputChannelsGroup_;
    std::unique_ptr<TSS::Button> playTestToneButton_;

    std::vector<juce::String> synthFromSourceIdentifiers_;
    juce::StringArray sampleRateValues_;
    juce::Array<int> bufferSizeValues_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsAudioPage)
};
