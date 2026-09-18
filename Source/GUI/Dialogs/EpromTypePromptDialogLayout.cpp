#include "EpromTypePromptDialog.h"

#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kDialogBorderColour = 0xff5E5E5E;
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
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    const int rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    const int rowsHeight = controlHeight * 4 + rowGap * 3;

    auto content = inner.reduced(padding);
    layout.buttonRow = content.removeFromBottom(buttonHeight);
    content.removeFromBottom(gapAboveButtons);

    const auto bodyFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const int gapUnderTitle = juce::roundToInt(bodyFont.getHeight());
    content.removeFromTop(gapUnderTitle);

    const int maxBodyHeight = juce::jmax(0, content.getHeight() - rowsHeight - rowGap);
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

    layout.bodyTextArea = content.removeFromTop(juce::jmin(bodyHeight, maxBodyHeight));
    content.removeFromTop(rowGap);
    layout.controlBand = content.removeFromTop(rowsHeight);

    const int labelWidth = juce::roundToInt(static_cast<float>(kLabelWidth_) * uiScale_);
    const int comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);
    const int rowWidth = labelWidth + comboWidth;
    const auto centredBand = layout.controlBand.withSizeKeepingCentre(rowWidth, rowsHeight);
    layout.deviceChromeBounds = centredBand.withY(centredBand.getY() + (controlHeight + rowGap) * 2)
                                    .withHeight(controlHeight);
    return layout;
}

juce::String EpromTypePromptDialog::searchingDetailWithDots() const
{
    static constexpr const char* kFrames[] = { ".", "..", "..." };
    return juce::String(PluginDisplayNames::Dialogs::EpromTypePrompt::kSearching)
           + kFrames[juce::jlimit(0, 2, searchingDotFrame_)];
}

void EpromTypePromptDialog::paintDeviceChrome(juce::Graphics& g, juce::Rectangle<int> bounds) const
{
    const auto font = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_);
    const auto badgeFont = skin_->getBaseFontBold().withHeight(font.getHeight());
    const int badgeHeight = juce::roundToInt(static_cast<float>(kDeviceBadgeHeight_) * uiScale_);
    const int badgePad = juce::roundToInt(static_cast<float>(kDeviceBadgePad_) * uiScale_);
    const int badgeGap = juce::roundToInt(static_cast<float>(kDeviceBadgeGap_) * uiScale_);

    const auto badgeFill = deviceRowView_.identityOk
        ? skin_->getColour(SkinColourId::kFooterMessageInfo)
        : skin_->getColour(SkinColourId::kFooterMessageError).withAlpha(0.8f);
    const auto detailColour = skin_->getColour(SkinColourId::kFooterMessageInfo);

    g.setFont(badgeFont);
    const auto badgeLabel = PluginDisplayNames::FooterPanel::kDeviceLabel;
    const int labelWidth = juce::roundToInt(juce::GlyphArrangement::getStringWidth(badgeFont, badgeLabel));
    const int badgeWidth = juce::jmin(bounds.getWidth(), labelWidth + 2 * badgePad);
    const int badgeY = bounds.getCentreY() - badgeHeight / 2;
    const juce::Rectangle<int> badgeBounds { bounds.getX(), badgeY, badgeWidth, badgeHeight };

    g.setColour(badgeFill);
    g.fillRect(badgeBounds);
    g.setColour(skin_->getColour(SkinColourId::kFooterPanelBackground));
    g.drawText(badgeLabel, badgeBounds, juce::Justification::centred, false);

    auto detailBounds = bounds;
    detailBounds.removeFromLeft(badgeWidth + badgeGap);
    g.setFont(font);
    g.setColour(detailColour);

    const juce::String detail = deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kSearching
        ? searchingDetailWithDots()
        : deviceRowView_.detailText;
    g.drawText(detail, detailBounds, juce::Justification::centredLeft, true);
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
    paintDeviceChrome(g, layout.deviceChromeBounds);
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
    const int rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    const int rowWidth = labelWidth + comboWidth;
    const int rowsHeight = controlHeight * 4 + rowGap * 3;

    const auto centredBand = layout.controlBand.withSizeKeepingCentre(rowWidth, rowsHeight);

    auto placeRow = [&](int rowIndex, TSS::Label& label, TSS::ComboBox& combo)
    {
        const int y = centredBand.getY() + rowIndex * (controlHeight + rowGap);
        label.setBounds(centredBand.getX(), y, labelWidth, controlHeight);
        label.setUiScale(uiScale_);
        combo.setBounds(centredBand.getX() + labelWidth, y, comboWidth, controlHeight);
        combo.setUiScale(uiScale_);
    };

    placeRow(0, *midiFromLabel_, *midiFromCombo_);
    placeRow(1, *midiToLabel_, *midiToCombo_);
    placeRow(3, *epromTypeLabel_, *epromTypeCombo_);

    auto buttonRow = layout.buttonRow;
    confirmButton_.setBounds(buttonRow.removeFromRight(confirmWidth));
    buttonRow.removeFromRight(buttonGap);
    specifyLaterButton_.setBounds(buttonRow.removeFromRight(laterWidth));
}
