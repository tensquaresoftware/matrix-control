#include "MasterInitConfirmDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

MasterInitConfirmDialog::MasterInitConfirmDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    resetButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, PluginDisplayNames::Dialogs::MasterInitConfirm::kConfirm);
    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, PluginDisplayNames::Dialogs::MasterInitConfirm::kCancel);

    resetButton_->onClick = [this] { confirm(); };
    cancelButton_->onClick = [this] { dismiss(); };
    // Cancel must not consume Return — Enter always confirms Reset (primary).
    cancelButton_->setWantsKeyboardFocus(false);
    addAndMakeVisible(*resetButton_);
    addAndMakeVisible(*cancelButton_);
}

MasterInitConfirmDialog::~MasterInitConfirmDialog() = default;

void MasterInitConfirmDialog::prepareForShow(const juce::String& moduleDisplayName,
                                             std::function<void()> onConfirm)
{
    globalReset_ = false;
    moduleDisplayName_ = moduleDisplayName;
    onConfirm_ = std::move(onConfirm);
    repaint();
}

void MasterInitConfirmDialog::prepareForGlobalShow(std::function<void()> onConfirm)
{
    globalReset_ = true;
    moduleDisplayName_.clear();
    onConfirm_ = std::move(onConfirm);
    repaint();
}

void MasterInitConfirmDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    DialogMatrixHelpers::applyButtonSkin(*resetButton_, skin);
    DialogMatrixHelpers::applyButtonSkin(*cancelButton_, skin);
    repaint();
}

void MasterInitConfirmDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    DialogMatrixHelpers::applyButtonUiScale(*resetButton_, uiScale);
    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale);
    resized();
    repaint();
}

int MasterInitConfirmDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> MasterInitConfirmDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignHeight) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;

    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

juce::String MasterInitConfirmDialog::formatBodyText() const
{
    if (globalReset_)
        return PluginDisplayNames::Dialogs::MasterGlobalInitConfirm::kBody;

    return juce::String(PluginDisplayNames::Dialogs::MasterInitConfirm::kBodyTemplate)
        .replace("{MODULE}", moduleDisplayName_);
}

void MasterInitConfirmDialog::dismiss()
{
    if (onDismissRequested_)
        onDismissRequested_();
}

void MasterInitConfirmDialog::confirm()
{
    if (onConfirm_)
        onConfirm_();

    dismiss();
}

void MasterInitConfirmDialog::paint(juce::Graphics& g)
{
    const auto dialogBounds = getDialogBounds();
    const int border = getBorderThickness();
    const int titleBarHeight = juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_);
    const auto title = globalReset_ ? PluginDisplayNames::Dialogs::MasterGlobalInitConfirm::kTitle
                                    : PluginDisplayNames::Dialogs::MasterInitConfirm::kTitle;

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = dialogBounds,
        .borderThickness = border,
        .titleBarHeight = titleBarHeight,
        .title = title,
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
    g.drawFittedText(formatBodyText(), textArea, juce::Justification::topLeft, 6);
}

void MasterInitConfirmDialog::resized()
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
    // LTR: Cancel left, Reset (primary / default) right
    resetButton_->setBounds(buttonRow.removeFromRight(buttonWidth));
    buttonRow.removeFromRight(buttonGap);
    cancelButton_->setBounds(buttonRow.removeFromRight(buttonWidth));
}

void MasterInitConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
        dismiss();
}

bool MasterInitConfirmDialog::keyPressed(const juce::KeyPress& key)
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
