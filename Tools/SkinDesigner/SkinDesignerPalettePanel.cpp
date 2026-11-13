#include "Tools/SkinDesigner/SkinDesignerPalettePanel.h"

#include "Tools/SkinDesigner/SkinDesignerChrome.h"
#include "Tools/SkinDesigner/SkinDesignerPalettePanelGeometry.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

namespace
{
    void paintElidedLeft(juce::Graphics& g,
                         const juce::Font& font,
                         const juce::String& text,
                         juce::Rectangle<int> bounds)
    {
        g.setFont(font);
        g.drawText(SkinDesignerChrome::elideToWidth(font, text, bounds.getWidth()),
                   bounds, juce::Justification::centredLeft);
    }
}

SkinDesignerPalettePanel::SkinDesignerPalettePanel()
{
    SkinDesignerChrome::stylePanelTitle(titleLabel_, "PALETTE");
    addAndMakeVisible(titleLabel_);

    // Same slot height as Mappings so section→content gaps can be kColumnHeaderGap (11).
    list_.setRowHeight(SkinDesignerChrome::kTableGapHeight);
    list_.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    list_.setWantsKeyboardFocus(true);
    escapeForwarder_.handler = &onEscapeRequested;
    list_.addKeyListener(&escapeForwarder_);
    mappingCountHover_.owner = this;
    list_.addMouseListener(&mappingCountHover_, true);
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

SkinDesignerPalettePanel::~SkinDesignerPalettePanel()
{
    if (listenedScrollBar_ != nullptr)
        listenedScrollBar_->removeListener(this);
    list_.removeMouseListener(&mappingCountHover_);
    list_.removeKeyListener(&escapeForwarder_);
}

void SkinDesignerPalettePanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinDesignerPalettePanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinDesignerPalettePanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    updateActionButtonEnablement();
}

void SkinDesignerPalettePanel::refresh()
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
    // rebuildRows cleared hover; re-hit under a stationary pointer.
    refreshMappingCountHoverFromPointer();
    updateActionButtonEnablement();
}

void SkinDesignerPalettePanel::updateActionButtonEnablement()
{
    const auto selected = getSelectedPastilleName();
    addButton_.setEnabled(! readOnly_);
    duplicateButton_.setEnabled(! readOnly_ && selected.isNotEmpty());
    deleteButton_.setEnabled(! readOnly_ && selected.isNotEmpty()
                             && layer_ != nullptr
                             && skinId_.isNotEmpty());
}

juce::String SkinDesignerPalettePanel::getSelectedPastilleName() const
{
    const int row = list_.getSelectedRow();
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return {};
    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kColour)
        return {};
    return item.name;
}

void SkinDesignerPalettePanel::selectPastille(const juce::String& name)
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

void SkinDesignerPalettePanel::clearSelection()
{
    list_.deselectAllRows();
}

void SkinDesignerPalettePanel::paint(juce::Graphics& g)
{
    SkinDesignerChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_, 0);
    SkinDesignerChrome::paintRoundedWell(g, list_.getBounds(), SkinDesignerChrome::surfacePanel());
}

void SkinDesignerPalettePanel::resized()
{
    const auto chrome = SkinDesignerChrome::layoutPanel(getLocalBounds(),
                                                      SkinDesignerChrome::titleTabWidth("PALETTE"),
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
        listBounds.setRight(listBounds.getRight() + SkinDesignerChrome::kPanelContentPad);
        list_.setBounds(listBounds);
        if (auto* vp = list_.getViewport())
            vp->setScrollBarThickness(SkinDesignerChrome::kScrollTrackWidth);
        attachScrollBarListener();
        layoutStickyHeadersOverlay();
        return;
    }

    addButton_.setVisible(true);
    duplicateButton_.setVisible(true);
    deleteButton_.setVisible(true);

    // Top: buttons, then content pad.
    auto actions = bounds.removeFromTop(SkinDesignerChrome::kButtonHeight);
    bounds.removeFromTop(SkinDesignerChrome::kPanelContentPad);

    // Same as Mappings: list extends into the right content pad; scroll thumb centred there.
    auto listBounds = bounds;
    listBounds.setRight(listBounds.getRight() + SkinDesignerChrome::kPanelContentPad);
    list_.setBounds(listBounds);
    if (auto* vp = list_.getViewport())
        vp->setScrollBarThickness(SkinDesignerChrome::kScrollTrackWidth);

    attachScrollBarListener();
    layoutStickyHeadersOverlay();
    layoutActionButtons({ list_.getX(), actions.getY(), paintWidth(), actions.getHeight() });
}

