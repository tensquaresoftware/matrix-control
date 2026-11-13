#include "Tools/SkinDesigner/SkinDesignerMappingsPanel.h"

#include <algorithm>
#include <set>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinDesigner/SkinDesignerChrome.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumnFilter.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumns.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

namespace
{
    constexpr int kEdgeExtra = 2;
    constexpr int kFocusPad = SkinDesignerChrome::kMappingsFocusPad;
}

SkinDesignerMappingsPanel::Row SkinDesignerMappingsPanel::makeMappingRow(
    const TSS::DevSkin::NamedSkin& skin,
    const TSS::SkinColourKey& key,
    int objectIndex) const
{
    Row row;
    row.kind = RowKind::kMapping;
    row.objectIndex = objectIndex;
    row.key = key;
    row.propertyLabel = TSS::DevSkin::toString(key.property);
    row.stateLabel = TSS::DevSkin::toString(key.state);
    row.colourSetLabel = TSS::DevSkin::toString(key.colourSet);
    row.colourSetHelpLabel = skin.colourSetLabels[static_cast<size_t>(key.colourSet)];

    const auto it = skin.mappings.find(key);
    if (it == skin.mappings.end() || it->second.isEmpty())
    {
        row.unbound = true;
        return row;
    }

    row.pastilleName = it->second;
    for (const auto& pastille : skin.palette)
    {
        if (pastille.name == row.pastilleName)
        {
            row.pastilleArgb = pastille.argb;
            row.pastillePending = pastille.pending;
            break;
        }
    }
    return row;
}

int SkinDesignerMappingsPanel::appendObjectTable(const TSS::DevSkin::NamedSkin& skin,
                                               TSS::SkinObjectType object,
                                               const std::vector<TSS::SkinColourKey>& objectKeys,
                                               int y)
{
    const int objectIndex = static_cast<int>(objects_.size());
    ObjectTable table;
    table.name = TSS::DevSkin::toString(object);
    table.sectionY = y;

    Row section;
    section.kind = RowKind::kSection;
    section.sectionTitle = table.name;
    section.objectIndex = objectIndex;
    section.heightPx = SkinDesignerChrome::kColumnHeaderHeight;
    rows_.push_back(section);
    y += section.heightPx;
    table.tableTop = y;

    for (size_t i = 0; i < objectKeys.size(); ++i)
    {
        auto row = makeMappingRow(skin, objectKeys[i], objectIndex);
        row.thickenTop = (i == 0);
        row.thickenBottom = (i + 1 == objectKeys.size());
        row.heightPx = SkinDesignerChrome::kColumnHeaderHeight
                       + (row.thickenTop ? kEdgeExtra : 0)
                       + (row.thickenBottom ? kEdgeExtra : 0);
        rows_.push_back(row);
        y += row.heightPx;
    }

    table.tableBottom = y;
    objects_.push_back(table);
    return y;
}

void SkinDesignerMappingsPanel::rebuildRows()
{
    rows_.clear();
    objects_.clear();
    const auto* skin = (layer_ != nullptr && skinId_.isNotEmpty())
                           ? layer_->findSkin(skinId_)
                           : nullptr;
    if (skin == nullptr)
    {
        focusedObject_ = -1;
        clearSelectionAndHovers();
        return;
    }

    const SkinDesignerMappingsColumnFilter::ActiveFilters filters {
        propertyFilter_, stateFilter_, colourSetFilter_, mappedColourFilter_ };

    int y = kFocusPad;
    bool anyTable = false;
    for (auto group : SkinDesignerMappingsColumnFilter::filteredObjectGroups(*skin, filters))
    {
        if (! SkinDesignerMappingsColumnFilter::objectMacroFilterShowsGroup(objectFilterName_,
                                                                          group.object))
            continue;

        SkinDesignerMappingsColumnFilter::sortFilteredKeys(*skin, group.keys, sortSpec_);

        if (anyTable)
        {
            Row gap;
            gap.kind = RowKind::kGap;
            gap.heightPx = SkinDesignerChrome::kTableGapHeight;
            rows_.push_back(gap);
            y += gap.heightPx;
        }
        y = appendObjectTable(*skin, group.object, group.keys, y);
        anyTable = true;
    }

    if (focusedObject_ >= static_cast<int>(objects_.size()))
        focusedObject_ = -1;

    std::set<TSS::SkinColourKey> visibleKeys;
    for (const auto& row : rows_)
        if (row.kind == RowKind::kMapping)
            visibleKeys.insert(row.key);
    selectedKeys_.erase(std::remove_if(selectedKeys_.begin(), selectedKeys_.end(),
                                       [&](const TSS::SkinColourKey& key)
                                       {
                                           return visibleKeys.find(key) == visibleKeys.end();
                                       }),
                        selectedKeys_.end());
    if (selectionAnchorKey_.has_value()
        && visibleKeys.find(*selectionAnchorKey_) == visibleKeys.end())
        selectionAnchorKey_.reset();
}

