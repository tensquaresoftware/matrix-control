#include "EpromTypePromptDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

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

    const auto bodyFont = DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_);
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
                         kMaxBodyFittedLines_);
    const int bodyHeight = juce::jmax(juce::roundToInt(bodyFont.getHeight()),
                                      juce::roundToInt(glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).getHeight()));

    layout.bodyTextArea = content.removeFromTop(juce::jmin(bodyHeight, maxBodyHeight));

    // Vertically centre the four control rows between body text and buttons.
    const int remainingHeight = content.getHeight();
    const int controlY = content.getY() + juce::jmax(0, (remainingHeight - rowsHeight) / 2);
    layout.controlBand = { content.getX(), controlY, content.getWidth(), rowsHeight };
    return layout;
}

juce::String EpromTypePromptDialog::searchingDetailWithDots() const
{
    static constexpr const char* kFrames[] = { ".", "..", "..." };
    return juce::String(PluginDisplayNames::Dialogs::EpromTypePrompt::kSearching)
           + kFrames[juce::jlimit(0, 2, searchingDotFrame_)];
}

void EpromTypePromptDialog::paint(juce::Graphics& g)
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
        .title = PluginDisplayNames::Dialogs::EpromTypePrompt::kTitle,
        .uiScale = uiScale_ });

    const auto layout = computeContentLayout();
    const auto bodyFont = DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_);
    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    g.setFont(bodyFont);
    g.drawFittedText(bodyText(), layout.bodyTextArea, juce::Justification::topLeft, kMaxBodyFittedLines_);
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

    auto placeRow = [&](int rowIndex, TSS::Label& label, juce::Component& field)
    {
        const int y = centredBand.getY() + rowIndex * (controlHeight + rowGap);
        label.setBounds(centredBand.getX(), y, labelWidth, controlHeight);
        label.setUiScale(uiScale_);
        field.setBounds(centredBand.getX() + labelWidth, y, comboWidth, controlHeight);
        if (auto* combo = dynamic_cast<TSS::ComboBox*>(&field))
            combo->setUiScale(uiScale_);
        else if (auto* value = dynamic_cast<TSS::ReadOnlyValueField*>(&field))
            value->setUiScale(uiScale_);
    };

    placeRow(0, *midiFromLabel_, *midiFromCombo_);
    placeRow(1, *midiToLabel_, *midiToCombo_);
    placeRow(2, *deviceLabel_, *deviceValueField_);
    placeRow(3, *epromTypeLabel_, *epromTypeCombo_);

    auto buttonRow = layout.buttonRow;
    confirmButton_->setBounds(buttonRow.removeFromRight(confirmWidth));
    buttonRow.removeFromRight(buttonGap);
    specifyLaterButton_->setBounds(buttonRow.removeFromRight(laterWidth));
}
