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

    masterSettingsOnlyButton_ = DialogMatrixHelpers::makeButton(skin, 148, Dialog::kMasterSettingsOnly);
    fullMasterButton_ = DialogMatrixHelpers::makeButton(skin, 268, Dialog::kFullMaster);
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

int MasterM1kmLoadChoiceDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> MasterM1kmLoadChoiceDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignHeight) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;

    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
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
    textArea.removeFromBottom(juce::roundToInt(40.0f * uiScale_));

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(bodyFont);
    g.drawFittedText(Dialog::kBody, textArea, juce::Justification::topLeft, 8);
}

void MasterM1kmLoadChoiceDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonHeight) * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);
    const int cancelWidth = juce::roundToInt(
        static_cast<float>(DialogMatrixHelpers::kDefaultButtonWidth) * uiScale_);
    const int settingsOnlyWidth = juce::roundToInt(148.0f * uiScale_);
    const int fullMasterWidth = juce::roundToInt(268.0f * uiScale_);

    auto buttonRow = inner.reduced(padding).removeFromBottom(buttonHeight);
    cancelButton_->setBounds(buttonRow.removeFromLeft(cancelWidth));
    buttonRow.removeFromLeft(buttonGap);
    masterSettingsOnlyButton_->setBounds(buttonRow.removeFromLeft(settingsOnlyWidth));
    buttonRow.removeFromLeft(buttonGap);
    fullMasterButton_->setBounds(buttonRow.removeFromLeft(fullMasterWidth));
}

void MasterM1kmLoadChoiceDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
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
