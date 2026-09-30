#include "BankTransferProgressDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Helpers/TextFitHelpers.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Widgets/Button.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    int scaledButtonHeight(float uiScale)
    {
        return juce::roundToInt(static_cast<float>(DialogMatrixHelpers::kDefaultButtonHeight) * uiScale);
    }

    int scaledBottomMargin(float uiScale)
    {
        return juce::roundToInt(static_cast<float>(DialogMatrixHelpers::kButtonBottomMargin) * uiScale);
    }
}

BankTransferProgressDialog::BankTransferProgressDialog(TSS::ISkin& skin)
    : skin_(&skin)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin,
        DialogMatrixHelpers::kDefaultButtonWidth,
        PluginDisplayNames::Dialogs::BankTransferProgress::kCancel);
    cancelButton_->onClick = [this]
    {
        if (onCancelRequested_ && cancelButton_->isEnabled())
            onCancelRequested_();
    };
    addAndMakeVisible(*cancelButton_);
}

BankTransferProgressDialog::~BankTransferProgressDialog() = default;

void BankTransferProgressDialog::prepareForShow(PrepareForShowArgs args)
{
    title_ = std::move(args.title);
    detail_ = std::move(args.detail);
    primaryMessage_ = std::move(args.message);
    primaryTotalSteps_ = juce::jmax(1, args.totalSteps);
    primaryCompletedSteps_ = 0;
    secondaryLaneActive_ = false;
    secondaryCompletedSteps_ = 0;
    contentLayout_ = args.layout;

    applyOperationPresentationFromTitle();

    onCancelRequested_ = std::move(args.onCancelRequested);
    setCancelEnabled(static_cast<bool>(onCancelRequested_));
    resized();
    repaint();
}

void BankTransferProgressDialog::applyOperationPresentationFromTitle()
{
    using namespace PluginDisplayNames::Dialogs::BankTransferProgress;
    using namespace PluginDisplayNames::PatchManagerSection::BankUtilityModule;

    // Terminology and detail placement follow the title; lane count comes from PrepareForShowArgs::layout.
    if (title_ == juce::String(kExportTitle))
    {
        headerLabel_ = juce::String(kDestinationFolderLabel);
        detailBelowProgress_ = false;
        detailInline_ = false;
        secondaryMessage_.clear();
        secondaryTotalSteps_ = 1;
        return;
    }

    if (title_ == juce::String(kCopyTitle))
    {
        headerLabel_ = juce::String(kDestinationLabel);
        detailBelowProgress_ = true;
        detailInline_ = true;
        secondaryMessage_.clear();
        secondaryTotalSteps_ = 1;
        return;
    }

    if (title_ == juce::String(kPasteTitle))
    {
        headerLabel_ = juce::String(kSourceLabel);
        detailBelowProgress_ = false;
        detailInline_ = true;
        secondaryMessage_ = juce::String(kPastingWritingMessage);
        secondaryTotalSteps_ = primaryTotalSteps_;
        return;
    }

    headerLabel_ = juce::String(kSourceFolderLabel);
    detailBelowProgress_ = true;
    detailInline_ = false;
    secondaryMessage_ = juce::String(kImportingWritingMessage);
    secondaryTotalSteps_ = primaryTotalSteps_;
}

void BankTransferProgressDialog::beginSecondaryPhase(const juce::String& message, int totalSteps)
{
    primaryCompletedSteps_ = primaryTotalSteps_;
    secondaryLaneActive_ = true;
    secondaryMessage_ = message;
    secondaryTotalSteps_ = juce::jmax(1, totalSteps);
    secondaryCompletedSteps_ = 0;
    resized();
    repaint();
}

void BankTransferProgressDialog::setProgress(int completedSteps)
{
    if (secondaryLaneActive_)
        secondaryCompletedSteps_ = juce::jlimit(0, secondaryTotalSteps_, completedSteps);
    else
        primaryCompletedSteps_ = juce::jlimit(0, primaryTotalSteps_, completedSteps);

    repaint();
}

void BankTransferProgressDialog::setMessage(const juce::String& message)
{
    if (secondaryLaneActive_)
        secondaryMessage_ = message;
    else
        primaryMessage_ = message;

    repaint();
}

