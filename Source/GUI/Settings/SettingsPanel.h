#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"

namespace TSS
{
    class ISkin;
}

class SettingsPanel : public juce::Component
{
public:
    static constexpr int kDesignWidth = 480;
    static constexpr int kDesignHeight = 244;

    SettingsPanel(TSS::ISkin& skin, bool isPluginMode);
    ~SettingsPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPluginMode);

    void populateAudioFromCombo(const juce::StringArray& channelNames,
                                const juce::StringArray& channelIds);
    juce::String getSelectedAudioFromSourceId() const;
    void selectAudioFromSourceId(const juce::String& sourceId);

    TSS::Slider& getHardwareLatencySlider() { return hardwareLatencySlider_; }
    TSS::ComboBox& getAudioFromComboBox() { return audioFromComboBox_; }
    TSS::Slider& getInputGainSlider() { return inputGainSlider_; }
    TSS::PeakIndicator& getPeakIndicator() { return peakIndicator_; }

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

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;

    TSS::Label hardwareLatencyLabel_;
    TSS::Slider hardwareLatencySlider_;
    TSS::Label audioFromLabel_;
    TSS::ComboBox audioFromComboBox_;
    TSS::Label inputGainLabel_;
    TSS::Slider inputGainSlider_;
    TSS::PeakIndicator peakIndicator_;
    TSS::Label masterOpsLabel_;
    TSS::Label masterOpsPlaceholder_;
    TSS::Label policiesLabel_;
    TSS::Label policiesPlaceholder_;
    TSS::Label defragLabel_;
    TSS::Label defragPlaceholder_;
    TSS::Label loggingLabel_;
    TSS::Label loggingPlaceholder_;

    std::vector<juce::String> audioFromSourceIdentifiers_;

    int findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                const juce::String& deviceId) const;
    juce::String getSelectedPortIdentifier(const TSS::ComboBox& combo,
                                           const std::vector<juce::String>& identifiers) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};
