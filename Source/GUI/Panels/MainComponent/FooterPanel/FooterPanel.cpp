#include "FooterPanel.h"


#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"

using TSS::SkinColourId;

const juce::Identifier FooterPanel::kMessageTextId("uiMessageText");
const juce::Identifier FooterPanel::kMessageSeverityId("uiMessageSeverity");

FooterPanel::FooterPanel(TSS::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvtsRef)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts(apvtsRef)
{
    setOpaque(true);
    // Écouter les changements de l'APVTS
    apvts.state.addListener(this);
    
    // Initialiser avec le message actuel s'il existe
    const auto messageTextVar = apvts.state.getProperty(kMessageTextId, juce::String());
    const auto severityStrVar = apvts.state.getProperty(kMessageSeverityId, juce::String());
    
    const juce::String messageText = messageTextVar.toString();
    const juce::String severityStr = severityStrVar.toString();
    
    if (messageText.isNotEmpty())
    {
        currentMessage = messageText;
        currentSeverity = parseSeverity(severityStr);
    }
}

FooterPanel::~FooterPanel()
{
    apvts.state.removeListener(this);
}

void FooterPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kFooterPanelBackground));

    const float systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this);
    const float borderThickness = TSS::ScaledDrawing::snappedStrokeThicknessFromDesign(
        static_cast<float>(TSS::Design::Panels::kPanelEdgeBorderThickness),
        uiScale_,
        systemDisplayScale,
        TSS::ScaledDrawing::StrokeSnapPolicy::kRound);

    auto borderLine = getLocalBounds().toFloat().removeFromTop(borderThickness);
    g.setColour(skin_->getColour(SkinColourId::kVerticalSeparatorLine));
    g.fillRect(borderLine);

    if (currentMessage.isEmpty() || currentSeverity == MessageSeverity::None)
        return;

    const int padding = juce::jmax(1, juce::roundToInt(static_cast<float>(kPadding_) * uiScale_));
    const int iconSize = juce::jmax(1, juce::roundToInt(static_cast<float>(kIconSize_) * uiScale_));
    
    auto bounds = getLocalBounds().reduced(padding);
    
    g.setColour(getSeverityColour(currentSeverity));
    g.setFont(skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_));
    
    const juce::String icon = getSeverityIcon(currentSeverity);
    if (icon.isNotEmpty())
    {
        const auto iconBounds = bounds.removeFromLeft(iconSize + padding);
        g.drawText(icon, iconBounds, juce::Justification::centredLeft);
    }
    
    g.drawText(currentMessage, bounds, juce::Justification::centredLeft);
}

void FooterPanel::resized()
{
}

void FooterPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
}

void FooterPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    repaint();
}

void FooterPanel::valueTreePropertyChanged(juce::ValueTree& tree,
                                          const juce::Identifier& property)
{
    if (property == kMessageTextId || property == kMessageSeverityId)
    {
        const auto messageText = tree.getProperty(kMessageTextId, juce::String()).toString();
        const auto severityStr = tree.getProperty(kMessageSeverityId, juce::String()).toString();
        
        currentMessage = messageText;
        currentSeverity = parseSeverity(severityStr);
        
        repaint();
    }
}

FooterPanel::MessageSeverity FooterPanel::parseSeverity(const juce::String& severityStr) const
{
    if (severityStr == "info")
        return MessageSeverity::Info;
    if (severityStr == "success")
        return MessageSeverity::Success;
    if (severityStr == "warning")
        return MessageSeverity::Warning;
    if (severityStr == "error")
        return MessageSeverity::Error;
    
    return MessageSeverity::None;
}

juce::Colour FooterPanel::getSeverityColour(MessageSeverity severity) const
{
    switch (severity)
    {
        case MessageSeverity::None:
            return skin_->getColour(SkinColourId::kLabelText);
        case MessageSeverity::Info:
            return juce::Colour(0xFF808080);  // Gris
        case MessageSeverity::Success:
            return juce::Colour(0xFF00FF00);  // Vert
        case MessageSeverity::Warning:
            return juce::Colour(0xFFFF8800);  // Orange
        case MessageSeverity::Error:
            return juce::Colour(0xFFFF0000);  // Rouge
        default:
            return skin_->getColour(SkinColourId::kLabelText);
    }
}

juce::String FooterPanel::getSeverityIcon(MessageSeverity severity) const
{
    switch (severity)
    {
        case MessageSeverity::None:
            return juce::String();
        case MessageSeverity::Info:
            return "ℹ";
        case MessageSeverity::Success:
            return "✓";
        case MessageSeverity::Warning:
            return "⚠";
        case MessageSeverity::Error:
            return "✗";
        default:
            return juce::String();
    }
}