void BankTransferProgressDialog::setDetail(const juce::String& detail)
{
    detail_ = detail;
    repaint();
}

void BankTransferProgressDialog::setCancelEnabled(bool enabled)
{
    cancelButton_->setEnabled(enabled);
}

void BankTransferProgressDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    DialogMatrixHelpers::applyButtonSkin(*cancelButton_, skin);
    repaint();
}

void BankTransferProgressDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale);
    resized();
    repaint();
}

int BankTransferProgressDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

int BankTransferProgressDialog::getDesignContentHeight() const noexcept
{
    return contentLayout_ == ContentLayout::DualLane ? kDesignHeightDual : kDesignHeightSingle;
}

juce::Rectangle<int> BankTransferProgressDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(getDesignContentHeight()) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;

    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void BankTransferProgressDialog::paintProgressBar(juce::Graphics& g,
                                                    juce::Rectangle<int> bounds,
                                                    float fraction,
                                                    bool enabled) const
{
    g.setColour(skin_->getColour(enabled ? SkinColourId::kSliderTrackEnabled
                                         : SkinColourId::kSliderTrackDisabled));
    g.fillRect(bounds);

    if (enabled)
    {
        auto fillBar = bounds.withWidth(
            juce::roundToInt(static_cast<float>(bounds.getWidth()) * fraction));
        g.setColour(skin_->getColour(SkinColourId::kSliderValueBarEnabled));
        g.fillRect(fillBar);
    }

    const int percent = enabled ? juce::roundToInt(fraction * 100.0f) : 0;
    g.setColour(skin_->getColour(enabled ? SkinColourId::kSliderTextEnabled
                                         : SkinColourId::kSliderTextDisabled));
    g.drawText(juce::String(percent) + juce::String(PluginDisplayNames::Units::kPercent),
               bounds,
               juce::Justification::centred,
               false);
}

juce::Rectangle<int> BankTransferProgressDialog::paintDetailHeader(juce::Graphics& g,
                                                                   juce::Rectangle<int> body,
                                                                   const juce::Font& bodyFont,
                                                                   bool belowProgress) const
{
    const float em = bodyFont.getHeight();
    const int gap1em = juce::roundToInt(em);
    const int lineHeight = juce::jmax(1, juce::roundToInt(em));

    if (belowProgress)
        body.removeFromTop(gap1em);

    g.setFont(bodyFont);
    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));

    if (detailInline_)
    {
        auto line = body.removeFromTop(lineHeight);
        juce::String inlineText = headerLabel_;
        if (detail_.isNotEmpty())
            inlineText = headerLabel_.trimEnd() + " " + detail_;

        const auto fitted = TSS::TextFitHelpers::fitWithAsciiEllipsis(
            inlineText, bodyFont, static_cast<float>(line.getWidth()), true);
        g.drawText(fitted, line, juce::Justification::centredLeft, false);
    }
    else
    {
        {
            auto labelLine = body.removeFromTop(lineHeight);
            g.drawText(headerLabel_, labelLine, juce::Justification::centredLeft, false);
        }

        if (detail_.isNotEmpty())
        {
            auto pathLine = body.removeFromTop(lineHeight);
            const auto fittedPath = TSS::TextFitHelpers::fitWithAsciiEllipsis(
                detail_, bodyFont, static_cast<float>(pathLine.getWidth()), true);
            g.drawText(fittedPath, pathLine, juce::Justification::centredLeft, false);
        }
    }

    if (! belowProgress)
        body.removeFromTop(gap1em);

    return body;
}

