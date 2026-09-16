#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "GUI/Helpers/ContextualHelpOverlay.h"
#include "GUI/Layout/PanelDimensions.h"

namespace TSS
{
    class ISkin;
    class ContextualHelpBinder;
}

class FooterPanel : public juce::Component,
                    public juce::ValueTree::Listener
{
public:
    FooterPanel(TSS::ISkin& skin,
                const FooterPanelDimensions& dimensions,
                juce::AudioProcessorValueTreeState& apvtsRef);
    ~FooterPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setMidiQueuePressureAlert(bool active);

    /**
     * Display-only left-band overlay; never writes APVTS sticky properties.
     * Returns a monotonic epoch so callers can clear only if they still own the overlay.
     */
    int setContextualHelpOverlay(const juce::String& detailText);
    void clearContextualHelpOverlay();
    void clearContextualHelpOverlayIfEpoch(int epoch);

    void valueTreePropertyChanged(juce::ValueTree& tree,
                                 const juce::Identifier& property) override;

private:
    enum class MessageSeverity
    {
        None,
        Info,
        Warning,
        Error
    };

    struct FooterBandLayout
    {
        juce::Rectangle<int> leftBand;
        juce::Rectangle<int> centreBand;
        juce::Rectangle<int> rightBand;
        int padding = 0;
    };

    struct BadgeDetailPaintArgs
    {
        juce::Rectangle<int> bounds;
        juce::String badgeLabel;
        juce::String detailText;
        juce::Colour badgeFill;
        juce::Colour badgeTextColour;
        juce::Colour detailColour;
        juce::Font font;
    };

    FooterPanelDimensions dimensions_;

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
    juce::String getSeverityPrefix(MessageSeverity severity) const;
    juce::String buildDeviceDetailText() const;
    FooterBandLayout computeBandLayout() const;
    void paintBadgeAndDetail(juce::Graphics& g, const BadgeDetailPaintArgs& args) const;
    void paintStatusMessage(juce::Graphics& g,
                            juce::Rectangle<int> bounds,
                            const juce::Font& font,
                            juce::Colour detailColour) const;
    void paintDeviceStatus(juce::Graphics& g,
                           juce::Rectangle<int> bounds,
                           const juce::Font& font,
                           juce::Colour detailColour) const;
    void paintMidiQueuePressureAlert(juce::Graphics& g,
                                     juce::Rectangle<int> bounds,
                                     const juce::Font& font,
                                     juce::Colour detailColour) const;
    void paintContextualHelp(juce::Graphics& g,
                             juce::Rectangle<int> bounds,
                             const juce::Font& font) const;
    void syncFromApvtsState(juce::ValueTree& tree);
    void updateDeviceHitAreaBounds();
    void registerDeviceContextualHelp();

    bool midiQueuePressureAlertActive_ = false;
    TSS::ContextualHelpOverlay contextualHelpOverlay_;
    int contextualHelpEpoch_ = 0;
    juce::Component deviceHitArea_;
    std::unique_ptr<TSS::ContextualHelpBinder> contextualHelpBinder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
};
