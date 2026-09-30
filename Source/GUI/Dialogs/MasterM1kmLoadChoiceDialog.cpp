#include "MasterM1kmLoadChoiceDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    namespace Dialog = PluginDisplayNames::Dialogs::MasterM1kmLoadChoice;
}

MasterM1kmLoadChoiceDialog::MasterM1kmLoadChoiceDialog(TSS::ISkin& skin,
                                                       std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    masterSettingsOnlyButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kMasterSettingsOnly);
    fullMasterButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kFullMaster);
    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kCancel);

    masterSettingsOnlyButton_->onClick = [this] { chooseMasterSettingsOnly(); };
    fullMasterButton_->onClick = [this] { chooseFullMaster(); };
    cancelButton_->onClick = [this] { dismiss(); };

    // No Return auto-pick: both load options are intentional; Escape / Cancel only.
    cancelButton_->setWantsKeyboardFocus(false);
    addAndMakeVisible(*masterSettingsOnlyButton_);
    addAndMakeVisible(*fullMasterButton_);
    addAndMakeVisible(*cancelButton_);
}

MasterM1kmLoadChoiceDialog::~MasterM1kmLoadChoiceDialog() = default;

void MasterM1kmLoadChoiceDialog::prepareForShow(std::function<void()> onMasterSettingsOnly,
                                                std::function<void()> onFullMaster)
{
    onMasterSettingsOnly_ = std::move(onMasterSettingsOnly);
    onFullMaster_ = std::move(onFullMaster);
    repaint();
}

void MasterM1kmLoadChoiceDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    DialogMatrixHelpers::applyButtonSkin(*masterSettingsOnlyButton_, skin);
    DialogMatrixHelpers::applyButtonSkin(*fullMasterButton_, skin);
    DialogMatrixHelpers::applyButtonSkin(*cancelButton_, skin);
    repaint();
}

void MasterM1kmLoadChoiceDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    DialogMatrixHelpers::applyButtonUiScale(*masterSettingsOnlyButton_, uiScale);
    DialogMatrixHelpers::applyButtonUiScale(*fullMasterButton_, uiScale);
    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale);
    resized();
    repaint();
}

DialogMatrixHelpers::TextModalLayout MasterM1kmLoadChoiceDialog::computeLayout() const
{
    const juce::String body(Dialog::kBody);
    return DialogMatrixHelpers::computeTextModalLayout({ .skin = *skin_,
                                                         .bodyText = body,
                                                         .hostBounds = getLocalBounds(),
                                                         .designWidth = kDesignWidth,
                                                         .uiScale = uiScale_ });
}

void MasterM1kmLoadChoiceDialog::dismiss()
{
    onMasterSettingsOnly_ = nullptr;
    onFullMaster_ = nullptr;

    if (onDismissRequested_)
        onDismissRequested_();
}

void MasterM1kmLoadChoiceDialog::chooseMasterSettingsOnly()
{
    auto cb = std::move(onMasterSettingsOnly_);
    onFullMaster_ = nullptr;

    if (cb)
        cb();

    dismiss();
}

void MasterM1kmLoadChoiceDialog::chooseFullMaster()
{
    auto cb = std::move(onFullMaster_);
    onMasterSettingsOnly_ = nullptr;

    if (cb)
        cb();

    dismiss();
}

void MasterM1kmLoadChoiceDialog::paint(juce::Graphics& g)
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
}

void MasterM1kmLoadChoiceDialog::resized()
{
    const auto scaledWidth = [this](const TSS::Button& button)
    {
        return DialogMatrixHelpers::estimateButtonWidth(*skin_, button.getButtonText(), uiScale_);
    };

    DialogMatrixHelpers::layoutCentredButtonRow(
        computeLayout().geometry.buttonRow,
        uiScale_,
        { { cancelButton_.get(), scaledWidth(*cancelButton_) },
          { masterSettingsOnlyButton_.get(), scaledWidth(*masterSettingsOnlyButton_) },
          { fullMasterButton_.get(), scaledWidth(*fullMasterButton_) } });
}

void MasterM1kmLoadChoiceDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! computeLayout().geometry.dialogBounds.contains(e.getPosition()))
        dismiss();
}

bool MasterM1kmLoadChoiceDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        dismiss();
        return true;
    }

    return Component::keyPressed(key);
}
