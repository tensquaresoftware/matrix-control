#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumns.h"
#include "Tools/SkinEditor/SkinEditorMappingsFilterMenu.h"

namespace
{
    constexpr int kColPad = SkinEditorChrome::kMappingsColumnPad;

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
        const int sortW = SkinEditorChrome::sortBadgeWidth();
        const int badgeW = SkinEditorChrome::clearBadgeWidth();
        const int labelSortGap = SkinEditorChrome::kButtonGap;
        const int sortClearGap = SkinEditorChrome::kSortToFilterClearGap;
        const int clearReserve = spec.filterActive ? (sortClearGap + badgeW) : 0;
        const int reserved = labelSortGap + sortW + clearReserve;
        const int labelMax = juce::jmax(0, spec.maxW - reserved);
        const int labelW = juce::jmin(labelMax, SkinEditorChrome::measureTextWidth(font, spec.text));
        *spec.labelBounds = { spec.x, area.getY(), juce::jmax(0, labelW + labelSortGap + sortW),
                              area.getHeight() };
        *spec.sortBounds = { spec.x + labelW + labelSortGap, area.getY(), sortW, area.getHeight() };
        if (spec.filterActive)
            *spec.clearBounds = { spec.sortBounds->getRight() + sortClearGap, area.getY(), badgeW,
                                  area.getHeight() };
    }
}

juce::String& SkinEditorMappingsPanel::filterValue(FilterColumn column) noexcept
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

const juce::String& SkinEditorMappingsPanel::filterValue(FilterColumn column) const noexcept
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

juce::StringArray SkinEditorMappingsPanel::presentValuesFor(FilterColumn column) const
{
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return {};

    using CF = SkinEditorMappingsColumnFilter::Column;
    CF mapped = CF::kProperty;
    switch (column)
    {
        case FilterColumn::kProperty: mapped = CF::kProperty; break;
        case FilterColumn::kState: mapped = CF::kState; break;
        case FilterColumn::kColourSet: mapped = CF::kColourSet; break;
        case FilterColumn::kMappedColour: mapped = CF::kMappedColour; break;
    }
    return SkinEditorMappingsColumnFilter::presentValues(*skin, mapped);
}

juce::Rectangle<int> SkinEditorMappingsPanel::legendFilterTargetBounds(FilterColumn column) const noexcept
{
    if (column == FilterColumn::kState)
        return stateLabelBounds_;
    if (column == FilterColumn::kColourSet)
        return colourSetLabelBounds_;
    if (column == FilterColumn::kMappedColour)
        return mappedLabelBounds_;
    return propertyLabelBounds_;
}

SkinEditorMappingsColumnFilter::SortColumn SkinEditorMappingsPanel::sortColumnFor(
    FilterColumn column) const noexcept
{
    using SC = SkinEditorMappingsColumnFilter::SortColumn;
    switch (column)
    {
        case FilterColumn::kProperty: return SC::kProperty;
        case FilterColumn::kState: return SC::kState;
        case FilterColumn::kColourSet: return SC::kColourSet;
        case FilterColumn::kMappedColour: return SC::kMappedColour;
    }
    return SC::kNone;
}

SkinEditorChrome::SortPictoKind SkinEditorMappingsPanel::sortPictoFor(FilterColumn column) const noexcept
{
    if (sortSpec_.column != sortColumnFor(column))
        return SkinEditorChrome::SortPictoKind::kInactive;
    return sortSpec_.ascending ? SkinEditorChrome::SortPictoKind::kAscending
                               : SkinEditorChrome::SortPictoKind::kDescending;
}

void SkinEditorMappingsPanel::cycleColumnSort(FilterColumn column)
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

bool SkinEditorMappingsPanel::populateColumnFilterMenu(juce::PopupMenu& menu,
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
        SkinEditorMappingsFilterMenu::appendMappedColourItems(menu, *skin, values, active);
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
                         SkinEditorMappingsColumnFilter::colourSetFilterMenuLabel(*skin, token),
                         true,
                         token == active);
        }
        return true;
    }

    for (int i = 0; i < values.size(); ++i)
        menu.addItem(i + 1, values[i], true, values[i] == active);
    return true;
}

