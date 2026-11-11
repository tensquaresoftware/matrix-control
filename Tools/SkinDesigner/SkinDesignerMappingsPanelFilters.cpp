#include "Tools/SkinDesigner/SkinDesignerMappingsPanel.h"

#include "Tools/SkinDesigner/SkinDesignerChrome.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumnFilter.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumns.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsFilterMenu.h"

namespace
{
    juce::PopupMenu::Options filterMenuOptions(juce::Component& host,
                                               juce::Rectangle<int> screenArea)
    {
        return juce::PopupMenu::Options()
            .withTargetComponent(&host)
            .withParentComponent(host.getTopLevelComponent())
            .withTargetScreenArea(screenArea);
    }

    struct LegendColumnPlace
    {
        juce::String text;
        int x = 0;
        int maxW = 0;
        juce::Rectangle<int>* labelBounds = nullptr;
        juce::Rectangle<int>* sortBounds = nullptr;
        juce::Rectangle<int>* clearBounds = nullptr;
        bool filterActive = false;
    };

    void placeLegendColumn(const juce::Rectangle<int>& area,
                           const juce::Font& font,
                           LegendColumnPlace spec)
    {
        const int sortW = SkinDesignerChrome::sortBadgeWidth();
        const int badgeW = SkinDesignerChrome::clearBadgeWidth();
        const int labelSortGap = SkinDesignerChrome::kButtonGap;
        const int sortClearGap = SkinDesignerChrome::kLegendChipGap;
        const int clearReserve = spec.filterActive ? (sortClearGap + badgeW) : 0;
        const int reserved = labelSortGap + sortW + clearReserve;
        const int labelMax = juce::jmax(0, spec.maxW - reserved);
        const int labelW = juce::jmin(labelMax, SkinDesignerChrome::measureTextWidth(font, spec.text));
        *spec.labelBounds = { spec.x, area.getY(), juce::jmax(0, labelW + labelSortGap + sortW),
                              area.getHeight() };
        *spec.sortBounds = { spec.x + labelW + labelSortGap, area.getY(), sortW, area.getHeight() };
        if (spec.filterActive)
            *spec.clearBounds = { spec.sortBounds->getRight() + sortClearGap, area.getY(), badgeW,
                                  area.getHeight() };
    }
}

juce::String& SkinDesignerMappingsPanel::filterValue(FilterColumn column) noexcept
{
    switch (column)
    {
        case FilterColumn::kProperty: return propertyFilter_;
        case FilterColumn::kState: return stateFilter_;
        case FilterColumn::kColourSet: return colourSetFilter_;
        case FilterColumn::kMappedColour: return mappedColourFilter_;
    }
    return propertyFilter_;
}

const juce::String& SkinDesignerMappingsPanel::filterValue(FilterColumn column) const noexcept
{
    switch (column)
    {
        case FilterColumn::kProperty: return propertyFilter_;
        case FilterColumn::kState: return stateFilter_;
        case FilterColumn::kColourSet: return colourSetFilter_;
        case FilterColumn::kMappedColour: return mappedColourFilter_;
    }
    return propertyFilter_;
}

juce::StringArray SkinDesignerMappingsPanel::presentValuesFor(FilterColumn column) const
{
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return {};

    using CF = SkinDesignerMappingsColumnFilter::Column;
    CF mapped = CF::kProperty;
    switch (column)
    {
        case FilterColumn::kProperty: mapped = CF::kProperty; break;
        case FilterColumn::kState: mapped = CF::kState; break;
        case FilterColumn::kColourSet: mapped = CF::kColourSet; break;
        case FilterColumn::kMappedColour: mapped = CF::kMappedColour; break;
    }
    return SkinDesignerMappingsColumnFilter::presentValues(*skin, mapped);
}

juce::Rectangle<int> SkinDesignerMappingsPanel::legendFilterTargetBounds(FilterColumn column) const noexcept
{
    if (column == FilterColumn::kState)
        return stateLabelBounds_;
    if (column == FilterColumn::kColourSet)
        return colourSetLabelBounds_;
    if (column == FilterColumn::kMappedColour)
        return mappedLabelBounds_;
    return propertyLabelBounds_;
}

