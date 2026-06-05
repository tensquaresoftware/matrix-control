#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

#include "GUI/Widgets/ActivityLed.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"

namespace tss
{
    class ISkin;
    class Label;
    class ComboBox;
}

class HeaderPanel : public juce::Component
{
public:
    static constexpr int kPortSentinelItemId = 0;

    HeaderPanel(tss::ISkin& skin, int width, int height);
    ~HeaderPanel() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPlugin);

    void populateMidiPortLists();
    void refreshPortLists() { populateMidiPortLists(); }

    void populateAudioFromComboForPlugin();
    void populateAudioFromComboForStandalone(const juce::StringArray& channelNames,
                                              const juce::StringArray& channelIds);

    juce::String getSelectedMidiFromPortIdentifier() const;
    juce::String getSelectedMidiToPortIdentifier() const;
    juce::String getSelectedKeyboardFromPortIdentifier() const;
    int getSelectedAudioFromChannelMode() const;
    juce::String getSelectedAudioFromSourceId() const;

    void selectAudioFromChannelMode(int mode);
    void selectAudioFromSourceId(const juce::String& sourceId);

    void selectMidiFromPort(const juce::String& deviceId);
    void selectMidiToPort(const juce::String& deviceId);
    void selectKeyboardFromPort(const juce::String& deviceId);

    tss::ComboBox& getMidiFromComboBox() { return midiFromComboBox_; }
    tss::ComboBox& getMidiToComboBox() { return midiToComboBox_; }
    tss::ComboBox& getKeyboardFromComboBox() { return keyboardFromComboBox_; }
    tss::ComboBox& getSkinComboBox() { return skinComboBox_; }
    tss::ComboBox& getUiScaleComboBox() { return uiScaleComboBox_; }
    tss::Button& getUiElementsButton() { return uiElementsButton_; }
    tss::ComboBox& getAudioFromComboBox() { return audioFromComboBox_; }
    tss::Slider& getInputGainSlider() { return inputGainSlider_; }
    tss::PeakIndicator& getPeakIndicator() { return peakIndicator_; }
    tss::ActivityLed& getInstrumentActivityLed() { return instrumentActivityLed_; }
    tss::ActivityLed& getEditorActivityLed() { return editorActivityLed_; }
    tss::ActivityLed& getMidiToActivityLed() { return midiToActivityLed_; }

    static int getGap() { return kGap_; }

private:
    void populateInputPortCombo(tss::ComboBox& combo, std::vector<juce::String>& identifiers);
    void populateOutputPortCombo(tss::ComboBox& combo, std::vector<juce::String>& identifiers);
    void configurePluginModeKeyboardFrom();
    void configureStandaloneKeyboardFrom();
    void configurePluginModeAudioFrom();
    void configureStandaloneAudioFrom(const juce::StringArray& channelNames,
                                      const juce::StringArray& channelIds);
    int findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                const juce::String& deviceId) const;
    juce::String getSelectedPortIdentifier(const tss::ComboBox& combo,
                                           const std::vector<juce::String>& identifiers) const;

    int width_;
    int height_;
    inline constexpr static int kMidiFromLabelWidth_ = 72;
    inline constexpr static int kMidiToLabelWidth_ = 56;
    inline constexpr static int kKeyboardFromLabelWidth_ = 108;
    inline constexpr static int kAudioFromLabelWidth_ = 88;
    inline constexpr static int kInputGainLabelWidth_ = 88;
    inline constexpr static int kPortComboBoxWidth_ = 96;
    inline constexpr static int kInputGainSliderWidth_ = 60;
    inline constexpr static int kPeakIndicatorWidth_ = 12;
    inline constexpr static int kActivityLedSize_ = 12;
    inline constexpr static int kSkinLabelWidth_ = 30;
    inline constexpr static int kUiScaleLabelWidth_ = 35;
    inline constexpr static int kScaleComboBoxWidth_ = 50;
    inline constexpr static int kSkinComboBoxWidth_ = 50;
    inline constexpr static int kGap_ = 5;
    inline constexpr static int kPacketExternalGap_ = kGap_ * 2;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kRightPadding_ = 15;
    inline constexpr static int kUiElementsButtonWidth_ = 88;

    tss::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;

    tss::Label midiFromLabel_;
    tss::ComboBox midiFromComboBox_;
    tss::ActivityLed editorActivityLed_;
    tss::Label midiToLabel_;
    tss::ComboBox midiToComboBox_;
    tss::ActivityLed midiToActivityLed_;
    tss::Label keyboardFromLabel_;
    tss::ComboBox keyboardFromComboBox_;
    tss::ActivityLed instrumentActivityLed_;
    tss::Label audioFromLabel_;
    tss::ComboBox audioFromComboBox_;
    tss::Label inputGainLabel_;
    tss::Slider inputGainSlider_;
    tss::PeakIndicator peakIndicator_;
    tss::Label skinLabel_;
    tss::ComboBox skinComboBox_;
    tss::Label uiScaleLabel_;
    tss::ComboBox uiScaleComboBox_;
    tss::Button uiElementsButton_;

    std::vector<juce::String> midiFromPortIdentifiers_;
    std::vector<juce::String> midiToPortIdentifiers_;
    std::vector<juce::String> keyboardFromPortIdentifiers_;
    std::vector<juce::String> audioFromSourceIdentifiers_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
