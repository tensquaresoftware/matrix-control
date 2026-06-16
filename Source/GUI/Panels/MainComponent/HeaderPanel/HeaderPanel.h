#pragma once

#include <functional>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Logo.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"

#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    class ISkin;
    class Label;
    class ComboBox;
}

class HeaderPanel : public juce::Component
{
public:
    static constexpr int kPortSentinelItemId = 1;
    static constexpr int kFirstDeviceItemId = 2;

    HeaderPanel(TSS::ISkin& skin, int width, int height);
    ~HeaderPanel() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPlugin);

    void populateMidiPortLists();
    void refreshPortLists() { populateMidiPortLists(); }

    void setCurrentSkinItemId(int skinItemId) { currentSkinItemId_ = skinItemId; }
    void setCurrentUiScaleId(int scaleId) { currentUiScaleId_ = scaleId; }

    std::function<void(int skinItemId)> onSkinSelected;
    std::function<void(int scaleId)> onUiScaleSelected;
    std::function<void()> onUiScaleReset;
    std::function<void()> onSettingsRequested;
    std::function<void()> onAboutRequested;
    std::function<void()> onUiTestsToggleRequested;

    juce::String getSelectedMidiFromPortIdentifier() const;
    juce::String getSelectedMidiToPortIdentifier() const;
    juce::String getSelectedKeyboardFromPortIdentifier() const;

    void selectMidiFromPort(const juce::String& deviceId);
    void selectMidiToPort(const juce::String& deviceId);
    void selectKeyboardFromPort(const juce::String& deviceId);

    void populateAudioFromCombo(const juce::StringArray& channelNames,
                                const juce::StringArray& channelIds);
    juce::String getSelectedAudioFromSourceId() const;
    void selectAudioFromSourceId(const juce::String& sourceId);

    TSS::ComboBox& getMidiFromComboBox() { return midiFromComboBox_; }
    TSS::ComboBox& getMidiToComboBox() { return midiToComboBox_; }
    TSS::ComboBox& getKeyboardFromComboBox() { return keyboardFromComboBox_; }
    TSS::Led& getInstrumentActivityLed() { return instrumentActivityLed_; }
    TSS::Led& getEditorActivityLed() { return editorActivityLed_; }
    TSS::Led& getMidiToActivityLed() { return midiToActivityLed_; }
    TSS::ComboBox& getAudioFromComboBox() { return audioFromComboBox_; }
    TSS::Slider& getInputGainSlider() { return inputGainSlider_; }
    TSS::PeakIndicator& getPeakIndicator() { return peakIndicator_; }

    static int getGap() { return kGap_; }

private:
    void showLogoPopup();

    void populateInputPortCombo(TSS::ComboBox& combo, std::vector<juce::String>& identifiers);
    void populateOutputPortCombo(TSS::ComboBox& combo, std::vector<juce::String>& identifiers);
    void updateKeyboardFromVisibility();
    void updateAudioControlsVisibility();
    void configureStandaloneKeyboardFrom();
    int findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                const juce::String& deviceId) const;
    juce::String getSelectedPortIdentifier(const TSS::ComboBox& combo,
                                           const std::vector<juce::String>& identifiers) const;

    int width_;
    int height_;
    inline constexpr static int kEditorMidiFromLabelWidth_ = 88;
    inline constexpr static int kMidiToLabelWidth_ = 40;
    inline constexpr static int kKeyboardFromLabelWidth_ = 84;
    inline constexpr static int kAudioFromLabelWidth_ = 64;
    inline constexpr static int kInputGainLabelWidth_ = 64;
    inline constexpr static int kPortComboBoxWidth_ = 112;
    inline constexpr static int kInputGainSliderWidth_ = 60;
    inline constexpr static int kPeakIndicatorWidth_ = 12;
    inline constexpr static int kLedSize_ = 12;
    inline constexpr static int kGap_ = 4;
    inline constexpr static int kPacketExternalGap_ = kGap_ * 4;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLeftPadding_ = 12;

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;
    int currentSkinItemId_ = static_cast<int>(TSS::Skin::SkinComboBoxItemId::kBlack);
    int currentUiScaleId_ = PluginIDs::Settings::ScaleLevels::kDefault;

    TSS::Logo logo_;
    TSS::Label midiFromLabel_;
    TSS::ComboBox midiFromComboBox_;
    TSS::Led editorActivityLed_;
    TSS::Label midiToLabel_;
    TSS::ComboBox midiToComboBox_;
    TSS::Led midiToActivityLed_;
    TSS::Label keyboardFromLabel_;
    TSS::ComboBox keyboardFromComboBox_;
    TSS::Led instrumentActivityLed_;
    TSS::Label audioFromLabel_;
    TSS::ComboBox audioFromComboBox_;
    TSS::Label inputGainLabel_;
    TSS::Slider inputGainSlider_;
    TSS::PeakIndicator peakIndicator_;

    std::vector<juce::String> midiFromPortIdentifiers_;
    std::vector<juce::String> midiToPortIdentifiers_;
    std::vector<juce::String> keyboardFromPortIdentifiers_;
    std::vector<juce::String> audioFromSourceIdentifiers_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
