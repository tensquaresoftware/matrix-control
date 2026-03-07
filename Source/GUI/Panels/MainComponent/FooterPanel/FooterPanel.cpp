#include "FooterPanel.h"


#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"

using tss::SkinColourId;

const juce::Identifier FooterPanel::kMessageTextId("uiMessageText");
const juce::Identifier FooterPanel::kMessageSeverityId("uiMessageSeverity");

FooterPanel::FooterPanel(tss::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvtsRef)
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
    
    if (currentMessage.isEmpty() || currentSeverity == MessageSeverity::None)
        return;
    
    auto bounds = getLocalBounds().reduced(kPadding_);
    
    // Couleur du texte selon la sévérité
    g.setColour(getSeverityColour(currentSeverity));
    g.setFont(skin_->getBaseFont());
    
    // Icône (optionnel)
    const juce::String icon = getSeverityIcon(currentSeverity);
    if (icon.isNotEmpty())
    {
        const auto iconBounds = bounds.removeFromLeft(kIconSize_ + kPadding_);
        g.drawText(icon, iconBounds, juce::Justification::centredLeft);
    }
    
    // Message
    g.drawText(currentMessage, bounds, juce::Justification::centredLeft);
}

void FooterPanel::resized()
{
}

void FooterPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
}

void FooterPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    resized();
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

