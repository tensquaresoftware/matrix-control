#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include <algorithm>
#include <set>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumns.h"
#include "Tools/SkinEditor/SkinEditorUiHelpers.h"

namespace
{
    constexpr int kEdgeExtra = 2;
    constexpr int kFocusPad = SkinEditorChrome::kMappingsFocusPad;
}

SkinEditorMappingsPanel::Row SkinEditorMappingsPanel::makeMappingRow(
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

int SkinEditorMappingsPanel::appendObjectTable(const TSS::DevSkin::NamedSkin& skin,
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
    section.heightPx = SkinEditorChrome::kColumnHeaderHeight;
    rows_.push_back(section);
    y += section.heightPx;
    table.tableTop = y;

    for (size_t i = 0; i < objectKeys.size(); ++i)
    {
        auto row = makeMappingRow(skin, objectKeys[i], objectIndex);
        row.thickenTop = (i == 0);
        row.thickenBottom = (i + 1 == objectKeys.size());
        row.heightPx = SkinEditorChrome::kColumnHeaderHeight
                       + (row.thickenTop ? kEdgeExtra : 0)
                       + (row.thickenBottom ? kEdgeExtra : 0);
        rows_.push_back(row);
        y += row.heightPx;
    }

    table.tableBottom = y;
    objects_.push_back(table);
    return y;
}

void SkinEditorMappingsPanel::rebuildRows()
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

    const SkinEditorMappingsColumnFilter::ActiveFilters filters {
        propertyFilter_, stateFilter_, colourSetFilter_, mappedColourFilter_ };

    int y = kFocusPad;
    bool anyTable = false;
    for (auto group : SkinEditorMappingsColumnFilter::filteredObjectGroups(*skin, filters))
    {
        if (! SkinEditorMappingsColumnFilter::objectMacroFilterShowsGroup(objectFilterName_,
                                                                          group.object))
            continue;

        SkinEditorMappingsColumnFilter::sortFilteredKeys(*skin, group.keys, sortSpec_);

        if (anyTable)
        {
            Row gap;
            gap.kind = RowKind::kGap;
            gap.heightPx = SkinEditorChrome::kTableGapHeight;
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

void SkinEditorMappingsPanel::paintColourSetCell(juce::Graphics& g,
                                                 const Row& item,
                                                 juce::Rectangle<int> cellArea) const
{
    const int contentH = cellArea.getHeight();
    const int badgeH = SkinEditorChrome::kColourSetIndexBadgeHeight;
    const auto& indexFont = SkinEditorChrome::fontBold(SkinEditorChrome::kColourSetIndexBadgeFont);
    const int textW = SkinEditorChrome::measureTextWidth(indexFont, item.colourSetLabel);
    const int padH = SkinEditorChrome::kColourSetIndexBadgePadH;
    const int badgeW = juce::jmax(SkinEditorChrome::kColourSetIndexBadgeMinWidth,
                                  textW + 2 * padH);
    const int badgeY = cellArea.getY() + (contentH - badgeH) / 2;
    const auto badge = juce::Rectangle<float>((float) cellArea.getX(),
                                              (float) badgeY,
                                              (float) badgeW,
                                              (float) badgeH);

    g.setColour(SkinEditorChrome::textMuted());
    g.fillRoundedRectangle(badge, SkinEditorChrome::kChromeBadgeCorner);
    g.setColour(SkinEditorChrome::surfaceTable());
    g.setFont(indexFont);
    // Float rect + measured width keeps "csN" optically centred in the tight chip.
    g.drawText(item.colourSetLabel, badge, juce::Justification::centred, false);
    // Always restore body font — Mapped colour is painted next on the same Graphics.
    g.setFont(SkinEditorChrome::fontRegular());

    if (item.colourSetHelpLabel.isEmpty())
        return;

    const int labelX = juce::roundToInt(badge.getRight()) + 4;
    g.setColour(SkinEditorChrome::textTableBody());
    // No ellipsis: labels are hard-capped at 16 ASCII chars at edit/import time.
    g.drawText(item.colourSetHelpLabel,
               labelX,
               cellArea.getY(),
               juce::jmax(0, cellArea.getRight() - labelX),
               contentH,
               juce::Justification::centredLeft,
               false);
}

void SkinEditorMappingsPanel::tryEditColourSetLabel(int row)
{
    if (readOnly_ || layer_ == nullptr || skinId_.isEmpty())
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.kind != RowKind::kMapping)
        return;

    juce::String next = item.colourSetHelpLabel;
    const SkinEditorUi::AskTextRequest request {
        "Colour set label (" + item.colourSetLabel + ")",
        "Shared help label for this Colour set (max "
            + juce::String(TSS::DevSkin::kColourSetLabelMaxChars) + " ASCII).",
        item.colourSetHelpLabel,
        TSS::DevSkin::kColourSetLabelMaxChars,
        TSS::DevSkin::colourSetLabelAllowedChars()
    };
    if (! SkinEditorUi::askText(request, next))
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

void SkinEditorMappingsPanel::tryBindRow(int row)
{
    if (readOnly_ || onRowActivated == nullptr)
        return;
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;
    if (rows_[static_cast<size_t>(row)].kind != RowKind::kMapping)
        return;

    onRowActivated(rows_[static_cast<size_t>(row)].key);
}

void SkinEditorMappingsPanel::maybeActivateMappingCell(int row, int pressX, int releaseX, bool skipBind)
{
    const auto cols = SkinEditorMappingsColumns::forWidth(paintWidth());
    if (SkinEditorMappingPolicy::isMappedColourColumn(releaseX, cols.mappedX)
        && SkinEditorMappingPolicy::isMappedColourColumn(pressX, cols.mappedX))
    {
        tryEditMappedColour(row);
        return;
    }
    if (SkinEditorMappingsColumns::isColourSetColumnX(releaseX, cols)
        || SkinEditorMappingsColumns::isColourSetColumnX(pressX, cols)
        || SkinEditorMappingsColumns::isLinkColumnX(releaseX, cols)
        || SkinEditorMappingsColumns::isLinkColumnX(pressX, cols))
        return;
    if (! skipBind
        && SkinEditorMappingsColumns::isPropertyColumnX(releaseX, cols)
        && SkinEditorMappingsColumns::isPropertyColumnX(pressX, cols))
        tryBindRow(row);
}

void SkinEditorMappingsPanel::handleMappingRowMouseUp(int row, const juce::MouseEvent& e)
{
    const auto& item = rows_[static_cast<size_t>(row)];
    const auto cols = SkinEditorMappingsColumns::forWidth(paintWidth());
    const int releaseX = e.x - kFocusPad;
    const int pressX = e.getMouseDownX() - kFocusPad;
    const bool toggle = e.mods.isCommandDown();
    const bool range = e.mods.isShiftDown();
    if (SkinEditorMappingsColumns::isLinkColumnX(releaseX, cols)
        && SkinEditorMappingsColumns::isLinkColumnX(pressX, cols))
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

void SkinEditorMappingsPanel::tryEditMappedColour(int row)
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

void SkinEditorMappingsPanel::setFocusedObject(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= static_cast<int>(objects_.size()))
        return;

    focusedObject_ = objectIndex;
    // Object combo is a macro-filter — do not sync it from row clicks / outline focus.
    content_.repaint();
}

SkinEditorMappingsPanel::ViewScrollAnchor SkinEditorMappingsPanel::captureViewScrollAnchor() const
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

void SkinEditorMappingsPanel::restoreViewScrollAnchor(const ViewScrollAnchor& anchor)
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

void SkinEditorMappingsPanel::paintObjectTableChrome(juce::Graphics& g,
                                                     ObjectTableChromeSpec spec) const
{
    const auto& table = *spec.table;
    const auto& cols = *spec.cols;
    const int sectionH = table.tableTop - table.sectionY;
    if (sectionH > 0)
    {
        const auto prop = SkinEditorMappingsColumns::propertyCutSlab(cols);
        SkinEditorChrome::paintTableSectionHeader(
            g,
            { (float) (spec.x0 + prop.getX()), (float) table.sectionY,
              (float) prop.getWidth(), (float) sectionH },
            table.name);

        // Continuity rail in State…Mapped — matches tab rail; top corners only (mock).
        const int railH = SkinEditorChrome::kTableSectionRailHeight;
        const int railY = table.tableTop - railH;
        if (railH > 0 && railY >= table.sectionY)
            SkinEditorMappingsColumns::fillCutColumnSlabs(
                g, cols, { { spec.x0, railY, spec.width, railH },
                           SkinEditorChrome::surfaceTableHeader(), true, false, 1 });
    }

    const int bodyH = table.tableBottom - table.tableTop;
    if (bodyH > 0)
        SkinEditorMappingsColumns::fillCutColumnSlabs(
            g, cols, { { spec.x0, table.tableTop, spec.width, bodyH },
                       SkinEditorChrome::surfaceTable(), false, true });
}