SkinDesignerMappingsColumnFilter::SortColumn SkinDesignerMappingsPanel::sortColumnFor(
    FilterColumn column) const noexcept
{
    using SC = SkinDesignerMappingsColumnFilter::SortColumn;
    switch (column)
    {
        case FilterColumn::kProperty: return SC::kProperty;
        case FilterColumn::kState: return SC::kState;
        case FilterColumn::kColourSet: return SC::kColourSet;
        case FilterColumn::kMappedColour: return SC::kMappedColour;
    }
    return SC::kNone;
}

SkinDesignerChrome::SortPictoKind SkinDesignerMappingsPanel::sortPictoFor(FilterColumn column) const noexcept
{
    if (sortSpec_.column != sortColumnFor(column))
        return SkinDesignerChrome::SortPictoKind::kInactive;
    return sortSpec_.ascending ? SkinDesignerChrome::SortPictoKind::kAscending
                               : SkinDesignerChrome::SortPictoKind::kDescending;
}

void SkinDesignerMappingsPanel::cycleColumnSort(FilterColumn column)
{
    const auto target = sortColumnFor(column);
    if (sortSpec_.column == target)
        sortSpec_.ascending = ! sortSpec_.ascending;
    else
    {
        sortSpec_.column = target;
        sortSpec_.ascending = true;
    }
    refresh();
}

bool SkinDesignerMappingsPanel::populateColumnFilterMenu(juce::PopupMenu& menu,
                                                       FilterColumn column,
                                                       const juce::StringArray& values)
{
    menu.setLookAndFeel(&getLookAndFeel());
    const auto& active = filterValue(column);
    if (column == FilterColumn::kMappedColour)
    {
        const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                               ? layer_->findSkin(skinId_)
                               : nullptr;
        if (skin == nullptr)
            return false;
        SkinDesignerMappingsFilterMenu::appendMappedColourItems(menu, *skin, values, active);
        return true;
    }

    if (column == FilterColumn::kColourSet)
    {
        const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                               ? layer_->findSkin(skinId_)
                               : nullptr;
        if (skin == nullptr)
            return false;
        for (int i = 0; i < values.size(); ++i)
        {
            const auto& token = values[i];
            menu.addItem(i + 1,
                         SkinDesignerMappingsColumnFilter::colourSetFilterMenuLabel(*skin, token),
                         true,
                         token == active);
        }
        return true;
    }

    for (int i = 0; i < values.size(); ++i)
        menu.addItem(i + 1, values[i], true, values[i] == active);
    return true;
}

void SkinDesignerMappingsPanel::showColumnFilterMenu(FilterColumn column)
{
    const auto values = presentValuesFor(column);
    if (values.isEmpty())
        return;

    juce::PopupMenu menu;
    if (! populateColumnFilterMenu(menu, column, values))
        return;

    const juce::String openedForSkinId = skinId_;
    menu.showMenuAsync(
        filterMenuOptions(*this, localAreaToGlobal(legendFilterTargetBounds(column))),
        [safe = juce::Component::SafePointer<SkinDesignerMappingsPanel>(this),
         column,
         values,
         openedForSkinId](int result)
        {
            if (safe == nullptr || result <= 0 || result > values.size())
                return;
            if (safe->skinId_ != openedForSkinId)
                return;
            safe->setColumnFilter(column, values[result - 1]);
        });
}

void SkinDesignerMappingsPanel::clearLegendHitTargets()
{
    propertyLabelBounds_ = {};
    propertySortBounds_ = {};
    propertyClearBounds_ = {};
    stateLabelBounds_ = {};
    stateSortBounds_ = {};
    stateClearBounds_ = {};
    colourSetLabelBounds_ = {};
    colourSetSortBounds_ = {};
    colourSetClearBounds_ = {};
    mappedLabelBounds_ = {};
    mappedSortBounds_ = {};
    mappedClearBounds_ = {};
    linkLabelBounds_ = {};
    linkChainBounds_ = {};
    linkBadgeBounds_ = {};
    globalClearBounds_ = {};
}

