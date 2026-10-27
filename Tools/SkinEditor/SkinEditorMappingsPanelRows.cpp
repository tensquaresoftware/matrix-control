#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include <map>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    constexpr int kEdgeExtra = 4;
    constexpr int kFocusPad = 1;
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
    if (layer_ == nullptr)
        return;

    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    std::vector<TSS::SkinObjectType> objectOrder;
    std::map<TSS::SkinObjectType, std::vector<TSS::SkinColourKey>> keysByObject;
    for (const auto& key : TSS::DevSkin::allStructuredKeys())
    {
        auto& bucket = keysByObject[key.object];
        if (bucket.empty())
            objectOrder.push_back(key.object);
        bucket.push_back(key);
    }

    int y = kFocusPad;
    for (size_t oi = 0; oi < objectOrder.size(); ++oi)
    {
        if (oi > 0)
        {
            Row gap;
            gap.kind = RowKind::kGap;
            gap.heightPx = SkinEditorChrome::kTableGapHeight;
            rows_.push_back(gap);
            y += gap.heightPx;
        }
        const auto object = objectOrder[oi];
        y = appendObjectTable(*skin, object, keysByObject[object], y);
    }
}
