#include "EpromTypePromptDialog.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;
}

EpromTypePromptDialog::EpromTypePromptDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
    , okButton_(PluginDisplayNames::Dialogs::EpromTypePrompt::kOk)
    , laterButton_(PluginDisplayNames::Dialogs::EpromTypePrompt::kLater)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    epromTypeCombo_ = std::make_unique<TSS::ComboBox>(
        kComboWidth_, kComboHeight_, TSS::comboBoxLookFromSkin(skin));
    epromTypeCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));

    okButton_.onClick = [this] { confirm(); };
    laterButton_.onClick = [this] { dismissAsLater(); };
    laterButton_.setWantsKeyboardFocus(false);
    laterButton_.setMouseClickGrabsKeyboardFocus(false);
    okButton_.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(*epromTypeCombo_);
    addAndMakeVisible(okButton_);
    addAndMakeVisible(laterButton_);
}

EpromTypePromptDialog::~EpromTypePromptDialog() = default;

void EpromTypePromptDialog::prepareForShow(MatrixDeviceTypes::Type deviceType,
                                           int preferredSelectedId,
                                           std::function<void(int selectedId)> onConfirm,
                                           std::function<void()> onLater)
{
    onConfirm_ = std::move(onConfirm);
    onLater_ = std::move(onLater);
    populateComboItems(deviceType, preferredSelectedId);
    repaint();
}

void EpromTypePromptDialog::populateComboItems(MatrixDeviceTypes::Type deviceType,
                                               int preferredSelectedId)
{
    const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
    const int selectedId = Core::EpromTypePolicy::coerceForDeviceFamily(preferredSelectedId, family);

    epromTypeCombo_->clear(juce::dontSendNotification);
    Core::EpromTypePolicy::forEachValidItem(family, [this](int id)
    {
        epromTypeCombo_->addItem(Core::EpromTypePolicy::displayNameForId(id), id);
    });
    epromTypeCombo_->setSelectedId(selectedId, juce::dontSendNotification);
}

void EpromTypePromptDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    epromTypeCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    epromTypeCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    repaint();
}

void EpromTypePromptDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

int EpromTypePromptDialog::getBorderThickness() const
{
    return juce::roundToInt(static_cast<float>(kBorderThickness_) * uiScale_);
}

juce::Rectangle<int> EpromTypePromptDialog::getDialogBounds() const
{
    const int border = getBorderThickness();
    const int dialogWidth = juce::roundToInt(static_cast<float>(kDesignWidth) * uiScale_) + border * 2;
    const int dialogHeight = juce::roundToInt(static_cast<float>(kDesignHeight) * uiScale_)
                             + juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_)
                             + border * 2;

    return getLocalBounds().withSizeKeepingCentre(dialogWidth, dialogHeight);
}

void EpromTypePromptDialog::dismissAsLater()
{
    if (onLater_)
        onLater_();

    if (onDismissRequested_)
        onDismissRequested_();
}

void EpromTypePromptDialog::confirm()
{
    const int selectedId = epromTypeCombo_->getSelectedId();
    if (onConfirm_)
        onConfirm_(selectedId > 0 ? selectedId : PluginIDs::Settings::EpromType::kDefault);

    if (onDismissRequested_)
        onDismissRequested_();
}

void EpromTypePromptDialog::paint(juce::Graphics& g)
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
    g.drawText(PluginDisplayNames::Dialogs::EpromTypePrompt::kTitle,
               titleBar,
               juce::Justification::centred,
               false);

    const auto bodyFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    const int padX = juce::roundToInt(12.0f * uiScale_);

    auto textArea = content;
    textArea.removeFromTop(gapUnderTitle);
    textArea = textArea.withTrimmedLeft(padX).withTrimmedRight(padX);
    textArea.removeFromBottom(juce::roundToInt(64.0f * uiScale_));

    g.setFont(bodyFont);
    g.drawFittedText(PluginDisplayNames::Dialogs::EpromTypePrompt::kBody,
                     textArea,
                     juce::Justification::topLeft,
                     6);
}

void EpromTypePromptDialog::resized()
{
    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(24.0f * uiScale_);
    const int buttonWidth = juce::roundToInt(72.0f * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);
    const int comboHeight = juce::roundToInt(static_cast<float>(kComboHeight_) * uiScale_);
    const int comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);

    auto content = inner.reduced(padding);
    auto buttonRow = content.removeFromBottom(buttonHeight);
    content.removeFromBottom(juce::roundToInt(8.0f * uiScale_));
    auto comboRow = content.removeFromBottom(comboHeight);

    epromTypeCombo_->setBounds(comboRow.withSizeKeepingCentre(comboWidth, comboHeight));
    epromTypeCombo_->setUiScale(uiScale_);

    // LTR: LATER left, OK (primary) right
    okButton_.setBounds(buttonRow.removeFromRight(buttonWidth));
    buttonRow.removeFromRight(buttonGap);
    laterButton_.setBounds(buttonRow.removeFromRight(buttonWidth));
}

void EpromTypePromptDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
        dismissAsLater();
}

bool EpromTypePromptDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        dismissAsLater();
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        confirm();
        return true;
    }

    return Component::keyPressed(key);
}
