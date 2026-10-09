#include "MutatorHistoryDefragConfirmDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Layout/ScaledDrawing.h"
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

DialogMatrixHelpers::TextModalLayout MutatorHistoryDefragConfirmDialog::computeLayout() const
{
    const juce::String body(Dialog::kBody);
    return DialogMatrixHelpers::computeTextModalLayout({
        .skin = *skin_,
        .bodyText = body,
        .hostBounds = getLocalBounds(),
        .designWidth = kDesignWidth,
        .uiScale = uiScale_,
        .systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
    });
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
    const auto layout = computeLayout();
    const auto& geometry = layout.geometry;

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = geometry.dialogBounds,
        .borderThickness = geometry.border,
        .titleBarHeight = geometry.titleBarHeight,
        .title = Dialog::kTitle,
        .uiScale = uiScale_ });

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    DialogMatrixHelpers::paintBodyText(g, layout.bodyFont, Dialog::kBody, geometry.textArea);
    DialogMatrixHelpers::paintActionFooterSeparator({
        .g = g,
        .skin = *skin_,
        .geometry = geometry,
        .uiScale = uiScale_,
        .systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
    });
}

void MutatorHistoryDefragConfirmDialog::resized()
{
    // LTR: Cancel left, Defrag (primary) right; the pair is centred.
    DialogMatrixHelpers::layoutCentredButtonRow(
        computeLayout().geometry.buttonRow,
        uiScale_,
        { { cancelButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, cancelButton_->getButtonText(), uiScale_) },
          { defragButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, defragButton_->getButtonText(), uiScale_) } });
}

void MutatorHistoryDefragConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! computeLayout().geometry.dialogBounds.contains(e.getPosition()))
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
