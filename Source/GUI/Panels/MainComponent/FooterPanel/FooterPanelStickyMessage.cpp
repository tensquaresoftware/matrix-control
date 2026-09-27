#include "FooterPanel.h"

#include "Core/Exceptions/ExceptionPropagator.h"
#include "GUI/Helpers/FooterSeverityBadge.h"
#include "GUI/Helpers/TextFitHelpers.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    bool detailUsesPathStyleTruncate(const juce::String& detailText)
    {
        return detailText.startsWith("Loaded ")
            || detailText.startsWith("Saved ")
            || detailText.contains(" - Loaded ")
            || detailText.contains(" — Loaded ")
            || detailText.startsWith(
                   PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages::kExportCompleteFooterStem);
    }
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
    const int badgeWidth = paintBadgeChrome(g, args, badgeHeight, badgePad);

    bounds.removeFromLeft(badgeWidth + badgeGap);
    g.setFont(args.font);
    g.setColour(args.detailColour);
    const auto fittedDetail = TSS::TextFitHelpers::fitWithAsciiEllipsis(
        args.detailText,
        args.font,
        static_cast<float>(bounds.getWidth()),
        detailUsesPathStyleTruncate(args.detailText));
    g.drawText(fittedDetail, bounds, juce::Justification::centredLeft, false);
}

int FooterPanel::paintBadgeChrome(juce::Graphics& g,
                                  const BadgeDetailPaintArgs& args,
                                  int badgeHeight,
                                  int badgePad) const
{
    if (args.chromeMode == BadgeChromeMode::SeverityIcon)
    {
        const int iconSize = TSS::ScaledLayout::scaledInt(
            static_cast<float>(dimensions_.iconSize), uiScale_);
        const auto square = TSS::severityBadgeSquareBounds(args.bounds, iconSize);
        const auto glyph = args.severityBadgeHovered
            ? TSS::makeSeverityCloseCrossShape()
            : TSS::severityPictoFor(toStickySeverity(args.stickySeverity));
        TSS::paintSeverityIconInSquare(g, {
            square,
            glyph,
            args.badgeFill,
            args.badgeTextColour
        });
        return square.getWidth();
    }

    const auto badgeFont = skin_->getBaseFontBold().withHeight(args.font.getHeight());
    g.setFont(badgeFont);
    const int labelWidth =
        juce::roundToInt(juce::GlyphArrangement::getStringWidth(badgeFont, args.badgeLabel));
    const int badgeWidth = juce::jmin(args.bounds.getWidth(), labelWidth + 2 * badgePad);
    const int badgeY = args.bounds.getCentreY() - badgeHeight / 2;
    const juce::Rectangle<int> badgeBounds {
        args.bounds.getX(),
        badgeY,
        badgeWidth,
        badgeHeight
    };
    g.setColour(args.badgeFill);
    g.fillRect(badgeBounds);
    g.setColour(args.badgeTextColour);
    g.drawText(args.badgeLabel, badgeBounds, juce::Justification::centred, false);
    return badgeWidth;
}

bool FooterPanel::helpCoversStickyBand() const
{
    const bool stickyError = currentSeverity == MessageSeverity::Error;
    return TSS::shouldPaintContextualHelpOverSticky(contextualHelpOverlay_.isActive(),
                                                    stickyError);
}

int FooterPanel::readInfoMessagePreference() const
{
    return static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kInfoMessage,
        PluginIDs::Settings::InfoMessage::kDefault));
}

void FooterPanel::cancelAutoClearTimer()
{
    stopTimer();
    autoClearArmed_ = false;
    autoClearPaused_ = false;
    autoClearRemainingMs_ = 0;
    autoClearDeadlineMs_ = 0;
}

void FooterPanel::armAutoClearTimer()
{
    cancelAutoClearTimer();
    autoClearArmed_ = true;
    autoClearRemainingMs_ = PluginIDs::Settings::InfoMessage::kAutoClearDurationMs;

    if (TSS::shouldPauseAutoClearForHelp(autoClearArmed_, helpCoversStickyBand()))
    {
        autoClearPaused_ = true;
        return;
    }

    autoClearPaused_ = false;
    autoClearDeadlineMs_ = juce::Time::getMillisecondCounter()
                           + static_cast<juce::uint32>(autoClearRemainingMs_);
    startTimer(autoClearRemainingMs_);
}

