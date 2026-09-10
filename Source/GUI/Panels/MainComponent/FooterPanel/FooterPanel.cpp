#include "FooterPanel.h"

#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Helpers/TextFitHelpers.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ColourChart.h"
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

FooterPanel::FooterPanel(TSS::ISkin& skin,
                         const FooterPanelDimensions& dimensions,
                         juce::AudioProcessorValueTreeState& apvtsRef)
    : dimensions_(dimensions)
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

FooterPanel::FooterBandLayout FooterPanel::computeBandLayout() const
{
    const int padding = juce::jmax(1, juce::roundToInt(static_cast<float>(dimensions_.padding) * uiScale_));
    const int bandHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.bandHeight), uiScale_);
    const int bandVerticalInset = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.bandVerticalInset), uiScale_);
    const int patchEditW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.patchEditPanelWidth), uiScale_);
    const int sharedW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.sharedPanelWidth), uiScale_);
    const int masterEditW = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.masterEditPanelWidth), uiScale_);
    const int gap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.interColumnGap), uiScale_);

    const auto area = getLocalBounds();
    const int bandY = area.getY() + bandVerticalInset;

    FooterBandLayout layout;
    layout.padding = padding;
    layout.leftBand = { area.getX(), bandY, patchEditW, bandHeight };
    layout.centreBand = { layout.leftBand.getRight() + gap, bandY, sharedW, bandHeight };
    layout.rightBand = { layout.centreBand.getRight() + gap, bandY, masterEditW, bandHeight };
    return layout;
}

void FooterPanel::paintBadgeAndDetail(juce::Graphics& g, const BadgeDetailPaintArgs& args) const
{
    auto bounds = args.bounds;
    const int badgeHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.severityBadgeHeight), uiScale_);
    const int badgePad = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.severityBadgeHorizontalPadding), uiScale_);
    const int badgeGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.severityBadgeToMessageGap), uiScale_);
    const auto badgeFont = skin_->getBaseFontBold().withHeight(args.font.getHeight());

    g.setFont(badgeFont);

    const int labelWidth = juce::roundToInt(juce::GlyphArrangement::getStringWidth(badgeFont, args.badgeLabel));
    const int badgeWidth = juce::jmin(bounds.getWidth(), labelWidth + 2 * badgePad);
    const int badgeY = bounds.getCentreY() - badgeHeight / 2;
    const juce::Rectangle<int> badgeBounds {
        bounds.getX(),
        badgeY,
        badgeWidth,
        badgeHeight
    };

    g.setColour(args.badgeFill);
    g.fillRect(badgeBounds);

    g.setColour(args.badgeTextColour);
    g.drawText(args.badgeLabel, badgeBounds, juce::Justification::centred, false);

    bounds.removeFromLeft(badgeWidth + badgeGap);
    g.setFont(args.font);
    g.setColour(args.detailColour);

    // Prefer start of status messages; Loaded/Saved locations keep the useful end via middle truncate.
    const bool usePathStyleTruncate = args.detailText.startsWith("Loaded ")
        || args.detailText.startsWith("Saved ");
    const auto fittedDetail = TSS::TextFitHelpers::fitWithAsciiEllipsis(
        args.detailText,
        args.font,
        static_cast<float>(bounds.getWidth()),
        usePathStyleTruncate);
    g.drawText(fittedDetail, bounds, juce::Justification::centredLeft, false);
}

void FooterPanel::paintStatusMessage(juce::Graphics& g,
                                     juce::Rectangle<int> bounds,
                                     const juce::Font& font,
                                     juce::Colour detailColour) const
{
    if (currentMessage.isEmpty() || currentSeverity == MessageSeverity::None)
        return;

    paintBadgeAndDetail(g, {
        bounds,
        getSeverityPrefix(currentSeverity),
        currentMessage,
        getSeverityColour(currentSeverity),
        skin_->getColour(SkinColourId::kFooterPanelBackground),
        detailColour,
        font
    });
}

