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

    /** Product combo + Dup row + separator + Editor combo + actions row. */
    int contentRowsHeight() noexcept
    {
        return SkinEditorChrome::kButtonHeight // Product combo + nav
               + SkinEditorChrome::kButtonGap
               + SkinEditorChrome::kButtonHeight // Product Duplicate
               + separatorBlockHeight()
               + SkinEditorChrome::kButtonHeight // Editor combo + nav
               + SkinEditorChrome::kButtonGap
               + SkinEditorChrome::kButtonHeight; // Dup / New / Ren / Del
    }

    void placeSquareNav(juce::Rectangle<int>& row, juce::TextButton& button, int squareW)
    {
        button.setBounds(row.removeFromLeft(squareW).withHeight(SkinEditorChrome::kButtonHeight));
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

    auto wireNav = [this](juce::TextButton& button,
                          juce::ComboBox& combo,
                          int delta,
                          const juce::String& tooltip)
    {
        addAndMakeVisible(button);
        button.setTooltip(tooltip);
        button.onClick = [this, &combo, delta] { cycleCombo(combo, delta); };
    };
    wireNav(productPrevButton_, productCombo_, -1, "Previous Skin");
    wireNav(productNextButton_, productCombo_, +1, "Next Skin");
    wireNav(editorPrevButton_, editorCombo_, -1, "Previous Skin");
    wireNav(editorNextButton_, editorCombo_, +1, "Next Skin");
}

void SkinEditorSkinsPanel::cycleCombo(juce::ComboBox& combo, int delta)
{
    const int count = combo.getNumItems();
    if (count <= 0)
        return;

    const int index = combo.getSelectedItemIndex();
    if (index < 0)
        return;

    const int next = index + delta;
    if (next < 0 || next >= count)
        return;

    combo.setSelectedItemIndex(next, juce::sendNotificationSync);
}

void SkinEditorSkinsPanel::setProductDuplicateEnabled(bool enabled)
{
    productDuplicateButton_.setEnabled(enabled);
}

void SkinEditorSkinsPanel::syncProductNavButtons()
{
    const int count = productCombo_.getNumItems();
    const int index = productCombo_.getSelectedItemIndex();
    productPrevButton_.setEnabled(count > 0 && index > 0);
    productNextButton_.setEnabled(count > 0 && index >= 0 && index < count - 1);
}

void SkinEditorSkinsPanel::syncEditorNavButtons()
{
    const int count = editorCombo_.getNumItems();
    const int index = editorCombo_.getSelectedItemIndex();
    editorPrevButton_.setEnabled(count > 0 && index > 0);
    editorNextButton_.setEnabled(count > 0 && index >= 0 && index < count - 1);
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

int SkinEditorSkinsPanel::labelColumnWidth() noexcept
{
    const auto font = SkinEditorChrome::fontRegular();
    return juce::jmax(SkinEditorChrome::measureLabelWidth("Product:", font),
                      SkinEditorChrome::measureLabelWidth("Editor:", font));
}

int SkinEditorSkinsPanel::squareNavButtonWidth() noexcept
{
    return SkinEditorChrome::kButtonHeight;
}

int SkinEditorSkinsPanel::comboWidthForContent(int contentW, int labelColW) noexcept
{
    constexpr int kMinCombo = 72;
    const int squareW = squareNavButtonWidth();
    const int trailing = SkinEditorChrome::kButtonGap + squareW
                         + SkinEditorChrome::kButtonGap + squareW;
    const int fixed = labelColW + trailing;
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

SkinEditorSkinsPanel::SelectorMetrics SkinEditorSkinsPanel::makeSelectorMetrics(
    int contentWidth) const noexcept
{
    SelectorMetrics metrics;
    metrics.labelColW = labelColumnWidth();
    metrics.squareW = squareNavButtonWidth();
    metrics.comboW = comboWidthForContent(contentWidth, metrics.labelColW);
    metrics.selectorSpanW = metrics.comboW + SkinEditorChrome::kButtonGap + metrics.squareW
                            + SkinEditorChrome::kButtonGap + metrics.squareW;
    return metrics;
}

void SkinEditorSkinsPanel::layoutComboNavRow(juce::Rectangle<int>& content,
                                             const SelectorMetrics& metrics,
                                             const ComboNavRowRefs& rowRefs)
{
    auto row = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    rowRefs.label.setBounds(row.removeFromLeft(metrics.labelColW));
    rowRefs.combo.setBounds(row.removeFromLeft(metrics.comboW));
    row.removeFromLeft(SkinEditorChrome::kButtonGap);
    placeSquareNav(row, rowRefs.prev, metrics.squareW);
    row.removeFromLeft(SkinEditorChrome::kButtonGap);
    placeSquareNav(row, rowRefs.next, metrics.squareW);
}

void SkinEditorSkinsPanel::layoutProductDuplicateRow(juce::Rectangle<int>& content, int labelColW)
{
    content.removeFromTop(SkinEditorChrome::kButtonGap);
    auto row = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    row.removeFromLeft(labelColW);
    productDuplicateButton_.setBounds(
        row.removeFromLeft(SkinEditorChrome::measureButtonWidth(productDuplicateButton_))
            .withHeight(SkinEditorChrome::kButtonHeight));
}

void SkinEditorSkinsPanel::layoutEditorActionRow(juce::Rectangle<int>& content,
                                                 const SelectorMetrics& metrics)
{
    // Dup / New / Ren / Del: same span as combo + < + >.
    content.removeFromTop(SkinEditorChrome::kButtonGap);
    auto actions = content.removeFromTop(SkinEditorChrome::kButtonHeight);
    actions.removeFromLeft(metrics.labelColW);
    auto span = actions.removeFromLeft(metrics.selectorSpanW);
    const int gaps = 3 * SkinEditorChrome::kButtonGap;
    const int avail = juce::jmax(4, metrics.selectorSpanW - gaps);
    const int w0 = avail / 4;
    const int w1 = avail / 4;
    const int w2 = avail / 4;
    const int w3 = avail - w0 - w1 - w2;

    auto placeCell = [&](juce::TextButton& button, int width, bool addGap)
    {
        button.setBounds(span.removeFromLeft(width).withHeight(SkinEditorChrome::kButtonHeight));
        if (addGap)
            span.removeFromLeft(SkinEditorChrome::kButtonGap);
    };
    placeCell(editorDuplicateButton_, w0, true);
    placeCell(newButton_, w1, true);
    placeCell(renameButton_, w2, true);
    placeCell(deleteButton_, w3, false);
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
    const auto metrics = makeSelectorMetrics(content.getWidth());

    layoutComboNavRow(content, metrics,
                      { productLabel_, productCombo_, productPrevButton_, productNextButton_ });
    layoutProductDuplicateRow(content, metrics.labelColW);

    content.removeFromTop(separatorGap());
    separatorY_ = content.getY();
    separatorX_ = content.getX();
    separatorW_ = content.getWidth();
    content.removeFromTop(1);
    content.removeFromTop(separatorGap());

    layoutComboNavRow(content, metrics,
                      { editorLabel_, editorCombo_, editorPrevButton_, editorNextButton_ });
    layoutEditorActionRow(content, metrics);
}
