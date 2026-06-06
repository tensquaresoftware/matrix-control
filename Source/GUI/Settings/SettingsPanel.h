#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"

namespace tss
{
    class ISkin;
}

class SettingsPanel : public juce::Component
{
public:
    static constexpr int kDesignWidth = 480;
    static constexpr int kDesignHeight = 320;

    SettingsPanel(tss::ISkin& skin, bool isPluginMode);
    ~SettingsPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPluginMode);

    void populateAudioFromCombo(const juce::StringArray& channelNames,
                                const juce::StringArray& channelIds);
    juce::String getSelectedAudioFromSourceId() const;
    void selectAudioFromSourceId(const juce::String& sourceId);

    tss::ComboBox& getSkinComboBox() { return skinComboBox_; }
    tss::ComboBox& getUiScaleComboBox() { return uiScaleComboBox_; }
    tss::Slider& getHardwareLatencySlider() { return hardwareLatencySlider_; }
    tss::ComboBox& getAudioFromComboBox() { return audioFromComboBox_; }
    tss::Slider& getInputGainSlider() { return inputGainSlider_; }
    tss::PeakIndicator& getPeakIndicator() { return peakIndicator_; }

private:
    void updateModeSpecificVisibility();
    void layoutContent(juce::Rectangle<int> bounds);

    static constexpr int kPortSentinelItemId = 0;
    inline constexpr static int kPadding_ = 16;
    inline constexpr static int kGap_ = 8;
    inline constexpr static int kRowGap_ = 12;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLabelWidth_ = 120;
    inline constexpr static int kComboWidth_ = 60;
    inline constexpr static int kSliderWidth_ = 80;
    inline constexpr static int kAudioFromComboWidth_ = 160;
    inline constexpr static int kPeakIndicatorWidth_ = 12;

    tss::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;

    tss::Label skinLabel_;
    tss::ComboBox skinComboBox_;
    tss::Label uiScaleLabel_;
    tss::ComboBox uiScaleComboBox_;
    tss::Label hardwareLatencyLabel_;
    tss::Slider hardwareLatencySlider_;
    tss::Label audioFromLabel_;
    tss::ComboBox audioFromComboBox_;
    tss::Label inputGainLabel_;
    tss::Slider inputGainSlider_;
    tss::PeakIndicator peakIndicator_;
    tss::Label masterOpsLabel_;
    tss::Label masterOpsPlaceholder_;
    tss::Label policiesLabel_;
    tss::Label policiesPlaceholder_;
    tss::Label defragLabel_;
    tss::Label defragPlaceholder_;
    tss::Label loggingLabel_;
    tss::Label loggingPlaceholder_;

    std::vector<juce::String> audioFromSourceIdentifiers_;

    int findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                const juce::String& deviceId) const;
    juce::String getSelectedPortIdentifier(const tss::ComboBox& combo,
                                           const std::vector<juce::String>& identifiers) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};
