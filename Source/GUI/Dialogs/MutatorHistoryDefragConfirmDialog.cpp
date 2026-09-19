#include "MutatorHistoryDefragConfirmDialog.h"

#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorHistoryDefrag;
}

MutatorHistoryDefragConfirmDialog::MutatorHistoryDefragConfirmDialog(
    TSS::ISkin& skin,
    std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
    , defragButton_(Dialog::kConfirm)
    , cancelButton_(Dialog::kCancel)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    defragButton_.onClick = [this] { confirm(); };
    cancelButton_.onClick = [this] { dismiss(); };
    // Cancel must not consume Return — Enter always confirms Defrag (primary).
    cancelButton_.setWantsKeyboardFocus(false);
    cancelButton_.setMouseClickGrabsKeyboardFocus(false);
    defragButton_.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(defragButton_);
    addAndMakeVisible(cancelButton_);
}

MutatorHistoryDefragConfirmDialog::~MutatorHistoryDefragConfirmDialog() = default;

void MutatorHistoryDefragConfirmDialog::prepareForShow(std::function<void()> onConfirm)
{
    onConfirm_ = std::move(onConfirm);
    repaint();
}

void MutatorHistoryDefragConfirmDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    repaint();
}

void MutatorHistoryDefragConfirmDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

int MutatorHistoryDefragConfirmDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> MutatorHistoryDefragConfirmDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignHeight) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;

    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void MutatorHistoryDefragConfirmDialog::dismiss()
{
    if (onDismissRequested_)
        onDismissRequested_();
}

void MutatorHistoryDefragConfirmDialog::confirm()
{
    auto cb = std::move(onConfirm_);
    if (cb)
        cb();

    dismiss();
}

void MutatorHistoryDefragConfirmDialog::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground).withAlpha(0.85f));

    const auto dialogBounds = getDialogBounds();
    const int border = getBorderThickness();

    g.setColour(juce::Colour(kDialogBorderColour));
    g.fillRect(dialogBounds);

    auto inner = dialogBounds.reduced(border);
    const int titleBarHeight = juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_);
    auto titleBar = inner.removeFromTop(titleBarHeight);
    auto content = inner;

    g.setColour(skin_->getColour(SkinColourId::kHeaderPanelBackground));
    g.fillRect(titleBar);
    g.fillRect(content);

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(skin_->getBaseFontBold().withHeight(skin_->getBaseFontBold().getHeight() * uiScale_));
    g.drawText(Dialog::kTitle, titleBar, juce::Justification::centred, false);

    // Custom modal scheme: exactly 1em under the title, then body copy (no extra top padding).
    const auto bodyFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    const int padX = juce::roundToInt(12.0f * uiScale_);

    auto textArea = content;
    textArea.removeFromTop(gapUnderTitle);
    textArea = textArea.withTrimmedLeft(padX).withTrimmedRight(padX);
    textArea.removeFromBottom(juce::roundToInt(36.0f * uiScale_));

    g.setFont(bodyFont);
    g.drawFittedText(Dialog::kBody, textArea, juce::Justification::topLeft, 6);
}

void MutatorHistoryDefragConfirmDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(24.0f * uiScale_);
    const int buttonWidth = juce::roundToInt(72.0f * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);

    auto buttonRow = inner.reduced(padding).removeFromBottom(buttonHeight);
    // LTR: Cancel left, Defrag (primary / default) right
    defragButton_.setBounds(buttonRow.removeFromRight(buttonWidth));
    buttonRow.removeFromRight(buttonGap);
    cancelButton_.setBounds(buttonRow.removeFromRight(buttonWidth));
}

void MutatorHistoryDefragConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
        dismiss();
}

bool MutatorHistoryDefragConfirmDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        dismiss();
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        confirm();
        return true;
    }

    return Component::keyPressed(key);
}