void SkinDesignerPalettePanel::layoutActionButtons(juce::Rectangle<int> actionRow)
{
    const int tableW = actionRow.getWidth();
    const int gaps = 2 * SkinDesignerChrome::kButtonGap;
    const bool longLabels = SkinDesignerChrome::paletteActionButtonsPreferLongLabels(tableW);
    duplicateButton_.setButtonText(longLabels ? "Duplicate" : "Dup");
    deleteButton_.setButtonText(longLabels ? "Delete" : "Del");

    // Equal thirds across the action row; leftover pixels go to Delete.
    const int inner = juce::jmax(3, tableW - gaps);
    const int newW = inner / 3;
    const int dupW = inner / 3;
    const int delW = inner - newW - dupW;

    addButton_.setBounds(actionRow.removeFromLeft(newW)
                             .withHeight(SkinDesignerChrome::kButtonHeight));
    actionRow.removeFromLeft(SkinDesignerChrome::kButtonGap);
    duplicateButton_.setBounds(actionRow.removeFromLeft(dupW)
                                   .withHeight(SkinDesignerChrome::kButtonHeight));
    actionRow.removeFromLeft(SkinDesignerChrome::kButtonGap);
    deleteButton_.setBounds(actionRow.removeFromLeft(delW)
                                .withHeight(SkinDesignerChrome::kButtonHeight));
}

int SkinDesignerPalettePanel::paintWidth() const noexcept
{
    // Keep table fills out of the scroll-track band even when the bar is hidden.
    return juce::jmax(1, list_.getWidth() - SkinDesignerChrome::kScrollTrackWidth);
}

void SkinDesignerPalettePanel::pushSplitListRow(Row row)
{
    row.isLowerListHalf = false;
    rows_.push_back(row);
    row.isLowerListHalf = true;
    rows_.push_back(row);
}

int SkinDesignerPalettePanel::nextUpperListRow(int row) const noexcept
{
    int i = row + 1;
    while (i < static_cast<int>(rows_.size()) && rows_[static_cast<size_t>(i)].isLowerListHalf)
        ++i;
    return i;
}

bool SkinDesignerPalettePanel::isRowSelectedVisually(int row) const noexcept
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

int SkinDesignerPalettePanel::getNumRows()
{
    return static_cast<int>(rows_.size());
}

bool SkinDesignerPalettePanel::isBlockBoundary(RowKind kind) noexcept
{
    return kind == RowKind::kGap || kind == RowKind::kSection || kind == RowKind::kEmptyHint;
}

bool SkinDesignerPalettePanel::isContentRow(RowKind kind) noexcept
{
    return kind == RowKind::kColour || kind == RowKind::kEmptyHint;
}

void SkinDesignerPalettePanel::roundFlagsForRow(int row, bool& roundTop, bool& roundBottom) const noexcept
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

void SkinDesignerPalettePanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    juce::ignoreUnused(selected, width);
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind == RowKind::kGap)
        return;

    const int paintW = paintWidth();
    const int fullH = SkinDesignerChrome::kColumnHeaderHeight;
    const int yOff = item.isLowerListHalf ? -height : 0;

    if (item.kind == RowKind::kSection)
    {
        // Section titles are painted by stickyHeaders_ (stacked sticky overlay).
        juce::ignoreUnused(paintW, fullH, yOff);
        return;
    }

    if (item.kind == RowKind::kEmptyHint)
    {
        // Darker than textMuted so the empty hint stays quiet under the Palette list.
        g.setColour(juce::Colour(0xff666666));
        paintElidedLeft(g, SkinDesignerChrome::fontRegular(), item.headerText,
                        { 6, yOff, juce::jmax(0, paintW - 12), fullH });
        return;
    }

    paintColourRow(g, item, row, yOff);
}