void SkinDesignerMappingsPanel::paintColourSetCell(juce::Graphics& g,
                                                 const Row& item,
                                                 juce::Rectangle<int> cellArea) const
{
    const int contentH = cellArea.getHeight();
    const int badgeH = SkinDesignerChrome::kColourSetIndexBadgeHeight;
    const auto& indexFont = SkinDesignerChrome::fontBold(SkinDesignerChrome::kColourSetIndexBadgeFont);
    const int textW = SkinDesignerChrome::measureTextWidth(indexFont, item.colourSetLabel);
    const int padH = SkinDesignerChrome::kColourSetIndexBadgePadH;
    const int badgeW = juce::jmax(SkinDesignerChrome::kColourSetIndexBadgeMinWidth,
                                  textW + 2 * padH);
    const int badgeY = cellArea.getY() + (contentH - badgeH) / 2;
    const auto badge = juce::Rectangle<float>((float) cellArea.getX(),
                                              (float) badgeY,
                                              (float) badgeW,
                                              (float) badgeH);

    g.setColour(SkinDesignerChrome::textMuted());
    g.fillRoundedRectangle(badge, SkinDesignerChrome::kChromeBadgeCorner);
    g.setColour(SkinDesignerChrome::surfaceTable());
    g.setFont(indexFont);
    // Float rect + measured width keeps "csN" optically centred in the tight chip.
    g.drawText(item.colourSetLabel, badge, juce::Justification::centred, false);
    // Always restore body font — Mapped colour is painted next on the same Graphics.
    g.setFont(SkinDesignerChrome::fontRegular());

    if (item.colourSetHelpLabel.isEmpty())
        return;

    const int labelX = juce::roundToInt(badge.getRight()) + 4;
    g.setColour(SkinDesignerChrome::textTableBody());
    // No ellipsis: labels are hard-capped at 16 ASCII chars at edit/import time.
    g.drawText(item.colourSetHelpLabel,
               labelX,
               cellArea.getY(),
               juce::jmax(0, cellArea.getRight() - labelX),
               contentH,
               juce::Justification::centredLeft,
               false);
}

void SkinDesignerMappingsPanel::tryEditColourSetLabel(int row)
{
    if (readOnly_ || layer_ == nullptr || skinId_.isEmpty())
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kMapping)
        return;

    juce::String next = item.colourSetHelpLabel;
    const SkinDesignerUi::AskTextRequest request {
        "Colour set label (" + item.colourSetLabel + ")",
        "Shared help label for this Colour set (max "
            + juce::String(TSS::DevSkin::kColourSetLabelMaxChars) + " ASCII).",
        item.colourSetHelpLabel,
        TSS::DevSkin::kColourSetLabelMaxChars,
        TSS::DevSkin::colourSetLabelAllowedChars(),
        "Label"
    };
    if (! SkinDesignerUi::askText(request, next))
        return;

    const auto result = layer_->setColourSetLabel(skinId_, item.key.colourSet, next);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Colour set label",
                                               result.getErrorMessage());
        return;
    }
    refresh();
}

