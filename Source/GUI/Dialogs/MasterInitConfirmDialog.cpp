#include "MasterInitConfirmDialog.h"

#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;
}

MasterInitConfirmDialog::MasterInitConfirmDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
    , resetButton_(PluginDisplayNames::Dialogs::MasterInitConfirm::kConfirm)
    , cancelButton_(PluginDisplayNames::Dialogs::MasterInitConfirm::kCancel)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    resetButton_.onClick = [this] { confirm(); };
    cancelButton_.onClick = [this] { dismiss(); };
    addAndMakeVisible(resetButton_);
    addAndMakeVisible(cancelButton_);
}

MasterInitConfirmDialog::~MasterInitConfirmDialog() = default;

void MasterInitConfirmDialog::prepareForShow(const juce::String& moduleDisplayName,
                                             std::function<void()> onConfirm)
{
    moduleDisplayName_ = moduleDisplayName;
    onConfirm_ = std::move(onConfirm);
    repaint();
}

void MasterInitConfirmDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    repaint();
}

void MasterInitConfirmDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
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
    g.drawText(PluginDisplayNames::Dialogs::MasterInitConfirm::kTitle,
               titleBar,
               juce::Justification::centred,
               false);

    const int padding = juce::roundToInt(12.0f * uiScale_);
    auto textArea = content.reduced(padding);
    textArea.removeFromBottom(juce::roundToInt(36.0f * uiScale_));

    g.setFont(skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_));
    g.drawFittedText(formatBodyText(), textArea, juce::Justification::topLeft, 6);
}

void MasterInitConfirmDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(24.0f * uiScale_);
    const int buttonWidth = juce::roundToInt(72.0f * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);

    auto buttonRow = inner.reduced(padding).removeFromBottom(buttonHeight);
    cancelButton_.setBounds(buttonRow.removeFromRight(buttonWidth));
    buttonRow.removeFromRight(buttonGap);
    resetButton_.setBounds(buttonRow.removeFromRight(buttonWidth));
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

    return Component::keyPressed(key);
}