void FooterPanel::pauseAutoClearTimerForHelp()
{
    if (! TSS::shouldPauseAutoClearForHelp(autoClearArmed_, helpCoversStickyBand()))
        return;

    if (isTimerRunning())
    {
        const auto now = juce::Time::getMillisecondCounter();
        const int remainingFromDeadline = autoClearDeadlineMs_ > now
            ? static_cast<int>(autoClearDeadlineMs_ - now)
            : 0;
        const int elapsedMs = autoClearRemainingMs_ - remainingFromDeadline;
        autoClearRemainingMs_ = TSS::remainingAutoClearDelayMs(autoClearRemainingMs_, elapsedMs);
        stopTimer();
    }

    autoClearPaused_ = true;
}

void FooterPanel::resumeAutoClearTimerAfterHelp()
{
    if (! TSS::shouldResumeAutoClearAfterHelp(autoClearArmed_,
                                              autoClearPaused_,
                                              helpCoversStickyBand(),
                                              autoClearRemainingMs_))
    {
        if (autoClearArmed_ && TSS::shouldFireAutoClear(autoClearRemainingMs_,
                                                        helpCoversStickyBand()))
            clearStickyMessage();
        return;
    }

    autoClearPaused_ = false;
    autoClearDeadlineMs_ = juce::Time::getMillisecondCounter()
                           + static_cast<juce::uint32>(autoClearRemainingMs_);
    startTimer(autoClearRemainingMs_);
}

void FooterPanel::syncAutoClearPolicyFromState()
{
    const auto sticky = toStickySeverity(currentSeverity);
    const bool hasSticky = currentMessage.isNotEmpty()
                           && currentSeverity != MessageSeverity::None;

    if (! hasSticky
        || ! TSS::shouldAutoClearStickySeverity(sticky, readInfoMessagePreference()))
    {
        cancelAutoClearTimer();
        return;
    }

    armAutoClearTimer();
}

void FooterPanel::clearStickyMessage()
{
    cancelAutoClearTimer();
    ExceptionPropagator::clearMessage(apvts);
}

void FooterPanel::timerCallback()
{
    stopTimer();

    const auto helpAtFire = TSS::autoClearTimerFireWhileHelpCovers(autoClearArmed_,
                                                                   helpCoversStickyBand());
    if (helpAtFire.deferred)
    {
        autoClearPaused_ = helpAtFire.paused;
        autoClearRemainingMs_ = helpAtFire.remainingMs;
        return;
    }

    if (autoClearArmed_ && TSS::shouldFireAutoClear(0, false))
        clearStickyMessage();
}

void FooterPanel::updateSeverityBadgeHitAreaBounds()
{
    const bool hasSticky = currentMessage.isNotEmpty()
                           && currentSeverity != MessageSeverity::None;
    const bool stickyVisible = TSS::shouldShowStickySeverityBadgeHitArea(
        hasSticky,
        helpCoversStickyBand());

    if (! stickyVisible)
    {
        severityBadgeHitArea_.setBounds({});
        if (severityBadgeHovered_)
        {
            severityBadgeHovered_ = false;
            repaint();
        }
        return;
    }

    const auto layout = computeBandLayout();
    const auto leftBounds = layout.leftBand.reduced(layout.padding, 0);
    const int iconSize = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dimensions_.iconSize), uiScale_);
    severityBadgeHitArea_.setBounds(TSS::severityBadgeSquareBounds(leftBounds, iconSize));
}

void FooterPanel::setSeverityBadgeHovered(bool hovered)
{
    if (severityBadgeHovered_ == hovered)
        return;

    severityBadgeHovered_ = hovered;
    repaint();
}

void FooterPanel::handleSeverityBadgeClick()
{
    if (currentMessage.isEmpty() || currentSeverity == MessageSeverity::None)
        return;

    clearStickyMessage();
}