void BankTransferProgressDialog::paintPhaseLane(juce::Graphics& g, const PhaseLanePaintArgs& args) const
{
    const float em = args.bodyFont.getHeight();
    const int gapHalfEm = juce::roundToInt(em * 0.5f);
    const int lineHeight = juce::jmax(1, juce::roundToInt(em));
    const int barHeight = juce::roundToInt(16.0f * uiScale_);

    g.setFont(args.bodyFont);
    g.setColour(args.enabled ? skin_->getColour(SkinColourId::kDarkPanelText)
                             : skin_->getColour(SkinColourId::kSliderTextDisabled));

    {
        auto progressLabel = args.body.removeFromTop(lineHeight);
        g.drawText(args.message, progressLabel, juce::Justification::centredLeft, false);
    }

    args.body.removeFromTop(gapHalfEm);

    auto progressBar = args.body.removeFromTop(barHeight);
    const float fraction = juce::jlimit(
        0.0f,
        1.0f,
        static_cast<float>(args.completedSteps)
            / static_cast<float>(juce::jmax(1, args.totalSteps)));
    paintProgressBar(g, progressBar, fraction, args.enabled);
}

void BankTransferProgressDialog::paintSingleLaneBody(juce::Graphics& g,
                                                     juce::Rectangle<int> body,
                                                     const juce::Font& bodyFont) const
{
    if (! detailBelowProgress_)
        body = paintDetailHeader(g, body, bodyFont, false);

    paintPhaseLane(g,
                   PhaseLanePaintArgs {
                       body,
                       bodyFont,
                       primaryMessage_,
                       primaryCompletedSteps_,
                       primaryTotalSteps_,
                       true });

    if (detailBelowProgress_)
        paintDetailHeader(g, body, bodyFont, true);
}

void BankTransferProgressDialog::paintDualLaneBody(juce::Graphics& g,
                                                   juce::Rectangle<int> body,
                                                   const juce::Font& bodyFont) const
{
    if (! detailBelowProgress_)
        body = paintDetailHeader(g, body, bodyFont, false);

    paintPhaseLane(g,
                   PhaseLanePaintArgs {
                       body,
                       bodyFont,
                       primaryMessage_,
                       primaryCompletedSteps_,
                       primaryTotalSteps_,
                       true });

    body.removeFromTop(juce::roundToInt(bodyFont.getHeight()));
    paintPhaseLane(g,
                   PhaseLanePaintArgs {
                       body,
                       bodyFont,
                       secondaryMessage_,
                       secondaryCompletedSteps_,
                       secondaryTotalSteps_,
                       secondaryLaneActive_ });

    if (detailBelowProgress_)
        paintDetailHeader(g, body, bodyFont, true);
}

void BankTransferProgressDialog::paint(juce::Graphics& g)
{
    const auto dialogBounds = getDialogBounds();
    const int border = getBorderThickness();
    const int titleBarHeight = juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_);

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = dialogBounds,
        .borderThickness = border,
        .titleBarHeight = titleBarHeight,
        .title = title_,
        .uiScale = uiScale_ });

    auto content = dialogBounds.reduced(border);
    content.removeFromTop(titleBarHeight);

    // Custom modal scheme: exactly 1em under the title, then content (no extra top padding).
    const auto bodyFont = DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    const int padX = juce::roundToInt(12.0f * uiScale_);
    // Same button height + bottom margin ints as resized() so body never overlaps the button row.
    const int bottomReserve = scaledButtonHeight(uiScale_) + scaledBottomMargin(uiScale_);

    auto body = content;
    body.removeFromTop(gapUnderTitle);
    body = body.withTrimmedLeft(padX).withTrimmedRight(padX);
    body.removeFromBottom(bottomReserve);

    if (contentLayout_ == ContentLayout::SingleLane)
        paintSingleLaneBody(g, body, bodyFont);
    else
        paintDualLaneBody(g, body, bodyFont);
}

void BankTransferProgressDialog::resized()
{
    auto content = getDialogBounds().reduced(getBorderThickness());
    content.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int buttonHeight = scaledButtonHeight(uiScale_);
    const int bottomMargin = scaledBottomMargin(uiScale_);
    const juce::Rectangle<int> buttonRow { content.getX(),
                                           content.getBottom() - bottomMargin - buttonHeight,
                                           content.getWidth(),
                                           buttonHeight };

    DialogMatrixHelpers::layoutCentredButtonRow(
        buttonRow,
        uiScale_,
        { { cancelButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, cancelButton_->getButtonText(), uiScale_) } });
}

bool BankTransferProgressDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey && cancelButton_->isEnabled() && onCancelRequested_)
    {
        onCancelRequested_();
        return true;
    }

    return false;
}
