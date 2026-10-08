#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/Services/DeviceSetupDeviceRow.h"
#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Dialogs/GettingStartedWizardFlow.h"
#include "GUI/Settings/SettingsAudioPage.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ReadOnlyValueField.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace TSS
{
    class ISkin;
}

/** GETTING STARTED wizard: Matrix monochrome chrome (pass Black skin; ignore Cream),
    live step controls, nav marking durable flags. */
class GettingStartedWizardDialog : public juce::Component,
                                   private juce::Timer
{
public:
    struct LiveDeviceStatus
    {
        bool deviceDetected = false;
        bool deviceMidiUnresponsive = false;
        MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
        juce::String deviceVersion;
    };

    struct HostBindings
    {
        int scaleId = 0;
        int skinId = 0;
        juce::String midiFromPortId;
        juce::String midiToPortId;
        juce::String keyboardFromPortId;
        int preferredEpromTypeId = 0;
        bool includeFirmwareSuggestionHint = false;
        bool useAudioResumeCopy = false;
        LiveDeviceStatus deviceStatus;
        juce::AudioDeviceManager* audioDeviceManager = nullptr;
        std::function<float()> peakLevelProvider;
        juce::StringArray synthFromChannelNames;
        juce::StringArray synthFromChannelIds;
        juce::String selectedSynthFromSourceId;

        std::function<void(int scaleId)> onScaleChanged;
        std::function<void(int skinId)> onSkinChanged;
        std::function<void(const juce::String& portId)> onMidiFromChanged;
        std::function<void(const juce::String& portId)> onMidiToChanged;
        std::function<void(int epromTypeId)> onEpromChanged;
        std::function<void()> onSearchingWindowStarted;
        std::function<void(const juce::String& portId)> onKeyboardFromChanged;
        std::function<void(const juce::String& sourceId)> onSynthFromChanged;
        std::function<void(GettingStartedWizard::Step step)> onContentStepCompleted;
        std::function<void()> onConfigureLater;
        std::function<void()> onContinuedFromIntro;
    };

    GettingStartedWizardDialog(TSS::ISkin& skin,
                               bool isPluginMode,
                               std::function<void()> onDismissRequested);

    ~GettingStartedWizardDialog() override;

    void prepareForShow(GettingStartedWizard::Step startStep, HostBindings bindings);
    /** Stop SEARCHING animation / inquiry timers (peer close must not leave Timer running). */
    void stopLiveTimers();
    void updateLiveDeviceStatus(const LiveDeviceStatus& status);
    void refreshEpromSuggestion(MatrixDeviceTypes::Type deviceType, int preferredSelectedId);
    void syncPortsFromHost(const juce::String& midiFromPortId,
                           const juce::String& midiToPortId,
                           bool repopulateLists);
    void syncKeyboardFromHost(const juce::String& keyboardFromPortId, bool repopulateLists);
    void populateSynthFromChannels(const juce::StringArray& names,
                                   const juce::StringArray& ids,
                                   const juce::String& selectedId);
    SettingsAudioPage* getAudioPage() const noexcept { return audioPage_.get(); }

    GettingStartedWizard::Step getCurrentStep() const noexcept { return step_; }

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    static constexpr size_t kNavButtonCount = 6;
    static constexpr int kLabelWidth_ = 92;
    static constexpr int kComboWidth_ = 140;
    static constexpr int kControlHeight_ = 20;
    static constexpr int kRowGap_ = 8;
    static constexpr int kSearchingDotsIntervalMs_ = 450;

    DialogMatrixHelpers::ModalGeometry computeGeometry() const;
    juce::Rectangle<int> controlBandBounds(const DialogMatrixHelpers::ModalGeometry& geometry) const;
    TSS::Button& buttonFor(GettingStartedWizard::NavButton button) const;
    void showStep(GettingStartedWizard::Step step);
    void handleButton(GettingStartedWizard::NavButton button);
    void requestDismiss();
    void buildStepControls(TSS::ISkin& skin);
    void applyControlLooks(TSS::ISkin& skin);
    void updateControlVisibility();
    void layoutStepControls(juce::Rectangle<int> band);
    void placeControlRow(juce::Rectangle<int> band,
                         int rowIndex,
                         TSS::Label& label,
                         juce::Component& field);
    void populateScaleAndSkinItems();
    void populateMidiPortLists();
    void populateEpromItems(MatrixDeviceTypes::Type deviceType, int preferredSelectedId);
    void wireControlCallbacks();
    void wireAppearanceControlCallbacks();
    void wireMidiControlCallbacks();
    void ensureAudioPage();
    void recomputeDeviceRow();
    void applySearchingWindowUpdate(const Core::DeviceSetupSearchingWindowUpdate& update);
    void refreshDeviceValueField();
    void syncAnimationTimer();
    void timerCallback() override;
    juce::String bodyText() const;

    std::function<void()> onDismissRequested_;
    HostBindings bindings_;
    TSS::ISkin* skin_;
    bool isPluginMode_;
    float uiScale_ = 1.0f;
    GettingStartedWizard::Step step_ = GettingStartedWizard::Step::kIntro;
    std::array<std::unique_ptr<TSS::Button>, kNavButtonCount> buttons_;

    bool suppressControlCallbacks_ = false;
    bool epromComboTouchedByUser_ = false;
    bool includeFirmwareSuggestionHint_ = false;
    bool useAudioResumeCopy_ = false;
    int searchingDotFrame_ = 0;
    Core::DeviceSetupSearchingWindowState searchingWindow_;
    LiveDeviceStatus liveStatus_;
    Core::DeviceSetupDeviceRowView deviceRowView_;

    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::ComboBox> scaleCombo_;
    std::unique_ptr<TSS::Label> skinLabel_;
    std::unique_ptr<TSS::ComboBox> skinCombo_;

    std::unique_ptr<TSS::Label> midiFromLabel_;
    std::unique_ptr<TSS::ComboBox> midiFromCombo_;
    std::unique_ptr<TSS::Label> midiToLabel_;
    std::unique_ptr<TSS::ComboBox> midiToCombo_;
    std::unique_ptr<TSS::Label> deviceLabel_;
    std::unique_ptr<TSS::ReadOnlyValueField> deviceValueField_;
    std::unique_ptr<TSS::Label> epromTypeLabel_;
    std::unique_ptr<TSS::ComboBox> epromTypeCombo_;
    std::vector<juce::String> midiFromPortIdentifiers_;
    std::vector<juce::String> midiToPortIdentifiers_;

    std::unique_ptr<TSS::Label> keyboardFromLabel_;
    std::unique_ptr<TSS::ComboBox> keyboardFromCombo_;
    std::vector<juce::String> keyboardFromPortIdentifiers_;

    std::unique_ptr<SettingsAudioPage> audioPage_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GettingStartedWizardDialog)
};
