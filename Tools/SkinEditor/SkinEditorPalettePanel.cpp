#include "Tools/SkinEditor/SkinEditorPalettePanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

SkinEditorPalettePanel::SkinEditorPalettePanel()
{
    SkinEditorChrome::stylePanelTitle(titleLabel_, "PALETTE");
    addAndMakeVisible(titleLabel_);

    // Same slot height as Mappings so section→content gaps can be kColumnHeaderGap (11).
    list_.setRowHeight(SkinEditorChrome::kTableGapHeight);
    list_.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    list_.setWantsKeyboardFocus(true);
    escapeForwarder_.handler = &onEscapeRequested;
    list_.addKeyListener(&escapeForwarder_);
    addAndMakeVisible(list_);
    addAndMakeVisible(addButton_);
    addAndMakeVisible(duplicateButton_);
    addAndMakeVisible(deleteButton_);
    addButton_.onClick = [this]
    {
        if (onAddRequested)
            onAddRequested();
    };
    duplicateButton_.onClick = [this]
    {
        if (onDuplicateRequested)
            onDuplicateRequested();
    };
    deleteButton_.onClick = [this]
    {
        if (onDeleteRequested)
            onDeleteRequested();
    };
}

SkinEditorPalettePanel::~SkinEditorPalettePanel()
{
    list_.removeKeyListener(&escapeForwarder_);
}

void SkinEditorPalettePanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinEditorPalettePanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinEditorPalettePanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    updateActionButtonEnablement();
}

void SkinEditorPalettePanel::refresh()
{
    rebuildRows();
    list_.updateContent();
    list_.repaint();
    resized();
    updateActionButtonEnablement();
}

void SkinEditorPalettePanel::updateActionButtonEnablement()
{
    const auto selected = getSelectedPastilleName();
    addButton_.setEnabled(! readOnly_);
    duplicateButton_.setEnabled(! readOnly_ && selected.isNotEmpty());
    deleteButton_.setEnabled(! readOnly_ && selected.isNotEmpty()
                             && layer_ != nullptr
                             && ! layer_->isProductPastilleName(selected));
}

juce::String SkinEditorPalettePanel::getSelectedPastilleName() const
{
    const int row = list_.getSelectedRow();
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return {};
    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kColour)
        return {};
    return item.name;
}

void SkinEditorPalettePanel::selectPastille(const juce::String& name)
{
    for (size_t i = 0; i < rows_.size(); ++i)
    {
        if (rows_[i].kind == RowKind::kColour && ! rows_[i].isLowerListHalf && rows_[i].name == name)
        {
            const juce::ScopedValueSetter<bool> scope(suppressSelectionNotify_, true);
            list_.selectRow(static_cast<int>(i));
            return;
        }
    }
}

void SkinEditorPalettePanel::clearSelection()
{
    list_.deselectAllRows();
}

void SkinEditorPalettePanel::paint(juce::Graphics& g)
{
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_, 0);
    SkinEditorChrome::paintRoundedWell(g, list_.getBounds(), SkinEditorChrome::surfacePanel());
}

void SkinEditorPalettePanel::resized()
{
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(),
                                                      SkinEditorChrome::titleTabWidth("PALETTE"),
                                                      0);
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto bounds = chrome.content;

    if (isShellEmpty())
    {
        addButton_.setVisible(false);
        duplicateButton_.setVisible(false);
        deleteButton_.setVisible(false);
        addButton_.setBounds({});
        duplicateButton_.setBounds({});
        deleteButton_.setBounds({});
        auto listBounds = bounds;
        listBounds.setRight(listBounds.getRight() + SkinEditorChrome::kPanelContentPad);
        list_.setBounds(listBounds);
        if (auto* vp = list_.getViewport())
            vp->setScrollBarThickness(SkinEditorChrome::kScrollTrackWidth);
        return;
    }

    addButton_.setVisible(true);
    duplicateButton_.setVisible(true);
    deleteButton_.setVisible(true);

    // Top: buttons, then content pad.
    auto actions = bounds.removeFromTop(SkinEditorChrome::kButtonHeight);
    bounds.removeFromTop(SkinEditorChrome::kPanelContentPad);

    // Same as Mappings: list extends into the right content pad; scroll thumb centred there.
    auto listBounds = bounds;
    listBounds.setRight(listBounds.getRight() + SkinEditorChrome::kPanelContentPad);
    list_.setBounds(listBounds);
    if (auto* vp = list_.getViewport())
        vp->setScrollBarThickness(SkinEditorChrome::kScrollTrackWidth);

    layoutActionButtons({ list_.getX(), actions.getY(), paintWidth(), actions.getHeight() });
}

