#include "EpromTypePromptDialog.h"

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

int EpromTypePromptDialog::getRowsHeight() const
{
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    const int rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    return controlHeight * 4 + rowGap * 3;
}

EpromTypePromptDialog::ContentLayout EpromTypePromptDialog::computeContentLayout() const
{
    const auto body = bodyText();
    const auto textLayout = DialogMatrixHelpers::computeTextModalLayout({ .skin = *skin_,
                                                                          .bodyText = body,
                                                                          .hostBounds = getLocalBounds(),
                                                                          .designWidth = kDesignWidth,
                                                                          .uiScale = uiScale_,
                                                                          .extraBandHeight = getRowsHeight() });
    ContentLayout layout;
    layout.geometry = textLayout.geometry;
    layout.bodyFont = textLayout.bodyFont;

    // Vertically centre the four control rows between body text and buttons.
    layout.controlBand = layout.geometry.band.withSizeKeepingCentre(layout.geometry.band.getWidth(),
                                                                    getRowsHeight());
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
    const auto layout = computeContentLayout();
    const auto& geometry = layout.geometry;

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = geometry.dialogBounds,
        .borderThickness = geometry.border,
        .titleBarHeight = geometry.titleBarHeight,
        .title = PluginDisplayNames::Dialogs::EpromTypePrompt::kTitle,
        .uiScale = uiScale_ });

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    DialogMatrixHelpers::paintBodyText(g, layout.bodyFont, bodyText(), geometry.textArea);
}

void EpromTypePromptDialog::resized()
{
    const auto layout = computeContentLayout();

    const int confirmWidth = juce::roundToInt(static_cast<float>(kConfirmButtonWidth_) * uiScale_);
    const int laterWidth = juce::roundToInt(static_cast<float>(kSpecifyLaterButtonWidth_) * uiScale_);
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    const int labelWidth = juce::roundToInt(static_cast<float>(kLabelWidth_) * uiScale_);
    const int comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);
    const int rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    const int rowWidth = labelWidth + comboWidth;

    const auto centredBand = layout.controlBand.withSizeKeepingCentre(rowWidth, getRowsHeight());

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

    // LTR: SPECIFY LATER, CONFIRM (primary) - the pair is centred.
    DialogMatrixHelpers::layoutCentredButtonRow(
        layout.geometry.buttonRow,
        uiScale_,
        { { specifyLaterButton_.get(), laterWidth }, { confirmButton_.get(), confirmWidth } });
}
