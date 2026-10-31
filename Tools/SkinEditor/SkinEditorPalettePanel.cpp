#include "Tools/SkinEditor/SkinEditorPalettePanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    constexpr int kSwatchX = 4;
    constexpr int kNameGap = 8;
    constexpr int kNameCountGap = 6;
    constexpr int kCountRightInset = 4;

    void paintElidedLeft(juce::Graphics& g,
                         const juce::Font& font,
                         const juce::String& text,
                         juce::Rectangle<int> bounds)
    {
        g.setFont(font);
        g.drawText(SkinEditorChrome::elideToWidth(font, text, bounds.getWidth()),
                   bounds, juce::Justification::centredLeft);
    }

    int mappingCountLeft(int paintW, int countColumnW) noexcept
    {
        return paintW - kCountRightInset - countColumnW;
    }
}

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
    addAndMakeVisible(stickyHeaders_);
    addAndMakeVisible(addButton_);
    addAndMakeVisible(duplicateButton_);
    addAndMakeVisible(deleteButton_);
    duplicateButton_.setTooltip("Duplicate");
    deleteButton_.setTooltip("Delete");

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
    if (listenedScrollBar_ != nullptr)
        listenedScrollBar_->removeListener(this);
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
    const int scrollY = (list_.getViewport() != nullptr)
                            ? list_.getViewport()->getViewPositionY()
                            : 0;
    rebuildRows();
    list_.updateContent();
    list_.repaint();
    stickyHeaders_.repaint();
    resized();
    // Restore after layout: selectRow/updateContent must not jump New/Dup adds.
    if (auto* vp = list_.getViewport())
        vp->setViewPosition(vp->getViewPositionX(), scrollY);
    stickyHeaders_.repaint();
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
            // Keep scroll put (New / Dup / refresh must not jump the viewport).
            list_.selectRow(static_cast<int>(i), true);
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
        attachScrollBarListener();
        layoutStickyHeadersOverlay();
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

    attachScrollBarListener();
    layoutStickyHeadersOverlay();
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

    if (item.kind == RowKind::kSection)
    {
        // Section titles are painted by stickyHeaders_ (stacked sticky overlay).
        juce::ignoreUnused(paintW, fullH, yOff);
        return;
    }

    if (item.kind == RowKind::kEmptyHint)
    {
        // Darker than textMuted so the empty hint stays quiet under Editor colours.
        g.setColour(juce::Colour(0xff666666));
        paintElidedLeft(g, SkinEditorChrome::fontRegular(), item.headerText,
                        { 6, yOff, juce::jmax(0, paintW - 12), fullH });
        return;
    }

    paintColourRow(g, item, row, yOff);
}

void SkinEditorPalettePanel::paintColourRow(juce::Graphics& g, const Row& item, int row, int yOff)
{
    const int paintW = paintWidth();
    const int fullH = SkinEditorChrome::kColumnHeaderHeight;
    bool roundTop = false;
    bool roundBottom = false;
    roundFlagsForRow(row, roundTop, roundBottom);

    const auto rowArea = juce::Rectangle<float>(0.0f, (float) yOff, (float) paintW, (float) fullH);
    if (isRowSelectedVisually(row))
        SkinEditorChrome::fillTableRow(g, { rowArea, juce::Colour(0xff303030), roundTop, roundBottom });

    const int sw = SkinEditorChrome::kColourSwatchSize;
    const int swY = yOff + (fullH - sw) / 2;
    const auto swatch = juce::Rectangle<int>(kSwatchX, swY, sw, sw);
    if (item.pending)
        SkinEditorChrome::paintPendingColourSwatch(g, swatch);
    else
        SkinEditorChrome::paintColourSwatch(g, swatch, juce::Colour(item.argb));

    const auto& bodyFont = SkinEditorChrome::fontRegular();
    const int countLeft = mappingCountLeft(paintW, mappingCountColumnWidth_);
    if (item.mappingCount > 0)
    {
        g.setColour(SkinEditorChrome::attention());
        g.setFont(bodyFont);
        g.drawText(juce::String(item.mappingCount),
                   juce::Rectangle<int>(countLeft, yOff, mappingCountColumnWidth_, fullH),
                   juce::Justification::centredRight, false);
    }

    const int nameLeft = kSwatchX + sw + kNameGap;
    g.setColour(SkinEditorChrome::textTableBody());
    paintElidedLeft(g, bodyFont, item.name,
                    { nameLeft, yOff, juce::jmax(0, countLeft - kNameCountGap - nameLeft), fullH });
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

    // Rename is name-text only — swatch and trailing count do nothing.
    const int sw = SkinEditorChrome::kColourSwatchSize;
    const int nameLeft = kSwatchX + sw + kNameGap;
    const int countLeft = mappingCountLeft(paintWidth(), mappingCountColumnWidth_);
    if (e.x < nameLeft || e.x >= countLeft - kNameCountGap)
        return;

    if (onRenamePastilleRequested)
        onRenamePastilleRequested(item.name);
}

void SkinEditorPalettePanel::rebuildRows()
{
    rows_.clear();
    stickySections_.clear();
    mappingCountColumnWidth_ = 0;
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return;

    const auto& countFont = SkinEditorChrome::fontRegular();
    // Floor at two digits so 9→10 does not reflow name elision for the whole list.
    mappingCountColumnWidth_ = SkinEditorChrome::measureTextWidth(countFont, "99");

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
        row.mappingCount = layer_->countMappingsReferencing(skinId_, pastille.name);
        mappingCountColumnWidth_ = juce::jmax(
            mappingCountColumnWidth_,
            SkinEditorChrome::measureTextWidth(countFont, juce::String(row.mappingCount)));
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

    StickySection sticky;
    sticky.upperListRow = static_cast<int>(rows_.size());
    sticky.title = title;
    stickySections_.push_back(sticky);

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
