#include "MatrixOrderedConfirmDialog.h"

#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    int estimateButtonWidth(const juce::String& text, float uiScale)
    {
        const int base = DialogMatrixHelpers::kDefaultButtonWidth;
        const int perChar = 8;
        const int estimated = base + juce::jmax(0, text.length() - 6) * perChar;
        return juce::roundToInt(static_cast<float>(juce::jlimit(base, 280, estimated)) * uiScale);
    }
}

MatrixOrderedConfirmDialog::MatrixOrderedConfirmDialog(
    TSS::ISkin& skin,
    float uiScale,
    const PluginEditorInternal::OrderedConfirmAlertOptions& options)
    : skin_(&skin)
    , uiScale_(uiScale)
    , title_(options.title)
    , message_(options.message)
    , hasMiddle_(options.middleLabel.isNotEmpty())
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, options.cancelLabel);
    primaryButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, options.primaryLabel);

    cancelButton_->setWantsKeyboardFocus(false);
    cancelButton_->onClick = [this] { finish(0); };
    primaryButton_->onClick = [this] { finish(1); };

    addAndMakeVisible(*cancelButton_);
    addAndMakeVisible(*primaryButton_);

    if (hasMiddle_)
    {
        middleButton_ = DialogMatrixHelpers::makeButton(
            skin, DialogMatrixHelpers::kDefaultButtonWidth, options.middleLabel);
        middleButton_->setWantsKeyboardFocus(false);
        middleButton_->onClick = [this] { finish(2); };
        addAndMakeVisible(*middleButton_);
    }

    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale_);
    DialogMatrixHelpers::applyButtonUiScale(*primaryButton_, uiScale_);
    if (middleButton_ != nullptr)
        DialogMatrixHelpers::applyButtonUiScale(*middleButton_, uiScale_);
}

int MatrixOrderedConfirmDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> MatrixOrderedConfirmDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth_) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignHeight_) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;
    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void MatrixOrderedConfirmDialog::finish(int code)
{
    if (isCurrentlyModal())
        exitModalState(code);
}

void MatrixOrderedConfirmDialog::paint(juce::Graphics& g)
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

    auto inner = dialogBounds.reduced(border);
    inner.removeFromTop(titleBarHeight);

    const auto bodyFont = DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    const int padX = juce::roundToInt(12.0f * uiScale_);

    auto textArea = inner;
    textArea.removeFromTop(gapUnderTitle);
    textArea = textArea.withTrimmedLeft(padX).withTrimmedRight(padX);
    textArea.removeFromBottom(juce::roundToInt(40.0f * uiScale_));

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(bodyFont);
    g.drawFittedText(message_, textArea, juce::Justification::topLeft, 8);
}

void MatrixOrderedConfirmDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonHeight) * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);

    auto buttonRow = inner.reduced(padding).removeFromBottom(buttonHeight);

    const int primaryWidth = estimateButtonWidth(primaryButton_->getButtonText(), uiScale_);
    primaryButton_->setBounds(buttonRow.removeFromRight(primaryWidth));

    if (hasMiddle_ && middleButton_ != nullptr)
    {
        buttonRow.removeFromRight(buttonGap);
        const int middleWidth = estimateButtonWidth(middleButton_->getButtonText(), uiScale_);
        middleButton_->setBounds(buttonRow.removeFromRight(middleWidth));
    }

    buttonRow.removeFromRight(buttonGap);
    const int cancelWidth = estimateButtonWidth(cancelButton_->getButtonText(), uiScale_);
    cancelButton_->setBounds(buttonRow.removeFromRight(cancelWidth));
}

void MatrixOrderedConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
        finish(0);
}

bool MatrixOrderedConfirmDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        finish(0);
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        finish(1);
        return true;
    }

    return Component::keyPressed(key);
}

MatrixMutatorDeleteConfirmDialog::MatrixMutatorDeleteConfirmDialog(TSS::ISkin& skin, float uiScale)
    : skin_(&skin)
    , uiScale_(uiScale)
    , dontAskAgain_(PluginDisplayNames::Dialogs::MutatorDeleteConfirm::kDontAskAgain)
{
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorDeleteConfirm;

    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    dontAskAgain_.setName({});
    dontAskAgain_.setWantsKeyboardFocus(false);
    dontAskAgain_.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(dontAskAgain_);

    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kCancel);
    deleteButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kDelete);

    cancelButton_->setWantsKeyboardFocus(false);
    cancelButton_->onClick = [this] { finish(false); };
    deleteButton_->onClick = [this] { finish(true); };

    addAndMakeVisible(*cancelButton_);
    addAndMakeVisible(*deleteButton_);

    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale_);
    DialogMatrixHelpers::applyButtonUiScale(*deleteButton_, uiScale_);
}

int MatrixMutatorDeleteConfirmDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> MatrixMutatorDeleteConfirmDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth_) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignHeight_) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;
    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void MatrixMutatorDeleteConfirmDialog::finish(bool confirmed)
{
    result_ = { confirmed, dontAskAgain_.getToggleState() };
    if (isCurrentlyModal())
        exitModalState(confirmed ? 1 : 0);
}

void MatrixMutatorDeleteConfirmDialog::paint(juce::Graphics& g)
{
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorDeleteConfirm;

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
    textArea.removeFromBottom(juce::roundToInt(72.0f * uiScale_));

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(bodyFont);
    g.drawFittedText(Dialog::kBody, textArea, juce::Justification::topLeft, 8);
}

void MatrixMutatorDeleteConfirmDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonHeight) * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);
    const int checkHeight = juce::roundToInt(22.0f * uiScale_);

    auto content = inner.reduced(padding);
    auto buttonRow = content.removeFromBottom(buttonHeight);
    content.removeFromBottom(buttonGap);
    auto checkRow = content.removeFromBottom(checkHeight);

    dontAskAgain_.setBounds(checkRow);

    const int deleteWidth = estimateButtonWidth(deleteButton_->getButtonText(), uiScale_);
    deleteButton_->setBounds(buttonRow.removeFromRight(deleteWidth));
    buttonRow.removeFromRight(buttonGap);
    const int cancelWidth = estimateButtonWidth(cancelButton_->getButtonText(), uiScale_);
    cancelButton_->setBounds(buttonRow.removeFromRight(cancelWidth));
}

void MatrixMutatorDeleteConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
        finish(false);
}

bool MatrixMutatorDeleteConfirmDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        finish(false);
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        finish(true);
        return true;
    }

    return Component::keyPressed(key);
}
