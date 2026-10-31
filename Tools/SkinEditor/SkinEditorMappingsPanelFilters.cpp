#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"

namespace
{
    constexpr int kColPad = SkinEditorChrome::kMappingsColumnPad;
    /** Match Palette list: swatch inset, size, and name gap (PalettePanel). */
    constexpr int kPaletteSwatchX = 4;
    constexpr int kPaletteNameGap = 8;
    constexpr int kMenuTickSlot = 16;

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

    struct MappedColourMenuSpec
    {
        juce::String name;
        juce::uint32 argb = 0;
        bool pending = false;
        bool unbound = false;
        bool foundInPalette = false;
        bool ticked = false;
    };

    /** Mapped colour filter row: Palette swatch + gap + name (combo-styled menu chrome). */
    class MappedColourFilterMenuItem final : public juce::PopupMenu::CustomComponent
    {
    public:
        explicit MappedColourFilterMenuItem(MappedColourMenuSpec spec)
            : spec_(std::move(spec))
        {
        }

        void getIdealSize(int& idealWidth, int& idealHeight) override
        {
            idealHeight = SkinEditorChrome::kColumnHeaderHeight;
            const int sw = SkinEditorChrome::kColourSwatchSize;
            const int nameW = SkinEditorChrome::measureTextWidth(SkinEditorChrome::fontRegular(),
                                                                spec_.name);
            idealWidth = kMenuTickSlot + kPaletteSwatchX + sw + kPaletteNameGap + nameW + 12;
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds();
            if (isItemHighlighted())
            {
                g.setColour(SkinEditorChrome::controlFillHover());
                g.fillRect(bounds);
            }

            if (spec_.ticked)
            {
                auto tickArea = bounds.removeFromLeft(kMenuTickSlot).toFloat().reduced(3.0f, 4.0f);
                auto tick = getLookAndFeel().getTickShape(1.0f);
                g.setColour(SkinEditorChrome::textPrimary());
                g.fillPath(tick, tick.getTransformToScaleToFit(tickArea, true));
            }
            else
            {
                bounds.removeFromLeft(kMenuTickSlot);
            }

            const int sw = SkinEditorChrome::kColourSwatchSize;
            const int contentH = bounds.getHeight();
            const int swY = bounds.getY() + (contentH - sw) / 2;
            const int swX = bounds.getX() + kPaletteSwatchX;
            if (! spec_.unbound && spec_.foundInPalette)
            {
                const auto swatch = juce::Rectangle<int>(swX, swY, sw, sw);
                if (spec_.pending)
                    SkinEditorChrome::paintPendingColourSwatch(g, swatch);
                else
                    SkinEditorChrome::paintColourSwatch(g, swatch, juce::Colour(spec_.argb));
            }

            const int nameLeft = bounds.getX() + kPaletteSwatchX + sw + kPaletteNameGap;
            g.setFont(SkinEditorChrome::fontRegular());
            g.setColour(spec_.unbound ? SkinEditorChrome::attention()
                                      : SkinEditorChrome::textPrimary());
            g.drawText(spec_.name,
                       nameLeft,
                       bounds.getY(),
                       juce::jmax(0, bounds.getRight() - nameLeft - 4),
                       contentH,
                       juce::Justification::centredLeft,
                       false);
        }

    private:
        MappedColourMenuSpec spec_;
    };

    MappedColourMenuSpec makeMappedMenuSpec(const TSS::DevSkin::NamedSkin& skin,
                                            const juce::String& name,
                                            bool ticked)
    {
        MappedColourMenuSpec spec;
        spec.name = name;
        spec.ticked = ticked;
        spec.unbound = (name == SkinEditorMappingsColumnFilter::kUnboundMenuValue);
        if (spec.unbound)
            return spec;
        for (const auto& pastille : skin.palette)
        {
            if (pastille.name == name)
            {
                spec.argb = pastille.argb;
                spec.pending = pastille.pending;
                spec.foundInPalette = true;
                break;
            }
        }
        return spec;
    }

    void appendMappedColourFilterItems(juce::PopupMenu& menu,
                                       const TSS::DevSkin::NamedSkin& skin,
                                       const juce::StringArray& values,
                                       const juce::String& active)
    {
        for (int i = 0; i < values.size(); ++i)
        {
            const auto& name = values[i];
            menu.addCustomItem(i + 1,
                               std::make_unique<MappedColourFilterMenuItem>(
                                   makeMappedMenuSpec(skin, name, name == active)),
                               nullptr,
                               name);
        }
    }

    juce::PopupMenu::Options filterMenuOptions(juce::Component& host,
                                               juce::Rectangle<int> screenArea)
    {
        return juce::PopupMenu::Options()
            .withTargetComponent(&host)
            .withParentComponent(host.getTopLevelComponent())
            .withTargetScreenArea(screenArea);
    }
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

juce::Rectangle<int> SkinEditorMappingsPanel::legendFilterTargetBounds(FilterColumn column) const noexcept
{
    if (column == FilterColumn::kState)
        return stateLabelBounds_;
    if (column == FilterColumn::kMappedColour)
        return mappedLabelBounds_;
    return propertyLabelBounds_;
}

bool SkinEditorMappingsPanel::populateColumnFilterMenu(juce::PopupMenu& menu,
                                                       FilterColumn column,
                                                       const juce::StringArray& values)
{
    menu.setLookAndFeel(&getLookAndFeel());
    const auto& active = filterValue(column);
    if (column != FilterColumn::kMappedColour)
    {
        for (int i = 0; i < values.size(); ++i)
            menu.addItem(i + 1, values[i], true, values[i] == active);
        return true;
    }

    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
        return false;
    appendMappedColourFilterItems(menu, *skin, values, active);
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
