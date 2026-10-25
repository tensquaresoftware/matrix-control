#include "Tools/SkinEditor/SkinEditorSkinsPanel.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromePanel.h"

namespace
{
    int separatorGap() noexcept
    {
        return SkinEditorChrome::kButtonGap * 2;
    }

    int separatorBlockHeight() noexcept
    {
        return separatorGap() + 1 + separatorGap();
    }

    /** Product row + separator + Editor row + New/Rename/Delete row. */
    int contentRowsHeight() noexcept
    {
        return SkinEditorChrome::kButtonHeight // Product: + combo + Duplicate
               + separatorBlockHeight()
               + SkinEditorChrome::kButtonHeight // Editor: + combo + Duplicate
               + SkinEditorChrome::kButtonGap
               + SkinEditorChrome::kButtonHeight; // New / Rename / Delete
    }

    void placeActionButton(juce::Rectangle<int>& row, juce::TextButton& button)
    {
        button.setBounds(row.removeFromLeft(SkinEditorChrome::measureButtonWidth(button))
                             .withHeight(SkinEditorChrome::kButtonHeight));
    }

    struct SelectorRow
    {
        juce::Label& label;
        juce::ComboBox& combo;
        juce::TextButton& duplicate;
    };

    void layoutSelectorRow(juce::Rectangle<int>& content,
                           const SelectorRow& rowWidgets,
                           int labelColW,
                           int comboW)
    {
        auto row = content.removeFromTop(SkinEditorChrome::kButtonHeight);
        rowWidgets.label.setBounds(row.removeFromLeft(labelColW));
        rowWidgets.combo.setBounds(row.removeFromLeft(comboW));
        row.removeFromLeft(SkinEditorChrome::kButtonGap);
        placeActionButton(row, rowWidgets.duplicate);
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
    return SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
           + SkinEditorChrome::kPanelContentPad + contentRowsHeight()
           + SkinEditorChrome::kPanelContentPad + SkinEditorChrome::kGutter;
}

int SkinEditorSkinsPanel::preferredHeightForMinWindow() noexcept
{
    return preferredHeight();
}

int SkinEditorSkinsPanel::labelColumnWidth() noexcept
{
    const auto font = SkinEditorChrome::fontRegular();
    return juce::jmax(SkinEditorChrome::measureLabelWidth("Product:", font),
                      SkinEditorChrome::measureLabelWidth("Editor:", font));
}

int SkinEditorSkinsPanel::comboWidthForContent(int contentW,
                                               int labelColW,
                                               int trailingButtonW) noexcept
{
    constexpr int kMinCombo = 72;
    const int fixed = labelColW + SkinEditorChrome::kButtonGap + trailingButtonW;
    return juce::jmax(kMinCombo, contentW - fixed);
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
    const int dupW = SkinEditorChrome::measureButtonWidth(productDuplicateButton_);
    const int comboW = comboWidthForContent(content.getWidth(), labelColW, dupW);

    layoutSelectorRow(content,
                      { productLabel_, productCombo_, productDuplicateButton_ },
                      labelColW,
                      comboW);

    content.removeFromTop(separatorGap());
    separatorY_ = content.getY();
    separatorX_ = content.getX();
    separatorW_ = content.getWidth();
    content.removeFromTop(1);
    content.removeFromTop(separatorGap());

    layoutSelectorRow(content,
                      { editorLabel_, editorCombo_, editorDuplicateButton_ },
                      labelColW,
                      comboW);

    // New/Rename/Delete: left = combo left, right = Duplicate right.
    // Delete matches Duplicate width; New absorbs the remainder (Rename natural width).
    content.removeFromTop(SkinEditorChrome::kButtonGap);
    auto actions = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    actions.removeFromLeft(labelColW);
    const int spanW = comboW + SkinEditorChrome::kButtonGap + dupW;
    auto span = actions.removeFromLeft(spanW);
    const int gaps = 2 * SkinEditorChrome::kButtonGap;
    const int deleteW = dupW;
    const int renameW = SkinEditorChrome::measureButtonWidth(renameButton_);
    const int newW = juce::jmax(1, spanW - gaps - renameW - deleteW);
    newButton_.setBounds(span.removeFromLeft(newW)
                             .withHeight(SkinEditorChrome::kButtonHeight));
    span.removeFromLeft(SkinEditorChrome::kButtonGap);
    renameButton_.setBounds(span.removeFromLeft(renameW)
                                .withHeight(SkinEditorChrome::kButtonHeight));
    span.removeFromLeft(SkinEditorChrome::kButtonGap);
    deleteButton_.setBounds(span.removeFromLeft(deleteW)
                                .withHeight(SkinEditorChrome::kButtonHeight));
}
