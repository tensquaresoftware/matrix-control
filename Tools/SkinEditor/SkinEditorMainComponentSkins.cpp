#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"

void SkinEditorMainComponent::layoutSkinsBar(juce::Rectangle<int> skins)
{
    skins.removeFromTop(SkinEditorChrome::kGutter);
    auto titleRow = skins.removeFromTop(SkinEditorChrome::kPanelTitleHeight);
    const int buttonY = titleRow.getY()
                        + (titleRow.getHeight() - SkinEditorChrome::kButtonHeight) / 2;
    auto row = juce::Rectangle<int>(titleRow.getX(), buttonY, titleRow.getWidth(),
                                    SkinEditorChrome::kButtonHeight);
    skinsBarSeparatorY_ = row.getY();
    skinsBarSeparatorH_ = row.getHeight();

    const int skinsTitleW = SkinEditorChrome::titleTabWidth("SKINS");
    skinsTitleLabel_.setBounds(titleRow.getX(), titleRow.getY() - 1, skinsTitleW, titleRow.getHeight());
    row.removeFromLeft(skinsTitleW);
    // Same trailing gap after Import as leading gap after SKINS title.
    row.removeFromLeft(SkinEditorChrome::kButtonGap);
    const int trailingGap = SkinEditorChrome::kButtonGap;
    const int comboW = skinsBarComboWidth(row.getWidth() - trailingGap);
    layoutSkinsSkinSelectors(row, comboW);

    auto placeGap = [&row]() { row.removeFromLeft(SkinEditorChrome::kButtonGap); };
    auto placeButton = [&row](juce::TextButton& b)
    {
        b.setBounds(row.removeFromLeft(SkinEditorChrome::measureButtonWidth(b))
                        .withHeight(SkinEditorChrome::kButtonHeight));
    };
    placeButton(newButton_);
    placeGap();
    placeButton(renameButton_);
    placeGap();
    placeButton(deleteButton_);
    layoutSkinsActionButtons(row);
}

int SkinEditorMainComponent::skinsBarComboWidth(int rowWidthSansTrailingGap) const
{
    constexpr int kMinCombo = 100;
    constexpr int kSep = SkinEditorChrome::kButtonGap * 4 + 1; // placeSkinsBarSeparator
    const auto labelFont = SkinEditorChrome::fontRegular();
    const int fixed = SkinEditorChrome::measureLabelWidth(productLabel_.getText(), labelFont)
                      + SkinEditorChrome::measureLabelWidth(editorLabel_.getText(), labelFont)
                      + SkinEditorChrome::measureButtonWidth(productDuplicateButton_)
                      + SkinEditorChrome::measureButtonWidth(editorDuplicateButton_)
                      + SkinEditorChrome::measureButtonWidth(newButton_)
                      + SkinEditorChrome::measureButtonWidth(renameButton_)
                      + SkinEditorChrome::measureButtonWidth(deleteButton_)
                      + SkinEditorChrome::measureButtonWidth(undoButton_)
                      + SkinEditorChrome::measureButtonWidth(redoButton_)
                      + SkinEditorChrome::measureButtonWidth(compareButton_)
                      + SkinEditorChrome::measureButtonWidth(pushButton_)
                      + SkinEditorChrome::measureButtonWidth(exportButton_)
                      + SkinEditorChrome::measureButtonWidth(importButton_)
                      + kSep * 4
                      + SkinEditorChrome::kButtonGap * 8; // single gaps between controls
    const int forCombos = juce::jmax(0, rowWidthSansTrailingGap - fixed);
    return juce::jmax(kMinCombo, forCombos / 2);
}

void SkinEditorMainComponent::layoutSkinsSkinSelectors(juce::Rectangle<int>& row, int comboWidth)
{
    const auto labelFont = SkinEditorChrome::fontRegular();
    const int productLabelW = SkinEditorChrome::measureLabelWidth(productLabel_.getText(), labelFont);
    const int editorLabelW = SkinEditorChrome::measureLabelWidth(editorLabel_.getText(), labelFont);
    auto placeGap = [&row]() { row.removeFromLeft(SkinEditorChrome::kButtonGap); };
    auto placeButton = [&row](juce::TextButton& b)
    {
        b.setBounds(row.removeFromLeft(SkinEditorChrome::measureButtonWidth(b))
                        .withHeight(SkinEditorChrome::kButtonHeight));
    };
    auto placeSkinsBarSeparator = [this, &row, &placeGap]()
    {
        placeGap();
        placeGap();
        skinsBarSeparatorXs_.push_back(row.getX());
        row.removeFromLeft(1);
        placeGap();
        placeGap();
    };

    productLabel_.setBounds(row.removeFromLeft(productLabelW));
    productCombo_.setBounds(row.removeFromLeft(comboWidth));
    placeGap();
    placeButton(productDuplicateButton_);
    placeSkinsBarSeparator();
    editorLabel_.setBounds(row.removeFromLeft(editorLabelW));
    editorCombo_.setBounds(row.removeFromLeft(comboWidth));
    placeGap();
    placeButton(editorDuplicateButton_);
    placeGap();
}

void SkinEditorMainComponent::layoutSkinsActionButtons(juce::Rectangle<int>& row)
{
    auto placeGap = [&row]() { row.removeFromLeft(SkinEditorChrome::kButtonGap); };
    auto placeButton = [&row](juce::TextButton& b)
    {
        const int w = SkinEditorChrome::measureButtonWidth(b);
        b.setBounds(row.removeFromLeft(w).withHeight(SkinEditorChrome::kButtonHeight));
    };
    auto placeSkinsBarSeparator = [this, &row, &placeGap]()
    {
        placeGap();
        placeGap();
        skinsBarSeparatorXs_.push_back(row.getX());
        row.removeFromLeft(1);
        placeGap();
        placeGap();
    };

    placeSkinsBarSeparator();
    placeButton(undoButton_);
    placeGap();
    placeButton(redoButton_);
    placeSkinsBarSeparator();
    placeButton(compareButton_);
    placeGap();
    placeButton(pushButton_);
    placeSkinsBarSeparator();
    placeButton(exportButton_);
    placeGap();
    placeButton(importButton_);
}
