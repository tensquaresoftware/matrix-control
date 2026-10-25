#include "Tools/SkinEditor/SkinEditorSkinsPanel.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromePanel.h"

namespace
{
    int separatorBlockHeight() noexcept
    {
        return SkinEditorChrome::kButtonGap + 1 + SkinEditorChrome::kButtonGap;
    }

    int contentRowsHeight(bool wrapRenameDelete) noexcept
    {
        // Product combo, Duplicate Product, separator, Editor combo, action row(s).
        const int productBlock = SkinEditorChrome::kButtonHeight
                                 + SkinEditorChrome::kButtonGap
                                 + SkinEditorChrome::kButtonHeight;
        const int editorCombo = SkinEditorChrome::kButtonHeight;
        const int actionRows = wrapRenameDelete ? 2 : 1;
        const int gapBeforeActions = SkinEditorChrome::kButtonGap;
        const int gapBetweenActionRows = wrapRenameDelete ? SkinEditorChrome::kButtonGap : 0;
        return productBlock + separatorBlockHeight() + editorCombo + gapBeforeActions
               + SkinEditorChrome::kButtonHeight * actionRows + gapBetweenActionRows;
    }

    bool editorActionsNeedWrap(int contentW) noexcept
    {
        const int needed = SkinEditorChrome::measureButtonWidth(juce::TextButton("Duplicate"))
                           + SkinEditorChrome::kButtonGap
                           + SkinEditorChrome::measureButtonWidth(juce::TextButton("New"))
                           + SkinEditorChrome::kButtonGap
                           + SkinEditorChrome::measureButtonWidth(juce::TextButton("Rename"))
                           + SkinEditorChrome::kButtonGap
                           + SkinEditorChrome::measureButtonWidth(juce::TextButton("Delete"));
        return needed > contentW;
    }
}

SkinEditorSkinsPanel::SkinEditorSkinsPanel()
{
    SkinEditorChrome::stylePanelTitle(titleLabel_, "SKINS");
    addAndMakeVisible(titleLabel_);

    for (auto* label : { &productLabel_, &editorLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        SkinEditorChrome::preventTextSquish(*label);
        addAndMakeVisible(*label);
    }

    addAndMakeVisible(productCombo_);
    addAndMakeVisible(editorCombo_);

    auto wire = [this](juce::TextButton& button, std::function<void()>& handler)
    {
        addAndMakeVisible(button);
        button.onClick = [&handler]
        {
            if (handler != nullptr)
                handler();
        };
    };
    wire(productDuplicateButton_, onDuplicateProduct);
    wire(editorDuplicateButton_, onDuplicateEditor);
    wire(newButton_, onNew);
    wire(renameButton_, onRename);
    wire(deleteButton_, onDelete);
}

void SkinEditorSkinsPanel::setProductDuplicateEnabled(bool enabled)
{
    productDuplicateButton_.setEnabled(enabled);
}

void SkinEditorSkinsPanel::setEditorActionsEnabled(bool canDuplicate,
                                                   bool canRenameDelete,
                                                   bool canNew)
{
    editorDuplicateButton_.setEnabled(canDuplicate);
    renameButton_.setEnabled(canRenameDelete);
    deleteButton_.setEnabled(canRenameDelete);
    newButton_.setEnabled(canNew);
}

int SkinEditorSkinsPanel::preferredHeight() noexcept
{
    const int contentW = SkinEditorChrome::kColorPanelWidth
                         - 2 * SkinEditorChrome::kPanelContentPad;
    const bool wrap = editorActionsNeedWrap(contentW);
    return SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
           + SkinEditorChrome::kPanelContentPad + contentRowsHeight(wrap)
           + SkinEditorChrome::kPanelContentPad + SkinEditorChrome::kGutter;
}

int SkinEditorSkinsPanel::labelColumnWidth() noexcept
{
    const auto font = SkinEditorChrome::fontRegular();
    return juce::jmax(SkinEditorChrome::measureLabelWidth("Product:", font),
                      SkinEditorChrome::measureLabelWidth("Editor:", font));
}

int SkinEditorSkinsPanel::comboWidthForContent(int contentW, int labelColW) noexcept
{
    constexpr int kMinCombo = 72;
    return juce::jmax(kMinCombo, contentW - labelColW);
}

void SkinEditorSkinsPanel::placeActionButton(juce::Rectangle<int>& row,
                                             juce::TextButton& button)
{
    button.setBounds(row.removeFromLeft(SkinEditorChrome::measureButtonWidth(button))
                         .withHeight(SkinEditorChrome::kButtonHeight));
}

void SkinEditorSkinsPanel::layoutProductRows(juce::Rectangle<int>& content, int labelColW, int comboW)
{
    auto productRow = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    productLabel_.setBounds(productRow.removeFromLeft(labelColW));
    productCombo_.setBounds(productRow.removeFromLeft(comboW));

    content.removeFromTop(SkinEditorChrome::kButtonGap);
    auto dupRow = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    placeActionButton(dupRow, productDuplicateButton_);

    content.removeFromTop(SkinEditorChrome::kButtonGap);
    separatorY_ = content.getY();
    separatorX_ = content.getX();
    separatorW_ = content.getWidth();
    content.removeFromTop(1);
    content.removeFromTop(SkinEditorChrome::kButtonGap);
}

void SkinEditorSkinsPanel::layoutEditorRows(juce::Rectangle<int>& content, int labelColW, int comboW)
{
    auto editorRow = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    editorLabel_.setBounds(editorRow.removeFromLeft(labelColW));
    editorCombo_.setBounds(editorRow.removeFromLeft(comboW));

    content.removeFromTop(SkinEditorChrome::kButtonGap);
    const bool wrap = editorActionsNeedWrap(content.getWidth());
    auto actions = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    placeActionButton(actions, editorDuplicateButton_);
    actions.removeFromLeft(SkinEditorChrome::kButtonGap);
    placeActionButton(actions, newButton_);
    if (wrap)
    {
        content.removeFromTop(SkinEditorChrome::kButtonGap);
        actions = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    }
    else
    {
        actions.removeFromLeft(SkinEditorChrome::kButtonGap);
    }
    placeActionButton(actions, renameButton_);
    actions.removeFromLeft(SkinEditorChrome::kButtonGap);
    placeActionButton(actions, deleteButton_);
}

void SkinEditorSkinsPanel::paint(juce::Graphics& g)
{
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_);
    if (separatorW_ > 0)
    {
        g.setColour(SkinEditorChrome::separatorLine().withAlpha(0.85f));
        g.fillRect(separatorX_, separatorY_, separatorW_, 1);
    }
}

void SkinEditorSkinsPanel::resized()
{
    separatorX_ = 0;
    separatorW_ = 0;
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(),
                                                      SkinEditorChrome::titleTabWidth("SKINS"));
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto content = chrome.content;

    const int labelColW = labelColumnWidth();
    const int comboW = comboWidthForContent(content.getWidth(), labelColW);
    layoutProductRows(content, labelColW, comboW);
    layoutEditorRows(content, labelColW, comboW);
}
