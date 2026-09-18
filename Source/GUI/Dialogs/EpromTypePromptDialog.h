#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/Services/DeviceSetupDeviceRow.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace TSS
{
    class ISkin;
}

class EpromTypePromptDialog : public juce::Component,
                              private juce::Timer
{
public:
    static constexpr int kDesignWidth = 420;
    static constexpr int kDesignHeight = 280;

    struct LiveDeviceStatus
    {
        bool deviceDetected = false;
        bool deviceMidiUnresponsive = false;
        MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
        juce::String deviceVersion;
    };

    struct PrepareForShowArgs
    {
        MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
        int preferredSelectedId = 0;
        bool includeFirmwareSuggestionHint = false;
        juce::String midiFromPortId;
        juce::String midiToPortId;
        LiveDeviceStatus deviceStatus;
        std::function<void(int selectedId)> onConfirm;
        std::function<void()> onLater;
        std::function<void(const juce::String& portId)> onMidiFromChanged;
        std::function<void(const juce::String& portId)> onMidiToChanged;
        std::function<void()> onSearchingWindowStarted;
    };

    EpromTypePromptDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested);
    ~EpromTypePromptDialog() override;

    void prepareForShow(PrepareForShowArgs args);
    void updateLiveDeviceStatus(const LiveDeviceStatus& status);
    void refreshEpromSuggestion(MatrixDeviceTypes::Type deviceType, int preferredSelectedId);
    void syncPortsFromHost(const juce::String& midiFromPortId,
                           const juce::String& midiToPortId,
                           bool repopulateLists);

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    struct ContentLayout
    {
        juce::Rectangle<int> bodyTextArea;
        juce::Rectangle<int> controlBand;
        juce::Rectangle<int> buttonRow;
        juce::Rectangle<int> deviceChromeBounds;
    };

    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    ContentLayout computeContentLayout() const;
    juce::String bodyText() const;
    void populateComboItems(MatrixDeviceTypes::Type deviceType, int preferredSelectedId);
    void populateMidiPortLists();
    void wireMidiComboCallbacks();
    void dismissAsLater();
    void confirm();
    void recomputeDeviceRow();
    void applySearchingWindowUpdate(const Core::DeviceSetupSearchingWindowUpdate& update);
    void syncAnimationTimer();
    void paintDeviceChrome(juce::Graphics& g, juce::Rectangle<int> bounds) const;
    juce::String searchingDetailWithDots() const;
    void timerCallback() override;

    std::function<void()> onDismissRequested_;
    std::function<void(int)> onConfirm_;
    std::function<void()> onLater_;
    std::function<void(const juce::String&)> onMidiFromChanged_;
    std::function<void(const juce::String&)> onMidiToChanged_;
    std::function<void()> onSearchingWindowStarted_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool includeFirmwareSuggestionHint_ = false;
    bool suppressMidiCallbacks_ = false;
    bool epromComboTouchedByUser_ = false;
    int searchingDotFrame_ = 0;
    Core::DeviceSetupSearchingWindowState searchingWindow_;
    LiveDeviceStatus liveStatus_;
    Core::DeviceSetupDeviceRowView deviceRowView_;

    std::unique_ptr<TSS::Label> midiFromLabel_;
    std::unique_ptr<TSS::ComboBox> midiFromCombo_;
    std::unique_ptr<TSS::Label> midiToLabel_;
    std::unique_ptr<TSS::ComboBox> midiToCombo_;
    std::unique_ptr<TSS::Label> epromTypeLabel_;
    std::unique_ptr<TSS::ComboBox> epromTypeCombo_;
    std::vector<juce::String> midiFromPortIdentifiers_;
    std::vector<juce::String> midiToPortIdentifiers_;
    juce::TextButton confirmButton_;
    juce::TextButton specifyLaterButton_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;
    inline constexpr static int kLabelWidth_ = 120;
    inline constexpr static int kComboWidth_ = 200;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kRowGap_ = 8;
    inline constexpr static int kSpecifyLaterButtonWidth_ = 120;
    inline constexpr static int kConfirmButtonWidth_ = 80;
    inline constexpr static int kSearchingDotsIntervalMs_ = 450;
    inline constexpr static int kDeviceBadgeHeight_ = 16;
    inline constexpr static int kDeviceBadgePad_ = 6;
    inline constexpr static int kDeviceBadgeGap_ = 6;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EpromTypePromptDialog)
};
