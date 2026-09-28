#include "FooterPanel.h"

#include "Core/MIDI/EditorOutboundGate.h"
#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using TSS::SkinColourId;

const juce::Identifier FooterPanel::kMessageTextId("uiMessageText");
const juce::Identifier FooterPanel::kMessageSeverityId("uiMessageSeverity");
const juce::Identifier FooterPanel::kDeviceDetectedId("deviceDetected");
const juce::Identifier FooterPanel::kDeviceTypeId(MatrixDeviceTypes::kApvtsPropertyName);
const juce::Identifier FooterPanel::kDeviceVersionId("deviceVersion");
const juce::Identifier FooterPanel::kDeviceMidiUnresponsiveId(Core::kDeviceMidiUnresponsiveProperty);

FooterPanel::FooterPanel(TSS::ISkin& skin,
                         const FooterPanelDimensions& dimensions,
                         juce::AudioProcessorValueTreeState& apvtsRef)
    : dimensions_(dimensions)
    , skin_(&skin)
    , apvts(apvtsRef)
{
    setOpaque(true);
    deviceHitArea_.setInterceptsMouseClicks(true, false);
    addAndMakeVisible(deviceHitArea_);
    severityBadgeHitArea_.setInterceptsMouseClicks(true, false);
    severityBadgeHitArea_.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(severityBadgeHitArea_);
    registerDeviceContextualHelp();
    apvts.state.addListener(this);
    syncFromApvtsState(apvts.state);
    syncAutoClearPolicyFromState();
}

FooterPanel::~FooterPanel()
{
    stopTimer();
    contextualHelpBinder_.reset();
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
        font,
        BadgeChromeMode::SeverityIcon,
        severityBadgeHovered_
    });
}