void SkinDesignerMappingsPanel::tryBindRow(int row)
{
    if (readOnly_ || onRowActivated == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;
    if (rows_[static_cast<size_t>(row)].kind != RowKind::kMapping)
        return;

    onRowActivated(rows_[static_cast<size_t>(row)].key);
}

void SkinDesignerMappingsPanel::maybeActivateMappingCell(int row, int pressX, int releaseX, bool skipBind)
{
    const auto cols = SkinDesignerMappingsColumns::forWidth(paintWidth());
    if (SkinDesignerMappingPolicy::isMappedColourColumn(releaseX, cols.mappedX)
        && SkinDesignerMappingPolicy::isMappedColourColumn(pressX, cols.mappedX))
    {
        tryEditMappedColour(row);
        return;
    }
    if (SkinDesignerMappingsColumns::isColourSetColumnX(releaseX, cols)
        || SkinDesignerMappingsColumns::isColourSetColumnX(pressX, cols)
        || SkinDesignerMappingsColumns::isLinkColumnX(releaseX, cols)
        || SkinDesignerMappingsColumns::isLinkColumnX(pressX, cols))
        return;
    if (! skipBind
        && SkinDesignerMappingsColumns::isPropertyColumnX(releaseX, cols)
        && SkinDesignerMappingsColumns::isPropertyColumnX(pressX, cols))
        tryBindRow(row);
}

void SkinDesignerMappingsPanel::handleMappingRowMouseUp(int row, const juce::MouseEvent& e)
{
    const auto& item = rows_[static_cast<size_t>(row)];
    const auto cols = SkinDesignerMappingsColumns::forWidth(paintWidth());
    const int releaseX = e.x - kFocusPad;
    const int pressX = e.getMouseDownX() - kFocusPad;
    const bool toggle = e.mods.isCommandDown();
    const bool range = e.mods.isShiftDown();
    if (SkinDesignerMappingsColumns::isLinkColumnX(releaseX, cols)
        && SkinDesignerMappingsColumns::isLinkColumnX(pressX, cols))
    {
        if (readOnly_)
            selectRowKey(item.key, toggle, range);
        else
            handleLinkChainClick(item.key);
        return;
    }

    selectRowKey(item.key, toggle, range);
    maybeActivateMappingCell(row, pressX, releaseX, toggle || range);
}

void SkinDesignerMappingsPanel::tryEditMappedColour(int row)
{
    if (onMappedColourClicked == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kMapping || item.unbound)
        return;

    onMappedColourClicked(item.key);
}

void SkinDesignerMappingsPanel::setFocusedObject(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= static_cast<int>(objects_.size()))
        return;

    focusedObject_ = objectIndex;
    // Object combo is a macro-filter — do not sync it from row clicks / outline focus.
    content_.repaint();
}

SkinDesignerMappingsPanel::ViewScrollAnchor SkinDesignerMappingsPanel::captureViewScrollAnchor() const
{
    ViewScrollAnchor anchor;
    anchor.fallbackY = viewport_.getViewPositionY();
    // Include Object-combo pin: scrollY == sectionY - kFocusPad (header just under sticky legend).
    for (const auto& object : objects_)
    {
        if (object.sectionY - kFocusPad > anchor.fallbackY)
            break;
        anchor.objectName = object.name;
        anchor.offsetFromSectionY = anchor.fallbackY - object.sectionY;
    }
    return anchor;
}

void SkinDesignerMappingsPanel::restoreViewScrollAnchor(const ViewScrollAnchor& anchor)
{
    int restoreY = anchor.fallbackY;
    if (anchor.objectName.isNotEmpty())
    {
        for (const auto& object : objects_)
        {
            if (object.name != anchor.objectName)
                continue;
            restoreY = object.sectionY + anchor.offsetFromSectionY;
            break;
        }
    }
    const int maxY = juce::jmax(0, content_.getHeight() - viewport_.getHeight());
    viewport_.setViewPosition(0, juce::jlimit(0, maxY, restoreY));
}

void SkinDesignerMappingsPanel::paintObjectTableChrome(juce::Graphics& g,
                                                     ObjectTableChromeSpec spec) const
{
    const auto& table = *spec.table;
    const auto& cols = *spec.cols;
    const int sectionH = table.tableTop - table.sectionY;
    if (sectionH > 0)
    {
        const auto prop = SkinDesignerMappingsColumns::propertyCutSlab(cols);
        SkinDesignerChrome::paintTableSectionHeader(
            g,
            { (float) (spec.x0 + prop.getX()), (float) table.sectionY,
              (float) prop.getWidth(), (float) sectionH },
            table.name);

        // Continuity rail in State…Mapped — matches tab rail; top corners only (mock).
        const int railH = SkinDesignerChrome::kTableSectionRailHeight;
        const int railY = table.tableTop - railH;
        if (railH > 0 && railY >= table.sectionY)
            SkinDesignerMappingsColumns::fillCutColumnSlabs(
                g, cols, { { spec.x0, railY, spec.width, railH },
                           SkinDesignerChrome::surfaceTableHeader(), true, false, 1 });
    }

    const int bodyH = table.tableBottom - table.tableTop;
    if (bodyH > 0)
        SkinDesignerMappingsColumns::fillCutColumnSlabs(
            g, cols, { { spec.x0, table.tableTop, spec.width, bodyH },
                       SkinDesignerChrome::surfaceTable(), false, true });
}