void SkinDesignerMappingsPanel::layoutLegendHitTargets()
{
    clearLegendHitTargets();
    if (stickyLegendBounds_.isEmpty())
        return;

    const auto area = stickyLegendBounds_;
    const auto cols = SkinDesignerMappingsColumns::forWidth(area.getWidth());
    const auto font = SkinDesignerChrome::fontBold();
    const int gap = SkinDesignerChrome::kButtonGap;
    const int sortW = SkinDesignerChrome::sortBadgeWidth();
    const int badgeW = SkinDesignerChrome::clearBadgeWidth();

    placeLegendColumn(area, font,
                      { "Property", area.getX() + cols.contentLeft(cols.propertyLeft()),
                        cols.contentWidth(cols.propertyW),
                        &propertyLabelBounds_, &propertySortBounds_,
                        &propertyClearBounds_, propertyFilter_.isNotEmpty() });
    placeLegendColumn(area, font,
                      { "State", area.getX() + cols.contentLeft(cols.stateLeft()),
                        cols.contentWidth(cols.stateW),
                        &stateLabelBounds_, &stateSortBounds_,
                        &stateClearBounds_, stateFilter_.isNotEmpty() });
    placeLegendColumn(area, font,
                      { "Colour set", area.getX() + cols.contentLeft(cols.colourSetLeft()),
                        cols.contentWidth(cols.colourSetW),
                        &colourSetLabelBounds_, &colourSetSortBounds_,
                        &colourSetClearBounds_, colourSetFilter_.isNotEmpty() });
    layoutLinkLegendHitTargets(area, cols, font);

    const int mappedContentW = cols.contentWidth(cols.mappedW);
    const int mappedReserve = (gap + sortW)
                              + (mappedColourFilter_.isNotEmpty()
                                     ? (SkinDesignerChrome::kLegendChipGap + badgeW)
                                     : 0);
    const auto mappedText = SkinDesignerChrome::elideToWidth(
        font, "Mapped colour", juce::jmax(0, mappedContentW - mappedReserve));
    placeLegendColumn(area, font,
                      { mappedText, area.getX() + cols.contentLeft(cols.mappedX), mappedContentW,
                        &mappedLabelBounds_, &mappedSortBounds_,
                        &mappedClearBounds_, mappedColourFilter_.isNotEmpty() });
    layoutGlobalClearBounds();
}

void SkinDesignerMappingsPanel::layoutLinkLegendHitTargets(
    juce::Rectangle<int> area,
    const SkinDesignerMappingsColumns::Layout& cols,
    const juce::Font& font)
{
    juce::ignoreUnused(font);
    const int pad = SkinDesignerChrome::kMappingsColumnPad;
    const int chipGap = SkinDesignerChrome::kLegendChipGap;
    const int linkChainW = SkinDesignerChrome::linkChainBadgeWidth();
    const int linkCountW = SkinDesignerChrome::linkCountBadgeWidth();
    // Lay out inside the visible cut slab (column width minus trailing cutter) so the
    // right pad after the count chip matches the left pad before the chain (6 px).
    const int slabW = cols.linkW - SkinDesignerChrome::kMappingsColumnDividerThickness;
    const int linkX = area.getX() + cols.linkLeft() + pad;
    linkChainBounds_ = { linkX, area.getY(), linkChainW, area.getHeight() };
    linkBadgeBounds_ = { linkChainBounds_.getRight() + chipGap, area.getY(),
                         linkCountW, area.getHeight() };
    linkLabelBounds_ = { linkX, area.getY(), juce::jmax(0, slabW - 2 * pad), area.getHeight() };
}

void SkinDesignerMappingsPanel::layoutGlobalClearBounds()
{
    if (! anyColumnFilterActive() || stickyLegendBounds_.isEmpty())
        return;

    const auto area = stickyLegendBounds_;
    const int badgeW = SkinDesignerChrome::clearBadgeWidth();
    const int gutterLeft = area.getRight();
    const int gutterRight = viewport_.getRight();
    const int clearX = gutterLeft + juce::jmax(0, (gutterRight - gutterLeft - badgeW) / 2);
    if (clearX + badgeW <= gutterRight)
        globalClearBounds_ = { clearX, area.getY(), badgeW, area.getHeight() };
}