void SkinEditorPalettePanel::layoutActionButtons(juce::Rectangle<int> actionRow)
{
    // Natural widths only — do not dump leftover into New (keeps short labels tight).
    const int tableW = actionRow.getWidth();
    const int gaps = 2 * SkinEditorChrome::kButtonGap;
    int newW = SkinEditorChrome::measurePaletteActionButtonWidth(addButton_);
    int dupW = SkinEditorChrome::measurePaletteActionButtonWidth(duplicateButton_);
    int delW = SkinEditorChrome::measurePaletteActionButtonWidth(deleteButton_);
    const int preferred = newW + dupW + delW + gaps;
    if (preferred > tableW)
    {
        const int inner = juce::jmax(3, tableW - gaps);
        const int sum = juce::jmax(1, newW + dupW + delW);
        newW = juce::jmax(1, (inner * newW) / sum);
        dupW = juce::jmax(1, (inner * dupW) / sum);
        delW = juce::jmax(1, inner - newW - dupW);
    }
    addButton_.setBounds(actionRow.removeFromLeft(newW)
                             .withHeight(SkinEditorChrome::kButtonHeight));
    actionRow.removeFromLeft(SkinEditorChrome::kButtonGap);
    duplicateButton_.setBounds(actionRow.removeFromLeft(dupW)
                                   .withHeight(SkinEditorChrome::kButtonHeight));
    actionRow.removeFromLeft(SkinEditorChrome::kButtonGap);
    deleteButton_.setBounds(actionRow.removeFromLeft(delW)
                                .withHeight(SkinEditorChrome::kButtonHeight));
}

int SkinEditorPalettePanel::paintWidth() const noexcept
{
    // Keep table fills out of the scroll-track band even when the bar is hidden.
    return juce::jmax(1, list_.getWidth() - SkinEditorChrome::kScrollTrackWidth);
}

void SkinEditorPalettePanel::pushSplitListRow(Row row)
{
    row.isLowerListHalf = false;
    rows_.push_back(row);
    row.isLowerListHalf = true;
    rows_.push_back(row);
}

int SkinEditorPalettePanel::nextUpperListRow(int row) const noexcept
{
    int i = row + 1;
    while (i < static_cast<int>(rows_.size()) && rows_[static_cast<size_t>(i)].isLowerListHalf)
        ++i;
    return i;
}

bool SkinEditorPalettePanel::isRowSelectedVisually(int row) const noexcept
{
    if (list_.isRowSelected(row))
        return true;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return false;
    if (rows_[static_cast<size_t>(row)].isLowerListHalf)
        return list_.isRowSelected(row - 1);
    if (row + 1 < static_cast<int>(rows_.size()) && rows_[static_cast<size_t>(row + 1)].isLowerListHalf)
        return list_.isRowSelected(row + 1);
    return false;
}

int SkinEditorPalettePanel::getNumRows()
{
    return static_cast<int>(rows_.size());
}

bool SkinEditorPalettePanel::isBlockBoundary(RowKind kind) noexcept
{
    return kind == RowKind::kGap || kind == RowKind::kSection || kind == RowKind::kEmptyHint;
}

bool SkinEditorPalettePanel::isContentRow(RowKind kind) noexcept
{
    return kind == RowKind::kColour || kind == RowKind::kEmptyHint;
}

void SkinEditorPalettePanel::roundFlagsForRow(int row, bool& roundTop, bool& roundBottom) const noexcept
{
    const auto& item = rows_[static_cast<size_t>(row)];
    const int upperListRow = item.isLowerListHalf ? row - 1 : row;
    const int next = nextUpperListRow(upperListRow);
    const auto nextKind = (next < static_cast<int>(rows_.size()))
                              ? rows_[static_cast<size_t>(next)].kind
                              : RowKind::kGap;

    int prev = upperListRow - 1;
    while (prev >= 0 && rows_[static_cast<size_t>(prev)].isLowerListHalf)
        --prev;
    const bool startsBlock = prev < 0 || isBlockBoundary(rows_[static_cast<size_t>(prev)].kind);

    roundTop = item.kind == RowKind::kSection || (isContentRow(item.kind) && startsBlock);
    roundBottom = (isContentRow(item.kind) && isBlockBoundary(nextKind))
                  || (item.kind == RowKind::kSection && ! isContentRow(nextKind));
}

void SkinEditorPalettePanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    juce::ignoreUnused(selected, width);
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind == RowKind::kGap)
        return;

    const int paintW = paintWidth();
    const int fullH = SkinEditorChrome::kColumnHeaderHeight;
    const int yOff = item.isLowerListHalf ? -height : 0;
    const auto rowArea = juce::Rectangle<float>(0.0f, (float) yOff, (float) paintW, (float) fullH);
    bool roundTop = false;
    bool roundBottom = false;
    roundFlagsForRow(row, roundTop, roundBottom);

    if (item.kind == RowKind::kSection)
    {
        SkinEditorChrome::fillTableRow(g, { rowArea, SkinEditorChrome::surfaceSection(),
                                            roundTop, roundBottom });
        g.setColour(SkinEditorChrome::textTableHeader());
        g.setFont(SkinEditorChrome::fontBold());
        g.drawText(item.headerText, 6, yOff, paintW - 12, fullH, juce::Justification::centredLeft);
        return;
    }

    if (item.kind == RowKind::kEmptyHint)
    {
        // Darker than textMuted so the empty hint stays quiet under Editor colours.
        g.setColour(juce::Colour(0xff666666));
        g.setFont(SkinEditorChrome::fontRegular());
        g.drawText(item.headerText, 6, yOff, paintW - 12, fullH, juce::Justification::centredLeft);
        return;
    }

    if (isRowSelectedVisually(row))
        SkinEditorChrome::fillTableRow(g, { rowArea, juce::Colour(0xff303030), roundTop, roundBottom });

    const int sw = SkinEditorChrome::kColourSwatchSize;
    const int swX = 4;
    const int swY = yOff + (fullH - sw) / 2;
    const auto swatch = juce::Rectangle<int>(swX, swY, sw, sw);
    if (item.pending)
        SkinEditorChrome::paintPendingColourSwatch(g, swatch);
    else
        SkinEditorChrome::paintColourSwatch(g, swatch, juce::Colour(item.argb));
    g.setColour(SkinEditorChrome::textTableBody());
    g.setFont(SkinEditorChrome::fontRegular());
    g.drawText(item.name, swX + sw + 8, yOff, paintW - swX - sw - 8, fullH,
               juce::Justification::centredLeft);
}

void SkinEditorPalettePanel::selectedRowsChanged(int)
{
    // Split rows paint a full visual line across two ListBox slots; JUCE only
    // invalidates the selected index, so force a full list repaint for highlight.
    list_.repaint();

    updateActionButtonEnablement();
    if (suppressSelectionNotify_)
        return;
    if (onPastilleSelected)
        onPastilleSelected(getSelectedPastilleName());
}

void SkinEditorPalettePanel::listBoxItemDoubleClicked(int row, const juce::MouseEvent& e)
{
    if (readOnly_ || row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kColour || item.isProduct)
        return;

    // Rename is name-text only — swatch double-click does nothing.
    const int sw = SkinEditorChrome::kColourSwatchSize;
    constexpr int kSwatchX = 4;
    constexpr int kNameGap = 8;
    if (e.x < kSwatchX + sw + kNameGap)
        return;

    if (onRenamePastilleRequested)
        onRenamePastilleRequested(item.name);
}

void SkinEditorPalettePanel::rebuildRows()
{
    rows_.clear();
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return;

    std::vector<Row> product;
    std::vector<Row> editor;
    for (const auto& pastille : skin->palette)
    {
        Row row;
        row.kind = RowKind::kColour;
        row.name = pastille.name;
        row.argb = pastille.argb;
        row.pending = pastille.pending;
        row.isProduct = layer_->isProductPastilleName(pastille.name);
        if (row.isProduct)
            product.push_back(row);
        else
            editor.push_back(row);
    }

    if (! product.empty())
        appendPaletteGroup("Product colours", product, false);
    // Always show Editor colours when editable (or when any editor pastille exists).
    if (! editor.empty() || ! readOnly_)
        appendPaletteGroup("Editor colours", editor, ! product.empty());
}

void SkinEditorPalettePanel::appendPaletteGroup(const juce::String& title,
                                                std::vector<Row>& colours,
                                                bool leadGap)
{
    if (leadGap)
    {
        Row gap;
        gap.kind = RowKind::kGap;
        rows_.push_back(gap);
    }

    Row section;
    section.kind = RowKind::kSection;
    section.headerText = title;
    pushSplitListRow(section);
    Row gap;
    gap.kind = RowKind::kGap;
    rows_.push_back(gap);
    if (colours.empty())
    {
        Row hint;
        hint.kind = RowKind::kEmptyHint;
        hint.headerText = "No editor colours yet.";
        pushSplitListRow(hint);
        return;
    }
    for (auto& row : colours)
        pushSplitListRow(row);
}
