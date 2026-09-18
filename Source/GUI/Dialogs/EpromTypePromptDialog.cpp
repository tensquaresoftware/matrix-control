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
    , confirmButton_(PluginDisplayNames::Dialogs::EpromTypePrompt::kConfirm)
    , specifyLaterButton_(PluginDisplayNames::Dialogs::EpromTypePrompt::kSpecifyLater)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    epromTypeLabel_ = std::make_unique<TSS::Label>(
        kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin),
        PluginDisplayNames::Dialogs::EpromTypePrompt::kEpromTypeLabel);
    epromTypeCombo_ = std::make_unique<TSS::ComboBox>(
        kComboWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin));
    epromTypeCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));

    confirmButton_.onClick = [this] { confirm(); };
    specifyLaterButton_.onClick = [this] { dismissAsLater(); };
    specifyLaterButton_.setWantsKeyboardFocus(false);
    specifyLaterButton_.setMouseClickGrabsKeyboardFocus(false);
    confirmButton_.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(*epromTypeLabel_);
    addAndMakeVisible(*epromTypeCombo_);
    addAndMakeVisible(confirmButton_);
    addAndMakeVisible(specifyLaterButton_);
}

EpromTypePromptDialog::~EpromTypePromptDialog() = default;

void EpromTypePromptDialog::prepareForShow(PrepareForShowArgs args)
{
    onConfirm_ = std::move(args.onConfirm);
    onLater_ = std::move(args.onLater);
    includeFirmwareSuggestionHint_ = args.includeFirmwareSuggestionHint;
    populateComboItems(args.deviceType, args.preferredSelectedId);
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
    epromTypeLabel_->setLook(TSS::labelLookFromSkin(skin));
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

EpromTypePromptDialog::ContentLayout EpromTypePromptDialog::computeContentLayout() const
{
    ContentLayout layout;

    auto inner = getDialogBounds().reduced(getBorderThickness());
    inner.removeFromTop(juce::roundToInt(static_cast<float>(kTitleBarHeight_) * uiScale_));

    const int padding = juce::roundToInt(12.0f * uiScale_);
    const int buttonHeight = juce::roundToInt(24.0f * uiScale_);
    const int gapAboveButtons = juce::roundToInt(8.0f * uiScale_);

    auto content = inner.reduced(padding);
    layout.buttonRow = content.removeFromBottom(buttonHeight);
    content.removeFromBottom(gapAboveButtons);

    const auto bodyFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    content.removeFromTop(gapUnderTitle);

    const int maxBodyHeight = juce::jmax(0, content.getHeight() / 2);
    juce::GlyphArrangement glyphs;
    glyphs.addFittedText(bodyFont,
                         bodyText(),
                         0.0f,
                         0.0f,
                         static_cast<float>(content.getWidth()),
                         static_cast<float>(maxBodyHeight),
                         juce::Justification::topLeft,
                         5);
    const int bodyHeight = juce::jmax(juce::roundToInt(bodyFont.getHeight()),
                                      juce::roundToInt(glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).getHeight()));

    layout.bodyTextArea = content.removeFromTop(bodyHeight);
    layout.controlBand = content;
    return layout;
}

juce::String EpromTypePromptDialog::bodyText() const
{
    juce::String text(PluginDisplayNames::Dialogs::EpromTypePrompt::kBody);
    if (includeFirmwareSuggestionHint_)
        text += PluginDisplayNames::Dialogs::EpromTypePrompt::kBodySuggestionSuffix;
    return text;
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
    auto contentFill = inner;

    g.setColour(skin_->getColour(SkinColourId::kHeaderPanelBackground));
    g.fillRect(titleBar);
    g.fillRect(contentFill);

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(skin_->getBaseFontBold().withHeight(skin_->getBaseFontBold().getHeight() * uiScale_));
    g.drawText(PluginDisplayNames::Dialogs::EpromTypePrompt::kTitle,
               titleBar,
               juce::Justification::centred,
               false);

    const auto layout = computeContentLayout();
    const auto bodyFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    g.setFont(bodyFont);
    g.drawFittedText(bodyText(), layout.bodyTextArea, juce::Justification::topLeft, 5);
}

void EpromTypePromptDialog::resized()
{
    const auto layout = computeContentLayout();

    const int confirmWidth = juce::roundToInt(static_cast<float>(kConfirmButtonWidth_) * uiScale_);
    const int laterWidth = juce::roundToInt(static_cast<float>(kSpecifyLaterButtonWidth_) * uiScale_);
    const int buttonGap = juce::roundToInt(8.0f * uiScale_);
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    const int labelWidth = juce::roundToInt(static_cast<float>(kLabelWidth_) * uiScale_);
    const int comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);
    const int rowWidth = labelWidth + comboWidth;

    const auto centredRow = layout.controlBand.withSizeKeepingCentre(rowWidth, controlHeight);
    epromTypeLabel_->setBounds(centredRow.getX(), centredRow.getY(), labelWidth, controlHeight);
    epromTypeLabel_->setUiScale(uiScale_);
    epromTypeCombo_->setBounds(centredRow.getX() + labelWidth, centredRow.getY(), comboWidth, controlHeight);
    epromTypeCombo_->setUiScale(uiScale_);

    auto buttonRow = layout.buttonRow;
    // LTR: SPECIFY LATER left, CONFIRM (primary) right
    confirmButton_.setBounds(buttonRow.removeFromRight(confirmWidth));
    buttonRow.removeFromRight(buttonGap);
    specifyLaterButton_.setBounds(buttonRow.removeFromRight(laterWidth));
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