void FooterPanel::paintDeviceStatus(juce::Graphics& g,
                                    juce::Rectangle<int> bounds,
                                    const juce::Font& font,
                                    juce::Colour detailColour) const
{
    const auto type = MatrixDeviceTypes::fromApvtsString(deviceType_);
    const bool deviceOk = deviceDetected_
        && MatrixDeviceTypes::isSupportedMatrixDevice(type);
    const auto badgeFill = deviceOk
        ? skin_->getColour(SkinColourId::kFooterMessageInfo)
        : skin_->getColour(SkinColourId::kFooterMessageError).withAlpha(0.8f);

    paintBadgeAndDetail(g, {
        bounds,
        PluginDisplayNames::FooterPanel::kDeviceLabel,
        buildDeviceDetailText(),
        badgeFill,
        skin_->getColour(SkinColourId::kFooterPanelBackground),
        detailColour,
        font
    });
}

void FooterPanel::paintMidiQueuePressureAlert(juce::Graphics& g,
                                             juce::Rectangle<int> bounds,
                                             const juce::Font& font,
                                             juce::Colour detailColour) const
{
    if (! midiQueuePressureAlertActive_)
        return;

    paintBadgeAndDetail(g, {
        bounds,
        PluginDisplayNames::FooterPanel::kMidiQueuePressureBadge,
        PluginDisplayNames::FooterPanel::kMidiQueuePressureMessage,
        juce::Colour(ColourChart::kRed),
        juce::Colour(ColourChart::kBlack),
        detailColour,
        font
    });
}

void FooterPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kFooterPanelBackground));

    const auto layout = computeBandLayout();

    g.setColour(skin_->getColour(SkinColourId::kBodyPanelBackground).withAlpha(0.0f));
    g.fillRect(layout.leftBand);
    g.fillRect(layout.centreBand);
    g.fillRect(layout.rightBand);

    const auto font = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const auto chromeGrey = skin_->getColour(SkinColourId::kFooterMessageInfo);

    paintStatusMessage(g, layout.leftBand.reduced(layout.padding, 0), font, chromeGrey);
    paintMidiQueuePressureAlert(g, layout.centreBand.reduced(layout.padding, 0), font, chromeGrey);
    paintDeviceStatus(g, layout.rightBand.reduced(layout.padding, 0), font, chromeGrey);
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

void FooterPanel::setMidiQueuePressureAlert(bool active)
{
    if (midiQueuePressureAlertActive_ == active)
        return;

    midiQueuePressureAlertActive_ = active;
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
    if (severityStr == "info" || severityStr == "success")
        return MessageSeverity::Info;
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
            return skin_->getColour(SkinColourId::kDarkPanelText);
        case MessageSeverity::Info:
            return skin_->getColour(SkinColourId::kFooterMessageInfo);
        case MessageSeverity::Warning:
            return skin_->getColour(SkinColourId::kFooterMessageWarning);
        case MessageSeverity::Error:
            return skin_->getColour(SkinColourId::kFooterMessageError).withAlpha(0.8f);
        default:
            return skin_->getColour(SkinColourId::kDarkPanelText);
    }
}

juce::String FooterPanel::getSeverityPrefix(MessageSeverity severity) const
{
    switch (severity)
    {
        case MessageSeverity::None:
            return {};
        case MessageSeverity::Info:
            return PluginDisplayNames::FooterPanel::kSeverityInfoPrefix;
        case MessageSeverity::Warning:
            return PluginDisplayNames::FooterPanel::kSeverityWarningPrefix;
        case MessageSeverity::Error:
            return PluginDisplayNames::FooterPanel::kSeverityErrorPrefix;
        default:
            return {};
    }
}

juce::String FooterPanel::buildDeviceDetailText() const
{
    if (! deviceDetected_)
        return PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail;

    const auto type = MatrixDeviceTypes::fromApvtsString(deviceType_);
    if (! MatrixDeviceTypes::isSupportedMatrixDevice(type))
        return PluginDisplayNames::FooterPanel::kDeviceUnknownDetail;

    juce::String detail = MatrixDeviceTypes::toDisplayString(type).toUpperCase();
    const auto versionDisplay = TSS::formatDeviceVersionForDisplay(deviceVersion_);
    if (versionDisplay.isNotEmpty())
        detail += " (V" + versionDisplay + ")";

    return detail;
}
