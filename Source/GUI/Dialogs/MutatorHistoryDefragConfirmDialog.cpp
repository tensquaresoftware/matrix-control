#include "MutatorHistoryDefragConfirmDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorHistoryDefrag;
}

MutatorHistoryDefragConfirmDialog::MutatorHistoryDefragConfirmDialog(
    TSS::ISkin& skin,
    std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    defragButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kConfirm);
    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kCancel);

    defragButton_->onClick = [this] { confirm(); };
    cancelButton_->onClick = [this] { dismiss(); };
    cancelButton_->setWantsKeyboardFocus(false);
    addAndMakeVisible(*defragButton_);
    addAndMakeVisible(*cancelButton_);
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
    DialogMatrixHelpers::applyButtonSkin(*defragButton_, skin);
    DialogMatrixHelpers::applyButtonSkin(*cancelButton_, skin);
    repaint();
}

void MutatorHistoryDefragConfirmDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    DialogMatrixHelpers::applyButtonUiScale(*defragButton_, uiScale);
    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale);
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
    const auto dialogBounds = getDialogBounds();
    const int border = getBorderThickness();
    const int titleBarHeight = juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_);

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = dialogBounds,
        .borderThickness = border,
        .titleBarHeight = titleBarHeight,
        .title = Dialog::kTitle,
        .uiScale = uiScale_ });

    auto inner = dialogBounds.reduced(border);
    inner.removeFromTop(titleBarHeight);

    const auto bodyFont = DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    const int padX = juce::roundToInt(12.0f * uiScale_);

    auto textArea = inner;
    textArea.removeFromTop(gapUnderTitle);
    textArea = textArea.withTrimmedLeft(padX).withTrimmedRight(padX);
    textArea.removeFromBottom(juce::roundToInt(36.0f * uiScale_));

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(bodyFont);
    g.drawFittedText(Dialog::kBody, textArea, juce::Justification::topLeft, 6);
}

void MutatorHistoryDefragConfirmDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonHeight) * uiScale_);
    const int buttonWidth = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonWidth) * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);

    auto buttonRow = inner.reduced(padding).removeFromBottom(buttonHeight);
    defragButton_->setBounds(buttonRow.removeFromRight(buttonWidth));
    buttonRow.removeFromRight(buttonGap);
    cancelButton_->setBounds(buttonRow.removeFromRight(buttonWidth));
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