void SkinEditorMappingsPanel::showColumnFilterMenu(FilterColumn column)
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
        [safe = juce::Component::SafePointer<SkinEditorMappingsPanel>(this),
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

void SkinEditorMappingsPanel::clearLegendHitTargets()
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

void SkinEditorMappingsPanel::layoutLegendHitTargets()
{
    clearLegendHitTargets();
    if (stickyLegendBounds_.isEmpty())
        return;

    const auto area = stickyLegendBounds_;
    const auto cols = SkinEditorMappingsColumns::forWidth(area.getWidth());
    const auto font = SkinEditorChrome::fontBold();
    const int gap = SkinEditorChrome::kButtonGap;
    const int sortW = SkinEditorChrome::sortBadgeWidth();
    const int badgeW = SkinEditorChrome::clearBadgeWidth();

    placeLegendColumn(area, font, { "Property", area.getX() + kColPad,
                                    juce::jmax(0, cols.propertyW - 4),
                                    &propertyLabelBounds_, &propertySortBounds_,
                                    &propertyClearBounds_, propertyFilter_.isNotEmpty() });
    placeLegendColumn(area, font, { "State", area.getX() + kColPad + cols.propertyW,
                                    juce::jmax(0, cols.stateW - 4),
                                    &stateLabelBounds_, &stateSortBounds_,
                                    &stateClearBounds_, stateFilter_.isNotEmpty() });
    placeLegendColumn(area, font,
                      { "Colour set", area.getX() + cols.colourSetLeft(),
                        juce::jmax(0, cols.colourSetW - 4),
                        &colourSetLabelBounds_, &colourSetSortBounds_,
                        &colourSetClearBounds_, colourSetFilter_.isNotEmpty() });

    // Link: title + chain control + optional count badge (not sortable / filterable).
    const int linkChainW = SkinEditorChrome::linkChainBadgeWidth();
    const auto linkHeader = linkHeaderState();
    const int linkCountW = (linkHeader.badgeCount > 0)
                               ? (gap + SkinEditorChrome::countBadgeWidth(linkHeader.badgeCount))
                               : 0;
    const int linkTitleMax = juce::jmax(0, cols.linkW - gap - linkChainW - linkCountW);
    const int linkTitleW = juce::jmin(linkTitleMax,
                                     SkinEditorChrome::measureTextWidth(font, "Link"));
    const int linkX = area.getX() + cols.linkLeft();
    linkLabelBounds_ = { linkX, area.getY(), linkTitleW + gap + linkChainW, area.getHeight() };
    linkChainBounds_ = { linkX + linkTitleW + gap, area.getY(), linkChainW, area.getHeight() };
    if (linkHeader.badgeCount > 0)
        linkBadgeBounds_ = { linkChainBounds_.getRight() + gap, area.getY(),
                             SkinEditorChrome::countBadgeWidth(linkHeader.badgeCount),
                             area.getHeight() };

    const int mappedReserve = (gap + sortW)
                              + (mappedColourFilter_.isNotEmpty()
                                     ? (SkinEditorChrome::kSortToFilterClearGap + badgeW)
                                     : 0);
    const auto mappedText = SkinEditorChrome::elideToWidth(
        font, "Mapped colour", juce::jmax(0, cols.mappedW - mappedReserve));
    placeLegendColumn(area, font, { mappedText, area.getX() + cols.mappedX, cols.mappedW,
                                    &mappedLabelBounds_, &mappedSortBounds_,
                                    &mappedClearBounds_, mappedColourFilter_.isNotEmpty() });
    layoutGlobalClearBounds();
}

void SkinEditorMappingsPanel::layoutGlobalClearBounds()
{
    if (! anyColumnFilterActive() || stickyLegendBounds_.isEmpty())
        return;

    const auto area = stickyLegendBounds_;
    const int badgeW = SkinEditorChrome::clearBadgeWidth();
    const int gutterLeft = area.getRight();
    const int gutterRight = viewport_.getRight();
    const int clearX = gutterLeft + juce::jmax(0, (gutterRight - gutterLeft - badgeW) / 2);
    if (clearX + badgeW <= gutterRight)
        globalClearBounds_ = { clearX, area.getY(), badgeW, area.getHeight() };
}

void SkinEditorMappingsPanel::paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const
{
    SkinEditorChrome::fillTableRow(g, { area.toFloat(), SkinEditorChrome::surfaceSection(),
                                        true, true });
    const auto cols = SkinEditorMappingsColumns::forWidth(area.getWidth());
    const auto font = SkinEditorChrome::fontBold();
    g.setFont(font);

    auto paintColumn = [&](const juce::String& text,
                           juce::Rectangle<int> labelBounds,
                           juce::Rectangle<int> sortBounds,
                           juce::Rectangle<int> clearBounds,
                           bool filterActive,
                           FilterColumn column)
    {
        const int sortW = SkinEditorChrome::sortBadgeWidth();
        const int titleW = juce::jmax(0, labelBounds.getWidth() - SkinEditorChrome::kButtonGap - sortW);
        g.setColour(filterActive ? SkinEditorChrome::attention()
                                 : SkinEditorChrome::textTableHeader());
        g.drawText(text, labelBounds.getX(), labelBounds.getY(), titleW, labelBounds.getHeight(),
                   juce::Justification::centredLeft, false);
        if (! sortBounds.isEmpty())
            SkinEditorChrome::drawSortBadge(g, sortBounds, sortPictoFor(column));
        if (filterActive && ! clearBounds.isEmpty())
            SkinEditorChrome::drawClearBadge(g, clearBounds);
    };

    paintColumn("Property", propertyLabelBounds_, propertySortBounds_, propertyClearBounds_,
                propertyFilter_.isNotEmpty(), FilterColumn::kProperty);
    paintColumn("State", stateLabelBounds_, stateSortBounds_, stateClearBounds_,
                stateFilter_.isNotEmpty(), FilterColumn::kState);
    paintColumn("Colour set", colourSetLabelBounds_, colourSetSortBounds_, colourSetClearBounds_,
                colourSetFilter_.isNotEmpty(), FilterColumn::kColourSet);
    paintLinkLegend(g);

    const int mappedReserve = (SkinEditorChrome::kButtonGap + SkinEditorChrome::sortBadgeWidth())
                              + (mappedColourFilter_.isNotEmpty()
                                     ? (SkinEditorChrome::kSortToFilterClearGap
                                        + SkinEditorChrome::clearBadgeWidth())
                                     : 0);
    paintColumn(SkinEditorChrome::elideToWidth(font, "Mapped colour",
                                               juce::jmax(0, cols.mappedW - mappedReserve)),
                mappedLabelBounds_, mappedSortBounds_, mappedClearBounds_,
                mappedColourFilter_.isNotEmpty(), FilterColumn::kMappedColour);
}

bool SkinEditorMappingsPanel::handleLegendClearClick(juce::Point<int> pos)
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

void SkinEditorMappingsPanel::mouseUp(const juce::MouseEvent& e)
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
        if (SkinEditorMappingsColumnFilter::headerGestureForClick(e.mods.isPopupMenu())
            == SkinEditorMappingsColumnFilter::HeaderGesture::kOpenFilter)
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
    handleHeader(FilterColumn::kMappedColour, mappedLabelBounds_);
}
