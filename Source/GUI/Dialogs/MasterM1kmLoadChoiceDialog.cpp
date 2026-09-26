#include "MasterM1kmLoadChoiceDialog.h"

#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;
    namespace Dialog = PluginDisplayNames::Dialogs::MasterM1kmLoadChoice;
}

MasterM1kmLoadChoiceDialog::MasterM1kmLoadChoiceDialog(TSS::ISkin& skin,
                                                       std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
    , masterSettingsOnlyButton_(Dialog::kMasterSettingsOnly)
    , fullMasterButton_(Dialog::kFullMaster)
    , cancelButton_(Dialog::kCancel)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    masterSettingsOnlyButton_.onClick = [this] { chooseMasterSettingsOnly(); };
    fullMasterButton_.onClick = [this] { chooseFullMaster(); };
    cancelButton_.onClick = [this] { dismiss(); };

    // No Return auto-pick: both load options are intentional; Escape / Cancel only.
    cancelButton_.setWantsKeyboardFocus(false);
    cancelButton_.setMouseClickGrabsKeyboardFocus(false);
    masterSettingsOnlyButton_.setMouseClickGrabsKeyboardFocus(false);
    fullMasterButton_.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(masterSettingsOnlyButton_);
    addAndMakeVisible(fullMasterButton_);
    addAndMakeVisible(cancelButton_);
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
    repaint();
}

void MasterM1kmLoadChoiceDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
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

    const auto bodyFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    const int padX = juce::roundToInt(12.0f * uiScale_);

    auto textArea = content;
    textArea.removeFromTop(gapUnderTitle);
    textArea = textArea.withTrimmedLeft(padX).withTrimmedRight(padX);
    textArea.removeFromBottom(juce::roundToInt(40.0f * uiScale_));

    g.setFont(bodyFont);
    g.drawFittedText(Dialog::kBody, textArea, juce::Justification::topLeft, 8);
}

void MasterM1kmLoadChoiceDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(24.0f * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);
    const int cancelWidth = juce::roundToInt(72.0f * uiScale_);
    const int settingsOnlyWidth = juce::roundToInt(148.0f * uiScale_);
    const int fullMasterWidth = juce::roundToInt(268.0f * uiScale_);

    auto buttonRow = inner.reduced(padding).removeFromBottom(buttonHeight);
    // LTR: Cancel left; load options to the right (Full Master farthest right).
    cancelButton_.setBounds(buttonRow.removeFromLeft(cancelWidth));
    buttonRow.removeFromLeft(buttonGap);
    masterSettingsOnlyButton_.setBounds(buttonRow.removeFromLeft(settingsOnlyWidth));
    buttonRow.removeFromLeft(buttonGap);
    fullMasterButton_.setBounds(buttonRow.removeFromLeft(fullMasterWidth));
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
