#include "Tools/SkinEditor/SkinEditorPalettePanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

SkinEditorPalettePanel::SkinEditorPalettePanel()
{
    SkinEditorChrome::stylePanelTitle(titleLabel_, "PALETTE");
    addAndMakeVisible(titleLabel_);

    // Same slot height as Mappings so section→content gaps can be kColumnHeaderGap (11).
    list_.setRowHeight(SkinEditorChrome::kTableGapHeight);
    list_.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
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
        if (rows_[i].kind == RowKind::kColour && ! rows_[i].isBottomHalf && rows_[i].name == name)
        {
            list_.selectRow(static_cast<int>(i));
            return;
        }
    }
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
    const int tableW = paintWidthForRow(list_.getWidth());
    auto actionRow = juce::Rectangle<int>(list_.getX(), actions.getY(), tableW, actions.getHeight());
    const int half = (actionRow.getWidth() - SkinEditorChrome::kButtonGap) / 2;
    addButton_.setBounds(actionRow.removeFromLeft(juce::jmax(1, half)));
    actionRow.removeFromLeft(SkinEditorChrome::kButtonGap);
    deleteButton_.setBounds(actionRow);
}

int SkinEditorPalettePanel::paintWidthForRow(int listRowWidth) const noexcept
{
    // Keep table fills out of the scroll-track band even when the bar is hidden.
    juce::ignoreUnused(listRowWidth);
    return juce::jmax(1, list_.getWidth() - SkinEditorChrome::kScrollTrackWidth);
}

void SkinEditorPalettePanel::pushContentRow(Row row)
{
    row.isBottomHalf = false;
    rows_.push_back(row);
    row.isBottomHalf = true;
    rows_.push_back(row);
}

int SkinEditorPalettePanel::nextContentRow(int row) const noexcept
{
    int i = row + 1;
    while (i < static_cast<int>(rows_.size()) && rows_[static_cast<size_t>(i)].isBottomHalf)
        ++i;
    return i;
}

bool SkinEditorPalettePanel::isRowSelectedVisually(int row) const noexcept
{
    if (list_.isRowSelected(row))
        return true;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return false;
    if (rows_[static_cast<size_t>(row)].isBottomHalf)
        return list_.isRowSelected(row - 1);
    if (row + 1 < static_cast<int>(rows_.size()) && rows_[static_cast<size_t>(row + 1)].isBottomHalf)
        return list_.isRowSelected(row + 1);
    return false;
}

int SkinEditorPalettePanel::getNumRows()
{
    return static_cast<int>(rows_.size());
}

void SkinEditorPalettePanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    juce::ignoreUnused(selected, width);
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind == RowKind::kGap)
        return;

    const int paintW = paintWidthForRow(width);
    const int fullH = SkinEditorChrome::kColumnHeaderHeight;
    const int yOff = item.isBottomHalf ? -height : 0;
    const auto rowArea = juce::Rectangle<float>(0.0f, (float) yOff, (float) paintW, (float) fullH);

    const int contentRow = item.isBottomHalf ? row - 1 : row;
    const int next = nextContentRow(contentRow);
    const auto nextKind = next < static_cast<int>(rows_.size())
                              ? rows_[static_cast<size_t>(next)].kind
                              : RowKind::kGap;
    int prev = contentRow - 1;
    while (prev >= 0 && rows_[static_cast<size_t>(prev)].isBottomHalf)
        --prev;
    const auto prevKind = prev >= 0 ? rows_[static_cast<size_t>(prev)].kind : RowKind::kGap;
    // Section is its own chip; colour tables start after the 11px gap under the title.
    const bool roundTop = item.kind == RowKind::kSection
                          || (item.kind == RowKind::kColour
                              && (prev < 0 || prevKind == RowKind::kGap || prevKind == RowKind::kSection));
    const bool roundBottom = (item.kind == RowKind::kColour
                              && (nextKind == RowKind::kGap || nextKind == RowKind::kSection))
                             || (item.kind == RowKind::kSection && nextKind != RowKind::kColour);

    if (item.kind == RowKind::kSection)
    {
        SkinEditorChrome::fillTableRow(g, rowArea, SkinEditorChrome::surfaceSection(),
                                       roundTop, roundBottom);
        g.setColour(SkinEditorChrome::textTableHeader());
        g.setFont(SkinEditorChrome::fontBold());
        g.drawText(item.headerText, 6, yOff, paintW - 12, fullH, juce::Justification::centredLeft);
        return;
    }

    if (isRowSelectedVisually(row))
        SkinEditorChrome::fillTableRow(g, rowArea, juce::Colour(0xff303030),
                                       roundTop, roundBottom);

    SkinEditorChrome::paintColourSwatch(g,
                                        juce::Rectangle<int>(4, yOff + 4, fullH - 8, fullH - 8),
                                        juce::Colour(item.argb));
    g.setColour(SkinEditorChrome::textTableBody());
    g.setFont(SkinEditorChrome::fontRegular());
    g.drawText(item.name, fullH + 4, yOff, paintW - fullH - 8, fullH, juce::Justification::centredLeft);
}

void SkinEditorPalettePanel::selectedRowsChanged(int)
{
    const auto name = getSelectedPastilleName();
    deleteButton_.setEnabled(! readOnly_ && name.isNotEmpty()
                             && layer_ != nullptr
                             && ! layer_->isProductPastilleName(name));
    if (name.isNotEmpty() && onPastilleSelected)
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
        row.product = layer_->isProductPastilleName(pastille.name);
        if (row.product)
            product.push_back(row);
        else
            editor.push_back(row);
    }

    auto pushSection = [this](const juce::String& title)
    {
        Row section;
        section.kind = RowKind::kSection;
        section.headerText = title;
        pushContentRow(section);
        // Same 11px air as under Mappings "Property / State / Mapped colour".
        Row gap;
        gap.kind = RowKind::kGap;
        rows_.push_back(gap);
    };

    if (! product.empty())
    {
        pushSection("Product colours");
        for (auto& row : product)
            pushContentRow(row);
    }
    if (! editor.empty() || ! readOnly_)
    {
        if (! product.empty())
        {
            Row gap;
            gap.kind = RowKind::kGap;
            rows_.push_back(gap);
        }
        pushSection("Editor colours");
        for (auto& row : editor)
            pushContentRow(row);
    }
}
