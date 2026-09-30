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

DialogMatrixHelpers::TextModalLayout MasterInitConfirmDialog::computeLayout() const
{
    const auto body = formatBodyText();
    return DialogMatrixHelpers::computeTextModalLayout({ .skin = *skin_,
                                                         .bodyText = body,
                                                         .hostBounds = getLocalBounds(),
                                                         .designWidth = kDesignWidth,
                                                         .uiScale = uiScale_ });
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
    const auto layout = computeLayout();
    const auto& geometry = layout.geometry;
    const auto title = globalReset_ ? PluginDisplayNames::Dialogs::MasterGlobalInitConfirm::kTitle
                                    : PluginDisplayNames::Dialogs::MasterInitConfirm::kTitle;

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = geometry.dialogBounds,
        .borderThickness = geometry.border,
        .titleBarHeight = geometry.titleBarHeight,
        .title = title,
        .uiScale = uiScale_ });

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    DialogMatrixHelpers::paintBodyText(g, layout.bodyFont, formatBodyText(), geometry.textArea);
}

void MasterInitConfirmDialog::resized()
{
    // LTR: Cancel left, Reset (primary / default) right; the pair is centred.
    DialogMatrixHelpers::layoutCentredButtonRow(
        computeLayout().geometry.buttonRow,
        uiScale_,
        { { cancelButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, cancelButton_->getButtonText(), uiScale_) },
          { resetButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, resetButton_->getButtonText(), uiScale_) } });
}

void MasterInitConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! computeLayout().geometry.dialogBounds.contains(e.getPosition()))
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