void SkinDesignerPalettePanel::paintColourRow(juce::Graphics& g, const Row& item, int row, int yOff)
{
    const int paintW = paintWidth();
    const int fullH = SkinDesignerChrome::kColumnHeaderHeight;
    bool roundTop = false;
    bool roundBottom = false;
    roundFlagsForRow(row, roundTop, roundBottom);

    const auto rowArea = juce::Rectangle<float>(0.0f, (float) yOff, (float) paintW, (float) fullH);
    if (isRowSelectedVisually(row))
        SkinDesignerChrome::fillTableRow(g, { rowArea, juce::Colour(0xff303030), roundTop, roundBottom,
                                            SkinDesignerChrome::kMappingsHighlightCorner });

    namespace Geo = SkinDesignerPaletteGeometry;
    const int sw = SkinDesignerChrome::kColourSwatchSize;
    const int swY = yOff + (fullH - sw) / 2;
    const auto swatch = juce::Rectangle<int>(Geo::kSwatchX, swY, sw, sw);
    if (item.pending)
        SkinDesignerChrome::paintPendingColourSwatch(g, swatch);
    else
        SkinDesignerChrome::paintColourSwatch(g, swatch, juce::Colour(item.argb));

    const auto& bodyFont = SkinDesignerChrome::fontRegular();
    const int countRight = Geo::mappingCountRight(paintW);
    const int countLeft = countRight - mappingCountColumnWidth_;
    if (item.mappingCount > 0)
    {
        // Shared right edge for every row; badge grows left on hover (no x jump).
        const int badgeW = SkinDesignerChrome::countBadgeWidth(item.mappingCount);
        const auto badgeArea = juce::Rectangle<int>(countRight - badgeW, yOff, badgeW, fullH);
        if (item.name == hoveredMappingCountName_)
        {
            SkinDesignerChrome::drawCountBadge(g, badgeArea, item.mappingCount);
        }
        else
        {
            g.setColour(SkinDesignerChrome::attention());
            g.setFont(SkinDesignerChrome::fontBold(SkinDesignerChrome::kCountBadgeFont));
            const juce::Rectangle<int> countArea { countLeft, yOff, mappingCountColumnWidth_, fullH };
            g.drawText(juce::String(item.mappingCount), countArea,
                       juce::Justification::centredRight, false);
        }
    }

    const int nameLeft = Geo::kSwatchX + sw + Geo::kNameGap;
    g.setColour(SkinDesignerChrome::textTableBody());
    paintElidedLeft(g, bodyFont, SkinDesignerUi::displayPastilleName(item.name),
                    { nameLeft, yOff, juce::jmax(0, countLeft - Geo::kNameCountGap - nameLeft), fullH });
}

void SkinDesignerPalettePanel::rebuildRows()
{
    rows_.clear();
    stickySections_.clear();
    hoveredMappingCountName_.clear();
    // Column width tracks badge metrics only so idle text and hover chips share one right edge.
    mappingCountColumnWidth_ = 0;
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return;

    // Floor at two-digit badge so hover capsule does not reflow names.
    mappingCountColumnWidth_ = SkinDesignerChrome::countBadgeWidth(99);

    std::vector<Row> colours;
    for (const auto& pastille : skin->palette)
    {
        Row row;
        row.kind = RowKind::kColour;
        row.name = pastille.name;
        row.argb = pastille.argb;
        row.pending = pastille.pending;
        row.isProduct = false;
        row.mappingCount = layer_->countMappingsReferencing(skinId_, pastille.name);
        mappingCountColumnWidth_ = juce::jmax(mappingCountColumnWidth_,
                                              SkinDesignerChrome::countBadgeWidth(row.mappingCount));
        colours.push_back(row);
    }

    if (! colours.empty() || ! readOnly_)
        appendPaletteGroup("Palette", colours, false);
}

void SkinDesignerPalettePanel::appendPaletteGroup(const juce::String& title,
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
        hint.headerText = "No designer colours yet";
        pushSplitListRow(hint);
        return;
    }
    for (auto& row : colours)
        pushSplitListRow(row);
}
