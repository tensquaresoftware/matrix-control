#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace TSS
{
    class ISkin;
}

class FooterPanel : public juce::Component,
                    public juce::ValueTree::Listener
{
public:
    FooterPanel(TSS::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvtsRef);
    ~FooterPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void valueTreePropertyChanged(juce::ValueTree& tree,
                                 const juce::Identifier& property) override;

private:
    enum class MessageSeverity
    {
        None,
        Info,
        Success,
        Warning,
        Error
    };
    
    int width_;
    int height_;
    inline constexpr static int kPadding_ = 8;
    inline constexpr static int kIconSize_ = 14;
    
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts;
    
    juce::String currentMessage;
    MessageSeverity currentSeverity = MessageSeverity::None;
    bool deviceDetected_ = false;
    juce::String deviceType_;
    juce::String deviceVersion_;
    
    static const juce::Identifier kMessageTextId;
    static const juce::Identifier kMessageSeverityId;
    static const juce::Identifier kDeviceDetectedId;
    static const juce::Identifier kDeviceTypeId;
    static const juce::Identifier kDeviceVersionId;
    
    MessageSeverity parseSeverity(const juce::String& severityStr) const;
    juce::Colour getSeverityColour(MessageSeverity severity) const;
    juce::String getSeverityIcon(MessageSeverity severity) const;
    juce::String buildDeviceIdentityText() const;
    void syncFromApvtsState(juce::ValueTree& tree);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
};
