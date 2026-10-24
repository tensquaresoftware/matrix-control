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
    addAndMakeVisible(deleteButton_);
    addButton_.onClick = [this]
    {
        if (onAddRequested)
            onAddRequested();
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
    addButton_.setEnabled(! readOnly_);
    deleteButton_.setEnabled(! readOnly_);
}

void SkinEditorPalettePanel::refresh()
{
    rebuildRows();
    list_.updateContent();
    list_.repaint();

    const auto selected = getSelectedPastilleName();
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
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_);
    SkinEditorChrome::paintRoundedWell(g, list_.getBounds(), SkinEditorChrome::surfacePanel());
}

void SkinEditorPalettePanel::resized()
{
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(),
                                                      SkinEditorChrome::titleTabWidth("PALETTE"));
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto bounds = chrome.content;

    // Top: buttons, then content pad.
    auto actions = bounds.removeFromTop(SkinEditorChrome::kButtonHeight);
    bounds.removeFromTop(SkinEditorChrome::kPanelContentPad);

    // Same as Mappings: list extends into the right content pad; scroll thumb centred there.
    auto listBounds = bounds;
    listBounds.setRight(listBounds.getRight() + SkinEditorChrome::kPanelContentPad);
    list_.setBounds(listBounds);
    if (auto* vp = list_.getViewport())
        vp->setScrollBarThickness(SkinEditorChrome::kScrollTrackWidth);

    // Align Add/Delete with the table paint width (not the scroll track).
    const int tableW = paintWidth();
    auto actionRow = juce::Rectangle<int>(list_.getX(), actions.getY(), tableW, actions.getHeight());
    const int half = (actionRow.getWidth() - SkinEditorChrome::kButtonGap) / 2;
    addButton_.setBounds(actionRow.removeFromLeft(juce::jmax(1, half)));
    actionRow.removeFromLeft(SkinEditorChrome::kButtonGap);
    deleteButton_.setBounds(actionRow);
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
    return kind == RowKind::kGap || kind == RowKind::kSection;
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

    roundTop = item.kind == RowKind::kSection
               || (item.kind == RowKind::kColour && startsBlock);
    roundBottom = (item.kind == RowKind::kColour && isBlockBoundary(nextKind))
                  || (item.kind == RowKind::kSection && nextKind != RowKind::kColour);
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

    if (isRowSelectedVisually(row))
        SkinEditorChrome::fillTableRow(g, { rowArea, juce::Colour(0xff303030), roundTop, roundBottom });

    SkinEditorChrome::paintColourSwatch(g,
                                        juce::Rectangle<int>(4, yOff + 4, fullH - 8, fullH - 8),
                                        juce::Colour(item.argb));
    g.setColour(SkinEditorChrome::textTableBody());
    g.setFont(SkinEditorChrome::fontRegular());
    g.drawText(item.name, fullH + 4, yOff, paintW - fullH - 8, fullH, juce::Justification::centredLeft);
}

void SkinEditorPalettePanel::selectedRowsChanged(int)
{
    // Split rows paint a full visual line across two ListBox slots; JUCE only
    // invalidates the selected index, so force a full list repaint for highlight.
    list_.repaint();

    const auto name = getSelectedPastilleName();
    deleteButton_.setEnabled(! readOnly_ && name.isNotEmpty()
                             && layer_ != nullptr
                             && ! layer_->isProductPastilleName(name));
    if (onPastilleSelected)
        onPastilleSelected(name);
}

void SkinEditorPalettePanel::rebuildRows()
{
    rows_.clear();
    if (layer_ == nullptr)
        return;

    const auto* skin = layer_->findSkin(skinId_);
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
        row.isProduct = layer_->isProductPastilleName(pastille.name);
        if (row.isProduct)
            product.push_back(row);
        else
            editor.push_back(row);
    }

    if (! product.empty())
        appendPaletteGroup("Product colours", product, false);
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
    for (auto& row : colours)
        pushSplitListRow(row);
}
