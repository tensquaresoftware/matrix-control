#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"

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

    juce::String getSelectedMidiFromPortIdentifier() const;
    juce::String getSelectedMidiToPortIdentifier() const;
    juce::String getSelectedKeyboardFromPortIdentifier() const;

    void selectMidiFromPort(const juce::String& deviceId);
    void selectMidiToPort(const juce::String& deviceId);
    void selectKeyboardFromPort(const juce::String& deviceId);

    tss::ComboBox& getMidiFromComboBox() { return midiFromComboBox_; }
    tss::ComboBox& getMidiToComboBox() { return midiToComboBox_; }
    tss::ComboBox& getKeyboardFromComboBox() { return keyboardFromComboBox_; }
    tss::ComboBox& getSkinComboBox() { return skinComboBox_; }
    tss::ComboBox& getUiScaleComboBox() { return uiScaleComboBox_; }
    tss::Button& getUiElementsButton() { return uiElementsButton_; }

    static int getGap() { return kGap_; }

private:
    void populateInputPortCombo(tss::ComboBox& combo, std::vector<juce::String>& identifiers);
    void populateOutputPortCombo(tss::ComboBox& combo, std::vector<juce::String>& identifiers);
    void configurePluginModeKeyboardFrom();
    void configureStandaloneKeyboardFrom();
    int findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                const juce::String& deviceId) const;
    juce::String getSelectedPortIdentifier(const tss::ComboBox& combo,
                                           const std::vector<juce::String>& identifiers) const;

    int width_;
    int height_;
    inline constexpr static int kMidiFromLabelWidth_ = 72;
    inline constexpr static int kMidiToLabelWidth_ = 56;
    inline constexpr static int kKeyboardFromLabelWidth_ = 108;
    inline constexpr static int kMidiPortComboBoxWidth_ = 96;
    inline constexpr static int kSkinLabelWidth_ = 30;
    inline constexpr static int kUiScaleLabelWidth_ = 35;
    inline constexpr static int kScaleComboBoxWidth_ = 50;
    inline constexpr static int kSkinComboBoxWidth_ = 50;
    inline constexpr static int kGap_ = 5;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLeftPadding_ = 15;
    inline constexpr static int kRightPadding_ = 15;
    inline constexpr static int kUiElementsButtonWidth_ = 88;

    tss::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;

    tss::Label midiFromLabel_;
    tss::ComboBox midiFromComboBox_;
    tss::Label midiToLabel_;
    tss::ComboBox midiToComboBox_;
    tss::Label keyboardFromLabel_;
    tss::ComboBox keyboardFromComboBox_;
    tss::Label skinLabel_;
    tss::ComboBox skinComboBox_;
    tss::Label uiScaleLabel_;
    tss::ComboBox uiScaleComboBox_;
    tss::Button uiElementsButton_;

    std::vector<juce::String> midiFromPortIdentifiers_;
    std::vector<juce::String> midiToPortIdentifiers_;
    std::vector<juce::String> keyboardFromPortIdentifiers_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