void SkinDesignerMappingsPanel::paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const
{
    const auto cols = SkinDesignerMappingsColumns::forWidth(area.getWidth());
    SkinDesignerMappingsColumns::fillCutColumnSlabs(
        g, cols, { area, SkinDesignerChrome::surfaceSection(), true, true });
    const auto font = SkinDesignerChrome::fontBold();
    g.setFont(font);

    auto paintColumn = [&](const juce::String& text,
                           juce::Rectangle<int> labelBounds,
                           juce::Rectangle<int> sortBounds,
                           juce::Rectangle<int> clearBounds,
                           bool filterActive,
                           FilterColumn column)
    {
        // Re-apply after badges (count / sort) which leave a smaller Graphics font.
        g.setFont(font);
        const int sortW = SkinDesignerChrome::sortBadgeWidth();
        const int titleW = juce::jmax(0, labelBounds.getWidth() - SkinDesignerChrome::kButtonGap - sortW);
        g.setColour(filterActive ? SkinDesignerChrome::attention()
                                 : SkinDesignerChrome::textTableHeader());
        g.drawText(text, labelBounds.getX(), labelBounds.getY(), titleW, labelBounds.getHeight(),
                   juce::Justification::centredLeft, false);
        if (! sortBounds.isEmpty())
            SkinDesignerChrome::drawSortBadge(g, sortBounds, sortPictoFor(column));
        if (filterActive && ! clearBounds.isEmpty())
            SkinDesignerChrome::drawClearBadge(g, clearBounds);
    };

    paintColumn("Property", propertyLabelBounds_, propertySortBounds_, propertyClearBounds_,
                propertyFilter_.isNotEmpty(), FilterColumn::kProperty);
    paintColumn("State", stateLabelBounds_, stateSortBounds_, stateClearBounds_,
                stateFilter_.isNotEmpty(), FilterColumn::kState);
    paintColumn("Colour set", colourSetLabelBounds_, colourSetSortBounds_, colourSetClearBounds_,
                colourSetFilter_.isNotEmpty(), FilterColumn::kColourSet);
    paintLinkLegend(g);

    const int mappedReserve = (SkinDesignerChrome::kButtonGap + SkinDesignerChrome::sortBadgeWidth())
                              + (mappedColourFilter_.isNotEmpty()
                                     ? (SkinDesignerChrome::kLegendChipGap
                                        + SkinDesignerChrome::clearBadgeWidth())
                                     : 0);
    paintColumn(SkinDesignerChrome::elideToWidth(font, "Mapped colour",
                                               juce::jmax(0, cols.mappedW - mappedReserve)),
                mappedLabelBounds_, mappedSortBounds_, mappedClearBounds_,
                mappedColourFilter_.isNotEmpty(), FilterColumn::kMappedColour);
}

bool SkinDesignerMappingsPanel::handleLegendClearClick(juce::Point<int> pos)
{
    if (objectClearBounds_.contains(pos))
    {
        clearObjectMacroFilter();
        return true;
    }
    if (globalClearBounds_.contains(pos))
    {
        clearAllColumnFilters();
        refresh();
        return true;
    }
    if (propertyClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kProperty);
        return true;
    }
    if (stateClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kState);
        return true;
    }
    if (colourSetClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kColourSet);
        return true;
    }
    if (mappedClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kMappedColour);
        return true;
    }
    return false;
}

void SkinDesignerMappingsPanel::mouseUp(const juce::MouseEvent& e)
{
    if (isShellEmpty() || ! e.mouseWasClicked())
        return;

    const auto pos = e.getPosition();
    if (handleLegendClearClick(pos))
        return;
    if (handleLinkLegendClick(pos))
        return;

    auto handleHeader = [&](FilterColumn column, juce::Rectangle<int> bounds)
    {
        if (! bounds.contains(pos))
            return false;
        if (SkinDesignerMappingsColumnFilter::headerGestureForClick(e.mods.isPopupMenu())
            == SkinDesignerMappingsColumnFilter::HeaderGesture::kOpenFilter)
            showColumnFilterMenu(column);
        else
            cycleColumnSort(column);
        return true;
    };

    if (handleHeader(FilterColumn::kProperty, propertyLabelBounds_))
        return;
    if (handleHeader(FilterColumn::kState, stateLabelBounds_))
        return;
    if (handleHeader(FilterColumn::kColourSet, colourSetLabelBounds_))
        return;
    if (handleHeader(FilterColumn::kMappedColour, mappedLabelBounds_))
        return;
    // Object / Hold / Unbound (and object-clear) — same preserve set as MainComponent.
    if (hitActionRowChrome(pos))
        return;

    // Sticky air / action-row empty / legend slab miss (list hits go to Content).
    clearRowSelection();
}
