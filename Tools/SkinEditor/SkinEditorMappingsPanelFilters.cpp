#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"

namespace
{
    constexpr int kColPad = SkinEditorChrome::kMappingsColumnPad;

    struct ColumnLayout
    {
        int propertyW = 0;
        int stateW = 0;
        int mappedX = 0;
        int mappedW = 0;
    };

    ColumnLayout columnsForWidth(int totalWidth) noexcept
    {
        const int mappedSpan = juce::jmin(SkinEditorChrome::kPaletteListPaintWidth,
                                          juce::jmax(0, totalWidth));
        return { SkinEditorChrome::kMappingsPropertyColumnWidth,
                 SkinEditorChrome::kMappingsStateColumnWidth,
                 totalWidth - mappedSpan,
                 mappedSpan };
    }

}

bool SkinEditorMappingsPanel::mappingKeyMatches(const TSS::DevSkin::NamedSkin& skin,
                                                const TSS::SkinColourKey& key) const
{
    return SkinEditorMappingsColumnFilter::keyMatches(
        skin, key, { propertyFilter_, stateFilter_, mappedColourFilter_ });
}

juce::String& SkinEditorMappingsPanel::filterValue(FilterColumn column) noexcept
{
    switch (column)
    {
        case FilterColumn::kProperty: return propertyFilter_;
        case FilterColumn::kState: return stateFilter_;
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
        case FilterColumn::kMappedColour: return mappedColourFilter_;
    }
    return propertyFilter_;
}

void SkinEditorMappingsPanel::clearAllColumnFilters()
{
    propertyFilter_.clear();
    stateFilter_.clear();
    mappedColourFilter_.clear();
}

void SkinEditorMappingsPanel::clearColumnFilter(FilterColumn column)
{
    filterValue(column).clear();
    refresh();
}

void SkinEditorMappingsPanel::setColumnFilter(FilterColumn column, const juce::String& value)
{
    filterValue(column) = value;
    refresh();
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
        case FilterColumn::kMappedColour: mapped = CF::kMappedColour; break;
    }
    return SkinEditorMappingsColumnFilter::presentValues(*skin, mapped);
}

void SkinEditorMappingsPanel::showColumnFilterMenu(FilterColumn column)
{
    const auto values = presentValuesFor(column);
    if (values.isEmpty())
        return;

    juce::PopupMenu menu;
    const auto& active = filterValue(column);
    for (int i = 0; i < values.size(); ++i)
        menu.addItem(i + 1, values[i], true, values[i] == active);

    const juce::Rectangle<int>* target = &propertyLabelBounds_;
    if (column == FilterColumn::kState)
        target = &stateLabelBounds_;
    else if (column == FilterColumn::kMappedColour)
        target = &mappedLabelBounds_;

    const juce::String openedForSkinId = skinId_;
    menu.showMenuAsync(
        juce::PopupMenu::Options()
            .withTargetComponent(this)
            .withTargetScreenArea(localAreaToGlobal(*target)),
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
    propertyClearBounds_ = {};
    stateLabelBounds_ = {};
    stateClearBounds_ = {};
    mappedLabelBounds_ = {};
    mappedClearBounds_ = {};
}

void SkinEditorMappingsPanel::layoutLegendHitTargets()
{
    clearLegendHitTargets();
    if (stickyLegendBounds_.isEmpty())
        return;

    const auto area = stickyLegendBounds_;
    const auto cols = columnsForWidth(area.getWidth());
    const auto font = SkinEditorChrome::fontBold();
    const int badgeW = SkinEditorChrome::clearBadgeWidth();
    const int gap = SkinEditorChrome::kButtonGap;

    auto place = [&](const juce::String& text,
                     int x,
                     int maxW,
                     juce::Rectangle<int>& labelBounds,
                     juce::Rectangle<int>& clearBounds,
                     bool active)
    {
        const int reserved = active ? (gap + badgeW) : 0;
        const int labelMax = juce::jmax(0, maxW - reserved);
        const int labelW = juce::jmin(labelMax, SkinEditorChrome::measureTextWidth(font, text));
        labelBounds = { x, area.getY(), juce::jmax(0, labelW), area.getHeight() };
        if (active)
            clearBounds = { labelBounds.getRight() + gap, area.getY(), badgeW, area.getHeight() };
    };

    place("Property",
          area.getX() + kColPad,
          juce::jmax(0, cols.propertyW - 4),
          propertyLabelBounds_,
          propertyClearBounds_,
          propertyFilter_.isNotEmpty());
    place("State",
          area.getX() + kColPad + cols.propertyW,
          juce::jmax(0, cols.stateW - 4),
          stateLabelBounds_,
          stateClearBounds_,
          stateFilter_.isNotEmpty());

    const int mappedReserve = mappedColourFilter_.isNotEmpty() ? (gap + badgeW) : 0;
    const auto mappedText = SkinEditorChrome::elideToWidth(
        font, "Mapped colour", juce::jmax(0, cols.mappedW - mappedReserve));
    place(mappedText,
          area.getX() + cols.mappedX,
          cols.mappedW,
          mappedLabelBounds_,
          mappedClearBounds_,
          mappedColourFilter_.isNotEmpty());
}

void SkinEditorMappingsPanel::paintStickyLegend(juce::Graphics& g, juce::Rectangle<int> area) const
{
    SkinEditorChrome::fillTableRow(g, { area.toFloat(), SkinEditorChrome::surfaceSection(),
                                        true, true });
    const auto cols = columnsForWidth(area.getWidth());
    const auto font = SkinEditorChrome::fontBold();
    g.setFont(font);

    auto paintLabel = [&](const juce::String& text,
                          juce::Rectangle<int> labelBounds,
                          juce::Rectangle<int> clearBounds,
                          bool active)
    {
        g.setColour(active ? SkinEditorChrome::attention()
                           : SkinEditorChrome::textTableHeader());
        g.drawText(text, labelBounds, juce::Justification::centredLeft, false);
        if (active && ! clearBounds.isEmpty())
            SkinEditorChrome::drawClearBadge(g, clearBounds);
    };

    paintLabel("Property", propertyLabelBounds_, propertyClearBounds_,
               propertyFilter_.isNotEmpty());
    paintLabel("State", stateLabelBounds_, stateClearBounds_,
               stateFilter_.isNotEmpty());

    const int mappedReserve = mappedColourFilter_.isNotEmpty()
                                  ? (SkinEditorChrome::kButtonGap + SkinEditorChrome::clearBadgeWidth())
                                  : 0;
    paintLabel(SkinEditorChrome::elideToWidth(font, "Mapped colour",
                                              juce::jmax(0, cols.mappedW - mappedReserve)),
               mappedLabelBounds_, mappedClearBounds_,
               mappedColourFilter_.isNotEmpty());
}

void SkinEditorMappingsPanel::mouseUp(const juce::MouseEvent& e)
{
    if (isShellEmpty() || ! e.mouseWasClicked())
        return;

    const auto pos = e.getPosition();
    if (propertyClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kProperty);
        return;
    }
    if (stateClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kState);
        return;
    }
    if (mappedClearBounds_.contains(pos))
    {
        clearColumnFilter(FilterColumn::kMappedColour);
        return;
    }
    if (propertyLabelBounds_.contains(pos))
    {
        showColumnFilterMenu(FilterColumn::kProperty);
        return;
    }
    if (stateLabelBounds_.contains(pos))
    {
        showColumnFilterMenu(FilterColumn::kState);
        return;
    }
    if (mappedLabelBounds_.contains(pos))
        showColumnFilterMenu(FilterColumn::kMappedColour);
}
