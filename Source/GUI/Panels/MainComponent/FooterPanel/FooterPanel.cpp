#include "FooterPanel.h"

#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

const juce::Identifier FooterPanel::kMessageTextId("uiMessageText");
const juce::Identifier FooterPanel::kMessageSeverityId("uiMessageSeverity");
const juce::Identifier FooterPanel::kDeviceDetectedId("deviceDetected");
const juce::Identifier FooterPanel::kDeviceTypeId(MatrixDeviceTypes::kApvtsPropertyName);
const juce::Identifier FooterPanel::kDeviceVersionId("deviceVersion");

FooterPanel::FooterPanel(TSS::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvtsRef)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts(apvtsRef)
{
    setOpaque(true);
    apvts.state.addListener(this);
    syncFromApvtsState(apvts.state);
}

FooterPanel::~FooterPanel()
{
    apvts.state.removeListener(this);
}

void FooterPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kFooterPanelBackground));

    const int padding = juce::jmax(1, juce::roundToInt(static_cast<float>(kPadding_) * uiScale_));
    const int iconSize = juce::jmax(1, juce::roundToInt(static_cast<float>(kIconSize_) * uiScale_));
    const int identityMinWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::Panels::Footer::kIdentityMinWidth),
        uiScale_);

    auto bounds = getLocalBounds().reduced(padding);
    const auto identityText = buildDeviceIdentityText();

    if (identityText.isNotEmpty())
    {
        const auto identityBounds = bounds.removeFromRight(juce::jmin(identityMinWidth, bounds.getWidth() / 2));
        g.setColour(skin_->getColour(SkinColourId::kLabelText));
        g.setFont(skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_));
        g.drawFittedText(identityText,
                         identityBounds,
                         juce::Justification::centredRight,
                         1,
                         1.0f);
    }

    if (currentMessage.isEmpty() || currentSeverity == MessageSeverity::None)
        return;

    g.setColour(getSeverityColour(currentSeverity));
    g.setFont(skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_));

    const juce::String icon = getSeverityIcon(currentSeverity);
    if (icon.isNotEmpty())
    {
        const auto iconBounds = bounds.removeFromLeft(iconSize + padding);
        g.drawText(icon, iconBounds, juce::Justification::centredLeft);
    }

    g.drawFittedText(currentMessage,
                     bounds,
                     juce::Justification::centredLeft,
                     1,
                     1.0f);
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
    if (property == kMessageTextId
        || property == kMessageSeverityId
        || property == kDeviceDetectedId
        || property == kDeviceTypeId
        || property == kDeviceVersionId)
    {
        syncFromApvtsState(tree);
        repaint();
    }
}

void FooterPanel::syncFromApvtsState(juce::ValueTree& tree)
{
    currentMessage = tree.getProperty(kMessageTextId, juce::String()).toString();
    currentSeverity = parseSeverity(tree.getProperty(kMessageSeverityId, juce::String()).toString());
    deviceDetected_ = static_cast<bool>(tree.getProperty(kDeviceDetectedId, false));
    deviceType_ = tree.getProperty(kDeviceTypeId, juce::String()).toString();
    deviceVersion_ = tree.getProperty(kDeviceVersionId, juce::String()).toString();
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
            return skin_->getColour(SkinColourId::kFooterMessageInfo);
        case MessageSeverity::Success:
            return skin_->getColour(SkinColourId::kFooterMessageSuccess);
        case MessageSeverity::Warning:
            return skin_->getColour(SkinColourId::kFooterMessageWarning);
        case MessageSeverity::Error:
            return skin_->getColour(SkinColourId::kFooterMessageError);
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

juce::String FooterPanel::buildDeviceIdentityText() const
{
    if (! deviceDetected_)
        return PluginDisplayNames::FooterPanel::kNoDevice;

    if (deviceType_.isEmpty())
        return PluginDisplayNames::FooterPanel::kNoDevice;

    juce::String identity = deviceType_;
    if (deviceVersion_.isNotEmpty())
        identity += " · v" + deviceVersion_;

    return identity;
}