void FooterPanel::paintDeviceStatus(juce::Graphics& g,
                                    juce::Rectangle<int> bounds,
                                    const juce::Font& font,
                                    juce::Colour detailColour) const
{
    const auto badgeFill = isDeviceIdentityOk()
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

void FooterPanel::paintContextualHelp(juce::Graphics& g,
                                      juce::Rectangle<int> bounds,
                                      const juce::Font& font) const
{
    if (! isContextualHelpEnabled())
        return;

    const bool stickyError = currentSeverity == MessageSeverity::Error;
    if (! TSS::shouldPaintContextualHelpOverSticky(contextualHelpOverlay_.isActive(),
                                                   stickyError))
        return;

    const auto helpChrome = juce::Colour(ColourChart::kContextualHelpChrome);
    paintBadgeAndDetail(g, {
        bounds,
        PluginDisplayNames::FooterPanel::kContextualHelpBadge,
        contextualHelpOverlay_.getDetail(),
        helpChrome,
        skin_->getColour(SkinColourId::kFooterPanelBackground),
        helpChrome,
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
    const auto leftBounds = layout.leftBand.reduced(layout.padding, 0);
    const bool stickyError = currentSeverity == MessageSeverity::Error;

    if (TSS::shouldPaintContextualHelpOverSticky(contextualHelpOverlay_.isActive(),
                                                 stickyError))
        paintContextualHelp(g, leftBounds, font);
    else
        paintStatusMessage(g, leftBounds, font, chromeGrey);

    paintMidiQueuePressureAlert(g, layout.centreBand.reduced(layout.padding, 0), font, chromeGrey);
    paintDeviceStatus(g, layout.rightBand.reduced(layout.padding, 0), font, chromeGrey);
}

void FooterPanel::resized()
{
    updateDeviceHitAreaBounds();
    updateSeverityBadgeHitAreaBounds();
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
    updateDeviceHitAreaBounds();
    updateSeverityBadgeHitAreaBounds();
    repaint();
}

void FooterPanel::setMidiQueuePressureAlert(bool active)
{
    if (midiQueuePressureAlertActive_ == active)
        return;

    midiQueuePressureAlertActive_ = active;
    repaint();
}

int FooterPanel::setContextualHelpOverlay(const juce::String& detailText)
{
    if (! isContextualHelpEnabled())
    {
        // HIDE (incl. after replaceState): drop any stale overlay; do not show.
        clearContextualHelpOverlay();
        return 0;
    }

    contextualHelpEpoch_ = TSS::nextContextualHelpOverlayEpoch(contextualHelpEpoch_);

    if (contextualHelpOverlay_.getDetail() != detailText)
    {
        contextualHelpOverlay_.setDetail(detailText);
        pauseAutoClearTimerForHelp();
        updateSeverityBadgeHitAreaBounds();
        repaint();
    }
    else if (helpCoversStickyBand())
    {
        pauseAutoClearTimerForHelp();
    }

    return contextualHelpEpoch_;
}

void FooterPanel::clearContextualHelpOverlay()
{
    if (! contextualHelpOverlay_.isActive())
        return;

    contextualHelpOverlay_.clear();
    resumeAutoClearTimerAfterHelp();
    updateSeverityBadgeHitAreaBounds();
    repaint();
}

void FooterPanel::clearContextualHelpOverlayIfEpoch(int epoch)
{
    if (! TSS::clearContextualHelpOverlayDetailIfEpoch(contextualHelpOverlay_,
                                                       epoch,
                                                       contextualHelpEpoch_))
        return;

    resumeAutoClearTimerAfterHelp();
    updateSeverityBadgeHitAreaBounds();
    repaint();
}

bool FooterPanel::isContextualHelpEnabled() const
{
    const int preferenceRaw = static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kContextualHelp,
        PluginIDs::Settings::ContextualHelp::kDefault));
    return TSS::isContextualHelpPreferenceEnabled(preferenceRaw);
}

void FooterPanel::clearContextualHelpOverlayIfPreferenceHidden()
{
    if (! isContextualHelpEnabled())
        clearContextualHelpOverlay();
}

void FooterPanel::valueTreePropertyChanged(juce::ValueTree& tree,
                                          const juce::Identifier& property)
{
    if (property.toString() == PluginIDs::Settings::kContextualHelp)
    {
        const int preferenceRaw = static_cast<int>(tree.getProperty(
            PluginIDs::Settings::kContextualHelp,
            PluginIDs::Settings::ContextualHelp::kDefault));
        // HIDE: clear immediately (no popup defer / delayed idle clear).
        if (TSS::shouldClearContextualHelpOverlayForPreference(preferenceRaw))
            clearContextualHelpOverlay();
        return;
    }

    if (property.toString() == PluginIDs::Settings::kInfoMessage)
    {
        syncAutoClearPolicyFromState();
        return;
    }

    if (property == kMessageTextId
        || property == kMessageSeverityId
        || property == kDeviceDetectedId
        || property == kDeviceTypeId
        || property == kDeviceVersionId
        || property == kDeviceMidiUnresponsiveId)
    {
        syncFromApvtsState(tree);
        if (property == kMessageTextId || property == kMessageSeverityId)
            syncAutoClearPolicyFromState();
        updateSeverityBadgeHitAreaBounds();
        repaint();
    }
}

void FooterPanel::valueTreeRedirected(juce::ValueTree&)
{
    // Host replaceState does not fire per-property changes; cancel stale timer before sync
    // so a paused remaining-0 arm cannot clearStickyMessage against the old tree.
    cancelAutoClearTimer();
    clearContextualHelpOverlayIfPreferenceHidden();
    syncFromApvtsState(apvts.state);
    syncAutoClearPolicyFromState();
    updateSeverityBadgeHitAreaBounds();
    repaint();
}

void FooterPanel::syncFromApvtsState(juce::ValueTree& tree)
{
    currentMessage = tree.getProperty(kMessageTextId, juce::String()).toString();
    currentSeverity = parseSeverity(tree.getProperty(kMessageSeverityId, juce::String()).toString());
    deviceDetected_ = static_cast<bool>(tree.getProperty(kDeviceDetectedId, false));
    deviceMidiUnresponsive_ = static_cast<bool>(tree.getProperty(kDeviceMidiUnresponsiveId, false));
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

TSS::StickyMessageSeverity FooterPanel::toStickySeverity(MessageSeverity severity) const
{
    switch (severity)
    {
        case MessageSeverity::Info:
            return TSS::StickyMessageSeverity::Info;
        case MessageSeverity::Warning:
            return TSS::StickyMessageSeverity::Warning;
        case MessageSeverity::Error:
            return TSS::StickyMessageSeverity::Error;
        case MessageSeverity::None:
        default:
            return TSS::StickyMessageSeverity::None;
    }
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
    if (! deviceDetected_ || deviceMidiUnresponsive_)
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

bool FooterPanel::isDeviceIdentityOk() const
{
    if (! deviceDetected_ || deviceMidiUnresponsive_)
        return false;

    return MatrixDeviceTypes::isSupportedMatrixDevice(
        MatrixDeviceTypes::fromApvtsString(deviceType_));
}

void FooterPanel::updateDeviceHitAreaBounds()
{
    const auto layout = computeBandLayout();
    deviceHitArea_.setBounds(layout.rightBand.reduced(layout.padding, 0));
}

void FooterPanel::registerDeviceContextualHelp()
{
    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        [this]() -> FooterPanel*
        {
            return this;
        });
    contextualHelpBinder_->bind(
        &deviceHitArea_,
        PluginDisplayNames::FooterPanel::ContextualHelp::kDevice);
}
