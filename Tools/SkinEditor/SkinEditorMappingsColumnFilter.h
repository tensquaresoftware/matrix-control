#pragma once

#include <map>
#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "GUI/Skins/Dev/DevSkinTypes.h"

/** Pure Mappings column-filter predicates (juce_core + DevSkin; no Component). */
namespace SkinEditorMappingsColumnFilter
{
    inline constexpr const char* kUnboundMenuValue = "Unbound";

    enum class Column
    {
        kProperty,
        kState,
        kMappedColour
    };

    struct ActiveFilters
    {
        juce::String property;
        juce::String state;
        juce::String mappedColour;
    };

    struct ObjectKeyGroup
    {
        TSS::SkinObjectType object {};
        std::vector<TSS::SkinColourKey> keys;
    };

    inline bool keyMatches(const TSS::DevSkin::NamedSkin& skin,
                           const TSS::SkinColourKey& key,
                           const ActiveFilters& filters)
    {
        if (filters.property.isNotEmpty()
            && TSS::DevSkin::toString(key.property) != filters.property)
            return false;
        if (filters.state.isNotEmpty()
            && TSS::DevSkin::toString(key.state) != filters.state)
            return false;
        if (filters.mappedColour.isEmpty())
            return true;

        const auto it = skin.mappings.find(key);
        const bool unbound = (it == skin.mappings.end() || it->second.isEmpty());
        if (filters.mappedColour == kUnboundMenuValue)
            return unbound;
        return ! unbound && it->second == filters.mappedColour;
    }

    /**
     * Inventory keys grouped by object under ActiveFilters.
     * Omits object groups with zero matching keys (empty sections).
     */
    inline std::vector<ObjectKeyGroup> filteredObjectGroups(const TSS::DevSkin::NamedSkin& skin,
                                                            const ActiveFilters& filters)
    {
        std::vector<TSS::SkinObjectType> objectOrder;
        std::map<TSS::SkinObjectType, std::vector<TSS::SkinColourKey>> keysByObject;
        for (const auto& key : TSS::DevSkin::allStructuredKeys())
        {
            auto& bucket = keysByObject[key.object];
            if (bucket.empty())
                objectOrder.push_back(key.object);
            bucket.push_back(key);
        }

        std::vector<ObjectKeyGroup> groups;
        for (const auto object : objectOrder)
        {
            ObjectKeyGroup group;
            group.object = object;
            for (const auto& key : keysByObject[object])
            {
                if (keyMatches(skin, key, filters))
                    group.keys.push_back(key);
            }
            if (! group.keys.empty())
                groups.push_back(std::move(group));
        }
        return groups;
    }

    /** Values present in the Editor skin inventory rows for one column. */
    inline juce::StringArray presentValues(const TSS::DevSkin::NamedSkin& skin, Column column)
    {
        juce::StringArray values;
        bool hasUnbound = false;
        for (const auto& key : TSS::DevSkin::allStructuredKeys())
        {
            if (column == Column::kProperty)
            {
                values.addIfNotAlreadyThere(TSS::DevSkin::toString(key.property));
                continue;
            }
            if (column == Column::kState)
            {
                values.addIfNotAlreadyThere(TSS::DevSkin::toString(key.state));
                continue;
            }

            const auto it = skin.mappings.find(key);
            if (it == skin.mappings.end() || it->second.isEmpty())
                hasUnbound = true;
            else
                values.addIfNotAlreadyThere(it->second);
        }

        values.sort(true);
        if (column == Column::kMappedColour && hasUnbound
            && ! values.contains(kUnboundMenuValue))
            values.insert(0, kUnboundMenuValue);
        return values;
    }
}
